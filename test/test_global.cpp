#include "RCC.h"
#include "init_struct.h"
#include "global.h"
#include <iostream>  
#include <type_traits>
#include <stdint.h>


int16_t count{0};
struct Mock_encoder
{
   int16_t operator=   (int16_t v){return count = v;}
   int16_t operator!=  (int16_t v){return count!= v;}
           operator    int16_t()  {return count;}
};



template <class InRegs_t, class OutRegs_t>
struct Mock_modbus
{
	uint8_t address;
   union {
      InRegs_t inRegs;
      uint16_t arInRegs[10] {0};
   };
   union {
      OutRegs_t outRegs;
      uint16_t arOutRegs[10];
   };
   union {
      InRegs_t inRegsMin;
      uint16_t arInRegsMin[10] {0};
   };
   union {
      InRegs_t inRegsMax;
      uint16_t arInRegsMax[10] {0};
   };
	
};
Mock_modbus<InRegs, OutRegs> modbus;

template <class Data>
struct Mock_flash
{
	int16_t brake{0};
	uint16_t time_pause{0};
	int16_t min_coordinate{-100};
	int16_t max_coordinate{1000};
	uint16_t modbusAddress{0};
	uint16_t factoryNumber{0};
	USART_::sSettings uartSet;
};
Mock_flash <FlashData> flash;


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

using Sense_right = MockPin<1>;
using Sense_left  = MockPin<2>;
using Sense_up    = MockPin<3>;
using Sense_down  = MockPin<4>;
using Origin      = MockPin<5>;
using Tilt        = MockPin<6>;

template<> bool Sense_right::set_ {false};
template<> bool Sense_left ::set_ {false};
template<> bool Sense_up   ::set_ {false};
template<> bool Sense_down ::set_ {false};
template<> bool Origin     ::set_ {false};
template<> bool Tilt       ::set_ {true};

struct Mock_control
{
   States& states;
	Mock_control(States& states) : states {states}{}
	enum class Speed  {slow  = 0b0, fast}  speed  = Speed ::slow;
   enum class Side   {right = 0b0, left}  side   = Side  ::right;
   enum class Launch {stop  = 0b0, start} launch = Launch::stop;
   enum class Finish {slow_stop = 0b0, fast_stop} finish = Finish::fast_stop;
	bool init_{false};
   void init      () {init_ = true;}
   void fast      () {speed  = Speed ::fast; states.fast = true; states.slow = false;}
   void slow      () {speed  = Speed ::slow; }
   void right     () {side   = Side  ::right;}
   void left      () {side   = Side  ::left; }
   void start     () {launch = Launch::start;}
   void stop_h    () {launch = Launch::stop; }
   void fast_stop () {finish = Finish::fast_stop;}
   void slow_stop () {finish = Finish::slow_stop;}
   bool is_right  () {return side == Side::right;}
   bool is_left   () {return side == Side::left ;}
};


struct Mock_search
{
   Mock_control& control;
   Mock_encoder& encoder;
   Mock_search(Mock_control& control, Mock_encoder& encoder) : control{control}, encoder{encoder}{}
   static bool stop_;
   static bool reset_;
   static bool operator_;
   bool is_done(){return Origin::set_;}
   bool not_found(){return not Origin::set_;}
   void stop() {stop_ = true;}
   void reset(){reset_ = true;}
   void operator()(){operator_ = true; stop_ = false; reset_ = false;}
};
bool Mock_search::stop_ {false};
bool Mock_search::reset_ {false};
bool Mock_search::operator_ {false};

