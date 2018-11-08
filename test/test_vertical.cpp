#include "RCC.h"
#include "vertical.h"
#include "iostream"
#include <type_traits>

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

struct Mock_control
{
   bool Up {false};
   bool Down {false};
   void init() {}
   void up     () {Up   = true;}
   void down   () {Down = true;}
   void stop_v () {Up   = false;
                   Down = false;}
   
} control;

using Sense_up   = MockPin<1>;
using Sense_down = MockPin<2>;

template<> bool Sense_up  ::set_ {false};
template<> bool Sense_down::set_ {false}; 

auto& Up   = control.Up;
auto& Down = control.Down;

int main ()
{
   const auto time_pause {5000};
   const auto pause {3000};
   volatile auto time_count {0};
   int error = 0; 
   Vertical<Mock_control, Sense_up, Sense_down> vertical {control, 5000};
   if (Up or Down) {
      std::cout << "Ошибка конструктора" << std::endl;
      ++error;
   }
   
   vertical.up();
   if (not Up) {
      std::cout << "Error test 2 " << std::endl;
      ++error;
   }
   
   Sense_up::set_ = true;
   if (not Up) {
      std::cout << "Error test 3 " << std::endl;
      ++error;
   }
   
   tickUpdater.notify();
   if (Up) {
      std::cout << "Error test 4 " << std::endl;
      ++error;
   }
   
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   
   vertical.down();
   if (not Down) {
      std::cout << "Error test 5 " << std::endl;
      ++error;
   }
   
   Sense_up  ::set_ = false;
   Sense_down::set_ = true;
   if (not Down) {
      std::cout << "Error test 6 " << std::endl;
      ++error;
   }
   
   tickUpdater.notify();
   if (Down) {
      std::cout << "Error test 7 " << std::endl;
      ++error;
   }

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // after pause

   vertical.up();
   if (not Up) {
      std::cout << "Error test 8" << std::endl;
      ++error;
   }

   Sense_up  ::set_ = false;
   Sense_down::set_ = false;

   vertical.down();
   if (Up){
      std::cout << "Error 9" << std::endl;
      ++error;
   }

   // without pause
   if (Down) {
      std::cout << "Error 10" << std::endl;
      ++error;
   }

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // after pause

    if (not Down) {
      std::cout << "Error 11" << std::endl;
      ++error;
   }

   vertical.up();
   if (Down) {
      std::cout << "Error 12" << std::endl;
      ++error;
   }

   // without pause
   if (Up) {
      std::cout << "Error 13" << std::endl;
      ++error;
   }
   
   while (++time_count <= time_pause/2)
      tickUpdater.notify();
   tickUpdater.notify();
   vertical.down();
   // while is pause

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
      // after pause

   if (Up) {
      std::cout << "Error 14" << std::endl;
      ++error;
   }
   if (not Down) {
      std::cout << "Error 15" << std::endl;
      ++error;
   }

   vertical.up();
   vertical.down();
   while (++time_count <= time_pause/2)
      tickUpdater.notify();
   tickUpdater.notify();
   vertical.up();
   // while is pause

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
      // after pause

   if (Down) {
      std::cout << "Error 16" << std::endl;
      ++error;
   }
   if (not Up) {
      std::cout << "Error 17" << std::endl;
      ++error;
   }

   Sense_up  ::set_ = true;
   Sense_down::set_ = false;
   vertical.stop();
   // without pause
   if (Up or Down) {
      std::cout << "Error 18" << std::endl;
      ++error;
   }
   
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   if (Up or Down) {
      std::cout << "Error 19" << std::endl;
      ++error;
   }

   vertical.down();
   Sense_up  ::set_ = false;
   Sense_down::set_ = true;
      
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   vertical.stop();
   if (Up or Down) {
      std::cout << "Error 20" << std::endl;
      ++error;
   }
   
   vertical.up();
   Sense_up  ::set_ = false;
   Sense_down::set_ = false;
   vertical.down();
   vertical.stop();
   if (Up or Down) {
      std::cout << "Error 21" << std::endl;
      ++error;
   }

   vertical.down();
   vertical.up();
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   vertical.stop();
   if (Up or Down) {
      std::cout << "Error 22" << std::endl;
      ++error;
   }

   vertical.down();
   vertical.up();
   while (++time_count <= time_pause/2)
      tickUpdater.notify();
   tickUpdater.notify();
   // while is pause
   vertical.stop();
   if (Up or Down) {
      std::cout << "Error 23" << std::endl;
      ++error;
   }
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;

   if (error == 0)
      std::cout << "\033[1;37mТест класса Vertical\033[0m    \033[1;32mpassed\033[0m" << std::endl;
   else 
      std::cout << "\033[1;37mТест класса Vertical\033[0m    \033[1;31mfailed\033[0m" << std::endl;
}

