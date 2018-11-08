#pragma once

template <class Control, class Vertical, class Sense_left, class Sense_right>
class Manual
{
   enum State {wait, move_right, move_left} state {State::wait};
   Control& control;
   Vertical& vertical;
public:
   Manual (Control& control, Vertical& vertical);
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

template <class Control, class Vertical, class Sense_left, class Sense_right>
Manual<Control, Vertical, Sense_left, Sense_right>::Manual (Control& control, Vertical& vertical) 
      : control{control}
      , vertical{vertical}
{}


template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::right()
{
   switch (state) {
      case wait:
         if (Sense_right::isClear()) {
            control.right();
            control.start();
            state = State::move_right;
         }
      break;
      case move_right:
      break;
      case move_left:
         control.stop_h();
         control.right();
         control.start();
         state = State::move_right;
      break;
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::left()
{
   switch (state) {
      case wait:
         if (Sense_left::isClear()) {
            control.left();
            control.start();
            state = State::move_left;
         }
      break;
      case move_right:
         control.stop_h();
         control.left();
         control.start();
         state = State::move_left;
      break;
      case move_left:
      break;
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::up()
{
   vertical.up();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::down()
{
   vertical.down();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::fast()
{
   control.fast();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::slow()
{
   control.slow();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::fast_stop()
{
   control.fast_stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::slow_stop()
{
   control.slow_stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::stop_h()
{
   control.stop_h();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::stop_v()
{
   vertical.stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::stop()
{
   reset();
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::reset()
{
   fast_stop();
   stop_h();
   stop_v();
   state = State::wait;
}

template <class Control, class Vertical, class Sense_left, class Sense_right>
void Manual<Control, Vertical, Sense_left, Sense_right>::operator()()
{
   switch (state) {
      case wait:
      break;
      case move_right:
         if (Sense_right::isSet()) {
            fast_stop();
            stop_h();
            state = State::wait;
         }
      break;
      case move_left:
         if (Sense_left::isSet()) {
            fast_stop();
            stop_h();
            state = State::wait;
         }
      break;
   }
}