struct Mock_calibration
{
   Mock_control& control;
	Mock_encoder& encoder;
	int16_t& min_coordinate;
	int16_t& max_coordinate;
	Mock_calibration(Mock_control& control, Mock_encoder& encoder, int16_t& min_coordinate, int16_t& max_coordinate) 
	: control{control}
	, encoder{encoder}
	, min_coordinate{min_coordinate}
	, max_coordinate{max_coordinate}
	{}
   static bool operator_;
   static bool stop_;
   static bool reset_;
   bool done(){return Sense_right::set_;}
   void stop(){stop_ = true;}
   void reset(){reset_ = true;}
   void operator()(){operator_ = true; stop_ = false; reset_ = false;}
};
bool Mock_calibration::operator_{false};
bool Mock_calibration::stop_{false};
bool Mock_calibration::reset_{false};


struct Mock_horizontal
{
	Mock_control& control;
	Mock_encoder& encoder;
    static int16_t brake;
	Mock_horizontal(Mock_control& control, Mock_encoder& encoder, int16_t&) 
	: control{control}
	, encoder{encoder}
	{}
};
int16_t Mock_horizontal::brake {0};

struct Mock_vertical
{
   Mock_control& control;
	static uint16_t time_pause;
	Mock_vertical(Mock_control& control, uint16_t&) 
	: control{control}
	{}
};
uint16_t Mock_vertical::time_pause {0};

bool move_coordinate{false};
struct Mock_automatic
{
   Mock_horizontal& horizontal;
	Mock_vertical& vertical;
	Mock_encoder& encoder;
	Mock_automatic(Mock_horizontal& horizontal, Mock_vertical& vertical, Mock_encoder& encoder)
	: horizontal{horizontal}
	, vertical {vertical}
	, encoder {encoder}
	{}
   static bool operator_;
   static bool stop_;
   static bool reset_;
   static bool up;
	static bool down;
   void move(int16_t coordinate){move_coordinate = true; stop_ = false; reset_ = false;}
   void move_up(){up = true;}
   void move_down(){down = true;}
   void stop(){stop_ = true; up = false; down =false; move_coordinate = false;}
   void reset(){reset_ = true; up = false; down = false; move_coordinate = false;}
   void operator()(){operator_ = true; stop_ = false; reset_ = false;}
};
bool Mock_automatic::operator_{false};
bool Mock_automatic::stop_    {false};
bool Mock_automatic::reset_   {false};
bool Mock_automatic::up       {false};
bool Mock_automatic::down     {false};


struct Mock_manual
{
   Mock_control& control;
	Mock_vertical& vertical;
	Mock_manual(Mock_control& control, Mock_vertical& vertical)
	: control{control}
	, vertical{vertical}
	{}
	static bool stop_h_   ;
	static bool stop_v_   ;
	static bool up_       ;
	static bool down_     ;
	static bool slow_stop_;
	static bool fast_stop_;
	static bool slow_     ;
	static bool fast_     ;
	static bool right_    ;
	static bool left_     ;
	static bool reset_    ;
   static bool stop_     ;
	static bool operatop_ ;
	void stop_h   (){stop_h_ = true;}
	void stop_v   (){stop_v_ = true; up_ = false; down_ = false;}
	void up       (){up_ = true; down_ = false; stop_v_ = false;}
	void down     (){down_ = true; up_ = false; stop_v_ = false;}
	void slow_stop(){slow_stop_ = true; fast_stop_ = false;}
	void fast_stop(){fast_stop_ = true; slow_stop_ = false;}
	void slow     (){slow_ = true; fast_ = false;}
	void fast     (){fast_ = true; slow_ = false;}
	void right    (){right_ = true; left_ = false;}
	void left     (){left_ = true; right_ = false;}
	void reset    (){reset_ = true;}
    void stop     (){stop_ = true;}
	void operator ()(){operatop_ = true;}
};
bool Mock_manual::stop_h_   {false};
bool Mock_manual::stop_v_   {false};
bool Mock_manual::up_       {false};
bool Mock_manual::down_     {false};
bool Mock_manual::slow_stop_{false};
bool Mock_manual::fast_stop_{false};
bool Mock_manual::slow_     {false};
bool Mock_manual::fast_     {false};
bool Mock_manual::right_    {false};
bool Mock_manual::left_     {false};
bool Mock_manual::reset_    {false};
bool Mock_manual::stop_     {false};
bool Mock_manual::operatop_ {false};


