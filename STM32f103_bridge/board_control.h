#ifndef BOARD_CONTROL_H_
#define BOARD_CONTROL_H_
#include "stdbool.h"
#include "stm32f10x.h"

//*Board module
#define BUFFER 500            // 1024
#define BOARD_MSG_TIMEOUT 100 // 100 //@param board msg timeout for 5 secs

extern bool board_msg_rdy;
extern uint8_t board_msg_delay;
extern char board_response[BUFFER];

void init_board_control(void);
void board_control_panel(void);

#endif