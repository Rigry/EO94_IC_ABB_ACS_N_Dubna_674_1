#pragma once
#include "modbusSlave.h"

#define ADR(reg) GET_ADR(InRegs, reg)

template <class Modbus, class Flash, class Encoder, class Horizontal, class Vertical, class Swing, class Manual, class Search, class Automatic,
          class Calibration, class Control, class Origin, class Sense_up, class Sense_middle, class Sense_down, class Tilt_1, class Tilt_2, class Sense_right, class Sense_left>
class Global
{
   enum State {wait_, search_, automatic_, calibration_, manual_, emergency_} state {State::wait_};
   Modbus& modbus;
   Flash& flash;
   Control control;
   Encoder encoder {};
   Horizontal horizontal;
   Vertical vertical;
   Swing swing;
   Manual  manual;
   Search search;
   Automatic automatic;
   Calibration calibration;

   Timer pause;

   bool zero {false};
   bool falling {false};
   bool lost_coordinate {true};
   bool calibration_done {false};
   void state_wait          (){state = State::wait_; modbus.outRegs.states.mode = States::Mode::wait;}
   void state_manual        (){state = State::manual_; modbus.outRegs.states.mode = States::Mode::manual_mode;}
   void state_search        (){state = State::search_; modbus.outRegs.states.mode = States::Mode::search;}
   void state_automatic     (){state = State::automatic_; modbus.outRegs.states.mode = States::Mode::auto_mode;}
   void state_calibration   (){state = State::calibration_; modbus.outRegs.states.mode = States::Mode::calibration;}
   bool state_is_wait       (){return state == State::wait_;}
   bool state_is_manual     (){return state == State::manual_;}
   bool state_is_search     (){return state == State::search_;}
   bool state_is_automatic  (){return state == State::automatic_;}
   bool state_is_emergency  (){return state == State::emergency_;}
   bool state_is_calibration(){return state == State::calibration_;}

public:
   
