#pragma once

#include "RCC.h"
#include "DebugVar.h"
#include "flash.h"
#include "literals.h"
#include "modbusSlave.h"


///////////////////////////////////////////////////////////////////////////////
// модбас
///////////////////////////////////////////////////////////////////////////////
using RXpin = PA3;
using TXpin = PA2;
using RTSpin = PA1;
using LEDpin = PB0;
const size_t bufSize = 255;
using USART_ = USART<USART1, bufSize, RXpin, TXpin, RTSpin, LEDpin>;
USART_ uart;

struct InRegs {
   USART_::sSettings uartSet;
   uint16_t          modbusAddress;
   uint16_t          password;
   uint16_t          factoryNumber;
};
struct OutRegs {
   uint16_t          deviceCode;
   uint16_t          factoryNumber;
   USART_::sSettings uartSet;
   uint16_t          modbusAddress;
};
auto modbus = MBslave<InRegs, OutRegs, USART_> (uart);



///////////////////////////////////////////////////////////////////////////////
// энергонезависимоые данные
///////////////////////////////////////////////////////////////////////////////
struct FlashData {
   uint16_t factoryNumber = 0;
   USART_::sSettings uartSet = {
      .parityEnable = false,
      .parity       = USART_::sParity::even,
      .stopBits     = USART_::sStopBits::_1, 
      .boudrate     = USART_::sBoudrate::_9600,
      .res          = 0
   };
   uint8_t modbusAddress = 1;
};
const uint8_t flashSector = 20;
auto flash = Flash<FlashData, flashSector> ( FlashData{} );







///////////////////////////////////////////////////////////////////////////////
// эта функция вызывается первой в startup файле
///////////////////////////////////////////////////////////////////////////////
extern "C" void CLKinit (void)
{
   volatile uint8_t i = 0;
   // FLASH->ACR |= FLASH_ACR_PRFTBE;
   FLASH::setLatency (FLASH::Latency::_1);

   RCC::HSEon();
   RCC::waitHSEready();
   while (++i) {}
   RCC::setAHBprescaler (RCC::AHBprescaler::AHBnotdiv);
   RCC::setAPBprecsaler (RCC::APBprescaler::APBnotdiv);
   RCC::setPLLmultiplier (RCC::PLLmultiplier::_6);
   RCC::setPLLsource (RCC::PLLsource::HSE);
   RCC::PLLon();
   RCC::waitPLLready();
   while (++i) {}
   RCC::systemClockSwitch (RCC::SystemClockSwitch::CS_PLL);
   while (++i) {}
}
