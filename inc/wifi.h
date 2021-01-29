#ifndef __WIFI_H
#define __WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"
	
#define HEADER_1B			0x55
#define HEADER_2B			0xAA
#define HEADER_VER		0x03 // 55 AA [00] - 00 indicates frame from module.
#define HEARTBEAT			0x01
	
#define CMD_HB				0x00
#define CMD_INFO			0x01	
#define CMD_WMODE			0x02		
#define CMD_WF_STAT		0x03		
#define CMD_RESET		  0x04
#define CMD_NET_CONF	0x05	
	
#define CMD_INPUT   	0x06
#define CMD_OUTPUT  	0x07	
	
#define CMD_QUERY			0x08
	
#define ID_SWITCH			0x01
#define ID_CURRTEMP		0x03
#define ID_WORKMODE		0x04
#define ID_CHILDLOCK	0x07
#define ID_BRIGHT			0x0A
#define ID_CURPOWER		0x0C
#define	ID_REMTIME		0x14
#define ID_FAULT			0x15
#define	ID_SCHEDULE		0x1A
#define ID_COMFORT		0x65
#define ID_ECO				0x66
#define ID_ANTIFR			0x67
#define ID_LCDOFF			0x68
#define ID_PROG				0x69
#define ID_SOUND			0x6A
#define ID_HEATMODE		0x6B
#define ID_OPENWINDOW	0x6C
#define ID_TIMER			0x6D
#define ID_TIMERTIME	0x6E
#define ID_CUSTOM_P	  0x6F

uint8_t chksum8(const uint8_t *buff, size_t len);
//void receive_uart_int(void);
void usart_transmit_frame(const uint8_t *buff, size_t len);
void query_settings();	
void reset_wifi_state();
void query_faults();
	
	
#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
