#ifndef RTC_H   //zabezpieczenie przed podwójną kompilacją
#define RTC_H

//Biblioteki
#include "stm32f0xx.h"
#include "lcd_stm0.h"

//---| Zmienne |---
volatile uint8_t f_rtc; //flaga przerwania

int8_t hourBCD;     //czas w kodzie BCD
int8_t minuteBCD;
int8_t secondBCD;

int8_t yearBCD;     //data w kodzie BCD 
int8_t monthBCD;
int8_t dayBCD;

int8_t weekDay;     //dzień tygodnia

int8_t alarm_hourBCD;   //czas alarmu w kodzie BCD
int8_t alarm_minuteBCD;

extern volatile uint32_t timeProgram;	//niepotrzebna
//---| Funkcje |---

//funkcje główne
void RTC_init(void);        //inicjalizacja zegara
void RTC_alarm_init(void);  //inicjalizacj alarmu i przerwania zegar
void RTC_IRQHandler(void);  //funkcja przerwania

//funkcje włączające i wyłączające zabezpieczenie.
void RTC_WriteProtectionDisable(void);
void RTC_WriteProtectionEnable(void);
void RTC_Alarm_WriteProtectionDisable(void);
void RTC_Alarm_WriteProtectionEnable(void);

//funkcje odczytujące czas
void updateDateTime(void);
void updateAlarm(void);

//funkcje kodujące i dekodujące
uint8_t bcdToBinary(uint8_t bcd);
uint8_t binaryToBcd(uint8_t binary);

//funkcje doyczące alarmu
uint8_t getAlarmMode(void);
void setAlarmMode(uint8_t mode);
uint8_t chechAlarm(void);

//funkcje wyświetlające czas
void displayTime(uint8_t hideIndex);
void displayDate(uint8_t hideIndex);
void displayWeekDay(uint8_t hideIndex);
void displayAlarm(uint8_t hideIndex);

//funkcje modyfikujące czas
void addTimeValue(uint8_t index,int8_t value);
void addDateValue(uint8_t index,int8_t value);
void addAlarmValue(uint8_t index,int8_t value);
void addWeekDayValue(int8_t value);

//funkcja korygująca dzień względem miesiąca
void correctDay(void);

#include "rtc.c" //dołączanie pliku c ???
#endif








