
#include "wifi.h"
#include "string.h"
#include "inverter.h"
#include "stream.h"

extern void SaveFlash();
extern void InitTimer();
extern void DrawMainScreen(uint32_t updater);
extern void deviceOFF();
extern void deviceON();

extern bool refresh_system;
extern struct DeviceSettings _settings;
extern uint8_t power_level_auto;	
extern uint16_t timer_time_set;
extern uint8_t _eventTimer;
extern StateBrightness _stateBrightness;
extern uint32_t idleTimeout;
extern uint32_t _timeoutSaveFlash;
extern uint8_t _error;
uint8_t rxcount = 0;
uint8_t idle_count = 0;
uint8_t crc = 0;
uint8_t answer_cmd[50] = {0x55, 0xAA, 0x03};
uint8_t answer_cmd1[50] = {0x55, 0xAA, 0x03};
uint8_t prod_info[] = "{\"p\":\"xr6jsgylldbpkaz9\",\"v\":\"1.1.1\",\"m\":0}";
//uint8_t prod_info[] = "{\"p\":\"6bbwxfx9leraqht1\",\"v\":\"1.1.1\",\"m\":0}";
//uint8_t prod_info[] = "{\"p\":\"jxe9szwafgw47a4h\",\"v\":\"1.1.1\",\"m\":0}";
extern uint8_t wifi_status;
uint8_t idle_flag_stat = 0;	
uint8_t recv_buffer[200];
void receive_uart_int(void);
uint8_t answer_out[300];
Stream answer_frame(answer_out, 300);
//uint8_t recv_buffer_compl[255];

uint8_t chksum8(const uint8_t *buff, size_t len)
{
    uint8_t sum;       
    for ( sum = 0 ; len != 0 ; len-- )
        sum += *(buff++); 
		
    return sum;
}

void usart_transmit_frame(const uint8_t *buff, size_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		usart_data_transmit(USART1, buff[i]);
		while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
	}
}

