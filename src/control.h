#pragma once

template <class Speed, class Launch, class Side, class Finish, class Up, class Down>
struct Control
{
   States& states;
   Control (States& states):states{states}{}
   void init()
   {
      Speed ::template configure<PinConf_t::Output>();
      Launch::template configure<PinConf_t::Output>();
      Side  ::template configure<PinConf_t::Output>();
      Finish::template configure<PinConf_t::Output>();
      Up    ::template configure<PinConf_t::Output>();
      Down  ::template configure<PinConf_t::Output>();
   }
   void up        () {Up     ::set  (); states.up        = true; states.stop_v    = false;}
   void down      () {Down   ::set  (); states.down      = true; states.stop_v    = false;}
   void stop_v    () {Up     ::clear(); states.stop_v    = true; states.up        = false;
                      Down   ::clear();                          states.down      = false;}
   void fast      () {Speed  ::set  (); states.fast      = true; states.slow      = false;}
   void slow      () {Speed  ::clear(); states.slow      = true; states.fast      = false;}
   void right     () {Side   ::clear(); states.right     = true; states.left      = false;}
   void left      () {Side   ::set  (); states.left      = true; states.right     = false;}
   void fast_stop () {Finish ::set  (); states.fast_stop = true; states.slow_stop = false;}
   void slow_stop () {Finish ::clear(); states.slow_stop = true; states.fast_stop = false;}
   void start     () {Launch ::set  (); states.stop_h    = false;}
   void stop_h    () {Launch ::clear(); states.stop_h    = true ;}
   bool is_right  () {return Side::isClear();}
   bool is_left   () {return Side::isSet()  ;}
};