#pragma once

template <class Control, class SenseLeft, class SenseRight, class Origin>
class Search 
{
   // Encoder &encoder;
   // const int16_t &origin;
   enum State {wait, left, right, ready, unready} state {State::wait};
public:
   // Search (Encoder &encoder, const int16_t &origin);
   void operator()();
};

// template <class Control, class SenseLeft, class SenseRight, class Origin, class Encoder>
// Search<Control, SenseLeft, SenseRight, Origin, Encoder>::Search(Encoder &encoder, const int16_t &origin)
   // : encoder {encoder}
   // , origin  {origin}
// {}

template <class Control, class SenseLeft, class SenseRight, class Origin>
void Search<Control, SenseLeft, SenseRight, Origin>::operator()()
{
   switch (state) {
      case wait:
         if (Origin::isClear()) {
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
         } else if (Origin::isSet())
            state = State::ready;
      break;
      case left:
         if(Origin::isSet()) {
            Control::fast_stop();
            Control::stop();
            state = State::ready;
         } else if (SenseLeft::isSet()) {
            Control::fast_stop();
            Control::stop();
            state = State::wait;
         }
      break;
      case right:
         if(Origin::isSet()) {
            Control::fast_stop();
            Control::stop();
            state = State::ready;
         } else if (SenseRight::isSet()) {
            Control::fast_stop();
            Control::stop();
            state = State::unready;
         }
      break;
      case ready:
         // encoder = origin;
         state = State::wait;
      break;
      case unready:
         if (SenseRight::isClear())
            state = State::wait;
   }
}
