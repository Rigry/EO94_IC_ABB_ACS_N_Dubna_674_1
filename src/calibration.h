#pragma once

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
class Calibration
{
   enum State {wait, left, right, complete} state {State::wait};
   Control& control;
   Encoder& encoder;
   int16_t& min_coordinate;
   int16_t& max_coordinate;
public:
   Calibration (Control& control, Encoder& encoder, int16_t& min_coordinate, int16_t& max_coordinate);
   bool done();
   void stop();
   void reset();
   void operator()();
};

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
Calibration<Control, Sense_up, Sense_left, Sense_right, Encoder>::Calibration (Control &control, Encoder &encoder,
                                                                     int16_t &min_coordinate, int16_t &max_coordinate)
   : control {control}
   , encoder {encoder}
   , min_coordinate {min_coordinate}
   , max_coordinate {max_coordinate}
{}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
bool Calibration<Control, Sense_up, Sense_left, Sense_right, Encoder>::done()
{
   if (state == State::complete)
      return true;
   else return false;
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
void Calibration<Control, Sense_up, Sense_left, Sense_right, Encoder>::stop()
{
   reset();
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
void Calibration<Control, Sense_up, Sense_left, Sense_right, Encoder>::reset()
{
   control.fast_stop();
   control.stop_h();
   state = State::wait;
}

template <class Control, class Sense_up, class Sense_left, class Sense_right, class Encoder>
void Calibration<Control, Sense_up, Sense_left, Sense_right, Encoder>::operator()()
{
   switch (state) {
      case wait:
         control.left ();
         control.slow ();
         control.fast_stop();
         control.start();
         state = State::left;
      break;
      case left:
         if(Sense_left::isSet()) {
            control.stop_h();
            min_coordinate = encoder;
            control.right();
            control.slow();
            control.start();
            state = State::right;
         }
      break;
      case right:
         if(Sense_right::isSet()) {
            control.stop_h();
            max_coordinate = encoder;
            state = State::complete;
         }
      break;
      case complete:
         state = State::wait;
      break;
   }
}