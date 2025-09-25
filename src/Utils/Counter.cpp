#include "Utils/Counter.h"

unsigned int globalCounter = 0;

Utils::TickCounter::TickCounter() : m_interval(1) {}

Utils::TickCounter::TickCounter(const int& interval) : m_interval(interval) {}

bool Utils::TickCounter::onTick() { return (globalCounter % 8) == 0; }

int Utils::TickCounter::tickCount() const { return ((globalCounter / 8) % m_interval); }

void Utils::resetGlobalCounter() { globalCounter = 0; }

void Utils::incrementGlobalCounter() { globalCounter++; }
