#include "rtc.h"
#ifdef RTC_H

/*
volatile uint8_t f_rtc=0;
uint8_t f_rtc_alarm;
volatile uint16_t l_sek=15;


int8_t hourBCD;
int8_t minuteBCD;
int8_t secondBCD;

int8_t yearBCD;
int8_t monthBCD;
int8_t dayBCD;
int8_t weekDay;

int8_t alarm_hourBCD;
int8_t alarm_minuteBCD;
*/

const uint8_t weekDayArr[]="PnWtSrCzPtSoNd";
const uint8_t dayInMonthArr[]={31,29,31,30,31,30,31,31,30,31,30,31};


void RTC_WriteProtectionDisable(void){
	RTC->WPR = 0xCA; //wpisanie klucza
	RTC->WPR = 0x53;
	RTC->ISR |= RTC_ISR_INIT;  //włączenie możliwości edycji
	while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF); //oczekiwanie na flagę
}

void RTC_WriteProtectionEnable(void){
	RTC->ISR &=~ RTC_ISR_INIT; 
	RTC->WPR = 0xFE;  //wpisanie błędnego klucza
	RTC->WPR = 0x64; 
}

void RTC_Alarm_WriteProtectionDisable(void){
	RTC->WPR = 0xCA;  //wpisanie klucza
	RTC->WPR = 0x53; 
	RTC->CR &= ~RTC_CR_ALRAE;  //włączenie możliwości edycji
	while((RTC->ISR & RTC_ISR_ALRAWF) != RTC_ISR_ALRAWF);  //oczekiwanie na flagę
}
void RTC_Alarm_WriteProtectionEnable(void){
	RTC->CR |= RTC_CR_ALRAE;
	RTC->WPR = 0xFE; //wpisanie błędnego klucza
	RTC->WPR = 0x64;
}

void RTC_init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;        //włączenie zegara
  PWR->CR |= PWR_CR_DBP;					//włączenie dostępu do rejestrów rtc

//wykonywane tylko raz po zaniku napięcia na kondensatorze zasilającego zegara rtc
if((RCC->BDCR & RCC_BDCR_LSERDY) == 0)				//sprawdzenie czy oscylator lse jest ustawiony
 {

  RCC->BDCR |= RCC_BDCR_LSEON;                        //wlacz LSE  
  while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0);         //czekanie aż zastartuje
  RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSE;  //wlaczenie rtc i tasktowania z lse

	//RTC->WPR = 0xCA;
	//RTC->WPR = 0x53;
  //RTC_WriteProtectionEnable();
 }
 
	//RTC_WriteProtectionDisable();
	//RTC->CR|= RTC_CR_BKP;
	//RTC_WriteProtectionEnable(); 

 
	updateDateTime();		//Odświeżenie daty i godziny
}

void RTC_alarm_init(void)
{
	RTC_Alarm_WriteProtectionDisable();		//wyłącznenie trybu ochrony dla alarmu
  RTC->ALRMAR |= RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK1; //ustawienie alarmu co 1s
  RTC->CR |= RTC_CR_ALRAIE; //| RTC_CR_ALRAE;            // włącz alarm i przerwanie od alarmu
	RTC_Alarm_WriteProtectionEnable();		//włącznenie trybu ochrony

  EXTI->IMR |= EXTI_IMR_MR17;              //Skonfigurowanie maski 
  EXTI->RTSR |= EXTI_RTSR_TR17;            //Skonfigurowanie wyzwalania przerwania
  NVIC_SetPriority(RTC_IRQn, 0);           //Ustawienie piorytetu przerwania
  NVIC_EnableIRQ(RTC_IRQn);                //Włączenie przerwania
 
	updateAlarm();					//Odświeżenie czasu alarmu
}

void RTC_IRQHandler(void)
{
  RTC->ISR &= ~RTC_ISR_ALRAF;   //skasuj flage alarmu
  EXTI->PR |= EXTI_PR_PR17;     //skasuj flage przerwania od rtc (EXTI17)
  f_rtc=1;                      //ustawienie flagi programowej
	//Odczyt z rejestrów
	//updateDateTime();
}

