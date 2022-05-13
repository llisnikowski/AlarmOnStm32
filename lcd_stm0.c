//********************************************************
//***  Procedury obslugi LCD alfanumerycznego         ****
//***     dla  STM32F072     Nucleo                   ****
//********************************************************
//****     Data: 23/02/2021    Kalus Piotr            ****
//********************************************************
//PC0    LCD d4      PD2   LCD EE
//PC1    LCD d5 
//PC2    LCD d6      PC11  LCD RS
//PC3    LCD d7 
//---------------------------------------------------

#include "lcd_stm0.h"

uint8_t kody_zn[]={
	//Ł - 8
	0x10,
	0x10,
	0x12,
	0x14,
	0x18,
	0x10,
	0x1f,
	0,
	
	//ś - 9
  0x08,
  0x04,
  0x0E,
  0x10,
  0x0E,
  0x01,
  0x1E,
  0x00,
	
	0,0,0,0,0,0,0,0,	//10

	//setting 1 - 11
	0x00,
  0x15,
  0x0E,
  0x1B,
  0x0E,
  0x14,
  0x01,
  0x01,
	
	//setting 2 - 12
	0x00,
  0x15,
  0x0E,
  0x1B,
  0x0E,
  0x14,
  0x03,
  0x03,

	//bell 1 - 13
	0x00,
  0x04,
  0x06,
  0x0A,
  0x0C,
  0x1E,
  0x00,
  0x04,

	//bell 2 - 14
	0x01,
  0x05,
  0x0e,
  0x0e,
  0x0e,
  0x1f,
  0,
  0x04,
	
	//bell 3 - 15
	0,
  0x04,
  0x0e,
  0x0e,
  0x0e,
  0x1f,
  0,
  0x04

};

//---------------------------------------------------

void Lcd_cmd(uint8_t data)     //najpierw 4 starsze, potem mlodsze
{
GPIOC->BSRR=((data & 0xF0)>>4) | ((0x0f+lcd_rs)<<16);  //wszystko naraz
	
GPIOD->BSRR = lcd_e;       //  _/  ee
waitus(2);
GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1

// - - MLODSZE BITY
GPIOC->BSRR=(data & 0x0F) | 0x000f0000;
GPIOD->BSRR = lcd_e;       //  _/  ee
waitus(2);
GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1

//czekaj
waitus(100);
}

//--------------------------------------------------
void Lcd_char(uint8_t data)
{

GPIOC->BSRR=((data & 0xF0)>>4) | lcd_rs | 0x000f0000;  //wszystko naraz 

GPIOD->BSRR = lcd_e;       //  _/  ee
waitus(2);
GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1

// - - MLODSZE BITY
GPIOC->BSRR=(data & 0x0F) | lcd_rs | 0x000f0000;

GPIOD->BSRR = lcd_e;       //  _/  ee
waitus(2);
GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1

//czekaj
waitus(100);
}
//****************************************************************

const unsigned char initlcd[] =
{
0x03,0x02,                  //
0x02,0x08,0x00,0x0C,
0x00,0x01,0x00,0x06,
0x01,0x04
};
//------------------------------
void Lcd_init ( void )
{
    uint8_t i;

    waitus(118000);               //15ms
    GPIOC->BRR = lcd_rs;          //instrukcje
    GPIOD->BSRR = lcd_e;          // ee_/

    GPIOC->BRR =  0x0000000f;     //linie danych na 0
    GPIOC->BSRR = 0x00000003;     // pierwsza dana konfig.
    waitus(200);
    GPIOD->BRR = lcd_e;           //  ee \_ ZATRZASK1
    waitus(16100);                //4.1 ms

    GPIOD->BSRR = lcd_e;          // ee_/
    GPIOC->BSRR = 0x00000003;     // druga dana konfig.
    waitus(1000);
    GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1
    waitus(1000);      //100 us

    for(i=0;i<12;i++)
       {
        GPIOD->BSRR = lcd_e;          // ee_/
        GPIOC->BRR = 0x000f;       //linie danych na 0
        GPIOC->BSRR = initlcd[i];
        waitus(80);                   //40
        GPIOD->BRR = lcd_e;      //  ee \_ ZATRZASK1
        waitus(10000);              //100 us
       }
			 
	Lcd_cmd(0x40);   //zapis znaków specjalnych
	for(i=0;i<64;i++)  Lcd_char(kody_zn[i]);

}  //end void Lcd_init


