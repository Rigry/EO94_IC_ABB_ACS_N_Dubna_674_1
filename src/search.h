enum Search {Wait, Left, Right, Ready, Unready} search {Search::Wait};

switch (search) {
            case Wait:
            break;
            case Left://save origin
               if(Origin::isSet()) {
                  search = Search::Ready;
               } else if (SenseLeft::isSet()) {
                  search = Search::Right;
               }
            break;
            case Right:
               if(Origin::isSet()) {
                  search = Search::Ready;
               } else if (SenseRight::isSet()) {
                  search = Search::Unready;
               }
            break;
            case Ready:
               global = Global::Automatic;
            break;
            case Unready:
               global = Global::Emergency;
            break;
      }
