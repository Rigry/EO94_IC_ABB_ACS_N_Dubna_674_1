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

using Sense_right = MockPin<1>;
using Sense_left  = MockPin<2>;
using Origin      = MockPin<3>;

template<> bool Sense_right::set_ {false};
template<> bool Sense_left ::set_ {false};
template<> bool Origin     ::set_ {false};


struct Mock_control
{
   enum class Speed  {slow  = 0b0, fast}  speed  = Speed ::slow;
   enum class Side   {right = 0b0, left}  side   = Side  ::right;
   enum class Launch {stop  = 0b0, start} launch = Launch::stop;
   enum class Finish {slow_stop = 0b0, fast_stop} finish = Finish::fast_stop;
   void init      () {}
   void fast      () {speed  = Speed ::fast; }
   void slow      () {speed  = Speed ::slow; }
   void right     () {side   = Side  ::right;}
   void left      () {side   = Side  ::left; }
   void start     () {launch = Launch::start;}
   void stop_h    () {launch = Launch::stop; }
   void fast_stop () {finish = Finish::fast_stop;}
   void slow_stop () {finish = Finish::slow_stop;}
   bool is_right  () {return side == Side::right;}
   bool is_left   () {return side == Side::left ;}
} control;

using Speed  = Mock_control::Speed;
using Side   = Mock_control::Side;
using Launch = Mock_control::Launch;
using Finish = Mock_control::Finish;

// Speed  Mock_control.speed {Speed ::slow };
// Side   Mock_control.side  {Side  ::right};
// Launch Mock_control.launch{Launch::stop };
// Finish Mock_control.finish{Finish::fast_stop};

auto& speed  = control.speed;
auto& side   = control.side;
auto& launch = control.launch;
auto& finish = control.finish;

const Speed  fast      {Speed ::fast };
const Speed  slow      {Speed ::slow };
const Side   right     {Side  ::right};
const Side   left      {Side  ::left };
const Launch start     {Launch::start};
const Launch stop      {Launch::stop };
const Finish fast_stop {Finish::fast_stop};
const Finish slow_stop {Finish::slow_stop};

int16_t count{0};
int16_t encoder{0};

int main ()
{
   int error = 0; 
   Search <Mock_control, Sense_left, Sense_right, Origin, int16_t> search {control, encoder};

   search();
   if (side != left or speed != slow or launch != start) {
         std::cout << "\033[31mError 1\033[0m" << std::endl;
         ++error;
   }

   Origin::set();
   encoder = 100;
   search();
   if (launch != stop or finish != fast_stop or encoder != 0) {
         std::cout << "\033[31mError 2\033[0m" << std::endl;
         ++error;
   }

   search();
   Origin::clear();
   search();
   if (side != left or speed != slow or launch != start) {
         std::cout << "\033[31mError 3\033[0m" << std::endl;
         ++error;
   }
   search();
   if (launch != start) {
         std::cout << "\033[31mError 4\033[0m" << std::endl;
         ++error;
   }
   Sense_left::set();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 5\033[0m" << std::endl;
         ++error;
   }
   search();
   Sense_left::clear();
   if (side != right or speed != slow or launch != start) {
         std::cout << "\033[31mError 6\033[0m" << std::endl;
         ++error;
   }

   Origin::set();
   encoder = 100;
   search();
   if (launch != stop or finish != fast_stop or encoder != 0) {
         std::cout << "\033[31mError 7\033[0m" << std::endl;
         ++error;
   }

   search();
   Sense_left::set();
   Origin::clear();
   search();
   Sense_left::clear();
   search();
   if (launch != start) {
         std::cout << "\033[31mError 8\033[0m" << std::endl;
         ++error;
   }
   Sense_right::set();
   search();
   search();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 9\033[0m" << std::endl;
         ++error;
   }

   Sense_right::clear();
   search.reset();
   if (launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 10\033[0m" << std::endl;
         ++error;
   }

   if (error == 0)
      std::cout << "\033[1;37mТест класса Search\033[0m      \033[1;32mpassed\033[0m" << std::endl;
   else 
      std::cout << "\033[1;37mТест класса Search\033[0m      \033[1;31mfailed\033[0m" << std::endl;
}