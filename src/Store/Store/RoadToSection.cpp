#include "pch.h"
#include "RoadToSection.h"


RoadToSection::RoadToSection(productType pProduct) : product{ pProduct }, isRoadFull{ false }, isCarOnRoad{ false } {
	ptrMutex = std::make_unique<std::mutex>();
}


RoadToSection::RoadToSection(const RoadToSection& road) : product{ road.product }, isRoadFull{ road.isRoadFull },
isCarOnRoad{ road.isCarOnRoad }, carQueue{ road.carQueue }, ptrMutex{ std::move(road.ptrMutex) }
{

}


RoadToSection& RoadToSection::operator=(const RoadToSection& road) {
	RoadToSection temp{ road };
	std::swap(temp, *this);
	return *this;
}


RoadToSection::RoadToSection(RoadToSection&& road) {
	*this = std::move(road);
}


RoadToSection& RoadToSection::operator=(RoadToSection&& road)
{
	std::swap(road, *this);
	return *this;
}


void RoadToSection::MovingFromQueueToRoad(GeneralQueue& queue) {
	while (true)
	{
		if (!queue.isCarInGeneralQueue[static_cast<size_t>(product)])
		{
			std::unique_lock<std::mutex> lock(isCarInQueue[static_cast<size_t>(product)]);
			isCarInQueueCondVars[static_cast<size_t>(product)].wait(lock, [&] { return queue.isCarInGeneralQueue[static_cast<size_t>(product)]; });
		}

		if (isRoadFull) 
		{
			std::unique_lock<std::mutex> lock(isRoadFullMutexes[static_cast<size_t>(product)]);
			isRoadFullCondVars[static_cast<size_t>(product)].wait(lock, [&] { return !isRoadFull; });
		}
		
		std::vector<Car> movingCars;
		
		size_t carCount = carQueue.size();
		// We remove the cars from the general queue and add them to the temporary vector.  
		// This is necessary to be able to send several cars on the road to the section at one moment
		while ((carCount < maxCars) && (!queue.generalQueues[static_cast<size_t>(product)].empty()))
		{
			std::lock_guard<std::mutex> lockGeneralQueue(generalQueueMutexes[static_cast<size_t>(product)]);
			Car car = queue.generalQueues[static_cast<size_t>(product)].front();
			movingCars.push_back(car);
			queue.generalQueues[static_cast<size_t>(product)].pop();
			LogQueueLeaving(car);
			carCount++;
		}
			
		if (queue.generalQueues[static_cast<size_t>(product)].empty())
			queue.isCarInGeneralQueue[static_cast<size_t>(product)] = false;

		std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));

		for (Car car : movingCars)
		{
			try {
				Add(car);
				std::lock_guard<std::mutex> lockQueue(queueOnRoadMutexes[static_cast<size_t>(product)]);
				if (!isCarOnRoad && !carQueue.empty())
				{
					{
						std::lock_guard<std::mutex> lock(/**ptrMutex*/isCarOnRoadMutexes[static_cast<size_t>(product)]);
						isCarOnRoad = true;
					}
					isCarOnRoadCondVars[static_cast<size_t>(product)].notify_one();
				}
			}
			catch (...) {
				std::cerr << "Problems when adding a car on the road to the section";
			}
			
		}

		if (carQueue.size() == maxCars)
			isRoadFull = true;
	}
}


void RoadToSection::Add(Car& car) {
	if (carQueue.size() < maxCars)
	{
		carQueue.push(car);
		LogArrivalOnRoad(car);
	}
	else
		throw std::exception("No place on the road");
}


void RoadToSection::LogQueueLeaving(const Car& car) const {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "left the general queue");
}


void RoadToSection::LogArrivalOnRoad(const Car& car) const {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "arrived on the road to section");
}