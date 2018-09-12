#pragma once

#include "RCC.h"
#include "DebugVar.h"
#include "flash.h"
#include "literals.h"
#include "modbusSlave.h"
#include <algorithm>
#include <iterator>


///////////////////////////////////////////////////////////////////////////////
// модбас
///////////////////////////////////////////////////////////////////////////////
using RXpin  = PA3;
using TXpin  = PA2;
using RTSpin = PA1;
using LEDpin = EmptyPin;
const size_t bufSize = 255;
using USART_ = USART<USART1, bufSize, RXpin, TXpin, RTSpin, LEDpin>;
USART_ uart;

struct Operation {
   enum Mode     {auto_mode = 0b0, manual_mode};
   enum Braking  {slow_stop = 0b0, fast_stop};
   enum Speed    {slow = 0b0, fast};
   enum Direct_h {right = 0b0, left};
   enum Direct_v {down = 0b0, up};
   bool enable      :1; // Bit 0 Start: prohibit (0), allow (1) a job;
   bool stop_h      :1; // Bit 1 Stop_h: stop horizontal movement (1);
   bool stop_v      :1; // Bit 2 Stop_v: stop vertical movement (1);
   Braking braking  :1; // Bit 3 Braking: fast_stop (0), slow_stop (1);
   Speed speed      :1; // Bit 4 Speed: slow (0), fast (1);
   Direct_h direct_h:1; // Bit 5 Direct_h: right (0), left (1);
   Direct_v direct_v:1; // Bit 6 Direct_v: down (0), up (1)
   Mode mode        :1; // Bit 7 Mode: auto mode (0), manual mode (1);
   uint16_t res     :8; // Bits 15:8 res: Reserved, must be kept cleared
};
struct Sensors {
   bool right     :1;  // Bit 0 Right: Right Emergency Sensor
   bool left      :1;  // Bit 1 Left: Left Emergensy Sensor
   bool tilt      :1;  // Bit 2 Tilt: Emergensy Sensor Tilt
   bool origin    :1;  // Bit 3 Origin: Origin Sensor
   bool up        :1;  // Bit 4 Up: Up Sensor
   bool down      :1;  // Bit 5 Down: Down Sensor
   uint16_t res   :10; // Bits 15:6 res: Reserved, must be kept cleared
};
struct Zone {
   bool _1  :1;
   bool _2  :1;
   bool _3  :1;
   bool _4  :1;
   bool _5  :1;
   bool _6  :1;
   bool _7  :1;
   bool _8  :1;
   bool _9  :1;
   bool _10 :1;
   bool _11 :1;
   bool _12 :1;
   bool _13 :1;
   bool _14 :1;
   bool _15 :1;
   bool _16 :1;
};
//struct Zone_Coordinate {
//   
//};
struct InRegs {
   USART_::sSettings uartSet;
   uint16_t  modbusAddress;
   uint16_t  password;
   uint16_t  factoryNumber;
   int16_t   coordinate;
   int16_t   origin;
   uint16_t  brake;
   int16_t   time_pause;
   Operation operation;
   Zone      zone;
   int16_t   zone_coordinate[16];
};
struct OutRegs {
   uint16_t          deviceCode;
   uint16_t          factoryNumber;
   USART_::sSettings uartSet;
   uint16_t          modbusAddress;
   int16_t           coordinate;
   Sensors           sensors;
};
MBslave<InRegs, OutRegs, USART_> modbus {uart};



///////////////////////////////////////////////////////////////////////////////
// энергонезависимые данные
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
   uint8_t  modbusAddress      =  1;
   uint16_t brake              =  100;
   uint16_t time_pause         =  3000;
   int16_t  origin             =  0;
   int16_t  min_coordinate     = -16384;
   int16_t  max_coordinate     =  16384;
   int16_t  zone_coordinate[16] { -16384 };//одним числом
};
auto flash = Flash<FlashData, 20_page> ( FlashData{} );







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
