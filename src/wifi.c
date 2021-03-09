
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
extern bool window_is_opened;
uint8_t rxcount = 0;
uint8_t idle_count = 0;
uint32_t crc = 0, crc_calc = 0;
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

uint32_t calc_crc32(uint8_t * data, uint32_t size);
uint32_t ntohl(uint32_t net);

//uint8_t recv_buffer_compl[255];




uint32_t calc_crc32(uint8_t * data, uint32_t size)
{
	uint32_t convert_size = ((size % 4) ? size / 4 + 1 : size / 4) ;
	uint32_t *raw_data = new uint32_t[convert_size];
	uint32_t *data_to_convert = new uint32_t[convert_size];
	
	memset(data_to_convert, 0, 20);
	//memset(data_to_convert1, 0, 20);

	memcpy(raw_data, data, size);
	
	for(uint32_t i = 0; i < convert_size; i++)
	{
		data_to_convert[i] = ntohl(raw_data[i]);
	}
	crc_deinit();	
	uint32_t crc32_block = crc_block_data_calculate(data_to_convert, convert_size, INPUT_FORMAT_WORD);
	delete []data_to_convert;
	delete []raw_data;
	return crc32_block;
}

uint32_t ntohl(uint32_t net) 
{
    uint8_t data[4] = {};
    memcpy(&data, &net, sizeof(data));

    return ((uint32_t) data[3] << 0)
         | ((uint32_t) data[2] << 8)
         | ((uint32_t) data[1] << 16)
         | ((uint32_t) data[0] << 24);
}




