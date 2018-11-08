#pragma once

template <class Horizontal, class Vertical, class Encoder>
class Automatic
{
   Horizontal& horizontal;
   Vertical  & vertical;
   Encoder   & encoder;
   int16_t coordinate {0};
   enum State {wait, horizontal_, vertical_} state {State::wait};
public:
   Automatic (Horizontal& horizontal, Vertical& vertical, Encoder& encoder);
   void move(int16_t coordinate);
   void move_up();
   void move_down();
   void stop();
   void reset();
   void operator()();
};

template <class Horizontal, class Vertical, class Encoder>
Automatic <Horizontal, Vertical, Encoder>::Automatic (Horizontal& horizontal, Vertical& vertical, Encoder& encoder)
   : horizontal {horizontal}
   , vertical   {vertical}
   , encoder    {encoder}
{}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::stop()
{
   reset();
}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::reset()
{
   horizontal.stop();
   vertical.stop();
   state = State::wait;
}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::move(int16_t coordinate)
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
   }
}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::move_up()
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
   }
}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::move_down()
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
   }
}

template <class Horizontal, class Vertical, class Encoder>
void Automatic <Horizontal, Vertical, Encoder>::operator()()
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
   }
}