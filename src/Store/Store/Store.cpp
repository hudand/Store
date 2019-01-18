// Store.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <ctime>
#include <thread>
#include <chrono>

std::vector<char> capacities{ 2, 5, 8 };
const size_t capacityCount = capacities.size();

enum productType { apples, plums, pears };
std::vector<productType> products { apples, plums, pears };
std::vector<std::string> productNames {"Apples", "Plums", "Pears"};
const size_t sectionCount = products.size();

const int unloadingTimeOfBox = 1000; // in milliseconds
const int moveTimeOfCar = 1000; // in milliseconds


struct Car {
	char capacity;
	productType product;

	Car() = default;
	
	Car(char pCapacity, productType pProduct) : product{ pProduct } {
		if (IsSuchCapacity(pCapacity))
			capacity = pCapacity;
		else
			throw std::exception("There is no car with such carrying capacity");
	}

private:

	bool IsSuchCapacity(char pCapacity) {
		for (auto c : capacities)
			if (c == pCapacity)
				return true;
		return false;
	}
};


struct CarGenerator {
	static Car Generate() {
		int timeInterval = 1 + rand() % maxTimeInterval;
		std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval));

		char capacity = capacities[rand() % sectionCount];
		productType product = products[rand() % sectionCount];
		return Car(capacity, product);
	}
private:
	static const int maxTimeInterval = 1000; // in milliseconds
};


struct GeneralQueue {
	std::vector<std::queue<Car>> generalQueues;

	void GenerateCars() {
		generalQueues.resize(sectionCount);
		srand(static_cast<unsigned int>(time(0)));

		while (true)
		{
			Car car = CarGenerator::Generate();
			Add(car);
			std::cout << "New car in queue: Product = " << productNames[car.product] << " ; capacity = " << (int)car.capacity << std::endl;
		}
	}

private:
	void Add(Car& car) {
		std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));
		generalQueues[car.product].push(car);
	}
};


struct RoadToSection {
	static const char maxCars = 4;
	std::queue<Car> carQueue;
	productType product;
	const int intervalAgain = 20; // the interval at which an attempt is made to add a car to the road (in milliseconds) 

	RoadToSection(productType pProduct) : product{ pProduct } {	}

	void MovingFromQueueToRoad(GeneralQueue& queue) {
		while (true)
		{
			if (carQueue.size() < 4) {
				if (!queue.generalQueues[product].empty())
				{
					while ((carQueue.size() < 4) && (!queue.generalQueues[product].empty()))
					{
						Car car = queue.generalQueues[product].front();
						try {
							Add(car);
							queue.generalQueues[product].pop();
							std::cout << "Car in road: Product = " << productNames[car.product] << " ; capacity = " 
								 << (int)car.capacity << std::endl;
						}
						catch (...) {
							;
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(intervalAgain));
			}
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalAgain));
		}
	}

private:
	void Add(Car& car) {
		if (carQueue.size() < 4)
			carQueue.push(car);
		else
			throw std::exception("No place on the road");
	}
};


struct StoreSection {
	bool isBusy{ false };
	Car currentCar;
	productType product;
	const int intervalForSection = 100; // the time interval at which an attempt is made to move a new car into a section (in milliseconds)
	const int intervalForRoad = 20; // the time interval at which an attempt is made to take a new car from road (in milliseconds)

	StoreSection(productType pProduct) : product{ pProduct } {	}

	void MoveToSection(Car& car) {
		if (!isBusy)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));
			isBusy = true;
			currentCar = car;
		}
		else
			throw std::exception("Section is busy");
	}

	void Unload() {
		std::this_thread::sleep_for(std::chrono::milliseconds(unloadingTimeOfBox * currentCar.capacity));
		isBusy = false;
	}

	void MovingFromRoadToSectionAndUnloading(RoadToSection& road) {
		while (true) {
			while (isBusy)
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalForSection));
			while (road.carQueue.empty())
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalForRoad));

			Car car = road.carQueue.front();

			try {
				MoveToSection(car);
				road.carQueue.pop();
			}
			catch (...) { ; }

			Unload();

			std::cout << "The car is unloaded: Product = " << productNames[car.product] << " ; capacity = " 
				<< (int)car.capacity << std::endl;
		}
	}
};


int main()
{
	GeneralQueue queue;
	
	std::vector<RoadToSection> roadsToSections;
	for (size_t i = 0; i < sectionCount; i++)
		roadsToSections.emplace_back(products[i]);

	std::vector<StoreSection> sections;
	for (size_t i = 0; i < sectionCount; i++)
		sections.emplace_back(products[i]);

//-------------------------------------------------------------------------------------------------------------

	std::thread queueThread(&GeneralQueue::GenerateCars, &queue);
		
	std::vector<std::thread> roadsThreads;
	for (size_t i = 0; i < sectionCount; i++)
		roadsThreads.emplace_back(&RoadToSection::MovingFromQueueToRoad, &roadsToSections[i], std::ref(queue));

	std::vector<std::thread> sectionsThreads;
	for (size_t i = 0; i < sectionCount; i++)
		sectionsThreads.emplace_back(&StoreSection::MovingFromRoadToSectionAndUnloading, &sections[i], 
			                         std::ref(roadsToSections[i]));

//-------------------------------------------------------------------------------------------------------------

	queueThread.join();
	for (size_t i = 0; i < sectionCount; i++)
		roadsThreads[i].join();

	for (size_t i = 0; i < sectionCount; i++)
		sectionsThreads[i].join();

}


