#pragma once

#include "pin.h"
#include "timers.h"

template <class Speed, class Start, class Side, class Stop>
class Horizontal 
{
   volatile enum State {wait, right_fast, right_slow, left_fast, left_slow, braking}
   state {State::wait};
   int16_t begin_braking {0};
public:
   int16_t coordinate {0};
   int16_t delta_coordinate;
   int16_t current_coordinate;//encoder&
   Horizontal (int16_t delta_coordinate);
   void stop();
   void move(int16_t coordinate);
   void operator() ();
};

template <class Speed, class Start, class Side, class Stop>
Horizontal <Speed, Start, Side, Stop>::Horizontal (int16_t delta_coordinate)
   : delta_coordinate {delta_coordinate}
{
   Speed ::template configure<PinConf_t::Output>();
   Start ::template configure<PinConf_t::Output>();
   Side  ::template configure<PinConf_t::Output>();
   Stop  ::template configure<PinConf_t::Output>();
}

template <class Speed, class Start, class Side, class Stop>
void Horizontal <Speed, Start, Side, Stop>::stop()
{
   Stop ::set();
   Start::clear();
   state = State::wait;
}

template <class Speed, class Start, class Side, class Stop>
void Horizontal <Speed, Start, Side, Stop>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
}

template <class Speed, class Start, class Side, class Stop>
void Horizontal <Speed, Start, Side, Stop>::operator() ()
{
   switch (state) {
      case wait:
         if (coordinate < current_coordinate) {
            if ((current_coordinate - coordinate) < 2*delta_coordinate)
               state = State::left_slow;
            else if ((current_coordinate - coordinate) > 2*delta_coordinate)
               state = State::left_fast;
            Start::set();
         } else if (coordinate > current_coordinate) {
            if ((coordinate - current_coordinate) < 2*delta_coordinate)
               state = State::right_slow;
            else if ((coordinate - current_coordinate) > 2*delta_coordinate)
               state = State::right_fast;
            Start::set();
         }
      break;
      case right_fast:
         Side::clear();
         Speed::set();
         if (current_coordinate == (coordinate - delta_coordinate) or coordinate < current_coordinate)
            state = State::braking;
      break;
      case right_slow:
         Side::clear();
         Speed::clear();
         if (current_coordinate == coordinate 
             or coordinate < current_coordinate) {
            Stop::set();
            Start::clear();
            state = State::wait;
            }
      break;
      case left_fast:
         Side::set();
         Speed::set();
         if (current_coordinate == (coordinate + delta_coordinate) or coordinate > current_coordinate)
            state = State::braking;
      break;
      case left_slow:
         Side::set();
         Speed::clear();
         if (current_coordinate == coordinate 
             or coordinate > current_coordinate) {
            Stop::set();
            Start::clear();
            state = State::wait;
         }
      break;
      case braking:
         Stop::clear();
         begin_braking = current_coordinate;
         if (current_coordinate == coordinate 
            or current_coordinate == (begin_braking + delta_coordinate)) {
            Stop::set();
            Start::clear();
            state = State::wait;
         } else if (coordinate < current_coordinate) {
                  if ((current_coordinate - coordinate) < 2*delta_coordinate)
                     state = State::left_slow;
                  else if ((current_coordinate - coordinate) > 2*delta_coordinate)
                        state = State::left_fast;
            Start::set();
         } else if (coordinate > current_coordinate) {
            if ((current_coordinate - coordinate) < 2*delta_coordinate) {
               state = State::right_slow;
            } else if ((coordinate - current_coordinate) > 2*delta_coordinate) {
               state = State::right_fast;
            }
            Start::set();
         }
      break;
   }
}