void usart_transmit_frame(const uint8_t *buff, size_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		usart_data_transmit(USART1, buff[i]);
		while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
	}
}



		uint16_t payload_len = 0;
		uint8_t  frame_dir   	= 0;
		uint16_t device_cmd = 0;
		uint32_t msq_num = 0;
		uint8_t  cmd_ver			= 0;


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

    uint16_t pointer = 0;

		crc_calc = 0;
		
		while(pointer != rxcount)
		{
			if(recv_buffer[pointer] == HEADER_1B && recv_buffer[pointer+1] == HEADER_2B) // FIND HEADER ACBD
			{
				
				cmd_ver 		= 	recv_buffer[pointer + 2];
				msq_num			=	((recv_buffer[pointer + 3]      << 24) | (recv_buffer[pointer + 4]      << 16) | (recv_buffer[pointer + 5]     << 8)  | recv_buffer[pointer + 6]);
				device_cmd  = ( recv_buffer[pointer + 7]      << 8)  +  recv_buffer[pointer + 8];
				frame_dir   =   recv_buffer[pointer + 9];
				payload_len = ((recv_buffer[pointer + 10]     << 8)  |  recv_buffer[pointer + 11]);
				crc = 				((recv_buffer[pointer + payload_len + 12] << 24) | (recv_buffer[pointer + payload_len + 13] << 16) | (recv_buffer[pointer + payload_len + 14] << 8) | recv_buffer[pointer + payload_len + 15]);
				uint8_t *payload = new uint8_t[payload_len];
				memcpy(payload, &recv_buffer[pointer + 12], payload_len);
				
				uint8_t *raw_data = new uint8_t[payload_len + 12];
				memcpy(raw_data, &recv_buffer[pointer], payload_len + 12);
/*
				for(uint8_t ii = 0; ii < (payload_len + 16); ii++)
				{
					frame[ii] = recv_buffer[pointer + ii];								
				}
*/
				crc_calc = calc_crc32(raw_data, payload_len + 12);
				
				if(crc == crc_calc)
				{
					if(device_cmd == ID_QUERY)
					{ 
						query_settings();						
					}
					else
					{
						answer_frame.clear();
						answer_frame.reset();
						answer_frame.put(HEADER_1B);
						answer_frame.put(HEADER_2B);
						answer_frame.put(cmd_ver);
						answer_frame.put(msq_num>>24);
						answer_frame.put(msq_num>>16);
						answer_frame.put(msq_num>>8);
						answer_frame.put(msq_num);
						answer_frame.put(device_cmd>>8);
						answer_frame.put(device_cmd);					
						answer_frame.put(CMD_OUTPUT);
						answer_frame.put(payload_len>>8);
						answer_frame.put(payload_len);
						answer_frame.put_str(payload, payload_len);
						answer_frame.put32(crc_calc);
						
						usart_transmit_frame(answer_frame.sptr(), answer_frame.count());
						
						
						if(device_cmd == ID_SWITCH)
						{
							uint8_t switch_device = payload[0];
							
							if(_settings.on != switch_device)
							{
								_settings.on = switch_device;
								
								if (_settings.on)
									deviceON();
								else
									deviceOFF();
							}
						}
						
						if(device_cmd == ID_WORKMODE)
						{
							uint8_t workmode = payload[0];
							
							_settings.workMode = (WorkMode)workmode;
							DrawMainScreen();
							refresh_system = true;
						}			
						
						if(device_cmd == ID_CHILDLOCK)
						{
							uint8_t childlock = payload[0];
							
							if(_settings.blocked != childlock)
							{
								_settings.blocked = childlock;
							}			
						}	
						
						if(device_cmd == ID_BRIGHT)
						{
							uint8_t bright = payload[0];
							if(_settings.brightness != bright)
							{
								_settings.brightness = bright;
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
							uint8_t temp = payload[0];
							if(_settings.workMode == WorkMode_Eco) CleanTemperature(_settings.tempComfort - _settings.tempEco,0,15);	
							if(_settings.workMode == WorkMode_Comfort) CleanTemperature(_settings.tempComfort,0,15);	
							_settings.tempComfort = temp;
							//if(_settings.workMode == WorkMode_Comfort) DrawTemperature(_settings.tempComfort,0,15);
							//if(_settings.workMode == WorkMode_Eco) DrawTemperature(_settings.tempComfort - _settings.tempEco,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}	

						if(device_cmd == ID_ECO)
						{
							uint8_t temp = payload[0];
							if(_settings.workMode == WorkMode_Eco) CleanTemperature(_settings.tempComfort - _settings.tempEco,0,15);	
							_settings.tempEco = temp;
							//if(_settings.workMode == WorkMode_Eco) DrawTemperature(_settings.tempComfort - _settings.tempEco,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}

						if(device_cmd == ID_ANTIFR)
						{
							uint8_t temp = payload[0];
							if(_settings.workMode == WorkMode_Antifrost) CleanTemperature(_settings.tempAntifrost,0,15);	
							_settings.tempAntifrost = temp;
							//if(_settings.workMode == WorkMode_Antifrost) DrawTemperature(_settings.tempAntifrost,0,15);
							DrawMainScreen(1);
							refresh_system = true;
						}		

						if(device_cmd == ID_LCDOFF)
						{							
							uint8_t lcdoff = payload[0];
							if(_settings.displayAutoOff != lcdoff)
							{
								_settings.displayAutoOff = lcdoff;
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
							uint8_t sound_sw = payload[0];
							if(_settings.soundOn != sound_sw)
							{
								_settings.soundOn = sound_sw;
							}			
						}
						
						if(device_cmd == ID_HEATMODE)
						{			
							uint8_t heat_mode = payload[0];
							if(_settings.heatMode != heat_mode)
							{
								_settings.heatMode = (HeatMode)heat_mode;
								DrawMainScreen();
								refresh_system = true;								
							}			
						}		
						
						if(device_cmd == ID_OPENWINDOW)
						{
							uint8_t open_window = payload[0];
							if(_settings.modeOpenWindow != open_window)
							{
								_settings.modeOpenWindow = open_window;
								DrawMainScreen();
								refresh_system = true;								
							}			
						}

						if(device_cmd == ID_TIMER)
						{
							uint8_t timer_sw = payload[0];
							if(_settings.timerOn != timer_sw)
							{
								_settings.timerOn = timer_sw;
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
							//query_settings();
						}			
						
						if(device_cmd == ID_PROG)
						{
							uint8_t prog_sw = payload[0];
							if(_settings.calendarOn != prog_sw)
							{
								_settings.calendarOn = prog_sw;
								if(_settings.calendarOn)
								{
									_settings.heatMode = HeatMode_Auto;
									_settings.timerOn = 0;
									_eventTimer = 0;
									InitTimer();
								}
								else
								{
									_settings.workMode = WorkMode_Comfort;
								}
								
								DrawMainScreen();
								refresh_system = true;
							}		
							query_settings();
						}				
						
						if(device_cmd == ID_TIMERTIME)
						{
							uint16_t temp_timer_time = (payload[0] << 8) + payload[1];
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
							uint8_t custom_p = payload[0];
							_settings.powerLevel = custom_p;
							refresh_system = true;
						}	
						
						if(device_cmd == ID_PRESETS)
						{
							memcpy(&_settings.calendar, &payload[0], 7);
							refresh_system = true;
						}	
						
						if(device_cmd == ID_CUSTOM_H)
						{
							memcpy(&_settings.custom, &payload[0], 24);
							refresh_system = true;
						}				
						
						if(device_cmd == ID_DATETIME)
						{
							rtc_deinit();
							rtc_parameter_struct rtc_from_module;
							
							rtc_from_module.rtc_year = decToBcd(payload[5]);
							rtc_from_module.rtc_month = decToBcd(payload[4]);
							rtc_from_module.rtc_date = decToBcd(payload[3]);
							rtc_from_module.rtc_hour = decToBcd(payload[2]);
							rtc_from_module.rtc_minute = decToBcd(payload[1]);
							rtc_from_module.rtc_second = decToBcd(payload[0]);
							rtc_from_module.rtc_day_of_week = payload[6];
							
							rtc_from_module.rtc_factor_asyn = 0x7FU;
							rtc_from_module.rtc_factor_syn = 0xFFU;
							rtc_from_module.rtc_display_format = RTC_24HOUR;	
							
							rtc_init(&rtc_from_module);
						}						
						
						if(device_cmd == ID_RESET)
						{							
							ResetAllSettings();
							SaveFlash();
							NVIC_SystemReset();
						}

						if(device_cmd == ID_WIFI)
						{							
							wifi_status = payload[0];
						}						
						
						refresh_mainscreen();
					}
				}
				delete []payload;	
				delete []raw_data;
			}
			pointer++;
		}
		//--------------
		rxcount = 0;
		//i = 0;
		idle_flag_stat = 0;
		//_timeoutSaveFlash = GetSystemTick() + SAVE_TIMEOUT;
	}
}

void reset_wifi_state()
{
	/*
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
		*/
}
/*
void query_datetime()
{
	answer_frame.clear();
	answer_frame.reset();
	answer_frame.put(HEADER_1B);
	answer_frame.put(HEADER_2B);
	answer_frame.put(HEADER_VER);
	answer_frame.put(CMD_DATETIME);
	answer_frame.put(0x00);
	answer_frame.put(0x00);	
	answer_frame.put(chksum8(answer_frame.sptr(), 6));
	usart_transmit_frame(answer_frame.sptr(), 7);
}
*/
void query_faults()
{
	/*
	answer_frame.clear();
	answer_frame.reset();
	answer_frame.put(HEADER_1B);
	answer_frame.put(HEADER_2B);
	answer_frame.put(cmd_ver);
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(0);
	answer_frame.put(ID_FAULT);					
	answer_frame.put(CMD_OUTPUT);
	answer_frame.put(0x00);
	answer_frame.put(0x01);
	answer_frame.put((_error == 1 ? 0x01 : 0x00) |
	                 (_error == 2 ? 0x02 : 0x00) |
	                 (_error == 3 ? 0x04 : 0x00) |
	                 (_error == 4 ? 0x08 : 0x00) );
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
	*/
}

void query_settings()
{
	
	answer_frame.clear();
	answer_frame.reset();
	answer_frame.put(HEADER_1B);
	answer_frame.put(HEADER_2B);
	answer_frame.put(cmd_ver);
	//msq_num++;
	answer_frame.put(msq_num>>24);
	answer_frame.put(msq_num>>16);
	answer_frame.put(msq_num>>8);
	answer_frame.put(msq_num);
	answer_frame.put(device_cmd>>8);
	answer_frame.put(device_cmd);					
	answer_frame.put(CMD_OUTPUT);
	answer_frame.put(0x00);
	answer_frame.put(0x6E);
	
	//switch
	answer_frame.put(ID_SWITCH);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.on);
	//Current temperature
	answer_frame.put(ID_CURRTEMP);	
	answer_frame.put(0);
	answer_frame.put(2);
	answer_frame.put(getTemperature()<0 ? 0xFF : 0x00);
	answer_frame.put(getTemperature());
	//Working mode
	answer_frame.put(ID_WORKMODE);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put((uint8_t)_settings.workMode);
	//Child lock
	answer_frame.put(ID_CHILDLOCK);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.blocked);	
	//Brightness
	answer_frame.put(ID_BRIGHT);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.brightness);	
	//Current power
	answer_frame.put(ID_CURPOWER);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(power_level_auto);
	//Remining time
	answer_frame.put(ID_REMTIME);
	answer_frame.put(0);	
	answer_frame.put(2);
	answer_frame.put(timer_time_set >> 8);
	answer_frame.put(timer_time_set);	
	//Fault
	answer_frame.put(ID_FAULT);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put((_error == 1 ? 0x01 : 0x00) |
	                 (_error == 2 ? 0x02 : 0x00) |
	                 (_error == 3 ? 0x04 : 0x00) |
	                 (_error == 4 ? 0x08 : 0x00) );
									 	
	//Schedule
	answer_frame.put(ID_PRESETS);
	answer_frame.put(0);
	answer_frame.put(0x07);
	answer_frame.put_str((uint8_t*)_settings.calendar,7);
	//Comfort temperature
	answer_frame.put(ID_COMFORT);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.tempComfort);	
	//ECO temperature
	answer_frame.put(ID_ECO);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.tempEco);		
	//Antifrost temperature
	answer_frame.put(ID_ANTIFR);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.tempAntifrost);	
	//Auto lcd off
	answer_frame.put(ID_LCDOFF);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.displayAutoOff);		
	//Programm
	answer_frame.put(ID_PROG);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.calendarOn);		
	//Sound
	answer_frame.put(ID_SOUND);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.soundOn);
	//Heat mode
	answer_frame.put(ID_HEATMODE);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.heatMode);		
	//Open window mode
	answer_frame.put(ID_OPENWINDOW);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.modeOpenWindow);
	//Timer
	answer_frame.put(ID_TIMER);
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.timerOn);
	//Timertime
	answer_frame.put(ID_TIMERTIME);
	answer_frame.put(0);	
	answer_frame.put(2);
	answer_frame.put(_settings.timerTime >> 8);
	answer_frame.put(_settings.timerTime);
	//custom power level
	answer_frame.put(ID_CUSTOM_P);	
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(_settings.powerLevel);
  //date/time
	answer_frame.put(ID_DATETIME);	
	answer_frame.put(0);
	answer_frame.put(7);

	rtc_parameter_struct rtc_from_module;
	rtc_current_time_get(&rtc_from_module); 
	
	answer_frame.put(bcdToDec(rtc_from_module.rtc_second));
	answer_frame.put(bcdToDec(rtc_from_module.rtc_minute));
	answer_frame.put(bcdToDec(rtc_from_module.rtc_hour));
	answer_frame.put(bcdToDec(rtc_from_module.rtc_date));
	answer_frame.put(bcdToDec(rtc_from_module.rtc_month));
	answer_frame.put(bcdToDec(rtc_from_module.rtc_year));
	answer_frame.put(rtc_from_module.rtc_day_of_week);
	//udid
	answer_frame.put(ID_UDID);	
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(PRODUCT_ID);
	//open window detect flag
	answer_frame.put(ID_DET_OW);	
	answer_frame.put(0);
	answer_frame.put(1);
	answer_frame.put(window_is_opened);
  //Firmware	
	answer_frame.put(ID_FIRMWARE);	
	answer_frame.put(0);
	answer_frame.put(4);
	answer_frame.put(0);
  answer_frame.put(MAJOR_V);	
	answer_frame.put(MINOR_V);
	answer_frame.put(DEBUG_V);  
	
	//crc32
	crc_calc = calc_crc32(answer_frame.sptr(), answer_frame.count());
	answer_frame.put32(crc_calc);
	usart_transmit_frame(answer_frame.sptr(), answer_frame.count());
	
}
