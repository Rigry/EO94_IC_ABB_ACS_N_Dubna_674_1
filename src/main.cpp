#include "init.h"
#include "ring_buffer.h"

/// реакция на изменение входных регистров модбаса
void reaction (uint16_t registrAddress);

// для отладки
RingBuffer<30> ring;


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
      ring.push (SysTick->VAL);
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

      default: break;
   }
}