uint8_t chechAlarm(void){
	if(hourBCD==alarm_hourBCD && minuteBCD==alarm_minuteBCD && secondBCD==0 
	   && getAlarmMode()){  //sprawdzenie czy czasy się zgadzają
		if(getAlarmMode() == 1) setAlarmMode(0); //zmiana trybu gdy jest równy 1
		return 1;  //jest alarm
	}
	return 0;  //brak alarmu
}

void updateDateTime(void){
	hourBCD = ((RTC->TR>>16)&0x3f);
	minuteBCD =((RTC->TR>>8)&0x7f);
	secondBCD =((RTC->TR)&0x7f);
	
	yearBCD = (RTC->DR>>16)&0xff;
	monthBCD = (RTC->DR>>8)&0x1f;
	dayBCD = (RTC->DR)&0x3f;
	weekDay = (RTC->DR>>13)&0x7;
}

void updateAlarm(void){
	alarm_hourBCD = ((RTC->ALRMAR>>16)&0x3f);
	alarm_minuteBCD =((RTC->ALRMAR>>8)&0x7f);
}

uint8_t bcdToBinary(uint8_t bcd){
	return (bcd>>4)*10 + ((bcd)&0xf);
}
uint8_t binaryToBcd(uint8_t binary){
	return ((binary/10)<<4)+(binary%10);
}


void displayTime(uint8_t hideIndex){
	//wyświetlenie godzine
	if(hideIndex != 1) Lcd_8dec_bcd(hourBCD);
	else Lcd_strr("  ");
	
	Lcd_strr(":");
	//wyświetlenie minut
	if(hideIndex != 2) Lcd_8dec_bcd(minuteBCD);
	else Lcd_strr("  ");
	
	Lcd_strr(":");
	//wyświetlenie sekund
	if(hideIndex != 3) Lcd_8dec_bcd(secondBCD);
	else Lcd_strr("  ");
}

void displayDate(uint8_t hideIndex){
	//wyświetlenie dnia
	if(hideIndex != 3) Lcd_8dec_bcd(dayBCD);
	else Lcd_strr("  ");
	
	Lcd_strr(".");
	//wyświetlenie miesiąca
	if(hideIndex != 2) Lcd_8dec_bcd(monthBCD);
	else Lcd_strr("  ");
	
	Lcd_strr(".");
	//wyświetlenie roku
	Lcd_8dec_2(20);
	if(hideIndex != 1) Lcd_8dec_bcd(yearBCD);
	else Lcd_strr("  ");
}

void displayWeekDay(uint8_t hideIndex){
	//wyświetlenie dnia tygodnia
	if(hideIndex != 1){
		Lcd_char(weekDayArr[(weekDay-1)*2]);  
		Lcd_char(weekDayArr[(weekDay-1)*2+1]);
	}
	else Lcd_strr("  ");
}

void displayAlarm(uint8_t hideIndex){
	//wyświetlenie godziny alarmu
	if(hideIndex != 1) Lcd_8dec_bcd(alarm_hourBCD);
	else Lcd_strr("  ");
	
	Lcd_strr(":");
	//wyświetlenie minut minuty alarmu
	if(hideIndex != 2) Lcd_8dec_bcd(alarm_minuteBCD);
	else Lcd_strr("  ");
}

uint8_t getAlarmMode(void){
	return RTC->BKP0R&0x3;
}

void setAlarmMode(uint8_t mode){
	if(mode>2) mode=0;
	RTC->BKP0R = (RTC->BKP0R& (~(0x3))) | mode&0x3;
}

void correctDay(void){
	int8_t monthB = bcdToBinary(monthBCD);
	int8_t dayB = bcdToBinary(dayBCD);
	
	if(dayB>dayInMonthArr[monthB-1]) dayB=dayInMonthArr[monthB-1];
	dayBCD = binaryToBcd(dayB);
	
	RTC_WriteProtectionDisable();
		RTC->DR = (RTC->DR & ~(RTC_DR_DU | RTC_DR_DT )) | binaryToBcd(dayB);
	RTC_WriteProtectionEnable();
}


