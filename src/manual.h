#pragma once

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
class Manual
{
   enum State {wait, move} state {State::wait};
   Control &control;
public:
   Manual (Control &control);
   void right();
   void left();
   void up();
   void down();
   void stop_h();
   void stop_v();
   void fast();
   void slow();
   void fast_stop();
   void slow_stop();
   void stop();
   void reset();
   void operator()();
};

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::Manual (Control &control) : control{control}{}


template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::right()
{
   switch (state) {
      case wait:
         if (Sense_right::isClear()) {
            control.right();
            control.start();
            state = State::move;
         }
      break;
      case move:
         if (Sense_right::isClear()) {
            control.stop_h();
            control.right();
            control.start();
         }
      break;
   }
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::left()
{
   switch (state) {
      case wait:
         if (Sense_left::isClear()) {
            control.left();
            control.start();
            state = State::move;
         }
      break;
      case move:
         if (Sense_left::isClear()) {
            control.stop_h();
            control.left();
            control.start();
         }
      break;
   }
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::up()
{
   switch (state) {
      case wait:
         if (Sense_up::isClear()) {
            control.up();
            state = State::move;
         }
      break;
      case move:
         if (Sense_up::isClear()) {
            control.stop_v();
            control.down();
         }
      break;
   }
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::down()
{
   switch (state) {
      case wait:
         if (Sense_down::isClear()) {
            control.down();
            state = State::move;
         }
      break;
      case move:
         if (Sense_left::isClear()) {
            control.stop_v();
            control.down();
         }
      break;
   }
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::fast()
{
   control.fast();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::slow()
{
   control.slow();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::fast_stop()
{
   control.fast_stop();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::slow_stop()
{
   control.slow_stop();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::stop_h()
{
   control.stop_h();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::stop_v()
{
   control.stop_v();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::stop()
{
   reset();
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::reset()
{
   fast_stop();
   stop_h();
   stop_v();
   state = State::wait;
}

template <class Control, class Sense_up, class Sense_down, class Sense_left, class Sense_right>
void Manual<Control, Sense_up, Sense_down, Sense_left, Sense_right>::operator()()
{
   switch (state) {
      case wait:
      break;
      case move:
         if (Sense_right::isSet() or Sense_left::isSet())
            stop_h();
         if (Sense_up::isSet() or Sense_down::isSet())
            stop_v();
      break;
   }
}