// тест метода reaction:

bool uartSet()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   // настройка №1
	modbus.inRegs.uartSet.parityEnable = true;
	modbus.inRegs.uartSet.parity       = USART_::sParity::odd;
	modbus.inRegs.uartSet.stopBits     = USART_::sStopBits::_2;
	modbus.inRegs.uartSet.boudrate     = USART_::sBoudrate::_14400;
	global.reaction(ADR(uartSet));

   good &= modbus.outRegs.uartSet.parityEnable;
	good &= (modbus.outRegs.uartSet.parity   == USART_::sParity::odd);
	good &= (modbus.outRegs.uartSet.stopBits == USART_::sStopBits::_2);
	good &= (modbus.outRegs.uartSet.boudrate == USART_::sBoudrate::_14400);
	good &= flash.uartSet.parityEnable;
	good &= (flash.uartSet.parity            == USART_::sParity::odd);
	good &= (flash.uartSet.stopBits          == USART_::sStopBits::_2);
	good &= (flash.uartSet.boudrate          == USART_::sBoudrate::_14400);
   // настройка №2
	modbus.inRegs.uartSet.parityEnable = true;
	modbus.inRegs.uartSet.parity       = USART_::sParity::even;
	modbus.inRegs.uartSet.stopBits     = USART_::sStopBits::_2;
	modbus.inRegs.uartSet.boudrate     = USART_::sBoudrate::_19200;
	global.reaction(ADR(uartSet));

   good &= modbus.outRegs.uartSet.parityEnable;
	good &= (modbus.outRegs.uartSet.parity   == USART_::sParity::even);
	good &= (modbus.outRegs.uartSet.stopBits == USART_::sStopBits::_2);
	good &= (modbus.outRegs.uartSet.boudrate == USART_::sBoudrate::_19200);
	good &= flash.uartSet.parityEnable;
	good &= (flash.uartSet.parity            == USART_::sParity::even);
	good &= (flash.uartSet.stopBits          == USART_::sStopBits::_2);
	good &= (flash.uartSet.boudrate          == USART_::sBoudrate::_19200);
   // настройка №3
	modbus.inRegs.uartSet.parityEnable = false;
	modbus.inRegs.uartSet.parity = USART_::sParity::even;
	modbus.inRegs.uartSet.stopBits = USART_::sStopBits::_1;
	modbus.inRegs.uartSet.boudrate = USART_::sBoudrate::_115200;
	global.reaction(ADR(uartSet));

	good &= not modbus.outRegs.uartSet.parityEnable;
	good &= (modbus.outRegs.uartSet.parity   == USART_::sParity::even);
	good &= (modbus.outRegs.uartSet.stopBits == USART_::sStopBits::_1);
	good &= (modbus.outRegs.uartSet.boudrate == USART_::sBoudrate::_115200);
	good &= not flash.uartSet.parityEnable;
	good &= (flash.uartSet.parity            == USART_::sParity::even);
	good &= (flash.uartSet.stopBits          == USART_::sStopBits::_1);
	good &= (flash.uartSet.boudrate          == USART_::sBoudrate::_115200);

   return good;
}

bool modbusAddress()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};

   modbus.inRegs.modbusAddress = 100;
	global.reaction(ADR(modbusAddress));
	good &= (modbus.outRegs.modbusAddress == 100);
	good &= (flash.modbusAddress          == 100);
	
	modbus.inRegs.modbusAddress = 0xFFF;
	global.reaction(ADR(modbusAddress));
	good &= (modbus.outRegs.modbusAddress == 0xFFF);
	good &= (flash.modbusAddress          == 0xFFF);

   return good;
}

