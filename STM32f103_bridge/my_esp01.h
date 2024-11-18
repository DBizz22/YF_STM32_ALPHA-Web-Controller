#ifndef MY_ESP01_H_
#define MY_ESP01_H_
#include "my_usart.h"
#include <stdint.h>

//@param esp01 pins
#define ESP01_RST_PIN PA0
#define ESP01_ENA_PIN PA1
#define ESP01_TX_PIN USART2_TX
#define ESP01_RX_PIN USART1_RX
#define COMMAND_SIZE 50
#define BUFFER_SIZE 2048
#define ESP01_TIMEOUT 100   //* Default timeout for 5 seconds

extern uint8_t esp01_usart_channel;
extern uint8_t pc_usart_channel; //@param to hold usart channel
extern uint16_t esp01_msg_delay;	//@param time to wait before sending esp01 response, multiple of 20	//5 seconds
extern char esp01_msg[BUFFER_SIZE];    //@param message from esp01
extern char pc_msg[BUFFER_SIZE];
extern bool esp01_msg_rdy;
extern bool pc_msg_rdy;

void init_ESP01(uint8_t pc_channel,uint8_t esp01_channel,uint8_t timer);
void wait_esp01_msg_rdy(void);
bool ESP01_Detect(void);
bool ESP01_station_mode(void);
bool ESP01_list_APs(void);
bool ESP01_disconnect(void);
bool ESP01_connect(const char * ssid,const char * password);
bool ESP01_multiple_connections(void);
bool ESP01_single_connection(void);
bool ESP01_create_TCP_server(void);
bool ESP01_delete_TCP_server(void);
bool ESP01_get_IP(void);
bool ESP01_send_client(char id,const char* data);

#endif