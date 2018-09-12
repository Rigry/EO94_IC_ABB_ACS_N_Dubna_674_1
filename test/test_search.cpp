#include "RCC.h"
#include "search.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>

template <int n>
struct MockPin
{
   static bool set_;
   using Mode = GPIO_ral::Mode_t;
   using OutType = GPIO_ral::OutType_t;
   using OutSpeed = GPIO_ral::OutSpeed_t;
   using PullResistor = GPIO_ral::PullResistor_t;
   using AF = GPIO_ral::AF;

   inline static void configure (Mode mode,
                           OutType type,
                           OutSpeed speed,
                           PullResistor res){}
   template<PinConf_t pinConf>
   static void configure() {}
   template<AF af>
   static void configureAltFunction(){}
   template <AF func> static void SetAltFunc(){}

   static void set()          {set_ = true;}
   static void clear()        {set_ = false;}
   static void set(bool b)    {}
   static bool isSet()        {return set_;}
   static bool isClear()      {return not set_;}
};

// template<> bool MockPin<1>::set_ {false};
// template<> bool MockPin<2>::set_ {false};
// template<> bool MockPin<3>::set_ {false};
// template<> bool MockPin<4>::set_ {false};

using SenseRight = MockPin<1>;
using SenseLeft  = MockPin<2>;
using Origin     = MockPin<3>;

template<> bool SenseRight::set_ {false};
template<> bool SenseLeft ::set_ {false}; 
template<> bool Origin    ::set_ {false};


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

int main ()
{
   int error = 0; 
   int16_t encoder;
   Search <Mock_control, SenseLeft, SenseRight, Origin> search;

   search();
   if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 1\033[0m" << std::endl;
         ++error;
   }

   Origin::set();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 2\033[0m" << std::endl;
         ++error;
   }

   search();
   Origin::clear();
   search();
   if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 3\033[0m" << std::endl;
         ++error;
   }
   search();
   if (launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 4\033[0m" << std::endl;
         ++error;
   }
   SenseLeft::set();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 5\033[0m" << std::endl;
         ++error;
   }
   search();
   SenseLeft::clear();
   if (side != right or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 6\033[0m" << std::endl;
         ++error;
   }

   Origin::set();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 7\033[0m" << std::endl;
         ++error;
   }

   search();
   SenseLeft::set();
   Origin::clear();
   search();
   SenseLeft::clear();
   search();
   if (launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 8\033[0m" << std::endl;
         ++error;
   }
   SenseRight::set();
   search();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 9\033[0m" << std::endl;
         ++error;
   }

   SenseRight::clear();
   Origin::set();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 10\033[0m" << std::endl;
         ++error;
   }

   search();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 11\033[0m" << std::endl;
         ++error;
   }


   if (error == 0)
      std::cout << "\033[32mtest_search Пройден\033[0m" << std::endl;
   else 
      std::cout << "\033[31mtest_search Провален\033[0m" << std::endl;
}