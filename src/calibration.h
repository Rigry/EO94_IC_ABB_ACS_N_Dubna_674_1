#pragma once

template <class Control, class SenseLeft, class SenseRight, class Origin>
class Calibration
{
   enum State {wait, left, right, back} 
      state {State::wait};
public:
   void operator()();
};

template <class Control, class SenseLeft, class SenseRight, class Origin>
void Calibration<Control, SenseLeft, SenseRight, Origin>::operator()()
{
   switch (state) {
         case wait:
            if (SenseLeft::isSet()) {
               Control::right();
               Control::slow();
               Control::slow_stop();
               Control::start();
               state = State::right;
            } else if (SenseLeft::isClear()) {
               Control::left ();
               Control::slow ();
               Control::slow_stop();
               Control::start();
               state = State::left;
            }
         break;
         case left:
            if(SenseLeft::isSet()) {
               Control::fast_stop();
               Control::stop();
               state = State::wait;
            }
         break;
         case right:
            if(SenseRight::isSet()) {
               Control::fast_stop();
               Control::stop();
               state = State::back;
            }
         break;
         case back:
            Control::left();
            Control::fast ();
            Control::slow_stop();
            Control::start();
            if(Origin::isSet()) {
               Control::fast_stop();
               Control::stop();
               state = State::wait;
            }
         break;
   }
}