   // State get () {return state;}
   Global(Modbus& modbus, Flash& flash)
      : modbus      {modbus}
      , flash       {flash}
      , control     {modbus.outRegs.states}
      , horizontal  {control, encoder, flash.brake}
      , vertical    {control, flash.time_pause}
      , swing       {control, flash.time_pause}
      , manual      {control, vertical, encoder}
      , search      {control, encoder}
      , automatic   {horizontal, vertical, swing, encoder}
      , calibration {control, encoder, flash.min_coordinate, flash.max_coordinate}
   {control.init();}
   Encoder get_encoder(){return encoder;}
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
               if (state_is_automatic())
                  automatic.move(modbus.inRegs.coordinate);
            break;
         case ADR (swing):
               if (state_is_automatic())
                  swing.start(modbus.inRegs.swing);
         case ADR (brake):
            flash.brake
               = horizontal.brake
               = modbus.outRegs.brake
               = modbus.inRegs.brake;
            break;
         case ADR (time_pause):
            flash.time_pause
               = vertical.time_pause
               = modbus.inRegs.time_pause;
            break;
         case ADR (operation):
            if (state_is_wait() and modbus.inRegs.operation.enable) {
               state_search();
               modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
            } else if (state_is_search()) {
               if (not modbus.inRegs.operation.enable) {
                  search.stop();
                  state_wait();
                  modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
						break;
               } 
               if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                  search.reset();
                  state_manual();
               }
            } else if (state_is_calibration()) {
               if (not modbus.inRegs.operation.enable) {
                  calibration.stop();
                  state_wait();
                  modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
					}
            } else if (state_is_automatic()) {
               if (not modbus.inRegs.operation.enable) {
                  automatic.stop();
                  state_wait();
                  modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
                  break;
               }
               if (modbus.inRegs.operation.stop_h or modbus.inRegs.operation.stop_v) {
                  automatic.reset();
               }
               if (modbus.inRegs.operation.up and not modbus.inRegs.operation.down) {
                  automatic.move_up();
               } else if (modbus.inRegs.operation.down and not modbus.inRegs.operation.up) {
                  automatic.move_down();
               }
               if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                  automatic.reset();
                  state_manual();
               } else if (modbus.inRegs.operation.mode == Operation::Mode::calibration and not calibration_done) {
                  automatic.reset();
                  state_calibration();
               }
            } else if (state_is_manual()) {
               if (not modbus.inRegs.operation.enable) {
                  manual.stop();
                  state_wait();
                  modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
						break;
               } 
               if (modbus.inRegs.operation.stop_h) {
                  manual.stop_h();
               }
               if (modbus.inRegs.operation.stop_v) {
                  manual.stop_v();
               }
               if (modbus.inRegs.operation.up and not modbus.inRegs.operation.down and not modbus.inRegs.operation.stop_v) {
                  manual.up();
               } else if (modbus.inRegs.operation.down and not modbus.inRegs.operation.up and not modbus.inRegs.operation.stop_v) {
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
               if (modbus.inRegs.operation.right and not modbus.inRegs.operation.left and not modbus.inRegs.operation.stop_h) {
                  manual.right();
               } else if (modbus.inRegs.operation.left and not modbus.inRegs.operation.right and not modbus.inRegs.operation.stop_h) {
                  manual.left();
               }
               if (modbus.inRegs.operation.mode == Operation::Mode::auto_mode and Tilt_1::isSet() and Tilt_2::isSet()) {
                  manual.reset();
                  state_automatic();
               }
            } else if (state_is_emergency()) {
               if (modbus.inRegs.operation.mode == Operation::Mode::manual_mode) {
                  if (pause.done()) {
                     state_manual();
                  }
               }
               if (not modbus.inRegs.operation.enable) {
                  state_wait();
                  modbus.outRegs.states.enable = modbus.inRegs.operation.enable;
               }
            }
            break;
            case ADR (step):
               if (state_is_manual()) {
                  if (modbus.inRegs.step.left)
                     manual.step_left (modbus.inRegs.step.distance);
                  else if (modbus.inRegs.step.right)
                     manual.step_right (modbus.inRegs.step.distance);
               }
            break;
            default: break;
      }
}
   void operator()()
   {
      modbus.outRegs.sensors.sense_up     = Sense_up    ::isSet();
      modbus.outRegs.sensors.sense_middle = Sense_middle::isSet();
      modbus.outRegs.sensors.sense_down   = Sense_down  ::isSet();
      modbus.outRegs.sensors.sense_right  = Sense_right ::isSet();
      modbus.outRegs.sensors.sense_left   = Sense_left  ::isSet();
      modbus.outRegs.sensors.origin       = Origin      ::isSet();
      modbus.outRegs.sensors.tilt         = (Tilt_1::isSet() or Tilt_2::isSet());
      if (modbus.outRegs.sensors.origin) encoder = 0;
      modbus.outRegs.coordinate           = encoder;
      modbus.outRegs.states.enable        = modbus.inRegs.operation.enable;
      modbus.outRegs.states.swing_done    = swing.is_done();
      modbus.outRegs.states.lost          = lost_coordinate;
      modbus.outRegs.states.stop_h        = not Control::Launch_::isSet();
      modbus.outRegs.states.stop_v        = not control.states.up and not control.states.down;
      switch (state) {
            case wait_:
            // wait enable from modbus
            break;
            case search_:
               if (not lost_coordinate) {
                  search.reset();
                     state = State::automatic_;
                     modbus.outRegs.states.mode = States::Mode::auto_mode;
               } else if (Sense_up::isClear()) {
                  search.stop();
                  state = State::emergency_;
                  modbus.outRegs.states.mode = States::Mode::emergency;
               } else if (Sense_up::isSet()) {
                  search();
                  if (search.is_done()) {
                     lost_coordinate = false;
                     modbus.outRegs.states.lost = lost_coordinate;
                     search.reset();
                     state = State::automatic_;
                     modbus.outRegs.states.mode = States::Mode::auto_mode;
                  } else if (search.not_found()) {
                     search.stop();
                     state = State::emergency_;
                     modbus.outRegs.states.mode = States::Mode::emergency;
                  } else if (Tilt_1::isClear() or Tilt_2::isClear()) {
                     falling = true;
                     lost_coordinate = true;
                     modbus.outRegs.states.lost = lost_coordinate;
                     search.stop();
                     state = State::emergency_;
                     modbus.outRegs.states.mode = States::Mode::emergency;
                  }
               }
            break;
            case automatic_:
               if (lost_coordinate) {
                  state = State::search_;
                  modbus.outRegs.states.mode = States::Mode::search;
               } else if (Tilt_1::isClear() or Tilt_2::isClear()) {
                  falling = true;
                  lost_coordinate = true;
                  modbus.outRegs.states.lost = lost_coordinate;
                  automatic.stop();
                  state = State::emergency_;
                  modbus.outRegs.states.mode = States::Mode::emergency;
               } else if (Sense_left::isSet() and encoder != flash.max_coordinate) {
                  automatic.stop();
                  encoder = flash.max_coordinate;
                  modbus.outRegs.coordinate = encoder;
               } else if (Sense_right::isSet() and encoder != flash.min_coordinate) {
                  automatic.stop();
                  encoder = flash.min_coordinate;
                  modbus.outRegs.coordinate = encoder;
               } else if (modbus.inRegs.coordinate == 0 and encoder == 0 and Origin::isClear()){
                  lost_coordinate = true;
                  state = State::search_;
               } else 
                  automatic();
            break;
            case calibration_:
               if (Sense_up::isClear()) {
                  calibration.stop();
                  state = State::emergency_;
                  modbus.outRegs.states.mode = States::Mode::emergency;
               } else if (Sense_up::isSet()) {
                  calibration();
                  if (calibration.done()) {
                     modbus.outRegs.min_coordinate = flash.min_coordinate;
                     modbus.outRegs.max_coordinate = flash.max_coordinate;
                     calibration.reset();
                     state = State::automatic_;
                     modbus.outRegs.states.mode = States::Mode::auto_mode;
                     calibration_done = true;
                  } else if (Tilt_1::isClear() or Tilt_2::isClear()) {
                     falling = true;
                     lost_coordinate = true;
                     modbus.outRegs.states.lost = lost_coordinate;
                     calibration.stop();
                     state = State::emergency_;
                     modbus.outRegs.states.mode = States::Mode::emergency;
                  }
               }
            break;
            case emergency_:
            break;
            case manual_:
               if ((Tilt_1::isClear() or Tilt_2::isClear()) and falling) {
                  manual();
               } else if ((Tilt_1::isClear() or Tilt_2::isClear()) and not falling) {
                  manual.stop();
                  falling = true;
                  lost_coordinate = true;
                  pause.stop();
                  pause.start(2000);
                  modbus.outRegs.states.lost = lost_coordinate;
                  state = State::emergency_;
                  modbus.outRegs.states.mode = States::Mode::emergency;
               } else if (Tilt_1::isSet() and Tilt_2::isClear()) {
                  manual();
                  falling = false;
               }
            break;
         }
   }
};