bool password_factoryNumber()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   // пароль для смены factoryNumber
	modbus.inRegs.password = 208;
	global.reaction(ADR(password));
	modbus.inRegs.factoryNumber = 100;
	global.reaction(ADR(factoryNumber));
	good &= (modbus.outRegs.factoryNumber == 100);
	good &= (flash.factoryNumber          == 100);
	// неправильный пароль не позволяет изменить factoryNumber
	modbus.inRegs.password = 108;
	global.reaction(ADR(password));
	modbus.inRegs.factoryNumber = 200;
	global.reaction(ADR(factoryNumber));
	good &= (modbus.outRegs.factoryNumber == 100);
	good &= (flash.factoryNumber          == 100);

   return good;
}

bool coordinate()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
    // реакция на координату не в атоматическом режиме
	modbus.inRegs.coordinate = 200;
	global.reaction(ADR (coordinate));
	good &= not move_coordinate;

   return good;
}

bool brake()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.brake = 100;
   global.reaction(ADR (brake));
   good &= (Mock_horizontal::brake == 100);
   good &= (flash.brake == 100);

   return good;
}

bool pause()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.time_pause = 3000;
   global.reaction(ADR (time_pause));
   good &= (Mock_vertical::time_pause == 3000);
   good &= (flash.time_pause == 3000);

   return good;
}

bool operation_enable()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
    // state = wait разрешение работы
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::search);

   return good;
}

bool search_disable()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   // state = search пришел запрет на работу
   modbus.inRegs.operation.enable = false;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::wait);

   return good;
}

bool search_to_manual()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   // переход в ручной режим
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::manual_mode);

   return good;
}

bool auto_disable()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   good &= (modbus.outRegs.states.mode == States::Mode::auto_mode);
   modbus.inRegs.operation.enable = false;
   global.reaction(ADR (operation));
   good &= Mock_automatic::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::wait);
	modbus.inRegs.operation.up = true;
   modbus.inRegs.operation.down = false;
	global.reaction(ADR (operation));
   good &= not Mock_automatic::up;
	good &= not Mock_automatic::down;

   return good;
}

bool auto_stop()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.stop_h = true;
   global.reaction(ADR (operation));
   good &= Mock_automatic::reset_;
   Mock_automatic::reset_ = false;
   modbus.inRegs.operation.stop_v = true;
   global.reaction(ADR (operation));
   good &= Mock_automatic::reset_;

   return good;
}

bool auto_move()
{
	bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
	modbus.inRegs.coordinate = 200;
	global.reaction(ADR (coordinate));
	good &= move_coordinate;
}

bool auto_move_disable()
{
	bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
	modbus.inRegs.coordinate = 200;
	global.reaction(ADR (coordinate));
	good &= move_coordinate;
	modbus.inRegs.operation.enable = false;
   global.reaction(ADR (operation));
	modbus.inRegs.operation.up = true;
   modbus.inRegs.operation.down = false;
   global.reaction(ADR (operation));
   good &= not Mock_automatic::up;
}

bool auto_up_down()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.up = true;
   modbus.inRegs.operation.down = false;
   global.reaction(ADR (operation));
   good &= Mock_automatic::up;
   Mock_automatic::up = false;
   modbus.inRegs.operation.up = false;
   modbus.inRegs.operation.down = true;
   global.reaction(ADR (operation));
   good &= Mock_automatic::down;
   Mock_automatic::down = false;
   modbus.inRegs.operation.up = true;
   modbus.inRegs.operation.down = true;
   global.reaction(ADR (operation));
   good &= not Mock_automatic::up;
	good &= not Mock_automatic::down;

   return good;
}

bool auto_to_manual()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::manual_mode);
   good &= Mock_automatic::reset_;

   return good;
}

bool auto_to_calibr()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::calibration);
   good &= Mock_automatic::reset_;

   return good;
}

bool manual_disable()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::manual_mode);
   modbus.inRegs.operation.enable = false;
   global.reaction(ADR (operation));
   good &= Mock_manual::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::wait);

   return good;
}

