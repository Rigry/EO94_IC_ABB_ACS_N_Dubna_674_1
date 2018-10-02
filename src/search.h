#pragma once

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
class Search 
{
   enum State {wait, left, right, ready, unready} state {State::wait};
   Control& control;
   Encoder& encoder;
public:
   Search (Control& control, Encoder& encoder);
   bool is_done();
   bool not_found();
   void stop();
   void reset();
   void operator()();
};

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::Search(Control& control, Encoder& encoder) 
   : control {control}
   , encoder {encoder}
{}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::stop()
{
   reset();
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::reset()
{
   control.fast_stop();
   control.stop_h();
   state = State::wait;
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
bool Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::is_done()
{
   if (state == State::ready)
      return true;
   else return false;
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
bool Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::not_found()
{
   if (state == State::unready)
      return true;
   else return false;
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_up, Sense_left, Sense_right, Origin, Encoder>::operator()()
{
   switch (state) {
      case wait:
         if (Origin::isClear()) {
            if (Sense_left::isSet()) {
               control.right();
               control.slow();
               control.fast_stop();
               control.start();
               state = State::right;
            } else if (Sense_left::isClear()) {
               control.left ();
               control.slow ();
               control.fast_stop();
               control.start();
               state = State::left;
            }
         } else if (Origin::isSet()) {
            encoder = 0;
            state = State::ready;
         }
      break;
      case left:
         if(Origin::isSet()) {
            control.stop_h();
            encoder = 0;
            state = State::ready;
         } else if (Sense_left::isSet()) {
            control.stop_h();
            state = State::wait;
         }
      break;
      case right:
         if(Origin::isSet()) {
            control.fast_stop();
            control.stop_h();
            encoder = 0;
            state = State::ready;
         } else if (Sense_right::isSet()) {
            control.fast_stop();
            control.stop_h();
            state = State::unready;
         }
      break;
      case ready:
         state = State::wait;
      break;
      case unready:
      break;
   }
}
