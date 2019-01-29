#pragma once

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
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

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
Search<Control, Sense_left, Sense_right, Origin, Encoder>::Search(Control& control, Encoder& encoder) 
   : control {control}
   , encoder {encoder}
{}

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_left, Sense_right, Origin, Encoder>::stop()
{
   reset();
}

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_left, Sense_right, Origin, Encoder>::reset()
{
   control.fast_stop();
   control.stop_h();
   state = State::wait;
}

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
bool Search<Control, Sense_left, Sense_right, Origin, Encoder>::is_done()
{
   return state == State::ready;
}

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
bool Search<Control, Sense_left, Sense_right, Origin, Encoder>::not_found()
{
   return state == State::unready;
}

template <class Control, class Sense_left, class Sense_right, class Origin, class Encoder>
void Search<Control, Sense_left, Sense_right, Origin, Encoder>::operator()()
{
   switch (state) {
      case wait:
         if (Origin::isSet()) {
            if (Sense_left::isSet()) {
               control.right();
               control.slow();
               control.start();
               state = State::right;
            } else if (Sense_left::isClear()) {
               control.left ();
               control.slow ();
               control.start();
               state = State::left;
            }
         } else if (Origin::isClear()) {
            encoder = 0;
            state = State::ready;
         }
      break;
      case left:
         if(Origin::isClear()) {
            control.fast_stop();
            control.stop_h();
            encoder = 0;
            state = State::ready;
         } else if (Sense_left::isSet()) {
            control.fast_stop();
            control.stop_h();
            state = State::wait;
         }
      break;
      case right:
         if(Origin::isClear()) {
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
