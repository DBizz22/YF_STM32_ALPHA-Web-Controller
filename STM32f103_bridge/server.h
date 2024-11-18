#ifndef SERVER_H_
#define SERVER_H_
#include "stdbool.h"
#include "stm32f10x.h"

//*Handling
#define REQUEST_BUFFER_SIZE 120
#define DEFAULT_AP "TP-LINK_E0B6"
#define DEFAULT_AP_PASSWORD ""
static const char *USERNAME = "divine";
static const char *PASSWORD = "dbizz22";
// char password[BUFFER];
void wait_esp01_msg_rdy(void);
const char *search_esp01_msg(const char *msg);
const char *search_esp01_msg_after(const char *msg);
void send_pc_msg(const char *msg);
char get_connection_id(void);
bool get_client_request(char *request);
bool load_login(void);
void make_string(char *dest, const char *start, uint8_t count);
bool check_login_details(const char *ssid, const char *password);
bool handle_login_request(void);
void verify_login_details(void);
bool load_control_webpage(void);
void init_server(void);
void load_webpages(void);

#endif