#ifndef __ClickButton_
#define __ClickButton_

#include "main.h"

#define BUTTON_PRESSED 				0x01
#define BUTTON_LONGPRESSED		0x02
#define BUTTON_LONGCLICK			0x04
extern uint8_t _error;
extern uint8_t btn_buff[2];
class ClickButton
{
	public:
		ClickButton(uint8_t button_num, uint16_t longClickTime = 2000, bool clickIsLong = false)
		{
			_button_number = button_num;
			_lastBounceTime = 0;
			_lastState = false;
			_btnState = false;
			_debounceTime = 10;
			_longClickTime = longClickTime;
			_action = 0;

			_pressed = false;
			_longressed = false;
			_clickIsLong = clickIsLong;
			_clicks = 0;
		};
		
		inline void setLongClickTime(uint16_t time)
		{
			_longClickTime = time;
		}
		
		inline bool getState()
		{
			return _btnState;
		}
		
		bool isLongPressed()
		{
			return _longressed;
		}
		
		bool isPressed()
		{
			return _pressed;
		}

		bool getPressed()
		{
			if (_pressed && (_action & BUTTON_PRESSED) )
			{
				_action &= ~BUTTON_PRESSED;
				return true;
			}
			
			return false;
		}

		bool getLongPressed()
		{
			if (_pressed && (_action & BUTTON_LONGPRESSED) )
			{
				_action &= ~BUTTON_LONGPRESSED;
				return true;
			}

			return false;
		}
		/*
		void init()
		{
			LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
			GPIO_InitStruct.Pin = _PinMask;
			GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
			LL_GPIO_Init(_GPIOx, &GPIO_InitStruct);
		}
		*/
		void update()
		{
			_btnState = xw09A_read_data(_button_number);
			
			
			// If the switch changed, due to noise or a button press, reset the debounce timer
			if (_btnState != _lastState)
			{
				_longressed = false;
				_lastBounceTime = GetSystemTick();
				_lastPressed = GetSystemTick();
			}
			
			// debounce the button (Check if a stable, changed state has occured)
			if (GetSystemTick() - _lastBounceTime > _debounceTime && _btnState != _pressed)
			{
				_pressed = _btnState;
				
				if (_pressed) 
				{
					_clicks = 1;
					_action |= BUTTON_PRESSED;
				}
			}
			
			// Check for "long click"
			if (_longClickTime > 0 && !_longressed && _pressed && (GetSystemTick() - _lastBounceTime > _longClickTime))
			{
				_longressed = true;
				_action |= BUTTON_LONGPRESSED;
			}
			
			if (_clickIsLong && _longressed)
			{
				int32_t timeout = 500;
				if (_clicks > 14)
					timeout = 100;
				if (_clicks > 4)
					timeout = 250;
				
				if (GetSystemTick() - _lastBounceTime > timeout)
				{
					_lastBounceTime = GetSystemTick();
					_action |= BUTTON_PRESSED;
					_clicks++;
				}
			}

			_lastState = _btnState;
		}
		
		inline uint32_t duration()
		{
			return GetSystemTick() - _lastPressed;
		}
	
	protected:
		uint8_t _button_number;
   	uint32_t _lastBounceTime;
		uint32_t _lastPressed;
		bool _pressed;
		bool _longressed;
		bool _lastState;
		bool _btnState;
		bool _clickIsLong;
		uint16_t _debounceTime;
		uint16_t _longClickTime;
		uint8_t _action;
		uint32_t _clicks;
};


#endif
