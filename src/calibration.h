enum Calibration {Left_slow, Right_slow, Return, Complet} calibration {Calibration::Left_slow};

switch (calibration) {
         case Left_slow://cохранить координату
            if(SenseLeft::isSet())
               calibration = Calibration::Right_slow;
         break;
         case Right_slow://сохранить координату
            if(SenseRight::isSet())
               calibration = Calibration::Return;
         break;
         case Return:
            if(Origin::isSet())
               calibration = Calibration::Complet;
         break;
         case Complet:
            global = Global::Automatic;
         break;
      }