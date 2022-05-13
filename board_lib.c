//==================================================================
//=====  Procedury pomocnicze do sterowania płytką STM32F0   =======
//==================================================================

#include "lcd_stm0.h"

extern uint16_t is1,is2, is3, is4;

void waitus(uint32_t  us)
{
  uint32_t i;
  for(i=0;i<us;i++)
    {
     __nop(); __nop(); __nop(); __nop(); __nop();  //5x8
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
     __nop(); __nop(); __nop(); __nop(); __nop();
    }
}
// ***************************************************
void wr595(uint8_t d)
{
  uint8_t i;
	GPIOC->BRR = PC3;
  for(i=0;i<8;i++)
     {
      if(d&0x01)  GPIOC->BSRR = PC2;  //swieci gdy 1
      else        GPIOC->BRR = PC2;
     
      GPIOC->BSRR = PC3;
      d=d>>1; 
      GPIOC->BRR = PC3;
     }
		 
	GPIOC->BSRR = PC10;	
	__nop(); __nop();	 
  GPIOC->BRR = PC10;	 
}

//**********************************************************
//zapis i2c blokowo
void i2c_wr(uint8_t adr , uint8_t sub , uint16_t il)
{
uint16_t i;

I2C2->CR2= (uint32_t) ((uint32_t)adr) | ((uint32_t) (il+1) << 16 ) | I2C_CR2_AUTOEND; 
I2C2->CR2 |= I2C_CR2_START;                            //start - wysyła adres układu
//- - - - - - - - - 
I2C2->TXDR = sub&255;   //subadres   to sprawdź czy potrzebna &255
while((I2C2->ISR & I2C_ISR_TXE)==0) {};  
  
for(i=0; i<il;i++)
   {I2C2->TXDR = buf_i2c[i];                 //send bajty bufora
     while((I2C2->ISR & I2C_ISR_TXE)==0) {};
   }
}

