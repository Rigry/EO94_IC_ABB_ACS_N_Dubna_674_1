#include "RCC.h"
#include "horizontal.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>

struct MockControl
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

using Speed  = MockControl::Speed;
using Side   = MockControl::Side;
using Launch = MockControl::Launch;
using Finish = MockControl::Finish;

// Speed  MockControl::Speed {Speed ::slow };
// Side   MockControl::Side  {Side  ::right};
// Launch MockControl::launch{Launch::stop };
// Finish MockControl::finish{Finish::fast_stop};

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
int16_t compare{0};
bool subs {false};
bool IRQn {false};
bool interrupt_ {false};

class MockEncoder
{
public:
//    Encoder ();
   MockEncoder& withInterrupt(){IRQn = true;}
   void subscribe(Interrupting* p) {subs = true;}
   void enableInterrupt (){interrupt_ = true;}
   void disableInterrupt(){interrupt_ = false;}
   void setCompare(int16_t v){compare = v;}
   int16_t operator=  (int16_t v) {return count = v;}
   int16_t operator+  (int16_t v) {return count + v;}
   int16_t operator-  (int16_t v) {return count - v;}
   bool    operator>  (int16_t v) {return count > v;}
   bool    operator<  (int16_t v) {return count < v;}
   bool    operator>= (int16_t v) {return count >=v;}
   bool    operator<= (int16_t v) {return count >=v;}
           operator   int16_t()   {return count;}
} encoder;

