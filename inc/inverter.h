#ifndef __INVERTER_H
#define __INVERTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#include "Pixels.h" 
//=========================================================	DEBUG ==================================================
//#define DEBUG
//==================================================================================================================
#define SW									320
#define SH									240
	
	
#define VERSION "12.5.5"		
#define MAJOR_V 12
#define MINOR_V 5
#define DEBUG_V 5
	
#define	BALANCE_RESISTOR 		33000.0
#define	BETA								3950.0 
#define ROOM_TEMP  					298.15
#define RESISTOR_ROOM_TEMP 	5000.0

//#define	BALANCE_RESISTOR 		33000.0
//#define	BETA								3950.0 
//#define ROOM_TEMP  					298.15
//#define RESISTOR_ROOM_TEMP 	50000.0

#define SAVE_TIMEOUT 				1000
#define MIN_TEMP_COMFORT 		10
#define MAX_TEMP_COMFORT 		35

#define BG_COLOR		RGB(0x0, 0x0, 0x0)
#define MAIN_COLOR	RGB(0xFF, 0xFF, 0xFF)
#define GREEN_COLOR	RGB(0x13, 0xA5, 0x38)

#define FlashAddress 0x0801F800

#define PRODUCT_ID		24

enum StateBrightness
{
	StateBrightness_ON,
	StateBrightness_LOW,
	StateBrightness_OFF
};

enum WorkMode
{	  
    WorkMode_Comfort,
		WorkMode_Eco,
    WorkMode_Antifrost,
	  WorkMode_Off
		
};

enum HeatMode
{
    HeatMode_Auto,
    HeatMode_User
};

struct TemperatureSettings
{
    int8_t min;
    int8_t max;
    int8_t desired;
    uint8_t heatLevel;
};

struct PresetSettings
{
    int8_t week;
    int8_t preset;
};

struct OnOffSettings
{
    int8_t current;
    int8_t parameter;
};

enum Preset_mode
{
	pComfort, pEco, pAntiFrost, pOff
};

struct Presets
{
	uint8_t hour[24];
};

struct CalendarInfo
{
	char* week;
	uint16_t x;
	uint16_t y;
};
const struct CalendarInfo _calendarInfo[7] = 
{
	{"MO", 30, 20},
	{"TU", 100, 20},
	{"WE", 170, 20},
	{"TH", 240, 20},
	{"FR", 65, 110},
	{"SA", 135, 110},
	{"SU", 205, 110},
};

const struct Presets _presets[7] = {
	{ pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco },
	{ pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pEco, pEco, pEco, pComfort, pComfort, pComfort, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco },
	{ pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco },
	{ pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort },
	{ pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco },
	{ pEco, pEco, pEco, pEco, pEco, pEco, pEco, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pComfort, pEco, pEco, pEco },
	{ pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost, pAntiFrost }
};



struct DeviceSettings
{
	uint8_t on;
	uint8_t blocked;
	uint8_t tempComfort;
	uint8_t tempEco;
	uint8_t tempAntifrost;
	uint8_t calendarOn;
	uint8_t brightness;
	uint8_t soundOn;
	uint8_t displayAutoOff;
	uint8_t half_power;
	HeatMode heatMode;
	uint8_t powerLevel;
	WorkMode workMode;
	uint8_t modeOpenWindow;
	uint8_t calendar[7];
	struct Presets custom;
	uint8_t timerOn;
	uint16_t timerTime;
	char UDID[16];
	uint32_t crc;
};

bool isValidDate(int d, int m, int y);
void InitTimer();
void MenuBack();
void CustomNext();
void CustomPrev();
void TimeMinus();
void TimePlus();
void DateMinus();
void DatePlus();
void TempMinus();
void TempPlus();
void On();
void Off();
void DrawEditParameter(void);
void AcceptParameter(void);
void DrawMainScreen(uint32_t updater = 0);
void DrawTemperature(int8_t, int8_t xo = 0, int8_t yo = 0);
void DrawTextSelected(int16_t x, int16_t y, char* text, bool selected, bool underline, int16_t oX, int16_t oY);
void DrawTextAligment(int16_t x, int16_t y, int16_t w, int16_t h, char* text, bool selected, bool underline = false, uint8_t border = 0, RGB fore = MAIN_COLOR, RGB back = BG_COLOR , bool round = 1);
void DrawMenuText(const char *text);
void PrepareEditParameter();
void DrawMenu();
void DrawMenuTitle(const char *text, int8_t yo = 0);
void DrawMenuTitle2(const char *text);
void ResetAllSettings();
int8_t getModeTemperature();
int8_t getTemperature();
void loop();
void DrawWifi();
void DrawWindowOpen();
void open_window_func();
WorkMode getCalendarMode();
uint8_t xw09A_read_data(uint8_t button_num);
void SysTick_Handler_Callback();
void smooth_backlight(uint8_t mode);
void TIMER_Heat_callback();
void alarm_set(uint8_t minutes);
void rtc_alarm_callback();
void receive_uart_int(void);
uint8_t bcdToDec(uint8_t val);
uint8_t decToBcd(uint8_t val);
//void drawRoundRect(int16_t x, int16_t y, int16_t width, int16_t height, int16_t radius, int16_t thikness);
void refresh_mainscreen();
void CleanTemperature(int8_t temp, int8_t xo, int8_t yo);
void set_watchdog(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
