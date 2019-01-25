#include "pch.h"
#include "Car.h"
#include "GlobalVariables.h"

size_t Car::counter{ 0 };

Car::Car(size_t pCapacity, productType pProduct) : product{ pProduct } 
{
	if (IsSuchCapacity(pCapacity)) {
		capacity = pCapacity;
		counter++;
		number = counter;
	}
	else
		throw std::exception("There is no car with such carrying capacity");
}


void Car::Log(std::ostream& stream, const std::string& processMessage) const {
	std::lock_guard<std::mutex> lock(logMutex);

	const auto now = std::chrono::system_clock::now();
	const auto now_to_time_t{ std::chrono::system_clock::to_time_t(now) };
	struct tm buf;
	localtime_s(&buf, &now_to_time_t);

	stream << std::put_time(&buf, "%X") << ":"
		<< std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000
		<< "\tThe car No." << number << " (" << capacity << " boxes of " << productNames[static_cast<size_t>(product)]
		<< ") " << processMessage << std::endl;
}


bool Car::IsSuchCapacity(size_t pCapacity) const {
	auto p = std::find(capacities.begin(), capacities.end(), pCapacity);
	return (p != capacities.end());
}
