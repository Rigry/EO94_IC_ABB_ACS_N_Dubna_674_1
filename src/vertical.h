#pragma once

#include "pin.h"
#include "timers.h"
#include <stdint.h>

/// Class Vertical служит для задания направления движения каретки с помощью методов up и down. 
/// Время паузы между сменой направления, time, задается конструктором.
template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown> 
class Vertical : Subscriber 
{
   volatile enum State {Wait, Up, Down, Pause_Wait, Pause_Up, Pause_Down} state {State::Wait};
   void notify() override;
   bool subscribe {false};
   void wake();
   void sleep();
   uint16_t time_count {0};
public:
   uint16_t time_pause;
   Vertical (uint16_t time_pause);
   void stop();
   void up();
   void down();
   bool isUp();
   bool isDown();
};

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::Vertical (uint16_t time_pause)
   : time_pause{time_pause}
{
   ContrlUp   ::template configure<PinConf_t::Output>();
   ContrlDown ::template configure<PinConf_t::Output>();
}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::stop ()
{
   ContrlUp  ::clear();
   ContrlDown::clear();
   state = State::Pause_Wait;
} 

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::up ()
{
   if ((not SenseUp::isSet()) and state == State::Wait) {
      ContrlUp::set();
      state = State::Up;
      wake();
   } else if (state == State::Down) {
         ContrlDown::clear();
         state = State::Pause_Up;
   } else if (state == State::Pause_Up or state == State::Pause_Down) {
         state = State::Pause_Up;
   } else if (state == State::Pause_Wait) {
         state = State::Pause_Wait;
   }

}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::down ()
{
   if ((not SenseDown::isSet()) and state == State::Wait) {
      ContrlDown::set();
      state = State::Down;
      wake();
   } else if (state == State::Up) {
         ContrlUp::clear();
         state = State::Pause_Down;
   } else if (state == State::Pause_Up or state == State::Pause_Down) {
         state = State::Pause_Down;
   } else if (state == State::Pause_Wait) {
         state = State::Pause_Wait;
   }

}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::wake ()
{
   if (not subscribe) {
      tickUpdater.subscribe(this);
      subscribe = true;
   }
}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::sleep ()
{
   if (subscribe) {
      tickUpdater.unsubscribe(this);
      subscribe = false;
   }
}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
bool Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::isUp ()
{
   return SenseUp::isSet();
}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
bool Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::isDown ()
{
   return SenseDown::isSet();
}

template <class SenseUp, class ContrlUp, class SenseDown, class ContrlDown>
void Vertical<SenseUp, ContrlUp, SenseDown, ContrlDown>::notify()
{
   switch (state) {
      case Wait:
         sleep();
      break;
      case Up:
         if (SenseUp::isSet()) {
            ContrlUp::clear();
            state = State::Pause_Wait;
         }
      break;
      case Down:
         if (SenseDown::isSet()) {
            ContrlDown::clear();
            state = State::Pause_Wait;
         }
      break;
      case Pause_Up:
         if (++time_count >= time_pause) {
            state = State::Up;
            ContrlUp::set();
            time_count = 0;
         }
      break;
      case Pause_Down:
         if (++time_count >= time_pause) {
            state = State::Down;
            ContrlDown::set();
            time_count = 0;
         }
      break;
      case Pause_Wait:
         if (++time_count >= time_pause) {
            state = State::Wait;
            time_count = 0;
         }
      break;
   }
}


