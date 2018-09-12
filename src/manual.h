#pragma once
#include "horizontal.h"

   void fast      () {Speed  ::set  ();}
   void slow      () {Speed  ::clear();}
   void right     () {Side   ::clear();}
   void left      () {Side   ::set  ();}
   void start     () {Launch ::set  ();}
   void stop      () {Launch ::clear();}
   void fast_stop () {Finish ::set  ();}
   void slow_stop () {Finish ::clear();}