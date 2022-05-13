#include "gpio_def.h"

typedef enum {

  OUT_PP_LS_No   = 32,           //patrz tabela 19 str 126 RM
  OUT_PP_LS_Up   = 33,
  OUT_PP_LS_Down = 34,
  OUT_PP_MS_No   = 36,
  OUT_PP_MS_Up   = 37,
  OUT_PP_MS_Down = 38,
  OUT_PP_HS_No   = 44,
  OUT_PP_HS_Up   = 45,
  OUT_PP_HS_Down = 46,
  OUT_OD_LS_No   = 48,
  OUT_OD_LS_Up   = 49,
  OUT_OD_LS_Down = 50,
  OUT_OD_MS_No   = 52,
  OUT_OD_MS_Up   = 53,
  OUT_OD_MS_Down = 54,
  OUT_OD_HS_No   = 60,
  OUT_OD_HS_Up   = 61,
  OUT_OD_HS_Down = 62,
  AF_PP_LS_No    = 64,
  AF_PP_LS_Up    = 65,
  AF_PP_LS_Down  = 66,
  AF_PP_MS_No    = 68,
  AF_PP_MS_Up    = 69,
  AF_PP_MS_Down  = 70,
  AF_PP_HS_No    = 76,
  AF_PP_HS_Up    = 77,
  AF_PP_HS_Down  = 78,
  AF_OD_LS_No    = 80,
  AF_OD_LS_Up    = 81,
  AF_OD_LS_Down  = 82,
  AF_OD_MS_No    = 84,
  AF_OD_MS_Up    = 85,
  AF_OD_MS_Down  = 86,
  AF_OD_HS_No    = 92,
  AF_OD_HS_Up    = 93,
  AF_OD_HS_Down  = 94,

  IN_Analog = 0x60,  //Analog input (ADC)
  IN_Floating = 0,   // Floating digital input.
  IN_No = 0,         // Floating digital input.
  IN_PU = 1,
  IN_PD = 2

} GpioMode_t;



//- Metoda dzielenia polowkowego/binarnego, algorytm z wiki:
//zamienia maske na nr pinu
uint16_t ctz(uint16_t x)
{
  uint8_t n=0;
  if ( x == 0)   return 16;

  if ((x & 0x00FF) == 0) {n = n + 8;   x = x >> 8; }
  if ((x & 0x000F) == 0) {n = n + 4;   x = x >> 4; }
  if ((x & 0x0003) == 0) {n = n + 2;   x = x >> 2; }
  if ((x & 0x0001) == 0)  n = n + 1;
return n;
}


//--------------------------------------------------------------------
//  MODE OT speed PUPDR  Wszystkich kombinacji out = 36 + 4 in
//   6 5  4   3 2   1 0
//------------------------------------------------------------------
void gpio_pin_cfg(GPIO_TypeDef * const __restrict__ port, GpioPin_t pin, GpioMode_t mode)
{
    uint32_t reset_mask, reg_val;
    uint8_t pinx;

  if (mode & 0x010u) port->OTYPER |= pin;   // OD open drain
  else port->OTYPER &= (uint32_t)~pin;      // PP push-pull

  //pin = __builtin_ctz(pin)*2;       //na razie nie dziala
  pinx = ctz(pin)*2;                  // moja procedura dziala

  reset_mask = ~(0x0003u << pinx);    //maska 2 bity przesunieta  na odpow miejsce

  reg_val = port->MODER;
  reg_val &= reset_mask;
  reg_val |= (((mode & 0x060u) >> 5u) << pinx );
  port->MODER = reg_val;

  reg_val = port->PUPDR;
  reg_val &= reset_mask;
  reg_val |= ((mode & 0x03u) << pinx );
  port->PUPDR = reg_val;

  reg_val = port->OSPEEDR;
  reg_val &= reset_mask;
  reg_val |= (((mode & 0x0Cu) >> 2u) << pinx);
  port->OSPEEDR = reg_val;

}
//*************************************************************************
//procedura do ustawiana funkcji alternatywnej gpioA i B (i C dla 072)
void gpio_af_cfg(GPIO_TypeDef * const __restrict__ port, GpioPin_t pin, uint8_t nr)
{
    uint32_t reg_val;
    volatile uint32_t * reg_adr;
    uint8_t pinx;

   //pin = __builtin_ctz(pin)*2;  //na razie nie działa
  pinx = ctz(pin)*4;    // moja procedura dziala

  //reset_mask = ~(0x0003u << pin);    //maska 2 bity przesunieta  na odpow miejsce

  reg_adr = &port->AFR[0];
  if ( pinx > 28) { pinx -= 32; reg_adr = &port->AFR[1]; }  //tu był blad !!!

  reg_val = *reg_adr;
  reg_val &= ~(0x0fu << pinx);                 //kasuj 4 bity maski
  reg_val |= (uint32_t)(nr & 0x07ul) << pinx;  //tylko 3 bity
  *reg_adr = reg_val;


}


//========================================================================
