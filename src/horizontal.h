#pragma once
#include <stdint.h>

template <class Control, class Encoder>
class Horizontal : Interrupting
{
   enum State {
      wait, right_fast, right_slow, left_fast, left_slow, braking
   } state {State::wait};
   enum Direction {right, left} direction {Direction::right};
   template <Direction> auto few();
   int16_t goal {0};
   Control& control;
	Encoder& encoder;
   int16_t coordinate {0};
public:
   int16_t brake;
   Horizontal (Control& control, Encoder& encoder, int16_t brake);
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
   control.fast_stop();
   control.stop_h();
   state = State::wait;
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
	encoder.enableInterrupt();
   
   switch (state) {
		case wait:
   		if (coordinate < encoder) {
   		   control.left();
   		   if (coordinate >= (encoder - brake)) {
					control.slow();
					encoder.compare(coordinate);
   		      state = State::left_slow;
   		   } else if (coordinate < (encoder - brake)) {
   		      control.fast();
					encoder.compare(coordinate + brake);
   		      state = State::left_fast;
   		   } 
   		   control.slow_stop();
				control.start();
   		} else if (coordinate > encoder) {
   		   control.right();
   		   if (coordinate <= (encoder + brake)) {
					control.slow();
					encoder.compare(coordinate);
   		      state = State::right_slow;
   		   } else if (coordinate > (encoder + brake)) {
					control.fast();
					encoder.compare(coordinate - brake);
   		      state = State::right_fast;
   		   } 
   		   control.slow_stop();
				control.start();
   		}
		break;
   	case right_fast:
      	if (coordinate <= (encoder + brake)) {
				control.slow();
				goal = encoder + brake;
				encoder.compare(goal);
      	   state = State::braking;
      	} else encoder.compare(coordinate - brake);
		break;
   	case right_slow:
      	if (coordinate < encoder) {
      	   control.fast_stop();
      	   control.stop_h();
      	   state = State::wait;
      	} else if (coordinate >= (encoder + brake)) {
      	   encoder.compare(coordinate - brake);
				control.fast();
      	   state = State::right_fast;
      	}
		break;
   	case left_fast:
      	if (coordinate >= (encoder - brake)) {
      	   encoder.compare(coordinate + brake);
				control.slow();
				goal = encoder - brake;
				encoder.compare(goal);
      	   state = State::braking;
      	} else encoder.compare(coordinate + brake);
		break;
   	case left_slow:
      	if (coordinate > encoder) {
      	   control.fast_stop();
      	   control.stop_h();
      	   state = State::wait;
      	} else if (coordinate < (encoder - brake)) {
      	   encoder.compare(coordinate - brake);
				control.fast();
      	   state = State::left_fast;
      	} 
		break;
   	case braking:
      	if (control.is_right() and few<right>()) {
      	   goal = coordinate;
      	} else if (control.is_right() and coordinate > (encoder + brake)) {
      	   control.fast();
				state = State::right_fast;
      	} else if (control.is_left() and few<left>()) {
      	   goal = coordinate;
      	} else if (control.is_left() and coordinate < (encoder - brake)) {
      	   control.fast();
				state = State::left_fast;
      	}
		break;
	}
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::interrupt()
{
   encoder.disableInterrupt();
	switch (state) {
      case wait:
      break;
      case right_fast:
         // if (encoder == (coordinate - brake)) 
            goal = coordinate;
				encoder.compare(goal);
				state = State::braking;
            control.slow();
      break;
      case right_slow:
         // if (encoder == coordinate) 
            control.fast_stop();
            control.stop_h();
            state = State::wait;
      break;
      case left_fast:
         // if (encoder == (coordinate + brake))
            goal = coordinate;
				encoder.compare(goal);
				state = State::braking;
            control.slow();
      break;
      case left_slow:
         // if (encoder == coordinate) {
            control.fast_stop();
            control.stop_h();
            state = State::wait;
      break;
      case braking:
			// if (encoder == goal) {
            control.fast_stop();
            control.stop_h();
            state = State::wait;
      break;
   }
}
