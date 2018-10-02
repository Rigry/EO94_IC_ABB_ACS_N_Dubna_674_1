#pragma once
#include "horizontal.h"
#include "vertical.h"
#include <boost/preprocessor/iteration/local.hpp>

template <class Horizontal, class Vertical, class Encoder, class Control>
class Automatic
{
   const int16_t (&zone_coordinate)[16];
   int16_t min_coordinate{0};//тоже по ссылке
   int16_t max_coordinate{12000};
   Horizontal &horizontal;
   Vertical   &vertical;
   Encoder    &encoder;
   
   int16_t temp {0};
   int16_t i {0};
   int16_t set_temp();//координата до занятой зоны
   int16_t set_zone();//определяет в какой зоне находится инкодер
public:
   void stop ();
   int16_t n_zone {0};
   bool zone [16] {false};
   int16_t coordinate {0};
   enum State {
      wait, horizontal_, vertical_, zone_
      } state {State::wait};
   Automatic (const int16_t (&zone_coordinate)[16], Horizontal &horizontal, Vertical &vertical, Encoder &encoder);
   void move(int16_t coordinate);
   void move();
   void operator()();
};


template <class Horizontal, class Vertical, class Encoder, class Control>
Automatic <Horizontal, Vertical, Encoder, Control>::Automatic (const int16_t (&zone_coordinate)[16],
                                             Horizontal &horizontal, Vertical &vertical, Encoder &encoder)
   : zone_coordinate {zone_coordinate}
   , horizontal      {horizontal}
   , vertical        {vertical}
   , encoder         {encoder}
{}

// template <class Horizontal, class Vertical, class Encoder, class Control>
// int16_t Automatic <Horizontal, Vertical, Encoder, Control>::set()
// {
//    return std::make_tuple(zone[i], zone_coordinate[i]);
// }

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::stop()
{
   horizontal.stop();
   vertical.stop();
}

template <class Horizontal, class Vertical, class Encoder, class Control>
int16_t Automatic <Horizontal, Vertical, Encoder, Control>::set_zone()
{
   if (encoder >= min_coordinate and encoder < zone_coordinate[0])
      n_zone = 0;
   #define  BOOST_PP_LOCAL_MACRO(i) if (encoder >= zone_coordinate[i] and encoder < zone_coordinate[i+1]) n_zone = i+1;
   #define  BOOST_PP_LOCAL_LIMITS (0, 14)
   #include BOOST_PP_LOCAL_ITERATE()
   return n_zone;
}

template <class Horizontal, class Vertical, class Encoder, class Control>
int16_t Automatic <Horizontal, Vertical, Encoder, Control>::set_temp()
{
   if (Control::is_right()) {
      #define  BOOST_PP_LOCAL_MACRO(i) if (zone[i]) temp = zone_coordinate[i-1];
      #define  BOOST_PP_LOCAL_LIMITS (1, 15)
      #include BOOST_PP_LOCAL_ITERATE()
      // if (zone[0])
         // temp = min_coordinate;
      // if (zone[1])
         // temp = zone_coordinate[0];
      // if (zone[2])
         // temp = zone_coordinate[1];
      // if (zone[15])
         // temp = zone_coordinate[14];
      
   } else if (Control::is_left()) {
      #define  BOOST_PP_LOCAL_MACRO(i) if (zone[i]) temp = zone_coordinate[i];
      #define  BOOST_PP_LOCAL_LIMITS (0, 14)
      #include BOOST_PP_LOCAL_ITERATE()
      // if (zone[0])
         // temp = zone_coordinate[0];
      // if (zone[1])
         // temp = zone_coordinate[1];
      // if (zone[15])
         // temp = max_coordinate;
   }
   return temp;
}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
   set_zone();
   switch (state) {
      case wait:
         if ((coordinate > encoder and (not zone [n_zone + 1])) 
         or (coordinate < encoder and (not zone [n_zone - 1]))) {
            if (vertical.isUp() or vertical.isDown()) {
               horizontal.move(coordinate);
               state = State::horizontal_;
            }
         } else if (zone[n_zone + 1] or zone[zone[n_zone - 1]]) {
               set_temp();
               horizontal.move(temp);
               state = State::zone_;
            } 
      break;
      case horizontal_:
         horizontal.move(coordinate);
      break;
      case vertical_:
      break;
      case zone_:
         if ((coordinate > encoder and (not zone [n_zone + 1])) 
         or (coordinate < encoder and (not zone [n_zone - 1]))) {
            if (vertical.isUp() or vertical.isDown()) {
               horizontal.move(coordinate);
               state = State::horizontal_;
            }
         } else if (zone[n_zone + 1] or zone[zone[n_zone - 1]]) {
                  state = State::zone_;
         } 
      break;
   }


}

template <class Horizontal, class Vertical, class Encoder, class Control>
void Automatic <Horizontal, Vertical, Encoder, Control>::move()
{
   if (state == State::wait) {
      state = State::vertical_;
      if (vertical.isUp())
         vertical.down();
      else 
         vertical.up();
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
         horizontal();
         if (encoder == coordinate)
            state = State::wait;
         else if ((Control::is_right() and zone[n_zone+1])
            or (Control::is_left() and zone[n_zone-1])) {
            set_temp();
            horizontal.move(temp);
            state = State::zone_;
         }
      break;
      case vertical_:
         tickUpdater.notify();
         if (vertical.isUp() or vertical.isDown())
            state = State::wait;
      break;
      case zone_://если стоим в зоне и зона занимается - надо свалить в свободную?или зона не займется?
         if ((not zone[n_zone+1] and Control::is_right()) or (not zone[n_zone-1] and Control::is_left())) {
            horizontal.move(coordinate);
            state = State::horizontal_;
            }
      break;
   }
}