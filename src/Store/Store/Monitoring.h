#pragma once
#include "StoreSection.h"

struct Monitoring {

	static void DisplayState(const std::vector<StoreSection>& sections, std::vector<RoadToSection>& roads, GeneralQueue& queue); 

private:
	static const int interval = 1000; // time interval through which the system status is displayed (in milliseconds)
};