bool manual_stop_h()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   Mock_manual::stop_h_ = false;
   Mock_manual::stop_v_ = false;
   modbus.inRegs.operation.stop_h = true;
   modbus.inRegs.operation.stop_v = false;
   global.reaction(ADR (operation));
   good &= Mock_manual::stop_h_;

   return good;
}

bool manual_stop_v()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   Mock_manual::stop_h_ = false;
   Mock_manual::stop_v_ = false;
   modbus.inRegs.operation.stop_h = false;
   modbus.inRegs.operation.stop_v = true;
   global.reaction(ADR (operation));
   good &= Mock_manual::stop_v_;

   return good;
}

bool manual_up()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   Mock_manual::stop_h_ = false;
   Mock_manual::stop_v_ = false;
   modbus.inRegs.operation.stop_h = false;
   modbus.inRegs.operation.stop_v = false;
   modbus.inRegs.operation.up = true;
   modbus.inRegs.operation.down = false;
   global.reaction(ADR (operation));
   good &= Mock_manual::up_;

   return good;
}

bool manual_down()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   Mock_manual::stop_h_ = false;
   Mock_manual::stop_v_ = false;
   modbus.inRegs.operation.stop_h = false;
   modbus.inRegs.operation.stop_v = false;
   modbus.inRegs.operation.up = false;
   modbus.inRegs.operation.down = true;
   global.reaction(ADR (operation));
   good &= Mock_manual::down_;

   return good;
}

bool manual_slow_stop()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.braking = Operation::Braking::slow_stop;
   global.reaction(ADR (operation));
   good &= Mock_manual::slow_stop_;

   return good;
}

bool manual_fast_stop()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.braking = Operation::Braking::fast_stop;
   global.reaction(ADR (operation));
   good &= Mock_manual::fast_stop_;

   return good;
}

bool manual_slow()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.speed == Operation::Speed::slow;
   global.reaction(ADR (operation));
   good &= Mock_manual::slow_;

   return good;
}

bool manual_fast()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.speed = Operation::Speed::fast;
   global.reaction(ADR (operation));
   good &= Mock_manual::fast_;

   return good;
}

bool manual_right()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.right = true;
   modbus.inRegs.operation.left = true;
   modbus.inRegs.operation.stop_h = true;
   global.reaction(ADR (operation));
   good &= not Mock_manual::right_;
   modbus.inRegs.operation.right = true;
   modbus.inRegs.operation.left = false;
   modbus.inRegs.operation.stop_h = false;
   global.reaction(ADR (operation));
   good &= Mock_manual::right_;

   return good;
}

bool manual_left()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.right = false;
   modbus.inRegs.operation.left = true;
   modbus.inRegs.operation.stop_h = true;
   global.reaction(ADR (operation));
   good &= not Mock_manual::left_;
   good &= not modbus.outRegs.states.left;
   modbus.inRegs.operation.right = false;
   modbus.inRegs.operation.left = true;
   modbus.inRegs.operation.stop_h = false;
   global.reaction(ADR (operation));
   good &= Mock_manual::left_;

   return good;
}

bool manual_to_auto()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode;
   Tilt::clear();
   global.reaction(ADR (operation));
   good &= not Mock_manual::reset_;
   good &= not (modbus.outRegs.states.mode == States::Mode::auto_mode);
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode;
   Tilt::set();
   global.reaction(ADR (operation));
   good &= Mock_manual::reset_;
   good &= (modbus.outRegs.states.mode == States::Mode::auto_mode);

   return good;
}

bool emergency_to_manual()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::clear();
   global();
   good &= Mock_search::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   good &= (modbus.outRegs.states.mode == States::Mode::manual_mode);

   return good;
}

// тест метода operator():

bool search_not_up()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::clear();
   global();
   good &= Mock_search::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;

   return good;
}

