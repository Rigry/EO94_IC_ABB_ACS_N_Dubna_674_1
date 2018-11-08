LIBRARY_PATH = /home/alexandr/code/mculib2

C_INCLUDES_ =  
C_INCLUDES_ += -I../src
C_INCLUDES_ += -I$(LIBRARY_PATH)
C_INCLUDES_ += -I$(LIBRARY_PATH)/hal
C_INCLUDES_ += -I$(LIBRARY_PATH)/hal/ral

C_INCLUDES_ += -I$(LIBRARY_PATH)/STM32F0_files
C_INCLUDES_ += -I$(LIBRARY_PATH)/STM32F0_files/CMSIS 

CPP_FLAGS = -std=c++17 $(C_INCLUDES_) -DTEST -g  -Wno-register -O0
BUILD_DIR = build

all: $(BUILD_DIR)
	g++ test_calibration.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_calibration
	g++ test_horizontal.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_horizontal
	g++ test_automatic.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_automatic
	g++ test_vertical.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_vertical
	g++ test_search.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_search
	g++ test_manual.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_manual
	g++ test_global.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_global
	./$(BUILD_DIR)/test_calibration
	./$(BUILD_DIR)/test_horizontal
	./$(BUILD_DIR)/test_automatic
	./$(BUILD_DIR)/test_vertical
	./$(BUILD_DIR)/test_search
	./$(BUILD_DIR)/test_manual
	./$(BUILD_DIR)/test_global

$(BUILD_DIR):
	mkdir $@

current: $(BUILD_DIR)
	g++-7 test_global.cpp $(CPP_FLAGS) -o ./$(BUILD_DIR)/test_global
	./$(BUILD_DIR)/test_global
