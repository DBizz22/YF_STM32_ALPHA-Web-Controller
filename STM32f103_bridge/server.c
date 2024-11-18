#include "server.h"
#include "my_esp01.h"
#include "my_usart.h"
#include "string.h"
#include "webpages.h"
#include <stdbool.h>
#include <stdint.h>

void send_pc_msg(const char *msg)
{
    uartSTR_TX(pc_usart_channel, msg);
}

void wait_pc_msg_rdy(void)
{
    while (!pc_msg_rdy)
    {
    }
    pc_msg_rdy = false;
}

const char *search_esp01_msg(const char *msg)
{
    const char *ptr;
    if ((ptr = strstr(esp01_msg, msg)) == NULL)
        return NULL;

    return ptr;
}

const char *search_esp01_msg_after(const char *msg)
{
    const char *ptr;
    if ((ptr = search_esp01_msg(msg)) == NULL)
        return NULL;

    ptr += strlen(msg);
    return ptr;
}

void make_string(char *dest, const char *start, uint8_t count)
{
    if (start == NULL)
        return;
    strncpy(dest, start, count);
    dest[count] = '\0';
}

char get_connection_id(void)
{
    const char *id = search_esp01_msg_after("+IPD,");
    if (id != NULL)
        return *id;
    else
        return NULL;
}

bool get_client_request(char *request)
{
    const char *request_start = search_esp01_msg_after("STM32F1?");
    const char *request_end = search_esp01_msg("REQUEST_END");
    if (request_start == NULL)
        return false;

    uint8_t request_len = request_end - request_start;
    make_string(request, request_start, request_len);
    request[request_len] = '/';
    request[request_len + 1] = '\0';
    return true;
}

bool check_login_details(const char *username, const char *password)
{
    if (strcmp(username, USERNAME) != 0)
        return false;
    if (strcmp(password, PASSWORD) != 0)
        return false;

    return true;
}

bool handle_login_request(void)
{
    wait_esp01_msg_rdy();
    if (!search_esp01_msg("STM32F1?"))
        return false;

    char username[REQUEST_BUFFER_SIZE], password[REQUEST_BUFFER_SIZE];
    const char *username_begin = search_esp01_msg_after("username=");
    make_string(username, username_begin, strlen(USERNAME));
    const char *password_begin = search_esp01_msg_after("password=");
    make_string(password, password_begin, strlen(PASSWORD));
    if (!check_login_details(username, password))
        return false;

    return true;
}

void verify_login_details(void)
{
    while (handle_login_request() == false)
    {
        send_pc_msg("Incorrect login details.\n");
        ESP01_send_client(get_connection_id(), "wrong");
    }
    send_pc_msg("Login successful.\n");
    ESP01_send_client(get_connection_id(), "ok");
}

bool load_control_webpage(void)
{
    wait_esp01_msg_rdy();
    return ESP01_send_client(get_connection_id(), CONTROL_WEBPAGE);
}

bool mode_setup(void)
{
    if (!ESP01_Detect())
    {
        send_pc_msg("*******ERROR********: ESP01 Undetected\n");
        return false;
    }

    if (!ESP01_station_mode())
    {
        send_pc_msg("*******ERROR********: Unable to set Station mode\n");
        return false;
    }

    return true;
}

bool list_available_APs(void)
{
    if (!ESP01_list_APs())
    {
        send_pc_msg("*******ERROR********: Unable to detect available APs\n");
        return false;
    }
    send_pc_msg("\tAvailable APs: \n");
    send_pc_msg(esp01_msg);
    return true;
}
bool auto_connect(void)
{
    if (!ESP01_connect(DEFAULT_AP, DEFAULT_AP_PASSWORD))
    {
        send_pc_msg("*******ERROR********: Unable to automaticaly connect to default AP\n");
        return false;
    }
    return true;
}

void make_custom_AP_details(char *ssid, char *password)
{
    send_pc_msg("Enter SSID to connect\n");
    wait_pc_msg_rdy();
    make_string(ssid, pc_msg, REQUEST_BUFFER_SIZE);
    send_pc_msg("Enter Password\n");
    wait_pc_msg_rdy();
    make_string(password, pc_msg, REQUEST_BUFFER_SIZE);
}

bool attempt_connection_loop(const char *ssid, const char *password, uint8_t attempt)
{
    while (attempt)
    {
        send_pc_msg("Attempting to connect...\n");
        if (ESP01_connect(ssid, password))
            return true;
        send_pc_msg("Connection failed. Retrying...\n");
        attempt--;
    }
    return false;
}

bool manual_connect(void)
{
    char ssid[REQUEST_BUFFER_SIZE], password[REQUEST_BUFFER_SIZE];

    send_pc_msg("\tManual Connect mode enabled\n");
    if (!list_available_APs())
        return false;

    make_custom_AP_details(ssid, password);
    if (!attempt_connection_loop(ssid, password, 5))
    {
        send_pc_msg("*******ERROR********: Unable to connect to  AP\n");
        return false;
    }

    return true;
}
bool server_setup(void)
{
    if (!ESP01_multiple_connections())
    {
        send_pc_msg("*******ERROR********: Unable to set Multiple Connections\n");
        return false;
    }

    if (!ESP01_create_TCP_server())
    {
        send_pc_msg("*******ERROR********: Unable to create TCP server\n");
        return false;
    }

    if (!ESP01_get_IP())
    {
        send_pc_msg("*******ERROR********: IP unavailable\n");
        return false;
    }

    send_pc_msg("Done initializing TCP server\n");
    send_pc_msg("\t*****SERVER IP*****\n");
    send_pc_msg(esp01_msg);
    return true;
}

void init_server(void)
{
    while (true)
    {
        send_pc_msg("\tInitializing TCP server\n");
        if (!mode_setup())
        {
            send_pc_msg("Restarting server setup...\n");
            continue;
        }

        if (!auto_connect())
        {
            if (!manual_connect())
            {
                send_pc_msg("Restarting server setup...\n");
                continue;
            }
        }

        if (!server_setup())
        {
            send_pc_msg("Restarting server setup...\n");
            continue;
        }
        break;
    }
}

bool load_login()
{
    wait_esp01_msg_rdy();
    char connection_id = get_connection_id();
    if (strstr(esp01_msg, "CONNECT") == NULL)
    {
        return false;
    }

    if (!ESP01_send_client(connection_id, LOGIN))
    {
        send_pc_msg("*******ERROR********: can't load website\n");
        return false;
    }
    send_pc_msg("Done setting up login webpage\n");
    return true;
}
void load_webpages(void)
{
    /* while (!load_login())
     {
         send_pc_msg("*****ERROR*****: Reloading Login webpage...\n");
     }
     verify_login_details();*/
    while (!load_control_webpage())
    {
        send_pc_msg("*****ERROR*****: Reloading Login webpage...\n");
    }
    send_pc_msg("Done setting up control webpage\n");
}
