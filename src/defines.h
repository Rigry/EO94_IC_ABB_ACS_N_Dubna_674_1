#pragma once

#include <stdint.h>

#define STM32F030x6

#define F_OSC   8000000UL
#define F_CPU   48000000UL

const uint32_t fCPU = F_CPU;
constexpr uint32_t FCPU() { return F_CPU; }