void receive_uart_int()
{
	
	//static uint8_t i = 0;
  //static uint8_t recv_counter = 0;
	/*
	if(idle_flag_stat==0)
	{
		recv_buffer[i++] = usart_data_receive(USART1);
	}*/
	if(idle_flag_stat)
	{
		/*
		uint8_t j = 0;
		
		for(;recv_counter <= 255;recv_counter++)
		{
			if(j == i) break;
			recv_buffer_compl[recv_counter] = recv_buffer[j++];
		}
		//str_len_info = strlen(prod_info);
		*/
		// PARSER
		uint8_t  pointer     = 0;
		uint16_t payload_len = 0;
		uint8_t  frame_cmd   = 0;
		uint8_t  device_cmd  = 0;
		while(pointer != rxcount/*recv_counter*/)
		{
			if(recv_buffer[pointer] == HEADER_1B && recv_buffer[pointer+1] == HEADER_2B) // FIND HEADER 55AA
			{
				//pointer += HEADER_LEN;
				//pointer++;
				frame_cmd = recv_buffer[pointer+3];
				if(frame_cmd  == CMD_INPUT)
				{
					device_cmd = recv_buffer[pointer+6];
				}
				payload_len = ((recv_buffer[pointer+4] << 8) | recv_buffer[pointer+5])  ;
				
				uint8_t *frame = new uint8_t[payload_len + 7];
				
				for(uint8_t ii = 0; ii < (payload_len + 7); ii++)
				{
					frame[ii] = recv_buffer[pointer + ii];								
				}
				
				crc = chksum8(frame, payload_len + 6);
				
				if(crc == frame[payload_len + 6])
				{
					answer_frame.clear();
					answer_frame.reset();
					answer_frame.put(HEADER_1B);
					answer_frame.put(HEADER_2B);
					answer_frame.put(HEADER_VER);
					
					
					if(frame_cmd == CMD_HB)
					{
						answer_frame.put(CMD_HB);
						answer_frame.put(0x00);
						answer_frame.put(0x01);
						answer_frame.put(HEARTBEAT);
						answer_frame.put(chksum8(answer_frame.sptr(), 7));
						usart_transmit_frame(answer_frame.sptr(), 8);
						//query_settings();

					}
					if(frame_cmd == CMD_INFO)
					{
						answer_frame.put(CMD_INFO);
						answer_frame.put(0x00);
						answer_frame.put(0x2A);
						answer_frame.put_str(prod_info,0x2A);
						answer_frame.put(chksum8(answer_frame.sptr(), 0x2A+6));
						usart_transmit_frame(answer_frame.sptr(), 0x2A+7);		
					}
					if(frame_cmd == CMD_WMODE)
					{
						answer_frame.put(CMD_WMODE);
						answer_frame.put(0x00);
						answer_frame.put(0x00);
						answer_frame.put(chksum8(answer_frame.sptr(), 6));
						usart_transmit_frame(answer_frame.sptr(), 7);
					}			
					if(frame_cmd == CMD_WF_STAT)
					{
						wifi_status = frame[6];
						answer_frame.put(CMD_WF_STAT);
						answer_frame.put(0x00);
						answer_frame.put(0x00);
						answer_frame.put(chksum8(answer_frame.sptr(), 6));
						usart_transmit_frame(answer_frame.sptr(), 7);
/*
						answer_cmd[3] = CMD_NET_CONF;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x01;
						answer_cmd[6] = 0x00;
						answer_cmd[7] = chksum8(answer_cmd, 7);
						usart_transmit_frame(answer_cmd, 8);		

						answer_cmd[3] = CMD_RESET;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x00;
						//memmove(answer_cmd+6, prod_info, 0x2A);
						//answer_cmd[6] = 0x00;
						answer_cmd[6] = chksum8(answer_cmd, 6);
						usart_transmit_frame(answer_cmd, 7);	*/			
					}			
					if(frame_cmd == CMD_QUERY)
					{ 
						query_settings();				
					}
					
					if(frame_cmd == CMD_INPUT)
					{
						
						if(device_cmd == ID_SWITCH)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_SWITCH);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
	
							if(_settings.on != frame[10])
							{
								_settings.on = frame[10];
								
								if (_settings.on)
									deviceON();
								else
									deviceOFF();
							}
					
						}
						if(device_cmd == ID_WORKMODE)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_WORKMODE);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							
							_settings.workMode = (WorkMode)frame[10];
							DrawMainScreen();
							refresh_system = true;
						}			
						
						if(device_cmd == ID_CHILDLOCK)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_CHILDLOCK);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.blocked != frame[10])
							{
								_settings.blocked = frame[10];
							}			
						}	
						
						if(device_cmd == ID_BRIGHT)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_BRIGHT);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.brightness != frame[10])
							{
								_settings.brightness = frame[10];
								if (_settings.brightness)
								{
									_stateBrightness = StateBrightness_ON;
								}
								else
								{
									_stateBrightness = StateBrightness_LOW;
								}
								//if(!_settings.displayAutoOff)
								{
									smooth_backlight(1);
							  }
							}			
						}		
						
						if(device_cmd == ID_COMFORT)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_COMFORT);
							answer_frame.put(2);
							answer_frame.put(0);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(frame[13]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							
							if(_settings.workMode == WorkMode_Eco) CleanTemperature(_settings.tempComfort - _settings.tempEco,0,15);	
							if(_settings.workMode == WorkMode_Comfort) CleanTemperature(_settings.tempComfort,0,15);	
							_settings.tempComfort = frame[13];
							//if(_settings.workMode == WorkMode_Comfort) DrawTemperature(_settings.tempComfort,0,15);
							//if(_settings.workMode == WorkMode_Eco) DrawTemperature(_settings.tempComfort - _settings.tempEco,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}	

						if(device_cmd == ID_ECO)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_ECO);
							answer_frame.put(2);
							answer_frame.put(0);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(frame[13]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							
							if(_settings.workMode == WorkMode_Eco) CleanTemperature(_settings.tempComfort - _settings.tempEco,0,15);	
							_settings.tempEco = frame[13];
							//if(_settings.workMode == WorkMode_Eco) DrawTemperature(_settings.tempComfort - _settings.tempEco,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}

						if(device_cmd == ID_ANTIFR)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_ANTIFR);
							answer_frame.put(2);
							answer_frame.put(0);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(frame[13]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
								
							if(_settings.workMode == WorkMode_Antifrost) CleanTemperature(_settings.tempAntifrost,0,15);	
							_settings.tempAntifrost = frame[13];
							//if(_settings.workMode == WorkMode_Antifrost) DrawTemperature(_settings.tempAntifrost,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}		

						if(device_cmd == ID_LCDOFF)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_LCDOFF);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.displayAutoOff != frame[10])
							{
								_settings.displayAutoOff = frame[10];
								if(!_settings.displayAutoOff)
								{
									if (_settings.brightness)
									{
										_stateBrightness = StateBrightness_ON;
									}
									else
									{
										_stateBrightness = StateBrightness_LOW;
									}
									smooth_backlight(1);
								}		
               idleTimeout = GetSystemTick(); 								
							}			
						}

				

						if(device_cmd == ID_SOUND)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_SOUND);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.soundOn != frame[10])
							{
								_settings.soundOn = frame[10];
							}			
						}
						
						if(device_cmd == ID_HEATMODE)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_HEATMODE);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.heatMode != frame[10])
							{
								_settings.heatMode = (HeatMode)frame[10];
								DrawMainScreen();
							  refresh_system = true;								
							}			
						}		
						
						if(device_cmd == ID_OPENWINDOW)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_OPENWINDOW);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.modeOpenWindow != frame[10])
							{
								_settings.modeOpenWindow = frame[10];
								DrawMainScreen();
							  refresh_system = true;								
							}			
						}

						if(device_cmd == ID_TIMER)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_TIMER);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.timerOn != frame[10])
							{
								_settings.timerOn = frame[10];
								if(_settings.timerOn)
								{
									if((getCalendarMode() == WorkMode_Off))
									{
										_settings.workMode = WorkMode_Comfort;
									}
									_settings.calendarOn = 0;
									_eventTimer = 0;
									InitTimer();
								}									
								DrawMainScreen();
							  refresh_system = true;								
							}			
							query_settings();
						}			
						
						if(device_cmd == ID_PROG)
						{
							
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_PROG);
							answer_frame.put(1);
							answer_frame.put(0);
							answer_frame.put(1);
							answer_frame.put(frame[10]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);
							
							if(_settings.calendarOn != frame[10])
							{
								_settings.calendarOn = frame[10];
								if(_settings.calendarOn)
								{
									_settings.heatMode = HeatMode_Auto;
									_settings.timerOn = 0;
									_eventTimer = 0;
									InitTimer();
								}							
								
								DrawMainScreen();
							  refresh_system = true;
							}		
							query_settings();
						}				
						
						if(device_cmd == ID_TIMERTIME)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_TIMERTIME);
							answer_frame.put(2);
							answer_frame.put(0);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(frame[12]);
							answer_frame.put(frame[13]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							
							uint16_t temp_timer_time = (frame[12] << 8) + frame[13];
							if(temp_timer_time <= 1439)
							{
								if((timer_time_set != temp_timer_time) &&(_settings.timerOn))
								{
									_settings.timerTime = temp_timer_time;
									timer_time_set = _settings.timerTime;
								}	
								else if(_settings.timerTime != temp_timer_time)
								{
									_settings.timerTime = temp_timer_time;
									timer_time_set = _settings.timerTime;
								}
							}				
						}
						if(device_cmd == ID_CUSTOM_P)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_CUSTOM_P);
							answer_frame.put(2);
							answer_frame.put(0);
							answer_frame.put(4);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(frame[13]);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							
							_settings.powerLevel = frame[13];
							refresh_system = true;
						}	
						if(device_cmd == ID_SCHEDULE)
						{
							answer_frame.put(CMD_OUTPUT);
							answer_frame.put(frame[4]);
							answer_frame.put(frame[5]);
							answer_frame.put(ID_SCHEDULE);
							answer_frame.put(0);
							answer_frame.put(0);
							answer_frame.put(0xA8);
							answer_frame.put_str(&frame[10],168);
							answer_frame.put(chksum8(answer_frame.sptr(), payload_len+6));
							usart_transmit_frame(answer_frame.sptr(), payload_len+7);	
							memcpy(&_settings.week_schedule, frame+10, 168);
							refresh_system = true;
						}	
					}
				}
				delete []frame;	
			}
			pointer++;
		}
		//--------------
		rxcount = 0;
		//i = 0;
		idle_flag_stat = 0;
		_timeoutSaveFlash = GetSystemTick() + SAVE_TIMEOUT;
	}
}

