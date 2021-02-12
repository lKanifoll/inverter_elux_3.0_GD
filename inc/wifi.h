#ifndef __WIFI_H
#define __WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"
	
#define HEADER_1B			0xAC
#define HEADER_2B			0xBD
#define HEADER_VER		0x03 
#define HEARTBEAT			0x01
	
#define CMD_HB				0x00
#define CMD_INFO			0x01	
#define CMD_WMODE			0x02		
#define CMD_WF_STAT		0x03		
#define CMD_RESET		  0x04
#define CMD_NET_CONF	0x05	
	
#define CMD_INPUT   	0x87
#define CMD_OUTPUT  	0x84	
	
#define CMD_QUERY			0x08
#define CMD_DATETIME	0x1C

#define ID_SWITCH			0x01
#define ID_CURRTEMP		0x1B
#define ID_WORKMODE		0x02
#define ID_CHILDLOCK	0x0C
#define ID_BRIGHT			0x0E
#define ID_CURPOWER		0x1D
#define	ID_REMTIME		0x14
#define ID_FAULT			0x13


#define ID_COMFORT		0x05
#define ID_ECO				0x03
#define ID_ANTIFR			0x1C
#define ID_LCDOFF			0x1F
#define ID_PROG				0x20
#define ID_SOUND			0x21
#define ID_HEATMODE		0x22
#define ID_OPENWINDOW	0x0D
#define ID_TIMER			0x23
#define ID_TIMERTIME	0x24


#define	ID_PRESETS		0x25
#define ID_CUSTOM_P	  0x27
#define ID_CUSTOM_H		0x26

uint8_t chksum8(const uint8_t *buff, size_t len);
//void receive_uart_int(void);
void usart_transmit_frame(const uint8_t *buff, size_t len);
void query_settings();	
void reset_wifi_state();
void query_faults();
void query_datetime();
	
#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
