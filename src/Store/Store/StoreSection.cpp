#include "pch.h"
#include "StoreSection.h"

StoreSection::StoreSection(productType pProduct) : product{ pProduct } {	}

void StoreSection::MovingFromRoadToSection(RoadToSection& road) {
	while (true) {
		if (!road.isCarOnRoad)
		{
			std::unique_lock<std::mutex> lock(/**road.ptrMutex*/isCarOnRoadMutexes[static_cast<size_t>(product)]);
			isCarOnRoadCondVars[static_cast<size_t>(product)].wait(lock, [&] { return road.isCarOnRoad; });
		}
		if (isBusy)
		{
			std::unique_lock<std::mutex> lock(/**road.ptrMutex*/isBusyMutexes[static_cast<size_t>(product)]);
			isBusyCondVars[static_cast<size_t>(product)].wait(lock, [&] { return !isBusy; });
		}
				
		Car car = road.carQueue.front();
		
		try 
		{
			{
				std::lock_guard<std::mutex> lock(queueOnRoadMutexes[static_cast<size_t>(product)]);
				road.carQueue.pop();
				if (road.carQueue.empty())
					road.isCarOnRoad = false;
			}
			
			LogRoadLeaving(car);
			if (road.carQueue.size() == road.maxCars - 1) {
				{
					std::lock_guard<std::mutex> lock(isRoadFullMutexes[static_cast<size_t>(product)]);
					road.isRoadFull = false;
				}
				isRoadFullCondVars[static_cast<size_t>(product)].notify_one();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));
			MoveToSection(car);
		}
		catch (...) {
			std::cerr << "Problems when moving the car to the section";
		}
	}
}


void StoreSection::MoveToSection(Car& car) {
	if (!isBusy)
	{
		{
			std::lock_guard<std::mutex> lock(isBusyMutexes[static_cast<size_t>(product)]);
			isBusy = true;
			currentCar = car;
			LogArrivalInSection(car);
		}
		isBusyCondVars[static_cast<size_t>(product)].notify_one();
	}
	else
		throw std::exception("Section is busy");
}


void StoreSection::LogRoadLeaving(const Car& car) const {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "left the road to section");
}


void StoreSection::LogArrivalInSection(const Car& car) const {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "arrived in the section");
}