void reset_wifi_state()
{
		answer_frame.clear();
		answer_frame.reset();
		answer_frame.put(HEADER_1B);
		answer_frame.put(HEADER_2B);
		answer_frame.put(HEADER_VER);
		answer_frame.put(CMD_NET_CONF);
		answer_frame.put(0x00);
		answer_frame.put(0x01);
	  answer_frame.put(0x00);
		answer_frame.put(chksum8(answer_frame.sptr(), 7));
		usart_transmit_frame(answer_frame.sptr(), 8);	
		delay_1ms(10);
		answer_frame.clear();
		answer_frame.reset();
		answer_frame.put(HEADER_1B);
		answer_frame.put(HEADER_2B);
		answer_frame.put(HEADER_VER);
		answer_frame.put(CMD_RESET);
		answer_frame.put(0x00);
		answer_frame.put(0x00);
		answer_frame.put(chksum8(answer_frame.sptr(), 6));
		usart_transmit_frame(answer_frame.sptr(), 7);
}


void query_faults()
{
	answer_frame.clear();
	answer_frame.reset();
	answer_frame.put(HEADER_1B);
	answer_frame.put(HEADER_2B);
	answer_frame.put(HEADER_VER);
	answer_frame.put(CMD_OUTPUT);
	answer_frame.put(0x00);
	answer_frame.put(0x0D);
	//Fault
	answer_frame.put(ID_FAULT);
	answer_frame.put(5);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(1);/*
	answer_frame.put((_error == 1 ? 0x01 : 0x00) |
	                 (_error == 2 ? 0x04 : 0x00) |
	                 (_error == 3 ? 0x05 : 0x00) |
	                 (_error == 4 ? 0x08 : 0x00) );*/
		//Current power
	answer_frame.put(ID_CURPOWER);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(power_level_auto);
	
	answer_frame.put(chksum8(answer_frame.sptr(),13+6));//98
	usart_transmit_frame(answer_frame.sptr(), 13+7);
}