bool search_up()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   global();
   good &= Mock_search::operator_;

   return good;
}

bool search_done()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   good &= Mock_search::reset_;
   good &= (modbus.outRegs.states.mode == States::Mode::auto_mode);
   good &= not modbus.outRegs.states.lost;

   return good;
}

bool search_not_found()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::clear();
   global();
   good &= Mock_search::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);

   return good;
}

bool search_tilt()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::clear();
   Tilt::set();
   global();
   good &= Mock_search::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);
   good &= modbus.outRegs.states.lost;

   return good;
}

bool auto_not_lost()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   Tilt::set();
   global();
   global();
   good &= Mock_automatic::operator_;
   good &= (modbus.outRegs.states.mode != States::Mode::search);

   return good;
}

bool auto_tilt()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   Tilt::clear();
   global();
   good &= Mock_automatic::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);
   Tilt::set();
   return good;
}

bool auto_lost_left()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   Sense_left::set();
   count = 100;
   global();
   good &= Mock_automatic::stop_;
   good &= (count == -100);
   good &= (modbus.outRegs.coordinate == -100);
   Sense_left::clear();
   return good;
}

bool auto_lost_right()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   Sense_right::set();
   count = 100;
   global();
   good &= Mock_automatic::stop_;
   good &= (count == 1000);
   good &= (modbus.outRegs.coordinate == 1000);
   Sense_right::clear();
   return good;
}

bool calibr_not_up()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   Sense_up::clear();
   global();
   good &= Mock_calibration::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);

   return good;
}

bool calibr_up()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   Sense_up::set();
   global();
   good &= Mock_calibration::operator_;

   return good;
}

bool calibr_done()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   Sense_up::set();
   Sense_right::set();
   global();
   good &= (modbus.outRegs.min_coordinate == -100);
   good &= (modbus.outRegs.max_coordinate == 1000);
   good &= Mock_calibration::reset_;
   good &= (modbus.outRegs.states.mode == States::Mode::auto_mode);
   good &= (modbus.outRegs.sensors.sense_up);

   return good;
}

bool calibr_tilt()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   Sense_up::set();
   Sense_right::clear();
   Tilt::clear();
   global();
   good &= Mock_calibration::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);

   return good;
}

bool calibr_auto_lost()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   Sense_up::set();
   Origin::set();
   global();
   modbus.inRegs.operation.mode = Operation::Mode::calibration;
   global.reaction(ADR (operation));
   Sense_up::set();
   Sense_right::clear();
   Tilt::clear();
   global();
   good &= Mock_calibration::stop_;
   good &= (modbus.outRegs.states.mode == States::Mode::emergency);
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   Tilt::set();
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode;
   global.reaction(ADR (operation));
   global();
   good &= (modbus.outRegs.states.mode == States::Mode::search);

   return good;
}

bool manual()
{
   bool good {true};
   Global <Mock_modbus<InRegs, OutRegs>, Mock_flash <FlashData>,
			  Mock_encoder, Mock_horizontal, Mock_vertical, 
			  Mock_manual, Mock_search, Mock_automatic, 
			  Mock_calibration, Mock_control, 
			  Origin, Sense_up, Sense_down, Tilt, Sense_right, Sense_left> 
			  global{modbus, flash};
   modbus.inRegs.operation.mode = Operation::Mode::auto_mode; // reset
   modbus.inRegs.operation.enable = true;
   global.reaction(ADR (operation));
   modbus.inRegs.operation.mode = Operation::Mode::manual_mode;
   global.reaction(ADR (operation));
   global();
   good &= Mock_manual::operatop_;

   return good;
}


