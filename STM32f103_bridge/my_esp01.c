#include "my_esp01.h"
#include "my_gpio.h"
#include "my_timer.h"
#include "my_usart.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


uint8_t timer_;
uint8_t esp01_usart_channel;
uint8_t pc_usart_channel;
uint16_t esp01_msg_delay = ESP01_TIMEOUT;
char esp01_msg[BUFFER_SIZE]; //@param message from esp01
char pc_msg[BUFFER_SIZE];
bool esp01_msg_rdy = false;
bool pc_msg_rdy = false;
//! change the initialization to receive uart functions as parameter
char cmd[COMMAND_SIZE];
char raw_data[BUFFER_SIZE + 1];

void wait_esp01_msg_rdy(void)
{
    while (!esp01_msg_rdy)
    {
    }
    esp01_msg_rdy = false;
}
void init_ESP01(uint8_t pc_channel, uint8_t esp01_channel, uint8_t timer)
{
    esp01_usart_channel = esp01_channel;
    pc_usart_channel = pc_channel;
    timer_ = timer;
    outMODE(ESP01_RST_PIN);
    outMODE(ESP01_ENA_PIN);
    pinON(ESP01_RST_PIN);
    pinON(ESP01_ENA_PIN);
    initTDELAY(timer_);
    timerDELAYMS(timer_, 2000); //* This delay to receive and get rid of initial response from esp01
    uartINT_RX(pc_channel, 115200);
    uartINT_RX(esp01_channel, 115200);
}

void send_ESP01(char *str)
{
    uartSTR_TX(esp01_usart_channel, str);
    uartSTR_TX(esp01_usart_channel, "\r\n");
}

bool ESP01_control_panel(char *cmd, const char *response, const uint8_t timeout_secs)
{
    esp01_msg_delay = timeout_secs * 20;
    send_ESP01(cmd);
    wait_esp01_msg_rdy();
    if (strstr(esp01_msg, response) != NULL && strstr(esp01_msg, "OK") != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ESP01_Detect(void)
{
    return ESP01_control_panel("AT", "", 1);
}

bool ESP01_station_mode(void)
{
    return ESP01_control_panel("AT+CWMODE_CUR=1", "", 1);
}

bool ESP01_list_APs(void)
{
    return ESP01_control_panel("AT+CWLAP", "", 5);
}

bool ESP01_join_AP(const char *ssid, const char *password)
{
    char msg[COMMAND_SIZE]; //* limitations on network info size
    sprintf(msg, "AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, password);
    return ESP01_control_panel(msg, "", 10);
}

bool ESP01_disconnect(void)
{
    return ESP01_control_panel("AT+CWQAP", "", 3);
}

bool ESP01_connect(const char *ssid, const char *password)
{
    if (!ESP01_join_AP(ssid, password))
        return false;
    return ESP01_control_panel("AT+CIPSTATUS", "2", 1);
}

bool ESP01_multiple_connections(void)
{
    return ESP01_control_panel("AT+CIPMUX=1", "", 1);
}

bool ESP01_single_connection(void)
{
    return ESP01_control_panel("AT+CIPMUX=0", "", 1);
}

bool ESP01_create_TCP_server(void)
{
    return ESP01_control_panel("AT+CIPSERVER=1,80", "", 1);
}

bool ESP01_delete_TCP_server(void)
{
    return ESP01_control_panel("AT+CIPSERVER=0,80", "", 1);
}

bool ESP01_get_IP(void)
{
    return ESP01_control_panel("AT+CIFSR", "", 1);
}

bool ESP01_send_client(char id, const char *data)
{
    uint32_t size = strlen(data);
    const char *ptr = data;
    while (size)
    {
        // memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "AT+CIPSEND=%c,%d", id, size >= BUFFER_SIZE ? BUFFER_SIZE : size);
        if (ESP01_control_panel(cmd, ">", 2))
        {
            if (size >= BUFFER_SIZE)
            {
                size -= (BUFFER_SIZE);
                memcpy(raw_data, ptr, BUFFER_SIZE);
                raw_data[BUFFER_SIZE] = '\0';
                if (!ESP01_control_panel(raw_data, "SEND", 2))
                    return false;
                ptr += BUFFER_SIZE;
            }
            else
            {
                memcpy(raw_data, ptr, size);
                raw_data[size] = '\0';
                if (!ESP01_control_panel(raw_data, "SEND", 2))
                    return false;
                size = 0;
            }
        }
        else
        {
            return false;
        }
    }
    sprintf(cmd, "AT+CIPCLOSE=%c", id);
    return ESP01_control_panel(cmd, "", 2);
}
