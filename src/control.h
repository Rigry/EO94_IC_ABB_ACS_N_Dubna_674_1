

template <class Speed, class Launch, class Side, class Finish>
struct Control
{
   static void init()
   {
      Speed  ::template configure<PinConf_t::Output>();
      Launch ::template configure<PinConf_t::Output>();
      Side   ::template configure<PinConf_t::Output>();
      Finish ::template configure<PinConf_t::Output>();
   }
   static void fast      () {Speed  ::set  ();}
   static void slow      () {Speed  ::clear();}
   static void right     () {Side   ::clear();}
   static void left      () {Side   ::set  ();}
   static void start     () {Launch ::set  ();}
   static void stop      () {Launch ::clear();}
   static void fast_stop () {Finish ::set  ();}
   static void slow_stop () {Finish ::clear();}
   static bool is_right  () {return Side::isClear();}
   static bool is_left   () {return Side::isSet()  ;}
};