#ifndef CLOCK_H
#define CLOCK_H

#include "stm32f0xx.h"
#include "rtc.h"
#include <stdbool.h>


//--| zmienne |--

#define BLINK_DELAY_TURN_OFF 60
volatile uint16_t timerBlink;	 //timer migania

#define BLINK_HOLD_OFF 100
volatile uint16_t timerBlinkHoldOff;//timer wstrzymania migania

#define TIME_NO_ACTION 1000
volatile uint16_t timerNoAction;    //timer czas bezczynności

uint8_t clockMode = 0;							//tryb ustawiania czas

typedef enum {tT_none=10 ,tT_time, tT_alarm, tT_date, tT_weekDay, tT_end}typeTime;
typeTime settingTypeTime = tT_none;		//wybór rodzaju czasu: godzina,alarm,data,dzień tygodnia

int8_t settingIndex;                        //wybór segmentu czasu 
const uint8_t maxSettingIndex[] = {3,3,3,1};//ilość elementów czasu



//--| funkcje |--

//funkcje wyświetlające
void displayClock(void);
void displayClock1(typeTime hideTime);
void displayClock2(typeTime hideTime);

//funkcja zmieniająca typ ustawianego czasu
void nextSettingTypeTime(bool reverse);

//funkcja ustawiająca tryb zegaru
void setClockMode(uint8_t nrMode);

//funkcja zmieniająca indeks
void nextSettingIndex(bool reverse);

//funkcja dodająca wartość do odpowiedniej części składowej czasu
void addValue(int8_t value);




#include "clock.c"
#endif