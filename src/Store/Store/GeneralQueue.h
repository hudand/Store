#pragma once
#include "Car.h"
//#include "Car"

struct GeneralQueue {
	std::vector<std::queue<Car>> generalQueues;
	std::vector<bool> isCarInGeneralQueue;

	void GenerateCars();

private:
	static const int maxTimeInterval = 2000; // in milliseconds

	void Add(Car& car);
	void Log(const Car& car) const; 
};

