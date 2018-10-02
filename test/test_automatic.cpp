#include "RCC.h"
#include "automatic.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>


class Mock_horizontal
{
public:
   bool move_ {false};
   void move (int16_t coordinate){move_ = true;}
   void operator()(){}
} horizontal;

class Mock_vertical
{
public:
   bool Sense_up  {false};
   bool Sense_down{false};
   bool up_       {false};
   bool down_     {false};
   void up    ()  {up_ = true;}
   void down  ()  {down_ = true;}
   bool isUp  ()  {return Sense_up;}
   bool isDown()  {return Sense_down;}
} vertical;

auto& move_ = horizontal.move_;
auto& Sense_up = vertical.Sense_up;
auto& Sense_down = vertical.Sense_down;
auto& up_ = vertical.up_;
auto& down_ = vertical.down_;

int main()
{
   int error = 0; 
   int16_t encoder {0};
   Automatic <Mock_horizontal, Mock_vertical, int16_t> automatic {horizontal, vertical, encoder};

   encoder = 0;
   Sense_up = true;
   automatic.move(300);
   if (move_ != true) {
      std::cout << "\033[36mError 1\033[0m" << std::endl;
      ++error;
   }
   while (++encoder != 300) {
      automatic();
      automatic.move(); //пробуем вертикальное перемещение
      if (up_ != false or down_ != false) {
         std::cout << "\033[36mError 2\033[0m" << std::endl;
         ++error;
      }
   }
   automatic();
   move_ = false;
   automatic.move();
   if (up_ != false or down_ != true) {
         std::cout << "\033[36mError 3\033[0m" << std::endl;
         ++error;
      }
   automatic();
   Sense_up = false;
   Sense_down = true;
   automatic();
   down_ = false;
   automatic.move();
   if (up_ != true or down_ != false) {
         std::cout << "\033[36mError 4\033[0m" << std::endl;
         ++error;
      }
   Sense_down = false;
   automatic();
   automatic.move(150);
   if (move_ != false) {
      std::cout << "\033[36mError 5\033[0m" << std::endl;
      ++error;
   }
   Sense_up = true;
   automatic();
   automatic.move(150);
   if (move_ != true) {
      std::cout << "\033[36mError 6\033[0m" << std::endl;
      ++error;
   }
   while (--encoder != 150) {
      automatic();
   }
   


   

   if (error == 0)
      std::cout << "\033[32mtest_automatic Пройден\033[0m" << std::endl;
   else 
      std::cout << "\033[31mtest_automatic Провален\033[0m" << std::endl;
}