void query_settings()
{
	answer_frame.clear();
	answer_frame.reset();
	answer_frame.put(HEADER_1B);
	answer_frame.put(HEADER_2B);
	answer_frame.put(HEADER_VER);
	answer_frame.put(CMD_OUTPUT);
	answer_frame.put(0x01);
	answer_frame.put(0x23);
	//switch
	answer_frame.put(ID_SWITCH);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.on);
	//Current temperature
	answer_frame.put(ID_CURRTEMP);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put((uint8_t)getTemperature());
	//Working mode
	answer_frame.put(ID_WORKMODE);
	answer_frame.put(4);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put((uint8_t)_settings.workMode);
	//Child lock
	answer_frame.put(ID_CHILDLOCK);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.blocked);	
	//Brightness
	answer_frame.put(ID_BRIGHT);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.brightness);	
	//Current power
	answer_frame.put(ID_CURPOWER);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(power_level_auto);
	//Remining time
	answer_frame.put(ID_REMTIME);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(timer_time_set >> 8);
	answer_frame.put(timer_time_set);	
	//Fault
	answer_frame.put(ID_FAULT);
	answer_frame.put(5);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put((_error == 1 ? 0x01 : 0x00) |
	                 (_error == 2 ? 0x04 : 0x00) |
	                 (_error == 3 ? 0x05 : 0x00) |
	                 (_error == 4 ? 0x08 : 0x00) );
									 	
	//Schedule
	answer_frame.put(ID_SCHEDULE);
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(0xA8);
	answer_frame.put_str((uint8_t*)_settings.week_schedule,168);
	//Comfort temperature
	answer_frame.put(ID_COMFORT);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(_settings.tempComfort);	
	//ECO temperature
	answer_frame.put(ID_ECO);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(_settings.tempEco);		
	//Antifrost temperature
	answer_frame.put(ID_ANTIFR);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(_settings.tempAntifrost);	
	//Auto lcd off
	answer_frame.put(ID_LCDOFF);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.displayAutoOff);		
	//Programm
	answer_frame.put(ID_PROG);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.calendarOn);		
	//Sound
	answer_frame.put(ID_SOUND);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.soundOn);
	//Heat mode
	answer_frame.put(ID_HEATMODE);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.heatMode);		
	//Open window mode
	answer_frame.put(ID_OPENWINDOW);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.modeOpenWindow);
	//Timer
	answer_frame.put(ID_TIMER);
	answer_frame.put(1);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.timerOn);
	//Timertime
	answer_frame.put(ID_TIMERTIME);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(timer_time_set >> 8);
	answer_frame.put(timer_time_set);
	//custom power level
	answer_frame.put(ID_CUSTOM_P);
	answer_frame.put(2);
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);	
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(_settings.powerLevel);

	
	answer_frame.put(chksum8(answer_frame.sptr(),291+6));//98
	usart_transmit_frame(answer_frame.sptr(), 291+7);
}
