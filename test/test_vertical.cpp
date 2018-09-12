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

// template<> bool MockPin<1>::set_ {false};
// template<> bool MockPin<2>::set_ {false};
// template<> bool MockPin<3>::set_ {false};
// template<> bool MockPin<4>::set_ {false};

using SenseUp   = MockPin<1>;
using SenseDown = MockPin<2>;
using Up        = MockPin<3>;
using Down      = MockPin<4>;

template<> bool SenseUp  ::set_ {false};
template<> bool SenseDown::set_ {false}; 
template<> bool Up       ::set_ {false};
template<> bool Down     ::set_ {false};

int main ()
{
   const auto time_pause {5000};
   const auto pause {3000};
   volatile auto time_count {0};
   int error = 0; 
   Vertical<SenseUp, Up, SenseDown, Down> vertical {5000};
   if (Up::set_ or Down::set_) {
      std::cout << "Ошибка конструктора" << std::endl;
      ++error;
   }
   
   vertical.up();
   if (not Up::set_) {
      std::cout << "Error test 2 " << std::endl;
      ++error;
   }
   
   SenseUp::set_ = true;
   if (not Up::set_) {
      std::cout << "Error test 3 " << std::endl;
      ++error;
   }
   
   tickUpdater.notify();
   if (Up::set_) {
      std::cout << "Error test 4 " << std::endl;
      ++error;
   }
   
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   
   vertical.down();
   if (not Down::set_) {
      std::cout << "Error test 5 " << std::endl;
      ++error;
   }
   
   SenseUp  ::set_ = false;
   SenseDown::set_ = true;
   if (not Down::set_) {
      std::cout << "Error test 6 " << std::endl;
      ++error;
   }
   
   tickUpdater.notify();
   if (Down::set_) {
      std::cout << "Error test 7 " << std::endl;
      ++error;
   }

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // after pause

   vertical.up();
   if (not Up::set_) {
      std::cout << "Error test 8" << std::endl;
      ++error;
   }

   SenseUp  ::set_ = false;
   SenseDown::set_ = false;

   vertical.down();
   if (Up::set_){
      std::cout << "Error 9" << std::endl;
      ++error;
   }

   // without pause
   if (Down::set_) {
      std::cout << "Error 10" << std::endl;
      ++error;
   }

   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // after pause

    if (not Down::set_) {
      std::cout << "Error 11" << std::endl;
      ++error;
   }

   vertical.up();
   if (Down::set_) {
      std::cout << "Error 12" << std::endl;
      ++error;
   }

   // without pause
   if (Up::set_) {
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

   if (Up::set_) {
      std::cout << "Error 14" << std::endl;
      ++error;
   }
   if (not Down::set_) {
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

   if (Down::set_) {
      std::cout << "Error 16" << std::endl;
      ++error;
   }
   if (not Up::set_) {
      std::cout << "Error 17" << std::endl;
      ++error;
   }

   SenseUp  ::set_ = true;
   SenseDown::set_ = false;
   vertical.stop();
   // without pause
   if (Up::set_ or Down::set_) {
      std::cout << "Error 18" << std::endl;
      ++error;
   }
   
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   if (Up::set_ or Down::set_) {
      std::cout << "Error 19" << std::endl;
      ++error;
   }

   vertical.down();
   SenseUp  ::set_ = false;
   SenseDown::set_ = true;
      
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;
   // After pause
   vertical.stop();
   if (Up::set_ or Down::set_) {
      std::cout << "Error 20" << std::endl;
      ++error;
   }
   
   vertical.up();
   SenseUp  ::set_ = false;
   SenseDown::set_ = false;
   vertical.down();
   vertical.stop();
   if (Up::set_ or Down::set_) {
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
   if (Up::set_ or Down::set_) {
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
   if (Up::set_ or Down::set_) {
      std::cout << "Error 23" << std::endl;
      ++error;
   }
   while (++time_count <= time_pause)
      tickUpdater.notify();
   time_count = 0;

   if (error == 0)
      std::cout << "\033[32mtest_vertical Пройден\033[0m" << std::endl;
   else 
      std::cout << "\033[31mtest_vertical Провален\033[0m" << std::endl;
}

