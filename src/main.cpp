#include "init.h"
#include "vertical.h"
#include "horizontal.h"
#include "ring_buffer.h"
#include "pin.h"
#include "global.h"
#include "search.h"
#include "automatic.h"
#include "calibration.h"
#include "manual.h"

/// реакция на изменение входных регистров модбаса
void reaction (uint16_t registrAddress);

// для отладки
// RingBuffer<30> ring;
using SenseRight = PA10;
using SenseLeft  = PA11;
using Origin     = PA12;
using Speed      = PA13;
using Tilt       = PA14;
using Start      = PA15;
using Side       = PB0;
using Stop       = PB1;

// <SenseRight> ::clockEnable;
// SenseLeft  ::clockEnable();
// Origin     ::clockEnable();

int16_t min_coordinate;
int16_t max_coordinate;

Horizontal <Speed, Start, Side, Stop> horizontal {flash.delta_coordinate};



int main(void)
{
   makeDebugVar();

   #define ADR(reg) GET_ADR(InRegs, reg)

   modbus.outRegs.deviceCode        = 7; // см ЭО-76
   modbus.outRegs.factoryNumber     = flash.factoryNumber;
   modbus.outRegs.uartSet           = flash.uartSet;
   modbus.outRegs.modbusAddress     = modbus.address = flash.modbusAddress;
   modbus.arInRegsMax[ADR(uartSet)] = 0b111111;
   modbus.inRegsMin.modbusAddress   = 1;
   modbus.inRegsMax.modbusAddress   = 255;

   modbus.init (flash.uartSet);

   while (1)
   {
      modbus (reaction);
      // для отладки
      // ring.push (SysTick->VAL);
      // спим до прерывания
      __WFI();
   }
}

///////////////////////////////////////////////////////////////////////////////
// РЕАКЦИЯ НА КОМАНДЫ МОДБАС
///////////////////////////////////////////////////////////////////////////////
void reaction (uint16_t registrAddress)
{
   static bool unblock = false;
   switch ( registrAddress ) {

      case ADR(uartSet):
         flash.uartSet
            = modbus.outRegs.uartSet
            = modbus.inRegs.uartSet;
         break;

      case ADR(modbusAddress):
         flash.modbusAddress 
            = modbus.outRegs.modbusAddress
            = modbus.inRegs.modbusAddress;
         break;

      case ADR(password):
         unblock = modbus.inRegs.password == 208;
         break;

      case ADR(factoryNumber):
         if (unblock) {
            unblock = false;
            flash.factoryNumber 
               = modbus.outRegs.factoryNumber
               = modbus.inRegs.factoryNumber;
         }
         unblock = true;
         break;

      case ADR (coordinate):
         horizontal.coordinate = modbus.inRegs.coordinate;
         break;

      case ADR (delta_coordinate):
         flash.delta_coordinate
            = horizontal.delta_coordinate
            = modbus.inRegs.delta_coordinate;
         break;

      case ADR (state):
         if (modbus.inRegs.state.mode == Mode::Manual)
            global = Global::Manual_;
         else if (modbus.inRegs.state.mode == Mode::Auto)
            global = Global::Automatic;            
         break;

      case ADR (zone):

         break;
      default: break;
   }
}