int main()
{
   std::cout << '\n'
             << "\033[1;37mТест класса Global:\033[0m" << std::endl;
   std::cout << "\033[1;37m   тест метода reaction:\033[0m" << std::endl;

   auto test = [](auto s, auto f){
      std::cout << s << (f() ? "\033[1;32mpassed\033[0m" : "\033[1;31mfailed\033[0m") << std::endl;
   };

   test ("\033[37m   uartSet                \033[0m",uartSet);
   test ("\033[37m   modbusAddress          \033[0m",modbusAddress);
   test ("\033[37m   password_factoryNumber \033[0m",password_factoryNumber);
   test ("\033[37m   coordinate             \033[0m",coordinate);
   test ("\033[37m   brake                  \033[0m",brake);
   test ("\033[37m   pause                  \033[0m",pause);
   test ("\033[37m   operation_enable       \033[0m",operation_enable);
   test ("\033[37m   search_disable         \033[0m",search_disable);
   test ("\033[37m   search_to_manual       \033[0m",search_to_manual);
   test ("\033[37m   auto_disable           \033[0m",auto_disable);
   test ("\033[37m   auto_stop              \033[0m",auto_stop);
	test ("\033[37m   auto_move              \033[0m",auto_move);
	test ("\033[37m   auto_move_disable      \033[0m",auto_move_disable);
   test ("\033[37m   auto_up_down           \033[0m",auto_up_down);
   test ("\033[37m   auto_to_manual         \033[0m",auto_to_manual);
   test ("\033[37m   auto_to_calibr         \033[0m",auto_to_calibr);
   test ("\033[37m   manual_disable         \033[0m",manual_disable);
   test ("\033[37m   manual_stop_h          \033[0m",manual_stop_h);
   test ("\033[37m   manual_stop_v          \033[0m",manual_stop_v);
   test ("\033[37m   manual_up              \033[0m",manual_up);
   test ("\033[37m   manual_down            \033[0m",manual_down);
   test ("\033[37m   manual_slow_stop       \033[0m",manual_slow_stop);
   test ("\033[37m   manual_fast_stop       \033[0m",manual_fast_stop);
   test ("\033[37m   manual_slow            \033[0m",manual_slow);
   test ("\033[37m   manual_fast            \033[0m",manual_fast);
   test ("\033[37m   manual_right           \033[0m",manual_right);
   test ("\033[37m   manual_left            \033[0m",manual_left);
   test ("\033[37m   manual_to_auto         \033[0m",manual_to_auto);
   test ("\033[37m   emergency_to_manual    \033[0m",emergency_to_manual);
   std::cout << std::endl;

   std::cout << "\033[1;37m   тест метода operator():\033[0m" << std::endl;

   auto test1 = [](auto s, auto f){
      std::cout << s << (f() ? "\033[1;32mpassed\033[0m" : "\033[1;31mfailed\033[0m") << std::endl;
   };

   test1 ("\033[37m   search_not_up          \033[0m",search_not_up);
   test1 ("\033[37m   search_up              \033[0m",search_up);
   test1 ("\033[37m   search_done            \033[0m",search_done);
   test1 ("\033[37m   search_not_found       \033[0m",search_not_found);
   test1 ("\033[37m   search_tilt            \033[0m",search_tilt);
   test1 ("\033[37m   auto_not_lost          \033[0m",auto_not_lost);
   test1 ("\033[37m   auto_tilt              \033[0m",auto_tilt);
   test1 ("\033[37m   auto_lost_left         \033[0m",auto_lost_left);
   test1 ("\033[37m   auto_lost_right        \033[0m",auto_lost_right);
   test1 ("\033[37m   calibr_not_up          \033[0m",calibr_not_up);
   test1 ("\033[37m   calibr_up              \033[0m",calibr_up);
   test1 ("\033[37m   calibr_done            \033[0m",calibr_done);
   test1 ("\033[37m   calibr_tilt            \033[0m",calibr_tilt);
   test1 ("\033[37m   calibr_auto_lost       \033[0m",calibr_auto_lost);
   test1 ("\033[37m   manual                 \033[0m",manual);
   std::cout << std::endl;
}