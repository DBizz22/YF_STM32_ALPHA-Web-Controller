#include "board_control.h"
#include "my_esp01.h"
#include "my_timer.h"
#include "my_usart.h"
#include "server.h"
#include "stdbool.h"

bool board_msg_rdy = false;
uint8_t board_msg_delay = BOARD_MSG_TIMEOUT;
char request[BUFFER];
char board_response[BUFFER];

void init_board_control(void)
{
    uartINT_RX(USART3_CH, 115200);
    timerINT_50MS(T3, 50);
}

void wait_board_msg_rdy(void)
{
    send_pc_msg("Waiting for Board response...\n");
    while (!board_msg_rdy)
    {
    }
    board_msg_rdy = false;
}

void send_control_board(const char *msg)
{
    uartSTR_TX(USART3_CH, msg);
}

void send_request_to_board(void)
{
    wait_esp01_msg_rdy();
    send_pc_msg("Sending request to board\n");
    if (!get_client_request(request))
    {
        send_pc_msg("Invalid request\n");
        ESP01_send_client(get_connection_id(), "Invalid request\n");
    }
    send_control_board(request);
    send_pc_msg("Sent request to board\n");
}

void send_board_response(void)
{
    wait_board_msg_rdy();
    ESP01_send_client(get_connection_id(), board_response);
    send_pc_msg("Board responded\n");
}

void board_control_panel(void)
{
    send_request_to_board();
    send_board_response();
}