#include "RCC.h"
#include "automatic.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>

struct Mock_control
{
   static enum class Speed  {slow  = 0b0, fast}  speed;
   static enum class Side   {right = 0b0, left}  side;
   static enum class Launch {stop  = 0b0, start} launch;
   static enum class Finish {slow_stop = 0b0, fast_stop} finish;
   static void init      () {}
   static void fast      () {speed  = Speed ::fast; }
   static void slow      () {speed  = Speed ::slow; }
   static void right     () {side   = Side  ::right;}
   static void left      () {side   = Side  ::left; }
   static void start     () {launch = Launch::start;}
   static void stop      () {launch = Launch::stop; }
   static void fast_stop () {finish = Finish::fast_stop;}
   static void slow_stop () {finish = Finish::slow_stop;}
   static bool is_right  () {return side == Side::right;}
   static bool is_left   () {return side == Side::left ;}
};

using Speed  = Mock_control::Speed;
using Side   = Mock_control::Side;
using Launch = Mock_control::Launch;
using Finish = Mock_control::Finish;

Speed  Mock_control::speed {Speed ::slow };
Side   Mock_control::side  {Side  ::right};
Launch Mock_control::launch{Launch::stop };
Finish Mock_control::finish{Finish::fast_stop};

auto& speed  = Mock_control::speed;
auto& side   = Mock_control::side;
auto& launch = Mock_control::launch;
auto& finish = Mock_control::finish;

const Speed  fast      {Speed ::fast };
const Speed  slow      {Speed ::slow };
const Side   right     {Side  ::right};
const Side   left      {Side  ::left };
const Launch start     {Launch::start};
const Launch stop      {Launch::stop };
const Finish fast_stop {Finish::fast_stop};
const Finish slow_stop {Finish::slow_stop};

class Mock_horizontal
{

};

class Mock_vertical
{

};

int main()
{
   int error = 0; 
   int16_t encoder {0};
   int16_t zone_coordinate[5];
   Mock_horizontal horizontal;
   Mock_vertical vertical;
   Automatic <Mock_horizontal, Mock_vertical, int16_t, Mock_control> {zone_coordinate[5], horizontal, vertical, encoder}
}