//*********************************************************
void Lcd_cls(void)
{
  uint8_t i;

    Lcd_cmd(128);     //linia 1
    for ( i = 0; i < 16; i++ )  Lcd_char(' ');

    Lcd_cmd(128+64);     //linia 2
    for ( i = 0; i < 16; i++ )  Lcd_char(' ');
    Lcd_cmd(128);
}
//---------------------------------------------------------
void Lcd_str(const char *dataPtr ) //dla łańcuchów we FLASH
{
    while ( *dataPtr )  Lcd_char(*dataPtr++ );
}

//---------------------------------
void Lcd_strr ( char *dataPtr ) //dla łańcuchów z pamięci RAM
{
    while ( *dataPtr ) Lcd_char( *dataPtr++ );
}
//----------------------------------------------------------------
void Lcd_8dec( uint8_t bin8)
{
uint8_t dzies,setki,jedn;
setki=bin8/100;
dzies=(bin8-setki*100) /10;
jedn= (bin8-setki*100)%10;

Lcd_char(setki+48);
Lcd_char(dzies+48);
Lcd_char(jedn+48);

}

void Lcd_8dec_2(uint8_t bin8){
	Lcd_char((bin8%100)/10+'0');  //napisanie pierwszej cyfry
	Lcd_char(bin8%10+'0');        //napisanie drugiej cyfry
}

void Lcd_8dec_bcd(uint8_t bcd8){
	Lcd_char((bcd8>>4) <= 9? (bcd8>>4) + '0' : '0');   //napisanie pierwszej cyfry
	Lcd_char((bcd8&0xf) <= 9? (bcd8&0xf) + '0' : '0'); //napisanie drugiej cyfry
}






//----------------------------------------------------------------
void Lcd_8hex( uint8_t bin8)
{
  uint8_t dz16,jedn;
  dz16=(bin8>>4)+48;
  if(dz16>57) dz16=dz16+7;
  Lcd_char(dz16);

  jedn=(bin8&15)+48;
  if(jedn>57) jedn=jedn+7;
  Lcd_char(jedn);
}
//*****************************************

void Lcd_16dec( uint16_t bin16)
{
uint16_t dztys, tys, setki, dzies;
uint16_t res;

dztys=bin16/10000;
Lcd_char(dztys+48);

res=bin16%10000;  tys=res/1000;
Lcd_char(tys+48);

res=res%1000;    setki=res/100;
Lcd_char(setki+48);

res=res%100;    dzies=res/10;
Lcd_char(dzies+48);

res=res%10;
Lcd_char(res+48);
}

//----------------------------------------------------------------
void Lcd_16hex( uint16_t bin16)
{
  uint8_t jedn;

  jedn=(bin16>>12)+48;
  if(jedn>57) jedn=jedn+7;
  Lcd_char(jedn);

  jedn=((bin16>>8)&15)+48;
  if(jedn>57) jedn=jedn+7;
  Lcd_char(jedn);

  jedn=((bin16>>4)&15)+48;
  if(jedn>57) jedn=jedn+7;
  Lcd_char(jedn);

  jedn=(bin16&15)+48;
  if(jedn>57) jedn=jedn+7;
  Lcd_char(jedn);
}
//============================================
void Lcd_bar(uint8_t bar)
{
uint8_t res;

Lcd_cmd(line2);
if(bar>=80)
  { for(res=0;res<16;res++) Lcd_char(12); }   //full
else
{
  for(res=0;res<16;res++) Lcd_char(' ');   //clear
	Lcd_cmd(line2);
  if(bar>=5)  { Lcd_char(12);}
  else goto reszta;

  if(bar>=10)	{ Lcd_char(12);}
  else goto reszta;

  if(bar>=15)	{ Lcd_char(12);}
  else goto reszta;

  if(bar>=20) { Lcd_char(12);}
  else goto reszta;

  if(bar>=25){ Lcd_char(12);}
  else goto reszta;

  if(bar>=30){ Lcd_char(12);}
  else goto reszta;

  if(bar>=35){ Lcd_char(12);}
  else goto reszta;

  if(bar>=40){ Lcd_char(12);}
  else goto reszta;

  if(bar>=45){ Lcd_char(12);}
  else goto reszta;

  if(bar>=50){ Lcd_char(12);}
  else goto reszta;

  if(bar>=55){ Lcd_char(12);}
  else goto reszta;

  if(bar>=60){ Lcd_char(12);}
  else goto reszta;

  if(bar>=65){ Lcd_char(12);}
  else goto reszta;

  if(bar>=70){ Lcd_char(12);}
  else goto reszta;

  if(bar>=75){ Lcd_char(12);}

  reszta:
	  res=bar%5;
	  if(res!=0) Lcd_char(res-1);
  }  //ens else

} //end



// *****************************************

