#pragma once
#include "horizontal.h"
#include "vertical.h"
#include <boost/preprocessor/iteration/local.hpp>

template <class Horizontal, class Vertical, class Encoder, class Control>
class Automatic
{
   const int16_t (&zone_coordinate)[16];
   int16_t min_coordinate{0};
   int16_t max_coordinate{12000};
   Horizontal &horizontal;
   Vertical   &vertical;
   Encoder    &encoder;
   bool zone [16];
   int16_t zone_n {0};
   int16_t coordinate {0};
   int16_t temp {0};
   int16_t i {0};
   void set_temp();
   void set_zone();
   public:
   enum State {
      wait, horizontal_, vertical_, zone_
      } state {State::wait};
   Automatic (const int16_t (&zone_coordinate)[16], Horizontal &horizontal, Vertical &vertical, Encoder &encoder);
   void move(int16_t coordinate);
   void move();
   void operator()();
};

// Automatic <Horizontal, Vertical> automatic {flash.zone_coordinate};
template <class Horizontal, class Vertical, class Encoder, class Control>
Automatic <Horizontal, Vertical, Encoder, Control>::Automatic (const int16_t (&zone_coordinate)[16], 
                                             Horizontal &horizontal, Vertical &vertical, Encoder &encoder)
   : zone_coordinate {zone_coordinate}
   , horizontal      {horizontal}
   , vertical        {vertical}
   , encoder         {encoder}
{}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::set_zone()
{
   if (encoder > min_coordinate and encoder < zone_coordinate[0])
      zone_n = 0;
   #define BOOST_PP_LOCAL_MACRO(i) if (encoder > zone_coordinate[i] and encoder < zone_coordinate[i+1]) zone_n = i+1;
   #define BOOST_PP_LOCAL_LIMITS (0, 14)
   #include BOOST_PP_LOCAL_ITERATE()
}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::set_temp()
{
   if (Control::is_right) {
      #define BOOST_PP_LOCAL_MACRO(i) if (zone[i]) temp = zone_coordinate[i-1];
      #define BOOST_PP_LOCAL_LIMITS (1, 15)
      #include BOOST_PP_LOCAL_ITERATE()
      // if (zone[0])
         // temp = min_coordinate;
      // if (zone[1])
         // temp = zone_coordinate[0];
      // if (zone[2])
         // temp = zone_coordinate[1];
      // if (zone[15])
         // temp = zone_coordinate[14];
      
   } else if (Control::is_left) {
      #define BOOST_PP_LOCAL_MACRO(i) if (zone[i]) temp = zone_coordinate[i];
      #define BOOST_PP_LOCAL_LIMITS (0, 14)
      #include BOOST_PP_LOCAL_ITERATE()
      // if (zone[0])
         // temp = zone_coordinate[0];
      // if (zone[1])
         // temp = zone_coordinate[1];
      // if (zone[15])
         // temp = max_coordinate;
   }
}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::move(int16_t coordinate)
{
   switch (state) {
      case wait:
         if (not zone [i]) {
            if (vertical.isUp() or vertical.isDown()) {
               horizontal.move(coordinate);
               state = State::horizontal_;
            }
         } else if (zone[i]) {
            state = State::zone_;
         }
      break;
      case horizontal_:
         horizontal.move();
         state = State::horizontal;
      break;
      case vertical_:

      break;
      case zone_:
         // if (zone_coordinate) если координата находится до занятой зоны-едем сразу
      break;
   }


}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::move()
{
   switch (state) {
      case wait:
         if (encoder == coordinate)
            state = State::vertical_;
      break;
      case horizontal_:

      break;
      case vertical_:
         
      break;
      case zone_:
     
      break;
   }

}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::operator()()
{
   switch (state) {
      case wait:
         //if (zone[i]) {
         //   temp = zone_coordinate[i];
         //   state = State::zone_;
         //}
      break;
      case horizontal_:
         set_zone();
         if (encoder == coordinate)
            state = State::wait;
         else if (Control::is_right()) {
            if (zone[zone_n+1]) /*ближайшая по ходу следования*/
               state = State::zone_;
            }
         else if (Control::is_left()) {
            if (zone[zone_n-1]) /*ближайшая по ходу следования*/
               state = State::zone_;
            }
      break;
      case vertical_:
         if (vertical.isUp()) {
            vertical.down();
            if (vertical.isDown())
               state = State::wait;
         } else {
            vertical.up();
            if (vertical.isUp())
               state = State::wait;
         } 
      break;
      case zone_://если стоим в зоне и зона занимается - надо свалить в свободную?или зона не займется?
         set_temp();
         horizontal.move(temp);
         if (not zone[i]) { /*ближайшая по ходу следования*/
         horizontal.move(coordinate);
         state = State::horizontal_;
         }
      break;
   }
}