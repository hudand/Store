#pragma once
#include "Car.h"
#include "GlobalVariables.h"
#include "RoadToSection.h"

struct StoreSection {
	bool isBusy{ false };
	Car currentCar;
	productType product;
	
	StoreSection(productType pProduct);

	void MovingFromRoadToSection(RoadToSection& road);

private:
	const int intervalForSection = 100; // the time interval at which an attempt is made to move a new car into a section (in milliseconds)
	const int intervalForRoad = 20; // the time interval at which an attempt is made to take a new car from road (in milliseconds)

	void MoveToSection(Car& car);
	void LogRoadLeaving(const Car& car) const;
	void LogArrivalInSection(const Car& car) const;
};

