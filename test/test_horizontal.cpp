#include "RCC.h"
#include "horizontal.h"
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

int main ()
{
   int error = 0; 
   int16_t encoder {0};
   Horizontal <Mock_control, int16_t> horizontal {encoder, 100};

   // wait->right_slow->wait
   horizontal.move(50);
   if (side != right or speed != slow or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 1\033[0m" << std::endl;
      ++error;
   }
   while (++encoder !=50) {
      horizontal();
      if (side != right or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 2\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal();
   if (launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 3\033[0m" << std::endl;
      ++error;
   }

   // wait->right_slow->wait by new coordinate
   encoder = 0;
   horizontal.move(80);
   while (++encoder != 50) {
      horizontal();
   }
   horizontal.move(30);
   if (launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 4\033[0m" << std::endl;
      ++error;
   }
   // wait->right_fast->braking->wait
   encoder = 0;
   horizontal.move(120);
   if (side != right or speed != fast or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 5\033[0m" << std::endl;
      ++error;
   }
   while (++encoder != 20) {
      horizontal();
      if (side != right or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 6\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal();
   while (++encoder !=120) {
      horizontal();
      if (side != right or speed != slow or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 7\033[0m" << std::endl;
            ++error;
      }
   }
   horizontal(); //to wait
   // wait->right_slow->right_fast->(new coordinate)braking->wait
   encoder = 0;
   horizontal.move(60);
   if (side != right or speed != slow or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 8\033[0m" << std::endl;
      ++error;
   }
   while (++encoder != 20) {
      horizontal();
      if (side != right or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 9\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal.move(150);
   if (side != right or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 10\033[0m" << std::endl;
         ++error;
   }
   while (++encoder != 40) {
      horizontal();
   }
   horizontal.move(120);
   if (side != right or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 11\033[0m" << std::endl;
         ++error;
   }
   while (++encoder != 120) {
      horizontal();
   }
   // breaking->right_fast
   horizontal.move(250);
   if (side != right or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 12\033[0m" << std::endl;
         ++error;
   }
   while (++encoder !=120) {
      horizontal();
   }
   // right_fast->right_fast
   horizontal.move(240);
   if (side != right or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 13\033[0m" << std::endl;
         ++error;
   }
   while (++encoder !=140) {
      horizontal();
   }
   horizontal(); //to braking
   while (++encoder !=200) {
      horizontal();
   }
   horizontal.move(250);
   if (side != right or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 14\033[0m" << std::endl;
         ++error;
   }
   while (++encoder != 250){
      horizontal();
   }
   horizontal(); //to wait

   // wait->left_slow->wait
   encoder = 0;
   horizontal.move(-50);
   if (side != left or speed != slow or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 15\033[0m" << std::endl;
      ++error;
   }
   while (--encoder !=-50) {
      horizontal();
      if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 16\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal();
   if (launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 17\033[0m" << std::endl;
      ++error;
   }
   // wait->left_slow->wait by new coordinate
   encoder = 0;
   horizontal.move(-80);
   while (--encoder != -50) {
      horizontal();
   }
   horizontal.move(-30);
   if (launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 18\033[0m" << std::endl;
      ++error;
   }
   // wait->left_fast->braking->wait
   encoder = 0;
   horizontal.move(-120);
   if (side != left or speed != fast or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 19\033[0m" << std::endl;
      ++error;
   }
   while (--encoder != -20) {
      horizontal();
      if (side != left or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 20\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal();
   while (--encoder !=-120) {
      horizontal();
      if (side != left or speed != slow or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 21\033[0m" << std::endl;
            ++error;
      }
   }
   horizontal(); //to wait
   // wait->left_slow->left_fast->(new coordinate)braking->wait
   encoder = 0;
   horizontal.move(-60);
   if (side != left or speed != slow or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 22\033[0m" << std::endl;
      ++error;
   }
   while (--encoder != -20) {
      horizontal();
      if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 23\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal.move(-150);
   if (side != left or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 24\033[0m" << std::endl;
         ++error;
   }
   while (--encoder != -40) {
      horizontal();
   }
   horizontal.move(-120);
   if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 25\033[0m" << std::endl;
         ++error;
   }
   while (--encoder != -120) {
      horizontal();
   }
   // breaking->left_fast
   horizontal.move(-250);
   if (side != left or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 26\033[0m" << std::endl;
         ++error;
   }
   while (--encoder !=-120) {
      horizontal();
   }
   // left_fast->right_fast
   horizontal.move(-240);
   if (side != left or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 27\033[0m" << std::endl;
         ++error;
   }
   while (--encoder !=-140) {
      horizontal();
   }
   horizontal(); //to braking
   while (--encoder !=-200) {
      horizontal();
   }
   horizontal.move(-250);
   if (side != left or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 28\033[0m" << std::endl;
         ++error;
   }
   while (--encoder != -250){
      horizontal();
   }
   horizontal(); //to wait



   if (error == 0)
      std::cout << "\033[32mtest_horizontal Пройден\033[0m" << std::endl;
   else 
      std::cout << "\033[31mtest_horizontal Провален\033[0m" << std::endl;
}