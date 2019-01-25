#pragma once
#include "Car.h"
#include "GlobalVariables.h"
#include "GeneralQueue.h"

struct RoadToSection {

	std::queue<Car> carQueue;
	bool isRoadFull;
	bool isCarOnRoad;
	mutable std::unique_ptr<std::mutex> ptrMutex;
	static const size_t maxCars = 4;
	
	RoadToSection(productType pProduct);
	RoadToSection(const RoadToSection& road);
	RoadToSection& operator=(const RoadToSection& road);
	RoadToSection(RoadToSection&& road);
	RoadToSection& operator=(RoadToSection&& road);

	void MovingFromQueueToRoad(GeneralQueue& queue);

private:
	productType product;

	void Add(Car& car);
	void LogQueueLeaving(const Car& car) const;
	void LogArrivalOnRoad(const Car& car) const;
};
