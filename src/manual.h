#pragma once

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
class Manual
{
   enum State {wait, move_right, move_left, _step_left, _step_right} state {State::wait};
   Control& control;
   Vertical& vertical;
   Encoder& encoder;
   int16_t target;
public:
   Manual (Control& control, Vertical& vertical, Encoder& encoder);
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
   void step_left (uint16_t);
   void step_right (uint16_t);
   void operator()();
};

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::Manual (Control& control, Vertical& vertical, Encoder& encoder) 
      : control  {control }
      , vertical {vertical}
      , encoder  {encoder }
{}


template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::right()
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
      case _step_left:
      case _step_right:
      break;
      case move_left:
         control.stop_h();
         control.right();
         control.start();
         state = State::move_right;
      break;
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::left()
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
      case _step_left:
      case _step_right:
      break;
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::up()
{
   vertical.up();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::down()
{
   vertical.down();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::fast()
{
   control.fast();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::slow()
{
   control.slow();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::fast_stop()
{
   control.fast_stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::slow_stop()
{
   control.slow_stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::stop_h()
{
   control.stop_h();
   state = State::wait;
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::stop_v()
{
   vertical.stop();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::stop()
{
   reset();
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::reset()
{
   fast_stop();
   stop_h();
   stop_v();
   state = State::wait;
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::step_left (uint16_t v)
{
   if (state == wait) {
      target = encoder;
      target -= v;
      slow();
      fast_stop();
      if (v != 0) {
         control.left();
         control.start();
      }
      state = _step_left;
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::step_right (uint16_t v)
{
   if (state == wait) {
      target = encoder;
      target += v;
      slow();
      fast_stop();
      if (v != 0) {
         control.right();
         control.start();
      }
      state = _step_right;     
   }
}

template <class Control, class Vertical, class Sense_left, class Sense_right, class Encoder>
void Manual<Control, Vertical, Sense_left, Sense_right, Encoder>::operator()()
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
      case _step_left:
         if (encoder <= target or Sense_left::isSet()) {
            stop_h();
            state = State::wait;
         }
      break;
      case _step_right:
         if (encoder >= target or Sense_right::isSet()) {
            stop_h();
            state = State::wait;
         }
      break;
   }
}