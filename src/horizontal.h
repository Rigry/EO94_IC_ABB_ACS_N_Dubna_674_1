#pragma once
#include <stdint.h>
#include "interrupt.h"

template <class Control, class Encoder>
class Horizontal : Interrupting
{
   volatile enum State {
      wait, right_fast, right_slow, left_fast, left_slow, braking
   } state {State::wait};
   enum Direction {right, left} direction {Direction::right};
   template <Direction> auto few();
   int16_t goal {0};
   Control& control;
   Encoder& encoder;
   int16_t coordinate {0};
   int16_t last_coordinate{0};
public:
   int16_t brake;
   Horizontal (Control& control, Encoder& encoder, int16_t brake);
   bool is_working(){return state != State::wait;}
   void stop ();
   void move (int16_t coordinate);
   void interrupt() override;
};

template <class Control, class Encoder>
Horizontal <Control, Encoder>::Horizontal (Control& control, Encoder& encoder, int16_t brake)
   : control {control}
   , encoder {encoder}
   , brake   {brake}
{encoder.withInterrupt().subscribe(this);}

template <class Control, class Encoder>
template <typename Horizontal<Control, Encoder>::Direction direction>
auto Horizontal <Control, Encoder>::few() 
{
   if constexpr (direction == right)
      return coordinate > goal and coordinate < (encoder + brake);
   else if constexpr (direction == left)
      return coordinate < goal and coordinate > (encoder - brake); 
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::stop()
{
   if (state != State::wait) {
      control.fast_stop();
      control.stop_h();
      state = State::wait;
   }
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
   if (coordinate != last_coordinate) {
      switch (state) {
         case wait:
            encoder.enableInterrupt();
            if (coordinate < encoder) {
               control.right();
               if (coordinate >= (encoder - brake)) {
                  control.slow();
                  control.fast_stop();
                  encoder.setCompare(coordinate);
                  state = State::right_slow;
               } else if (coordinate < (encoder - brake)) {
                  control.fast();
                  control.slow_stop();
                  encoder.setCompare(coordinate + brake);
                  state = State::right_fast;
               } 
               control.start();
            } else if (coordinate > encoder) {
               control.left();
               if (coordinate <= (encoder + brake)) {
                  control.slow();
                  control.fast_stop();
                  encoder.setCompare(coordinate);
                  state = State::left_slow;
               } else if (coordinate > (encoder + brake)) {
                  control.fast();
                  control.slow_stop();
                  encoder.setCompare(coordinate - brake);
                  state = State::left_fast;
               } 
               control.start();
            }
         break;
         case right_fast:
            if (coordinate <= (encoder - brake)) {
               control.slow();
               goal = encoder + brake;
               encoder.setCompare(goal);
               state = State::braking;
            } else encoder.setCompare(coordinate + brake);
         break;
         case right_slow:
            if (coordinate < encoder) {
               control.fast_stop();
               control.stop_h();
               state = State::wait;
               move (coordinate);
            } else if (coordinate >= (encoder - brake)) {
               encoder.setCompare(coordinate + brake);
               control.fast();
               state = State::right_fast;
            }
         break;
         case left_fast:
            if (coordinate >= (encoder + brake)) {
               encoder.setCompare(coordinate - brake);
               control.slow();
               goal = encoder - brake;
               encoder.setCompare(goal);
               state = State::braking;
            } else encoder.setCompare(coordinate - brake);
         break;
         case left_slow:
            if (coordinate > encoder) {
               control.fast_stop();
               control.stop_h();
               state = State::wait;
               move (coordinate);
            } else if (coordinate <= (encoder + brake)) {
               encoder.setCompare(coordinate - brake);
               control.fast();
               state = State::left_fast;
            } 
         break;
         case braking:
            if (control.is_right() and few<right>()) {
               goal = coordinate;
            } else if (control.is_right() and coordinate > (encoder - brake)) {
               control.fast();
               encoder.setCompare(coordinate + brake);
               state = State::right_fast;
            } else if (control.is_left() and few<left>()) {
               goal = coordinate;
            } else if (control.is_left() and coordinate < (encoder + brake)) {
               control.fast();
               encoder.setCompare(coordinate - brake);
               state = State::left_fast;
            }
         break;
      }
   }
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::interrupt()
{
   switch (state) {
      case wait:
      break;
      case right_fast:
         // if (encoder == (coordinate - brake)) 
            goal = coordinate;
            encoder.setCompare(goal);
            state = State::braking;
            control.slow();
      break;
      case right_slow:
         // if (encoder == coordinate) 
            control.stop_h();
            last_coordinate = coordinate;
            state = State::wait;
            encoder.disableInterrupt();
      break;
      case left_fast:
         // if (encoder == (coordinate + brake))
            goal = coordinate;
            encoder.setCompare(goal);
            state = State::braking;
            control.slow();
      break;
      case left_slow:
         // if (encoder == coordinate) {
            control.stop_h();
            last_coordinate = coordinate;
            state = State::wait;
            encoder.disableInterrupt();
      break;
      case braking:
         // if (encoder == goal) {
            control.fast_stop();
            control.stop_h();
            last_coordinate = coordinate;
            state = State::wait;
            encoder.disableInterrupt();
      break;
   }
}