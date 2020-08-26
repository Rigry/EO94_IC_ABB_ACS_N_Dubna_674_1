#pragma once

#include "pin.h"
#include "timers.h"
#include <stdint.h>

/// Class Vertical служит для задания направления движения каретки с помощью методов up и down. 
/// Время паузы между сменой направления, time, задается конструктором.
template <class Control, class Sense_up, class Sense_middle, class Sense_down>
class Swing : Subscriber 
{
   volatile enum State {Wait, Middle, Down, Pause_Wait, Pause_Middle, Pause_Down} state {State::Wait};
   void notify() override;
   bool subscribe {false};
   void wake();
   void sleep();
   uint16_t time_count {0};
   Control& control;
   uint16_t qty_swings {0};
   bool done{false};
public:
   uint16_t time_pause;
   Swing (Control& control, uint16_t time_pause);
   void stop();
   void start(uint16_t qty_swings);
   void reset(){done = false;};
   bool is_done(){return done;}
   bool isWorking(){return not ((state == State::Wait) or (state == State::Pause_Wait));}
};

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
Swing<Control, Sense_up, Sense_middle, Sense_down>::Swing (Control &control, uint16_t time_pause)
   : control   {control}
   , time_pause{time_pause}
{}

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
void Swing<Control, Sense_up, Sense_middle, Sense_down>::stop ()
{
   if (state != State::Wait) {
      control.stop_v();
      state = State::Pause_Wait;
   }
} 

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
void Swing<Control, Sense_up, Sense_middle, Sense_down>::start (uint16_t qty_swings)
{
   if (not done and qty_swings) {
      if ((not Sense_up::isSet()) and not (Sense_middle::isSet()) and state == State::Wait) {
         control.up();
         state = State::Middle;
         this->qty_swings = qty_swings;
         wake();
      } else if (Sense_middle::isSet() and state == State::Wait) {
         control.down();
         state = State::Down;
         this->qty_swings = qty_swings;
         wake();
      } else if (Sense_up::isSet() and state == State::Wait) {
         state = State::Wait;
      } else if (state == State::Pause_Middle) {
            state = State::Pause_Middle;
      } else if (state == State::Pause_Down) {
         state = State::Pause_Down;
      } else if (state == State::Pause_Wait) {
            state = State::Pause_Wait;
      }
   } else if (qty_swings == 0) {
      done = false;
      state = State::Wait;
   }
}

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
void Swing<Control, Sense_up, Sense_middle, Sense_down>::wake ()
{
   if (not subscribe) {
      tickUpdater.subscribe(this);
      subscribe = true;
   }
}

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
void Swing<Control, Sense_up, Sense_middle, Sense_down>::sleep ()
{
   if (subscribe) {
      tickUpdater.unsubscribe(this);
      subscribe = false;
   }
}

template <class Control, class Sense_up, class Sense_middle, class Sense_down>
void Swing<Control, Sense_up, Sense_middle, Sense_down>::notify()
{
   switch (state) {
      case Wait:
         sleep();
      break;
      case Middle:
         if (Sense_middle::isSet()) {
            control.stop_v();
            state = State::Pause_Down;
         }
      break;
      case Down:
         if (Sense_down::isSet()) {
            qty_swings--;
            control.stop_v();
            if (qty_swings)
               state = State::Pause_Middle;
            else 
               state = Pause_Wait;
         }
      break;
      case Pause_Middle:
         if (++time_count >= time_pause) {
            state = State::Middle;
            control.up();
            time_count = 0;
         }
      break;
      case Pause_Down:
         if (++time_count >= time_pause) {
            state = State::Down;
            control.down();
            time_count = 0;
         }
      break;
      case Pause_Wait:
         if (++time_count >= time_pause) {
            state = State::Wait;
            time_count = 0;
            done = true;
         }
      break;
   }
}


