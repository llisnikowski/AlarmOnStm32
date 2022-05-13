//********************************************************
//***  Procedury obslugi LCD alfanumerycznego         ****
//***     dla  STM32F072     Nucleo                   ****
//********************************************************
//****     Data: 23/02/2021    Kalus Piotr            ****
//********************************************************
//PC0    LCD d4 (D7     PD2   LCD EE
//PC1    LCD d5 (D6
//PC2    LCD d6 (D5     PC11  LCD RS
//PC3    LCD d7 (D4
//---------------------------------------------------

//#include "stm32f0xx.h"
#ifndef LCD_STM0
#define LCD_STM0

#define lcd_e  PD2   //  PD2   
#define lcd_rs PC11   // PC11  1 data   0 instr


#define  line1 128
#define  line2 128+0x40    //+64
#define  line3 128+0x14
#define  line4 128+0x54

void Lcd_cmd( uint8_t data );
void Lcd_char(uint8_t ch);
void Lcd_init( void );

void Lcd_cls(void);

void Lcd_str( const char *dataPtr );  //dla lancuchow we FLASH
void Lcd_strr(char *dataPtr );       //dla lancuchow z pamieci RAM
void Lcd_8dec(uint8_t bin8);
void Lcd_8hex(uint8_t bin8);
void Lcd_16dec( uint16_t bin16);
void Lcd_16hex( uint16_t bin16);

void Lcd_8dec_2(uint8_t bin8);
void Lcd_8dec_bcd(uint8_t bcd8);

void Lcd_bar(uint8_t bar);

/*const uint8_t linijka_led[] = {
0x00,    //00000000
0x01,    //00000001
0x02,    //00000010
0x04,    //00000100
0x08,    //00001000
0x10,    //00010000
0x20,    //00100000
0x40,    //01000000
0x80,    //10000000
0x81,    //10000001
0x82,    //10000010
0x84,    //10000100
0x88,    //10001000
0x90,    //10010000
0xa0,    //10100000
0xc0,    //11000000
0xc1,    //11000001
0xc2,    //11000010
0xc4,    //11000100
0xc8,    //11001000
0xd0,    //11010000
0xe0,    //11100000
0xe1,    //11100001
0xe2,    //11100010
0xe4,    //11100100
0xe8,    //11101000
0xf0,    //11110000
0xf1,    //11110001
0xf2,    //11110010
0xf4,    //11110100
0xf8,    //11111000
0xf9,    //11111001
0xfa,    //11111010
0xfc,    //11111100
0xfd,    //11111101
0xfe,    //11111110
0xff    //11111111
};*/

//===========================
#endif