int main ()
{
   int error = 0; 
   encoder = 0;
   Horizontal <MockControl, MockEncoder> horizontal {control, encoder, 100};
   if (subs != true or IRQn != true) {
      std::cout << "\033[31mError 1\033[0m" << std::endl;
      ++error;
   }

   // wait->right_slow->wait
   horizontal.move(50);
   if (interrupt_ != true or compare != 50 or side != right    
      or speed != slow or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 2\033[0m" << std::endl;
      ++error;
   }
   while (++count != compare) {
      if (interrupt_ != true or side != right or speed != slow 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 3\033[0m" << std::endl;
         ++error;
      }
   }
   if (count == compare) {
      horizontal.interrupt();
       if (interrupt_ != false or launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 4\033[0m" << std::endl;
      ++error;
      }
   }
   // wait->right_slow->wait->left_slow->wait by new coordinate
   encoder = 0;
   horizontal.move(80);
   encoder = 50;
   horizontal.move(30);
   if (interrupt_ != true or compare != 30 or side != left or speed != slow
      or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 5\033[0m" << std::endl;
      ++error;
      }
   while (--count != compare) {
      if (interrupt_ != true or side != left or speed != slow 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 6\033[0m" << std::endl;
         ++error;
      }
   }
   if (count == compare) {
      horizontal.interrupt();
       if (interrupt_ != false or launch != stop or finish != fast_stop) {
      std::cout << "\033[31mError 7\033[0m" << std::endl;
      ++error;
      }
   }
   // wait->right_fast->braking->wait
   encoder = 0;
   horizontal.move(120);
   if (interrupt_ != true or compare != 20 or side != right or speed != fast 
      or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 8\033[0m" << std::endl;
      ++error;
   }
   while (++count != compare) {
      if (interrupt_ != true or compare != 20 or side != right or speed != fast 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 9\033[0m" << std::endl;
         ++error;
      }
   }
   if (count == compare) {
      horizontal.interrupt();
      if (interrupt_ != true or compare != 120 or side != right or speed != slow 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 10\033[0m" << std::endl;
         ++error;
      }
   }
   while (++count != compare) {
      if (interrupt_ != true or compare != 120 or side != right or speed != slow 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 11\033[0m" << std::endl;
         ++error;
      }
   }
   if (count == compare) {
      horizontal.interrupt();
      if (interrupt_ != false or compare != 120 or launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 12\033[0m" << std::endl;
         ++error;
      }
   }
   // wait->right_slow->right_fast->braking->wait
   encoder = 0;
   horizontal.move(60);
   if (interrupt_ != true or compare != 60 or side != right or speed != slow 
      or finish != slow_stop or launch != start) {
      std::cout << "\033[31mError 13\033[0m" << std::endl;
      ++error;
   }
   encoder = 20;
   horizontal.move(150);
   if (interrupt_ != true or compare != 50 or side != right or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 14\033[0m" << std::endl;
         ++error;
   }
   encoder = 50;
   if (count == compare) {
      horizontal.interrupt();
      if (interrupt_ != true or compare != 150 or side != right 
         or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 15\033[0m" << std::endl;
         ++error;
      }
   }
   while (++count != compare) {
      if (interrupt_ != true or compare != 150 or side != right or speed != slow 
         or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 16\033[0m" << std::endl;
         ++error;
      }
   }
   if (count == compare) {
      horizontal.interrupt();
      if (interrupt_ != false or compare != 150 or launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 17\033[0m" << std::endl;
         ++error;
      }
   }

   // right_fast->breaking->right_fast
   encoder = 0;
   horizontal.move(120);
   encoder = 20;
   if (count == compare) horizontal.interrupt();
   horizontal.move(250);
   if (interrupt_ != true or compare != 150 or side != right 
      or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 18\033[0m" << std::endl;
         ++error;
   }
   encoder = 150;
   if (count == compare) {
      horizontal.interrupt();
      if (interrupt_ != true or compare != 250 or side != right 
         or speed != slow or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 19\033[0m" << std::endl;
         ++error;
      }
   }
   encoder = 250;
   if (count == compare) horizontal.interrupt();

   // right_fast->right_fast
   encoder = 0;
   horizontal.move(240);
   encoder = 100;
   horizontal.move(300);
   if (interrupt_ != true or compare != 200 or side != right 
      or speed != fast or launch != start or finish != slow_stop) {
         std::cout << "\033[31mError 20\033[0m" << std::endl;
         ++error;
   }
   encoder = 200;
   if (count == compare) horizontal.interrupt();
   encoder = 300;
   if (count == compare) horizontal.interrupt();
   
   // wait->left_slow->wait
   encoder = 0;
      horizontal.move(-50);
      if (interrupt_ != true or compare != -50 or side != left    
         or speed != slow or finish != slow_stop or launch != start) {
         std::cout << "\033[31mError 22\033[0m" << std::endl;
         ++error;
      }
      while (--count != compare) {
         if (interrupt_ != true or side != left or speed != slow 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 23\033[0m" << std::endl;
            ++error;
         }
      }
      if (count == compare) {
         horizontal.interrupt();
          if (interrupt_ != false or launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 24\033[0m" << std::endl;
         ++error;
         }
      }
      // wait->left_slow->wait-right_slow->wait by new coordinate
      encoder = 0;
      horizontal.move(-80);
      encoder = -50;
      horizontal.move(-30);
      if (interrupt_ != true or compare != -30 or side != right or speed != slow
         or finish != slow_stop or launch != start) {
         std::cout << "\033[31mError 25\033[0m" << std::endl;
         ++error;
         }
      while (--count != compare) {
         if (interrupt_ != true or compare != -30 or side != right or speed != slow 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 26\033[0m" << std::endl;
            ++error;
         }
      }
      if (count == compare) {
         horizontal.interrupt();
          if (interrupt_ != false or launch != stop or finish != fast_stop) {
         std::cout << "\033[31mError 27\033[0m" << std::endl;
         ++error;
         }
      }
      // wait->left_fast->braking->wait
      encoder = 0;
      horizontal.move(-120);
      if (interrupt_ != true or compare != -20 or side != left or speed != fast 
         or finish != slow_stop or launch != start) {
         std::cout << "\033[31mError 28\033[0m" << std::endl;
         ++error;
      }
      while (--count != compare) {
         if (interrupt_ != true or compare != -20 or side != left or speed != fast 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 29\033[0m" << std::endl;
            ++error;
         }
      }
      if (count == compare) {
         horizontal.interrupt();
         if (interrupt_ != true or compare != -120 or side != left or speed != slow 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 30\033[0m" << std::endl;
            ++error;
         }
      }
      while (--count != compare) {
         if (interrupt_ != true or compare != -120 or side != left or speed != slow 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 31\033[0m" << std::endl;
            ++error;
         }
      }
      if (count == compare) {
         horizontal.interrupt();
         if (interrupt_ != false or compare != -120 or launch != stop or finish != fast_stop) {
            std::cout << "\033[31mError 32\033[0m" << std::endl;
            ++error;
         }
      }
      // wait->left_slow->left_fast->braking->wait
      encoder = 0;
      horizontal.move(-60);
      if (interrupt_ != true or compare != -60 or side != left or speed != slow 
         or finish != slow_stop or launch != start) {
         std::cout << "\033[31mError 33\033[0m" << std::endl;
         ++error;
      }
      encoder = -20;
      horizontal.move(-150);
      if (interrupt_ != true or compare != -50 or side != left or speed != fast or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 34\033[0m" << std::endl;
            ++error;
      }
      encoder = -50;
      if (count == compare) {
         horizontal.interrupt();
         if (interrupt_ != true or compare != -150 or side != left 
            or speed != slow or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 35\033[0m" << std::endl;
            ++error;
         }
      }
      while (--count != compare) {
         if (interrupt_ != true or compare != -150 or side != left or speed != slow 
            or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 36\033[0m" << std::endl;
            ++error;
         }
      }
      if (count == compare) {
         horizontal.interrupt();
         if (interrupt_ != false or compare != -150 or launch != stop or finish != fast_stop) {
            std::cout << "\033[31mError 37\033[0m" << std::endl;
            ++error;
         }
      }
      // left_fast->breaking->left_fast
      encoder = 0;
      horizontal.move(-120);
      encoder = -20;
      if (count == compare) horizontal.interrupt();
      horizontal.move(-250);
      if (interrupt_ != true or compare != -150 or side != left 
         or speed != fast or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 38\033[0m" << std::endl;
            ++error;
      }
      encoder = -150;
      if (count == compare) {
         horizontal.interrupt();
         if (interrupt_ != true or compare != -250 or side != left 
            or speed != slow or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 39\033[0m" << std::endl;
            ++error;
         }
      }
      encoder = -250;
      if (count == compare) horizontal.interrupt();
   
      // left_fast->left_fast
      encoder = 0;
      horizontal.move(-240);
      encoder = -100;
      horizontal.move(-300);
      if (interrupt_ != true or compare != -200 or side != left 
         or speed != fast or launch != start or finish != slow_stop) {
            std::cout << "\033[31mError 40\033[0m" << std::endl;
            ++error;
      }
   encoder = -200;
   if (count == compare) horizontal.interrupt();
   encoder = -300;
   if (count == compare) horizontal.interrupt();


   if (error == 0)
      std::cout << "\033[1;37mТест класса Horizontal\033[0m  \033[1;32mpassed\033[0m" << std::endl;
   else 
      std::cout << "\033[1;37mТест класса Horizontal\033[0m  \033[1;31mfailed\033[0m" << std::endl;
}