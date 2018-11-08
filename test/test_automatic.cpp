#include "RCC.h"
#include "automatic.h"
#include <iostream>
#include <type_traits>
#include <stdint.h>


class Mock_horizontal
{
public:
   bool move_  {false};
   bool working{false};
   void move (int16_t coordinate){working =true; move_ = true;}
   void operator()(){}
   bool is_working(){return working;}
} horizontal;

class Mock_vertical
{
public:
   bool Sense_up   {false};
   bool Sense_down {false};
   bool up_        {false};
   bool down_      {false};
   bool working    {false};
   void up    ()   {working =true; up_ = true;}
   void down  ()   {working =true; down_ = true;}
   bool isUp  ()   {return Sense_up;}
   bool isDown()   {return Sense_down;}
   bool isWorking(){return working;}
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
      automatic.move_up(); //пробуем вертикальное перемещение
      if (up_ != false or down_ != false) {
         std::cout << "\033[36mError 2\033[0m" << std::endl;
         ++error;
      }
   }
   horizontal.working = false;
   automatic();
   move_ = false;
   automatic.move_down();
   if (up_ != false or down_ != true) {
         std::cout << "\033[36mError 3\033[0m" << std::endl;
         ++error;
      }
   automatic();
   Sense_up = false;
   Sense_down = true;
   automatic();
   down_ = false;
   automatic.move_up();
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
   vertical.working = false;
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
      std::cout << "\033[1;37mТест класса Automatic\033[0m   \033[1;32mpassed\033[0m" << std::endl;
   else 
      std::cout << "\033[1;37mТест класса Automatic\033[0m   \033[1;32mfailed\033[0m" << std::endl;
}