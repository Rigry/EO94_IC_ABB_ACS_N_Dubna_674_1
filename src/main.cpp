// #define STM32F030x6

// #include "stm32f0xx.h"
// #include <iostream>
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
#include "encoder_.h"
#include <boost/preprocessor/iteration/local.hpp>

/// реакция на изменение входных регистров модбаса
void reaction (uint16_t registrAddress);

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
using Tilt        = DI8;
using Up          = DO7;
using Down        = DO8;
using Speed       = DO1;
using Launch      = DO2;
using Side        = DO3;
using Finish      = DO4;

// int16_t min_coordinate;
// int16_t max_coordinate;
// int16_t origin;
// int16_t encoder;

int main(void)
{
   using Encoder    = Encoder   <TIM1, DI2, DI1, false>;
   using Control    = Control   <Speed, Launch, Side, Finish, Up, Down>;
   using Horizontal = Horizontal<Control, Encoder>;
   using Vertical   = Vertical  <Control, Sense_up, Sense_down>;
   

   makeDebugVar();
   Control control {modbus.outRegs.states};
   control.init();

   #define ADR(reg) GET_ADR(InRegs, reg)

   modbus.outRegs.deviceCode        = 7; // см ЭО-76
   modbus.outRegs.factoryNumber     = flash.factoryNumber;
   modbus.outRegs.uartSet           = flash.uartSet;
   modbus.outRegs.modbusAddress     = modbus.address = flash.modbusAddress;
   modbus.outRegs.min_coordinate    = flash.min_coordinate;
   modbus.outRegs.max_coordinate    = flash.max_coordinate;
   modbus.arInRegsMax[ADR(uartSet)] = 0b111111;
   modbus.inRegsMin.modbusAddress   = 1;
   modbus.inRegsMax.modbusAddress   = 255;
   modbus.inRegsMin.coordinate      = flash.min_coordinate;
   modbus.inRegsMax.coordinate      = flash.max_coordinate;
   modbus.inRegsMax.brake           = 12000;

   modbus.init (flash.uartSet);

   Encoder encoder;
   Horizontal horizontal {control, encoder, flash.brake};
   Vertical vertical {control, flash.time_pause};
//    Encoder encoder;
   Automatic   <Horizontal, Vertical, Encoder> automatic {horizontal, vertical, encoder};
   Calibration <Control, Sense_up, Sense_left, Sense_right, Encoder> calibration {control, encoder, flash.min_coordinate, flash.max_coordinate};
   Search      <Control, Sense_up, Sense_left, Sense_right, Origin, Encoder> search {control, encoder};
   Manual      <Control, Sense_up, Sense_down, Sense_left, Sense_right> manual {control};
   
   enum State {Wait, Search, Automatic, Calibration, Manual, Emergency} state {State::Wait};
   bool lost_coordinate {false};

   while (1)
   {
      switch (state) {
         case Wait:
         // wait enable from modbus
         break;
         case Search:
            if (Sense_up::isClear()) {
               search.stop();
               state = State::Emergency;
               modbus.outRegs.states.mode = States::Mode::emergensy;
            } else if (Sense_up::isSet()) {
               search();
               if (search.is_done()) {
                  lost_coordinate = false;
                  search.reset();
                  state = State::Automatic;
                  modbus.outRegs.states.mode = States::Mode::auto_mode;
               } else if (search.not_found()) {
                  search.stop();
                  state = State::Emergency;
                  modbus.outRegs.states.mode = States::Mode::emergensy;
               } else if (Tilt::isSet()) {
                  lost_coordinate = true;
                  search.stop();
                  state = State::Emergency;
                  modbus.outRegs.states.mode = States::Mode::emergensy;
               }
            }
         break;
         case Automatic:
            if (lost_coordinate)
               state = State::Search;
            else if (Tilt::isSet()) {
               lost_coordinate = true;
               automatic.stop();
               state = State::Emergency;
               modbus.outRegs.states.mode = States::Mode::emergensy;
            } else if (Sense_left::isSet() and encoder != flash.min_coordinate) {
               automatic.stop();
               encoder = flash.min_coordinate;
            } else if (Sense_right::isSet() and encoder != flash.max_coordinate) {
               automatic.stop();
               encoder = flash.max_coordinate;
            } else 
            automatic();
         break;
         case Calibration:
            if (Sense_up::isClear()) {
               calibration.stop();
               state = State::Emergency;
               modbus.outRegs.states.mode = States::Mode::emergensy;
            } else if (Sense_up::isSet() and Origin::isSet()) {
               calibration();
               if (calibration.done()) {
                  modbus.outRegs.min_coordinate = flash.min_coordinate;
                  modbus.outRegs.max_coordinate = flash.max_coordinate;
                  calibration.reset();
                  state = State::Automatic;
                  modbus.outRegs.states.mode = States::Mode::auto_mode;
               } else if (Tilt::isSet()) {
                  lost_coordinate = true;
                  calibration.stop();
                  state = State::Emergency;
                  modbus.outRegs.states.mode = States::Mode::emergensy;
               }
            }
         break;
         case Emergency:
         break;
         case Manual:
         break;
      }

      modbus.outRegs.sensors.sense_up    = Sense_up   ::isSet();
      modbus.outRegs.sensors.sense_down  = Sense_down ::isSet();
      modbus.outRegs.sensors.sense_right = Sense_right::isSet();
      modbus.outRegs.sensors.sense_left  = Sense_left ::isSet();
      modbus.outRegs.sensors.origin      = Origin     ::isSet();
      modbus.outRegs.sensors.tilt        = Tilt       ::isSet();

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
                  automatic.move(modbus.inRegs.coordinate);
               break;

            // case ADR (origin):
               // flash.origin = modbus.inRegs.origin;
               // break;

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
               if (modbus.inRegs.operation.enable)
                  state = State::Search;
               if (state == State::Search) {
                  if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                     search.reset();
                     modbus.outRegs.states.mode = States::Mode::manual_mode;
                     state = State::Manual;
                  }
               }
               if (state == State::Automatic) {
                  if (modbus.inRegs.operation.stop_h or modbus.inRegs.operation.stop_v) {
                     automatic.reset();
                  }
                  if (modbus.inRegs.operation.direct_v == Operation::Direct_v::up) {
                     automatic.move();
                  } else if (modbus.inRegs.operation.direct_v == Operation::Direct_v::down) {
                     automatic.move();
                  }
                  if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                     automatic.reset();
                     modbus.outRegs.states.mode = States::Mode::manual_mode;
                     state = State::Manual;
                  } else if (modbus.inRegs.operation.mode == Operation::Mode::calibration) {
                     automatic.reset();
                     modbus.outRegs.states.mode = States::Mode::calibration;
                     state = State::Calibration;
                  }
               }
               if (state == State::Manual) {
                  if (modbus.inRegs.operation.stop_h) {
                     manual.stop_h();
                  }
                  if (modbus.inRegs.operation.stop_v) {
                     manual.stop_v();
                  }
                  if (modbus.inRegs.operation.direct_v == Operation::Direct_v::up) {
                     manual.up();
                  } else if (modbus.inRegs.operation.direct_v == Operation::Direct_v::down) {
                     manual.down();
                  }
                  if (modbus.inRegs.operation.braking == Operation::Braking::slow_stop) {
                     manual.slow_stop();
                  } else if (modbus.inRegs.operation.braking == Operation::Braking::fast_stop) {
                     manual.fast_stop();
                  }
                  if (modbus.inRegs.operation.speed == Operation::Speed::slow) {
                     manual.slow();
                  } else if (modbus.inRegs.operation.speed == Operation::Speed::fast) {
                     manual.fast();
                  }
                  if (modbus.inRegs.operation.direct_h == Operation::Direct_h::right) {
                     manual.right();
                  } else if (modbus.inRegs.operation.direct_h == Operation::Direct_h::left) {
                     manual.left();
                  }
                  if (modbus.inRegs.operation.mode == Operation::Mode::auto_mode and Tilt::isClear()) {
                     manual.reset();
                     modbus.outRegs.states.mode = States::Mode::auto_mode;
                     state = State::Automatic;
                  }
               if (state == State::Emergency)
                  if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                     modbus.outRegs.states.mode = States::Mode::manual_mode;
                     state = State::Manual;
                  }
               }
            break;

            // case ADR(zone):
            //    automatic.zone[0]  = modbus.inRegs.zone._0 ;
            //    automatic.zone[1]  = modbus.inRegs.zone._1 ;
            //    automatic.zone[2]  = modbus.inRegs.zone._2 ;
            //    automatic.zone[3]  = modbus.inRegs.zone._3 ;
            //    automatic.zone[4]  = modbus.inRegs.zone._4 ;
            //    automatic.zone[5]  = modbus.inRegs.zone._5 ;
            //    automatic.zone[6]  = modbus.inRegs.zone._6 ;
            //    automatic.zone[7]  = modbus.inRegs.zone._7 ;
            //    automatic.zone[8]  = modbus.inRegs.zone._8 ;
            //    automatic.zone[9]  = modbus.inRegs.zone._9;
            //    automatic.zone[10] = modbus.inRegs.zone._10;
            //    automatic.zone[11] = modbus.inRegs.zone._11;
            //    automatic.zone[12] = modbus.inRegs.zone._12;
            //    automatic.zone[13] = modbus.inRegs.zone._13;
            //    automatic.zone[14] = modbus.inRegs.zone._14;
            //    automatic.zone[15] = modbus.inRegs.zone._15;
            // break;
     
            // #define BOOST_PP_LOCAL_MACRO(n) case ADR(zone_coordinate)+n:
            // flash.zone_coordinate[n] = modbus.inRegs.zone_coordinate[n];break;
            // #define BOOST_PP_LOCAL_LIMITS (1, 15)
            // #include BOOST_PP_LOCAL_ITERATE()
            
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