void addTimeValue(uint8_t index,int8_t value){
	//wpisanie wartości do zmienny pomoczniczych 
	int8_t hourB = bcdToBinary(hourBCD);
	int8_t minuteB = bcdToBinary(minuteBCD);
	int8_t secondB = bcdToBinary(secondBCD);
	if(index==1){ //godziny
		hourB += value;
		if(hourB>=24) hourB-=24;
		else if(hourB<0) hourB+=24;
		hourBCD = binaryToBcd(hourB);
	}
	else if(index==2){ //minuty
		minuteB += value;
		if(minuteB>=60) minuteB-=60;
		else if(minuteB<0) minuteB+=60;
		minuteBCD = binaryToBcd(minuteB);
	}
	else if(index==3){ //sekundy
		secondB = value>=0? (secondB/5+value)*5 : ((secondB+4)/5+value)*5;
		if(secondB>=60) secondB-=60;
		else if(secondB<0) secondB+=60;
		secondBCD = binaryToBcd(secondB);
		displayClock();
	}
	RTC_WriteProtectionDisable();  //aktualizacja rejestrów
		RTC->TR = RTC_TR_PM | (binaryToBcd(hourB)<<16) | (binaryToBcd(minuteB)<<8) | (secondBCD);
	RTC_WriteProtectionEnable();
}

void addDateValue(uint8_t index,int8_t value){
	int8_t yearB = bcdToBinary(yearBCD);
	int8_t monthB = bcdToBinary(monthBCD);
	int8_t dayB = bcdToBinary(dayBCD);
	if(index==1){ //rok
		yearB += value;
		if(yearB>=100) yearB-=100;
		else if(yearB<0)yearB+=100;
		yearBCD = binaryToBcd(yearB);
	}
	else if(index==2){//miesiąc
		monthB += value;
		if(monthB>12) monthB-=12;
		else if(monthB<1) monthB+=12;
		monthBCD = binaryToBcd(monthB);
	}
	else if(index==3){//dzień
		dayB += value;
		if(dayB>dayInMonthArr[monthB-1]) dayB-=dayInMonthArr[monthB-1];
		else if(dayB<1) dayB+=dayInMonthArr[monthB-1];
		dayBCD = binaryToBcd(dayB);
	}

	RTC_WriteProtectionDisable();  //aktualizacja rejestrów
		RTC->DR = (RTC->DR & RTC_DR_WDU) | (binaryToBcd(yearB)<<16) | (binaryToBcd(monthB)<<8) | binaryToBcd(dayB);
	RTC_WriteProtectionEnable();
}

void addAlarmValue(uint8_t index,int8_t value){
	if(index==3){ //tryb alarmu
		int8_t mode = getAlarmMode()+value;
		if(mode>=3) mode-=3;
		else if(mode<0) mode+=3;
		setAlarmMode(mode);
		return;
	}
	
	int8_t alarm_hourB = bcdToBinary(alarm_hourBCD);
	int8_t alarm_minuteB = bcdToBinary(alarm_minuteBCD);
	if(index==1){ //godzina alarmu
		alarm_hourB += value;
		if(alarm_hourB>=24) alarm_hourB-=24;
		else if(alarm_hourB<0) alarm_hourB+=24;
		alarm_hourBCD = binaryToBcd(alarm_hourB);
	}
	else if(index==2){ //minuta alarmu
		alarm_minuteB += value;
		if(alarm_minuteB>=60) alarm_minuteB-=60;
		else if(alarm_minuteB<0) alarm_minuteB+=60;
		alarm_minuteBCD = binaryToBcd(alarm_minuteB);
	}
	
	RTC_Alarm_WriteProtectionDisable();  //aktualizacja rejestrów
		RTC->ALRMAR &= ~(RTC_ALRMAR_HT | RTC_ALRMAR_HU | RTC_ALRMAR_MNT |RTC_ALRMAR_MNU);
		RTC->ALRMAR |= (binaryToBcd(alarm_hourB)<<16) | (binaryToBcd(alarm_minuteB)<<8);
	RTC_Alarm_WriteProtectionEnable();
	updateAlarm();  //odświeżenie rejestrów
}

void addWeekDayValue(int8_t value){
	int8_t weekDay2 = weekDay;
	weekDay2 += value;
	if(weekDay2>7) weekDay2-=7;
	else if(weekDay2<=0) weekDay2+=7;
	weekDay = weekDay2;

	RTC_WriteProtectionDisable();  //aktualizacja rejestrów
		RTC->DR = (RTC->DR&~RTC_DR_WDU)|(weekDay2<<13);
	RTC_WriteProtectionEnable();
}











#endif