//**********************************************************
//odczyt  i2c blokowo
void i2c_rd(uint8_t adr , uint8_t sub , uint16_t il)
{
uint16_t i;

I2C2->CR2= (uint32_t) ((uint32_t)adr) | ((uint32_t) 1<< 16 );        //il=1 bajty subadr
I2C2->CR2 |= I2C_CR2_START;       //start - wysyła adres układu
   
I2C2->TXDR = sub; while((I2C2->ISR & I2C_ISR_TXE)==0) {}; //waitus(200); //wyslij adres sub l
   
//-   -   -   -   - przejdź w tryb odczytu
I2C2->CR2= (uint32_t) ( (uint32_t)(adr+1) | ((uint32_t) il<< 16 ) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
I2C2->CR2 |= I2C_CR2_START;       //start - wysyła adres układu
    
for(i=0; i<il;i++)
  {
   while((I2C2->ISR & I2C_ISR_RXNE) == 0);   //czekaj dopoki buf pusty
   buf_i2c[i]=I2C2->RXDR;  
  }

while((I2C2->ISR & I2C_ISR_STOPF) == 0){};
I2C2->ICR |=I2C_ICR_STOPCF;               //skasuj flage stopu
        
}

//**********************************************************
//zapis i2c blokowo do EEprom 24c16
// UWAGA! w adresie zawarty nr strony 3-bity [P]
// 1 0 1 0 P2 P1 P0 r/w  
void AT24c16_wr(uint16_t sub , uint8_t il)
{
uint16_t i;
uint8_t adr=(0xA0 | ((sub>>7)&0x0E) );  //kasuj najml.bit

I2C1->CR2= (uint32_t) ((uint32_t)adr) | ((uint32_t) (il+1) << 16 ) | I2C_CR2_AUTOEND; 
I2C1->CR2 |= I2C_CR2_START;       //start - wysyła adres układu
//- - - - - - - - - 

I2C1->TXDR = sub&255;   //subadres mlodszy
while((I2C1->ISR & I2C_ISR_TXE)==0) {};  
  
for(i=0; i<il;i++)
  {I2C1->TXDR = buf_i2c[i];                 //send bajty bufora
   while((I2C1->ISR & I2C_ISR_TXE)==0) {};
  }
}

//**********************************************************
//odczyt  i2c blokowo eeprom 24c16
void AT24c16_rd(uint16_t sub , uint8_t il)
{
uint16_t i;
uint8_t adr=(0xA0 | ((sub>>7)&0x0E) );  //kasuj najml.bit
	
I2C1->CR2= (uint32_t) ((uint32_t)adr) | ((uint32_t) 1<< 16 );        //il=1 bajt subadr
I2C1->CR2 |= I2C_CR2_START;       //start - wysyła adres układu  
I2C1->TXDR = sub&255; while((I2C1->ISR & I2C_ISR_TXE)==0) {}; //wyslij adres sub l
     
//-   -   -   -   - przejdź w tryb odczytu
I2C1->CR2= (uint32_t) ( (uint32_t)(adr+1) | ((uint32_t) il<< 16 ) | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND);
I2C1->CR2 |= I2C_CR2_START;       //start - wysyła adres układu
    
for(i=0; i<il;i++)
  {while((I2C1->ISR & I2C_ISR_RXNE) == 0);   //czekaj dopoki buf pusty 
   buf_i2c[i]=I2C1->RXDR;  
  }

while((I2C1->ISR & I2C_ISR_STOPF) == 0){};
I2C1->ICR |=I2C_ICR_STOPCF;               //skasuj flage stopu
        
}

//=======================================================================================
uint16_t ADC1_rd(uint8_t ch) 
{
  ADC1->CHSELR=1<<ch; //kanały: PA0..PA7,PB0,PB1,PC0..PC5
  while((ADC1->ISR&ADC_ISR_ADRDY)!=1); // adc ready
  ADC1->CR=ADC_CR_ADSTART;       //start conv ADC , kasowany sprzetowo
  
  while((ADC1->CR&ADC_CR_ADSTART)!=0); //  
  return( (uint16_t)(ADC1->DR));
}

//=================================================================================
void RTC_rd(void)
{
Lcd_cmd(line1);
Lcd_char(((RTC->TR>>20)&3)+48); Lcd_char(((RTC->TR>>16)&15)+48); Lcd_char(':');
Lcd_char(((RTC->TR>>12)&7)+48); Lcd_char(((RTC->TR>>8)&15)+48);  Lcd_char('.');
Lcd_char(((RTC->TR>>4)&7)+48);  Lcd_char((RTC->TR&15)+48);
}

void RTC_time(uint32_t time)
{
  RTC->WPR = 0xCA;   RTC->WPR = 0x53;

  RTC->ISR |= RTC_ISR_INIT;
  while((RTC->ISR & RTC_ISR_INITF)==0);

  RTC->TR= (uint32_t) time; 
  RTC->ISR &= (uint32_t)~RTC_ISR_INIT;  //wyjdź z trybu Init;  

  //RTC_WaitForSynchro();
  RTC->WPR = 0xFF;  //wlacz ochronę rtc 
}
// -- -- -- -- -- -- --- 
void RTC_date(uint32_t date, uint8_t dztyg)
{
  RTC->WPR = 0xCA;  RTC->WPR = 0x53;
  RTC->ISR |= RTC_ISR_INIT;
  while((RTC->ISR & RTC_ISR_INITF)==0);
  
	date |= (uint32_t) dztyg<<13;
	RTC->DR = date;

  RTC->ISR &= (uint32_t)~RTC_ISR_INIT;  
  RTC->WPR = 0xFF;  //wlacz ochrone rtc 
}

//------------------------------------------
const uint8_t dtyg[]="PnWtSrCzPtSoNi";
void Lcd_Time(void)
{
  Lcd_char(((RTC->TR>>20)&3)+48); Lcd_char(((RTC->TR>>16)&15)+48); Lcd_char(':');
  Lcd_char(((RTC->TR>>12)&7)+48); Lcd_char(((RTC->TR>>8)&15)+48);  Lcd_char('.');
  Lcd_char(((RTC->TR>>4)&7)+48);  Lcd_char((RTC->TR&15)+48);  
}

void Lcd_Time2(void)
{
  Lcd_char(dtyg[(((RTC->DR>>13)&7)-1)*2]);  Lcd_char(dtyg[(((RTC->DR>>13)&7)-1)*2+1]);
  Lcd_char(' ');
  Lcd_Time();
}

void Lcd_Date(void)
{
  Lcd_char(((RTC->DR>>4)&3)+48);  Lcd_char((RTC->DR&15)+48); Lcd_char('-');
  Lcd_char(((RTC->DR>>12)&1)+48); Lcd_char(((RTC->DR>>8)&15)+48);  Lcd_str("-20");
  Lcd_char(((RTC->DR>>20)&15)+48); Lcd_char(((RTC->DR>>16)&15)+48);
  // Lcd_char('_'); Lcd_char(((RTC->DR>>13)&7)+48);  nr dnia tyg
}
void Lcd_Date2(void)
{
  Lcd_char(dtyg[(((RTC->DR>>13)&7)-1)*2]);  Lcd_char(dtyg[(((RTC->DR>>13)&7)-1)*2+1]);
  Lcd_char(' ');
  Lcd_Date();
}

//=============================================================================

void RTC_send(void)
{
printf("\r\n %c%c:",((RTC->TR>>20)&3)+48 , ((RTC->TR>>16)&15)+48);
printf("%c%c.",((RTC->TR>>12)&7)+48 , ((RTC->TR>>8)&15)+48);
printf("%c%c\r\n",((RTC->TR>>4)&7)+48, (RTC->TR&15)+48  );
}

//================================================================================
void SPI2_wr(uint8_t Data)
{
  while((SPI2->SR&SPI_SR_TXE)==0){};    //TXEmpty ale może jeszcze wysyłać poprzedni bajt (buforowanie)
  *(__IO uint8_t *) & SPI2->DR=Data;
}

//======================================================
uint8_t SPI2_wrd(uint8_t Data)
{

  while((SPI2->SR&SPI_SR_TXE)==0){};    //TXEmpty ale może jeszcze wysyłać poprzedni bajt (buforowanie)
  *(__IO uint8_t *) & SPI2->DR=Data;

  while((SPI2->SR&SPI_SR_RXNE) == 0)  { }
  return (uint8_t) SPI2->DR;
}
//===========================================================================

