#pragma once

template <class Horizontal, class Vertical, class Swing, class Encoder>
class Automatic
{
   Horizontal& horizontal;
   Vertical  & vertical;
   Swing     & swing;
   Encoder   & encoder;
   int16_t coordinate {0};
   uint16_t qty_swings {0};
   enum State {wait, horizontal_, vertical_, swing_} state {State::wait};
public:
   Automatic (Horizontal& horizontal, Vertical& vertical, Swing& swing, Encoder& encoder);
   void move(int16_t coordinate);
   void move_up();
   void move_down();
   void swing_start(uint16_t qty_swings);
   void stop();
   void reset();
   void operator()();
};

template <class Horizontal, class Vertical, class Swing, class Encoder>
Automatic <Horizontal, Vertical, Swing, Encoder>::Automatic (Horizontal& horizontal, Vertical& vertical, Swing& swing, Encoder& encoder)
   : horizontal {horizontal}
   , vertical   {vertical}
   , swing      {swing}
   , encoder    {encoder}
{}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::stop()
{
   reset();
}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::reset()
{
   horizontal.stop();
   vertical.stop();
   swing.stop();
   state = State::wait;
}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::move(int16_t coordinate)
{
   this->coordinate = coordinate;
   switch (state) {
      case wait:
         if (not vertical.isWorking() and (vertical.isUp() or vertical.isDown())) {
            horizontal.move(coordinate);
            state = State::horizontal_;
         }
      break;
      case horizontal_:
         horizontal.move(coordinate);
      break;
      case vertical_:
      break;
      case swing_:
      break;
   }
}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::move_up()
{
   switch(state) {
      case wait:
         if (not vertical.isUp())
            vertical.up();
         state = State::vertical_;
      break;
      case horizontal_:
      break;
      case vertical_:
         if (not vertical.isUp())
            vertical.up();
      break;
      case swing_:
      break;
   }
}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::move_down()
{
   switch(state) {
      case wait:
         if (not vertical.isDown())
            vertical.down();
         state = State::vertical_;
      break;
      case horizontal_:
      break;
      case vertical_:
         if (not vertical.isDown())
            vertical.down();
      break;
      case swing_:
      break;
   }
}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::swing_start(uint16_t qty_swings)
{
   this->qty_swings = qty_swings;
   switch(state) {
      case wait:
         if (not swing.is_done())
            swing.start();
         state = State::swing_;
      break;
      case horizontal_:
      break;
      case vertical_:
      break;
      case swing_:
      break;
   }

}

template <class Horizontal, class Vertical, class Swing, class Encoder>
void Automatic <Horizontal, Vertical, Swing, Encoder>::operator()()
{
   switch (state) {
      case wait:
      break;
      case horizontal_:
            // horizontal.interrupt();
         if (not horizontal.is_working()) {
            state = State::wait;
         }
      break;
      case vertical_:
         if (not vertical.isWorking() and (vertical.isUp() or vertical.isDown())){
            state = State::wait;
         }
      break;
      case swing_:
         if (not swing.isWorking()){
            state = State::wait;
         }
      break;
   }
}