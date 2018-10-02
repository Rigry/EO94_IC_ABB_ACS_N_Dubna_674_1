#include "RCC.h"
#include "automatic.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>
#include <thread>
#include <boost/preprocessor/iteration/local.hpp>

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
public:
   bool move_ {false};
   void move (int16_t coordinate){move_ = true;}
   void operator()(){}
};


class Mock_vertical
{
public:
   bool SenseUp  {false};
   bool SenseDown{false};
   bool up_      {false};
   bool down_    {false};
   void up    () {up_ = true;}
   void down  () {down_ = true;}
   bool isUp  () {return SenseUp;}
   bool isDown() {return SenseDown;}
};


int main()
{
   int error = 0; 
   int16_t encoder {0};
   int16_t zone_coordinate[16] {50,100,150,200,250};
   Mock_horizontal horizontal;
   Mock_vertical vertical;
   Automatic <Mock_horizontal, Mock_vertical, int16_t, Mock_control> automatic {zone_coordinate, horizontal, vertical, encoder};

   // bool set_zone (automatic.n_zone)
   // {
   //    #define  BOOST_PP_LOCAL_MACRO(i) if (n == i) return automatic.zone[i] = true;;
   //    #define  BOOST_PP_LOCAL_LIMITS (0, 15)
   //    #include BOOST_PP_LOCAL_ITERATE()
   // }
   // std::thread zone (set_zone, automatic.n_zone);
//    zone.detach();
   // std::future<int> z = std::async(std::launch::async, set_zone)

   encoder = 70;
   automatic.zone [2] = true;
   vertical.SenseUp = true;
   automatic.move(120);
   Mock_control::right();
   automatic();
   if (horizontal.move_!=true) {
      std::cout << "\033[36mError 1\033[0m" << std::endl;
      ++error;
   }
   horizontal.move_ = false;
   automatic.move(30);
   if (horizontal.move_!=true) {
      std::cout << "\033[36mError 2\033[0m" << std::endl;
      ++error;
   }
   horizontal.move_ = false;
   automatic.zone[2] = false;
   encoder = 30;
   automatic();
   if (horizontal.move_!=false) {
      std::cout << "\033[36mError 3\033[0m" << std::endl;
      ++error;
   }

   automatic.move();
   vertical.SenseUp = false;
   automatic();
   if (not vertical.down_) {
      std::cout << "\033[36mError 4\033[0m" << std::endl;
      ++error;
   }
   vertical.down_ = false;

   vertical.SenseDown = true;
   automatic();
   automatic.move();
   vertical.SenseDown = false;
   automatic();
   vertical.SenseUp = true;
   if (not vertical.up_) {
      std::cout << "\033[36mError 5\033[0m" << std::endl;
      ++error;
   }
   vertical.up_ = false;
   automatic();

   vertical.SenseDown = true;
   vertical.SenseUp = false;
   automatic.move(10);
   Mock_control::left();
   automatic();
   if (not horizontal.move_) {
      std::cout << "\033[36mError 6\033[0m" << std::endl;
      ++error;
   }

   encoder = 10;
   automatic();
   encoder = 120;
   automatic.move(200);
   Mock_control::right();
   if (not horizontal.move_) {
      std::cout << "\033[36mError 7\033[0m" << std::endl;
      ++error;
   }
   horizontal.move_ = false;
   encoder = 170;
   automatic.zone[4] = true;
   automatic();
   if (not horizontal.move_) {
      std::cout << "\033[36mError 8\033[0m" << std::endl;
      ++error;
   }
   horizontal.move_ = false;
   automatic.zone[4] = false;
   automatic();
   if (not horizontal.move_) {
      std::cout << "\033[36mError 9\033[0m" << std::endl;
      ++error;
   }
   automatic();
   encoder = 220;
   automatic();
   automatic.move(70);
   Mock_control::left();
   automatic.zone[2] = true;
   while (--encoder!=150) {
      automatic();
   }
   automatic.zone[2] = false;
   while (--encoder!=70) {
      automatic();
   }
   if (not horizontal.move_) {
      std::cout << "\033[36mError 10\033[0m" << std::endl;
      ++error;
   }
   encoder = 70;
   automatic();

   automatic.zone[2] = true;
   automatic.move(90);


   if (error == 0)
      std::cout << "\033[32mtest_automatic Пройден\033[0m" << std::endl;
   else 
      std::cout << "\033[31mtest_automatic Провален\033[0m" << std::endl;
}