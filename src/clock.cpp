#include "types.h"

#include <iostream>
#include <iomanip>

Clock::Clock(std::string funcname) : m_funcname(funcname) {
	std::cout << "Clock started for " << funcname << std::endl;
	m_clock = clock();
}
Clock::~Clock() {
	double time = (clock() - m_clock) / (CLOCKS_PER_SEC + 0.0);
	std::cout << std::fixed << std::setw(11) << std::setprecision(6);
	std::cout << "Finished " << m_funcname << "(" << time << ")\n";
}
