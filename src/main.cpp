#include "init.h"
#include "ring_buffer.h"
#include "pin.h"
#include "search.h"
#include "automatic.h"
#include "calibration.h"
#include "vertical.h"
#include "horizontal.h"
#include "manual.h"
#include "control.h"
#include "global.h"
#include "encoder_.h"

// для отладки
// RingBuffer<30> ring;

using DI1  = PA9 ;
using DI2  = PA8 ;
using DI3  = PB15;
using DI4  = PB14;
using DI5  = PB13;
using DI6  = PB12;
using DI7  = PB11;
using DI8  = PB10;
using DI9  = PB2 ;
using DI10 = PB1 ;
using DO1  = PB0 ;
using DO2  = PA7 ;
using DO3  = PA6 ;
using DO4  = PA5 ;
using DO5  = PB5 ;
using DO6  = PB4 ;
using DO7  = PB3 ;
using DO8  = PA15;

using Sense_up    = DI3;
using Sense_down  = DI4;
using Sense_right = DI5;
using Sense_left  = DI6;
using Origin      = DI7;
using Tilt_1      = DI8;
using Tilt_2      = DI9;
using Up          = DO7;
using Down        = DO8;
using Speed       = DO1;
using Launch      = DO2;
using Side        = DO3;
using Finish      = DO4;

__IO auto pa = reinterpret_cast<PA*>(PA::Base);

int main(void)
{
   using Encoder     = Encoder     <TIM1, DI2, DI1, false>;
   using Control     = Control     <Speed, Launch, Side, Finish, Up, Down>;
   using Horizontal  = Horizontal  <Control, Encoder>;
   using Vertical    = Vertical    <Control, Sense_up, Sense_down>;
   using Automatic   = Automatic   <Horizontal, Vertical, Encoder>;
   using Calibration = Calibration <Control, Sense_left, Sense_right, Encoder>;
   using Search      = Search      <Control, Sense_left, Sense_right, Origin, Encoder>;
   using Manual      = Manual      <Control, Vertical, Sense_left, Sense_right, Encoder>;
   using Modbus      = MBslave     <InRegs, OutRegs, USART_>;
   using Flash       = decltype(flash);
   using Global      = Global      <Modbus, Flash, Encoder, Horizontal, Vertical, Manual, Search, Automatic,
                                    Calibration, Control, Origin, Sense_up, Sense_down, Tilt_1, Tilt_2, Sense_right, Sense_left>;

   Global global {modbus, flash};
   // Control control{modbus.outRegs.states};
   // control.init();

   makeDebugVar();
   pa->doSome();

   modbus.outRegs.deviceCode        = 7; // см ЭО-76
   modbus.outRegs.factoryNumber     = flash.factoryNumber;
   modbus.outRegs.uartSet           = flash.uartSet;
   modbus.outRegs.modbusAddress     = modbus.address = flash.modbusAddress;
   modbus.outRegs.min_coordinate    = flash.min_coordinate;
   modbus.outRegs.max_coordinate    = flash.max_coordinate;
   modbus.outRegs.brake             = flash.brake;
   modbus.arInRegsMax[ADR(uartSet)] = 0b111111;
   modbus.inRegsMin.modbusAddress   = 1;
   modbus.inRegsMax.modbusAddress   = 255;
   modbus.inRegsMin.coordinate      = uint16_t(flash.min_coordinate);
   modbus.inRegsMax.coordinate      = uint16_t(flash.max_coordinate);
   modbus.signed_[ADR(coordinate)]  = true;
   modbus.inRegsMax.brake           = 12000;
   // modbus.inRegs.operation.enable = true;
   modbus.init (flash.uartSet);
   
   // auto x{0};
   // auto y{0};
   // auto z{0};
   // global.reaction(ADR (operation));

   // CONFIGURE_PIN(PA7, Output);
   // PA7::set();
   while (1)
   {
      
      global();
      // x = GPIOA->MODER;
      // y = GPIOB->MODER;
      // const auto& rcc = reinterpret_cast<RCC_TypeDef*>(RCC_BASE);
      // z = rcc->AHBENR; 
      
      modbus ([&](uint16_t registrAddress){global.reaction(registrAddress);});
      // для отладки
      // ring.push (SysTick->VAL);
      // спим до прерывания
      __WFI();
   }
}




