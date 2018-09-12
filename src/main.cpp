// #include <iostream>
#include "init.h"
#include "vertical.h"
#include "horizontal.h"
#include "ring_buffer.h"
#include "pin.h"
// #include "global.h"
#include "search.h"
#include "automatic.h"
#include "calibration.h"
// #include "manual.h"
#include "control.h"
#include <boost/preprocessor/iteration/local.hpp>
// #define BOOTS_PP_LOCAL_MACRO(n) 
// ADR(zone_coordinate + n):flash.zone_coordinate[n] = modbus.inRegs.zone_coordinate[n];
// #define BOOTS_ (0,15)

/// реакция на изменение входных регистров модбаса
void reaction (uint16_t registrAddress);



// для отладки
// RingBuffer<30> ring;
using SenseUp    = PB2;
using SenseDown  = PB4;
using SenseRight = PA10;
using SenseLeft  = PA11;
using Origin     = PA12;
using Tilt       = PA14;
using ContrlUp   = PB3;
using ContrlDown = PB5;
using Speed      = PA13;
using Launch     = PA15;
using Side       = PB0;
using Finish     = PB1;


int16_t min_coordinate;
int16_t max_coordinate;
// int16_t origin;
int16_t encoder;

int main(void)
{
   using Control = Control <Speed, Launch, Side, Finish>;
   makeDebugVar();
   Control::init();

   #define ADR(reg) GET_ADR(InRegs, reg)

   modbus.outRegs.deviceCode        = 7; // см ЭО-76
   modbus.outRegs.factoryNumber     = flash.factoryNumber;
   modbus.outRegs.uartSet           = flash.uartSet;
   modbus.outRegs.modbusAddress     = modbus.address = flash.modbusAddress;
   modbus.arInRegsMax[ADR(uartSet)] = 0b111111;
   modbus.inRegsMin.modbusAddress   = 1;
   modbus.inRegsMax.modbusAddress   = 255;

   modbus.init (flash.uartSet);
   using Horizontal = Horizontal<Control, int16_t>;
   using Vertical   = Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>;
   Horizontal horizontal {encoder, flash.brake};
   Vertical vertical {flash.time_pause};
   Automatic <Horizontal, Vertical, int16_t, Control> automatic {flash.zone_coordinate, horizontal, vertical, encoder};
   Calibration <Control, SenseLeft, SenseRight, Origin> calibration;
   Search <Control, SenseLeft, SenseRight, Origin> search;
   
   enum State {Search, Automatic, Calibration, Manual, Emergency} state {State::Search};

   while (1)
   {
      
      // switch (state) { //automatic and manual are called from modbus
      //    case Search:
      //       search();
      //       if (origin::isSet()) {
      //          encoder = flash.origin;
      //          state = State::Automatic;
      //       } else if ((SenseRight::isSet() or Tilt::isSet()) {
      //          state = State::Emergency;
      //       }
      //    break;
      //    case Automatic:
      //       if (SenseLeft::isSet() or SenseRight::isSet() or Tilt::isSet())
      //          state = State::Emergency;
      //       else if (true /*coordinate lost*/)
      //          state = State::Search;
      //       else if (true /*калибровка*/)
      //          state = State::Calibration;
      //    break;
      //    case Calibration:
      //       calibration();
      //       if (SenseLeft::isSet())
      //          flash.min_coordinate = encoder;
      //       if (SenseRight::isSet())
      //          flash.max_coordinate = encoder;
      //       if (Origin::isSet()) {
      //          state = State::Automatic;
      //       } else if (Tilt::isSet())
      //          state = State::Emergency;
      //    break;
      //    case Emergency:
      //       horizontal.main_stop();
      //       vertical.main_stop();
      //    break;
      // }
      automatic();
      search();
      calibration();
      modbus.outRegs.sensors.up     = SenseUp   ::isSet();
      modbus.outRegs.sensors.down   = SenseDown ::isSet();
      modbus.outRegs.sensors.right  = SenseRight::isSet();
      modbus.outRegs.sensors.left   = SenseLeft ::isSet();
      modbus.outRegs.sensors.origin = Origin    ::isSet();
      modbus.outRegs.sensors.tilt   = Tilt      ::isSet();

      modbus ([&](uint16_t registrAddress) {
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
               horizontal.move(modbus.inRegs.coordinate);
               break;

            case ADR (origin):
               flash.origin = modbus.inRegs.origin;
               break;

            case ADR (brake):
               flash.brake
                  = horizontal.brake
                  = modbus.inRegs.brake;
               break;

            case ADR (time_pause):
               flash.time_pause
                  = vertical.time_pause
                  = modbus.inRegs.time_pause;
               break;

            case ADR (operation):
               if (modbus.inRegs.operation.enable) {
                  Control::start();
               } else if (not modbus.inRegs.operation.enable) {
                  Control::stop();
               }
               if (modbus.inRegs.operation.stop_h) {
                  horizontal.stop();
               }
               if (modbus.inRegs.operation.stop_v) {
                  vertical.stop();
               }
               if (modbus.inRegs.operation.braking == Operation::Braking::slow_stop) {
                  Control::slow_stop();
               } else if (modbus.inRegs.operation.braking == Operation::Braking::fast_stop) {
                  Control::fast_stop();
               }
               if (modbus.inRegs.operation.speed == Operation::Speed::slow) {
                  Control::slow();
               } else if (modbus.inRegs.operation.speed == Operation::Speed::fast) {
                  Control::fast();
               }
               if (modbus.inRegs.operation.direct_h == Operation::Direct_h::right) {
                  Control::right();
               } else if (modbus.inRegs.operation.direct_h == Operation::Direct_h::left) {
                  Control::left();
               }
                if (modbus.inRegs.operation.direct_v == Operation::Direct_v::up) {
                  vertical.up();
               } else if (modbus.inRegs.operation.direct_v == Operation::Direct_v::down) {
                  vertical.down();
               }
               if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                  state = State::Manual;
               }
               else if (modbus.inRegs.operation.mode == Operation::Mode::auto_mode) {
                  state = State::Automatic;
               }
               break;
            #define BOOST_PP_LOCAL_MACRO(n) case ADR(zone_coordinate)+n:\
            flash.zone_coordinate[n] = modbus.inRegs.zone_coordinate[n];break;
            #define BOOST_PP_LOCAL_LIMITS (1, 15)
            #include BOOST_PP_LOCAL_ITERATE()
            
            default: break;
         }
      });
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

}
