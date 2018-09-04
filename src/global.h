enum Global {Search, Automatic, Calibration, Manual_, Emergency} global {Global::Search};

switch (global) { //add manual
            case Global::Search:
               if (Origin::isSet()) {
                  global = Global::Automatic;
               } else if (Origin::isClear() or Tilt::isSet()) {
                  global = Global::Emergency;
               }
            break;
            case Global::Automatic:
               if (SenseLeft::isSet() or SenseRight::isSet() or Tilt::isSet())
                  global = Global::Emergency;
               else if (true /*coordinate lost*/)
                  global = Global::Search;
               else if (true /*manual*/)
                  global = Global::Manual_;
            break;
            case Global::Calibration:
               if (calibration == Complet) {
                  global = Global::Automatic;
               } else if (Tilt::isClear())
                  global = Global::Emergency;
                     
            break;
            case Global::Manual_:
               if (true /*auto*/)
                  global = Global::Automatic;
            break;
            case Global::Emergency:
               horizontal.stop();
               if (true /*manual*/)
                  global = Global::Manual_;
            break;
      }