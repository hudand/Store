#include "pch.h"
#include "GeneralQueue.h"
#include "CarGenerator.h"

void GeneralQueue::GenerateCars() {
	generalQueues.resize(sectionCount);
	isCarInGeneralQueue.resize(sectionCount);
	std::fill(isCarInGeneralQueue.begin(), isCarInGeneralQueue.end(), false);

	srand(static_cast<unsigned int>(time(0)));

	while (true)
	{
		Car car = CarGenerator::Generate();
		Add(car);
		int timeInterval = 1 + rand() % maxTimeInterval;
		std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval));
	}
}


void GeneralQueue::Add(Car& car) {

	size_t product = static_cast<size_t>(car.product);

	if (generalQueues[product].empty())
	{
		generalQueues[product].push(car);
		{
			std::lock_guard<std::mutex> lock(isCarInQueue[product]);
			isCarInGeneralQueue[product] = true;
		}
		Log(car);
		isCarInQueueCondVars[product].notify_one();
	}
	else
	{
		generalQueues[product].push(car);
		Log(car);
	}
}


void GeneralQueue::Log(const Car& car) const {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "added to general queue");
}
