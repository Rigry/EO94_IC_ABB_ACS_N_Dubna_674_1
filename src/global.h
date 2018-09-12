#pragma once
#include "search.h"
#include "calibration.h"

struct Global
{
   Search search;

   enum State {Search, Automatic, Calibration, Manual, Emergency} 
   state {State::Search};

   switch (state) { //automatic and manual are called from modbus
      case Search:
         if (search.isReady()) {
            state = State::Automatic;
         } else if ((not search) or Tilt::isSet()) {
            state = State::Emergency;
         }
      break;
      case Automatic:
         if (SenseLeft::isSet() or SenseRight::isSet() or Tilt::isSet())
            state = State::Emergency;
         else if (true /*coordinate lost*/)
            state = State::Search;
         else if (true /*калибровка*/)
            state = State::Calibration;
      break;
      case Calibration:
         if (calibration) {
            state = State::Automatic;
         } else if (Tilt::isClear())
            state = State::Emergency;
      break;
      case Emergency:
         horizontal.main_stop();
         vertical.main_stop();
      break;
   }
}