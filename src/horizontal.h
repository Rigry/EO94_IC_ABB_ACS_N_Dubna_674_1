#pragma once
#include <stdint.h>

template <class Control, class Encoder>
class Horizontal 
{
   enum State {
      wait, right_fast, right_slow, left_fast, left_slow, braking
   } state {State::wait};
   enum Direction {right, left} direction {Direction::right};
   template <Direction> auto few();
   int16_t goal {0};
   Encoder& encoder;
   int16_t coordinate {0};
public:
   uint16_t brake;
   Horizontal (Encoder& encoder, uint16_t brake);
   void stop ();
   void move (int16_t coordinate);
   void operator()();
};

template <class Control, class Encoder>
Horizontal <Control, Encoder>::Horizontal (Encoder& encoder, uint16_t brake)
   : encoder {encoder}
   , brake   {brake}
{}

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
   Control::fast_stop();
   Control::stop();
   state = State::wait;
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
   
   switch (state) {
		case wait:
   		if (coordinate < encoder) {
   		   Control::left();
   		   if (coordinate >= (encoder - brake)) {
   		      Control::slow();
   		      state = State::left_slow;
   		   } else if (coordinate < (encoder - brake)) {
   		      Control::fast();
   		      state = State::left_fast;
   		   } 
   		   Control::slow_stop();
				Control::start();
   		} else if (coordinate > encoder) {
   		   Control::right();
   		   if (coordinate <= (encoder + brake)) {
   		      Control::slow();
   		      state = State::right_slow;
   		   } else if (coordinate > (encoder + brake)) {
   		      Control::fast();
   		      state = State::right_fast;
   		   } 
   		   Control::slow_stop();
				Control::start();
   		}
		break;
   	case right_fast:
      	if (coordinate <= (encoder + brake)) {
      	   Control::slow();
				goal = encoder + brake;
      	   state = State::braking;
      	} 
		break;
   	case right_slow:
      	if (coordinate < encoder) {
      	   Control::fast_stop();
      	   Control::stop();
      	   state = State::wait;
      	} else if (coordinate >= (encoder + brake)) {
      	   Control::fast();
      	   state = State::right_fast;
      	}
		break;
   	case left_fast:
      	if (coordinate >= (encoder - brake)) {
      	   Control::slow();
				goal = encoder - brake;
      	   state = State::braking;
      	} 
		break;
   	case left_slow:
      	if (coordinate > encoder) {
      	   Control::fast_stop();
      	   Control::stop();
      	   state = State::wait;
      	} else if (coordinate < (encoder - brake)) {
      	   Control::fast();
      	   state = State::left_fast;
      	} 
		break;
   	case braking:
      	if (Control::is_right() and few<right>()) {
      	   goal = coordinate;
      	} else if (Control::is_right() and coordinate > (encoder + brake)) {
      	   Control::fast();
				state = State::right_fast;
      	} else if (Control::is_left() and few<left>()) {
      	   goal = coordinate;
      	} else if (Control::is_left() and coordinate < (encoder - brake)) {
      	   Control::fast();
				state = State::left_fast;
      	}
		break;
	}
}

template <class Control, class Encoder>
void Horizontal <Control, Encoder>::operator()()
{
   switch (state) {
      case wait:
      break;
      case right_fast:
         if (encoder == (coordinate - brake)) {
            goal = coordinate;
				state = State::braking;
            Control::slow();
         }
      break;
      case right_slow:
         if (encoder == coordinate) {
            Control::fast_stop();
            Control::stop();
            state = State::wait;
            }
      break;
      case left_fast:
         if (encoder == (coordinate + brake)) {
            goal = coordinate;
				state = State::braking;
            Control::slow();
         }
      break;
      case left_slow:
         if (encoder == coordinate) {
            Control::fast_stop();
            Control::stop();
            state = State::wait;
         }
      break;
      case braking:
			if (encoder == goal) {
            Control::fast_stop();
            Control::stop();
            state = State::wait;
         }
      break;
   }
}

