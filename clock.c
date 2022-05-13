#include "clock.h"
#ifdef CLOCK_H


void displayClock(void){
	Lcd_cls(); //wyczyszczenie wyświetlacza
	Lcd_cmd(line1);
	displayTime(0); //wyświetlanie czasu
	Lcd_strr(" A:");
	displayAlarm(0); //wyświetlanie alarmu
	Lcd_cmd(line2);
	displayDate(0); //wyświetlanie daty
	Lcd_strr(" ");
	displayWeekDay(0); //wyświetlanie dnia tygodnia
	
	Lcd_cmd(line2+14); 
	Lcd_char(10+clockMode); //wyświetlanie symbolu ustawiania
	
	Lcd_cmd(line2+15); 
	Lcd_char(13+getAlarmMode()); //wyświetlanie symbolu alarmu

}

void displayClock1(typeTime hideTime){
	Lcd_cls(); //wyczyszczenie wyświetlacza
	if(hideTime != tT_time){ //wyświetlanie czasu
		Lcd_cmd(line1);
		displayTime(0);
	}
		
	if(hideTime != tT_alarm){ //wyświetlanie alarmu
		Lcd_cmd(line1+9);
		Lcd_strr("A:");
		displayAlarm(0);
		Lcd_cmd(line2+15);
		Lcd_char(13+getAlarmMode());
	}
	if(hideTime != tT_date){  //wyświetlanie daty
		Lcd_cmd(line2);
		displayDate(0);
	}
	if(hideTime != tT_weekDay){ //wyświetlanie dnia tygodnia
		Lcd_cmd(line2+11);
		displayWeekDay(0);
	}
		
	Lcd_cmd(line2+14); 
	Lcd_char(10+clockMode); //wyświetlanie symbolu ustawiania
}	

void displayClock2(typeTime hideTime){
	Lcd_cls(); //wyczyszczenie wyświetlacza
	Lcd_cmd(line1); //wyświetlanie czasu
	displayTime(hideTime == tT_time ? settingIndex:0);
	
	Lcd_cmd(line1+9);
	Lcd_strr("A:"); //wyświetlanie alarmu
	displayAlarm(hideTime == tT_alarm ? settingIndex:0);
	
	Lcd_cmd(line2);  //wyświetlanie daty
	displayDate(hideTime == tT_date ? settingIndex:0);
	
	Lcd_cmd(line2+11); //wyświetlanie dnia tygodnia
	displayWeekDay(hideTime == tT_weekDay ? settingIndex:0);
	
	Lcd_cmd(line2+14);
	Lcd_char(10+clockMode); //wyświetlanie symbolu ustawiania
	
	Lcd_cmd(line2+15); //wyświetlanie symbolu alarmu
	if(hideTime != tT_alarm || settingIndex!=3) Lcd_char(13+getAlarmMode());
}

void setClockMode(uint8_t nrMode){
	clockMode = nrMode;
	switch(clockMode){
		case 1:
			settingTypeTime = tT_time;
			break;
		case 2:
			settingIndex = 1;
			break;
		default:
			return;
	}
	
	timerBlink = 0;
	timerNoAction = TIME_NO_ACTION;
}





void nextSettingTypeTime(bool reverse){
	if(!reverse){
		settingTypeTime++;
		if(settingTypeTime >= tT_end) settingTypeTime-=4;
	}
	else{
		settingTypeTime--;
		if(settingTypeTime <= tT_none) settingTypeTime+=4;
	}
	timerBlink = 0;
	timerNoAction = TIME_NO_ACTION;
}




void nextSettingIndex(bool reverse){
	if(!reverse){
		settingIndex++;
		if(settingIndex>maxSettingIndex[settingTypeTime-11]){setClockMode(0);}
	}
	else{
		settingIndex--;
		if(settingIndex == 0){ clockMode = 1;}
	}
	if(settingTypeTime == tT_date)correctDay();
	
	waitus(1000);
	timerBlink = 0;
	timerBlinkHoldOff = 0;
	timerNoAction = TIME_NO_ACTION;
}

void addValue(int8_t value){
	//printf("%d",settingTypeTime);
	switch(settingTypeTime){
		case tT_time:
			addTimeValue(settingIndex,value);
			break;
		case tT_alarm:
			addAlarmValue(settingIndex,value);
			break;
		case tT_date:
			addDateValue(settingIndex,value);
			break;
		case tT_weekDay:
			addWeekDayValue(value);
			break;
	}
	
	waitus(1000);
	timerBlinkHoldOff = BLINK_HOLD_OFF;
	f_rtc=1;
	timerNoAction = TIME_NO_ACTION;
}






#endif