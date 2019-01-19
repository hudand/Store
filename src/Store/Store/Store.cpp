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
#include <mutex>

std::vector<size_t> capacities{ 2, 5, 8 };
const size_t capacityCount = capacities.size();

enum productType { apples, plums, pears };
std::vector<productType> products { apples, plums, pears };
std::vector<std::string> productNames {"apples", "plums", "pears"};
const size_t sectionCount = products.size();

const int unloadingTimeOfBox = 1000; // in milliseconds
const int moveTimeOfCar = 1000; // in milliseconds

std::mutex coutMutex;


struct Car {
	
	size_t capacity;
	productType product;

	Car() = default;
	
	Car(size_t pCapacity, productType pProduct) : product{ pProduct } {
		if (IsSuchCapacity(pCapacity))
			capacity = pCapacity;
		else
			throw std::exception("There is no car with such carrying capacity");
	}

private:

	bool IsSuchCapacity(size_t pCapacity) {
		for (auto c : capacities)
			if (c == pCapacity)
				return true;
		return false;
	}
};


struct CarGenerator {
	static Car Generate() {
		size_t capacity = capacities[rand() % sectionCount];
		productType product = products[rand() % sectionCount];
		return Car(capacity, product);
	}
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
			int timeInterval = 1 + rand() % maxTimeInterval;
			std::this_thread::sleep_for(std::chrono::milliseconds(timeInterval));
		}
	}

private:
	static const int maxTimeInterval = 2000; // in milliseconds

	void Add(Car& car) {
		generalQueues[car.product].push(car);
	}
};


struct RoadToSection {
	
	std::queue<Car> carQueue;

	RoadToSection(productType pProduct) : product{ pProduct } {	}

	void MovingFromQueueToRoad(GeneralQueue& queue) {
		while (true)
		{
			if (carQueue.size() < 4) {
				if (!queue.generalQueues[product].empty())
				{
					size_t carCount = carQueue.size();
					std::vector<Car> movingCars;

					while ((carCount < 4) && (!queue.generalQueues[product].empty()))
					{
						Car car = queue.generalQueues[product].front();
						movingCars.push_back(car);
						queue.generalQueues[product].pop();
						carCount++;
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));

					for (Car car : movingCars)
						try {
							Add(car);
						}
						catch (...) {
							;
						}
				}
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(intervalAgain));
			}
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalAgain));
		}
	}

private:
	static const size_t maxCars = 4;
	const int intervalAgain = 20; // the interval at which an attempt is made to add a car to the road (in milliseconds)
	productType product;

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
	
	StoreSection(productType pProduct) : product{ pProduct } {	}

	void MovingFromRoadToSectionAndUnloading(RoadToSection& road) {
		while (true) {
			while (isBusy)
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalForSection));
			while (road.carQueue.empty())
				std::this_thread::sleep_for(std::chrono::milliseconds(intervalForRoad));

			Car car = road.carQueue.front();

			try {
				road.carQueue.pop();
				std::this_thread::sleep_for(std::chrono::milliseconds(moveTimeOfCar));
				MoveToSection(car);
			}
			catch (...) { ; }

			Unload();

		}
	}
private:
	productType product;
	const int intervalForSection = 100; // the time interval at which an attempt is made to move a new car into a section (in milliseconds)
	const int intervalForRoad = 20; // the time interval at which an attempt is made to take a new car from road (in milliseconds)

	void MoveToSection(Car& car) {
		if (!isBusy)
		{
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
};


struct Monitoring {

	static void DisplayState(const std::vector<StoreSection>& sections, std::vector<RoadToSection>& roads, GeneralQueue& queue) {
		while (true) {
			system("cls");

			std::cout << "============================== STORE ==================================" << std::endl;
			for (size_t i = 0; i < sections.size(); i++) {
				if (!sections[i].isBusy)
					std::cout << "The section " << i+1 << " is free" << std::endl;
				else
					std::cout << "On the section " << i+1 << " the car (" << sections[i].currentCar.capacity
						<< " boxes of "	<< productNames[sections[i].currentCar.product] << ") is unloading" << std::endl;
			}

			std::cout << "============================== ROADS ==================================" << std::endl;

			for (size_t i = 0; i < roads.size(); i++) {
				std::cout << "The road to section " << i + 1 << ":" << std::endl;

				std::lock_guard<std::mutex> guard(coutMutex);
				for (size_t j = 0; j < roads[i].carQueue.size(); j++) {
					Car car = roads[i].carQueue.front();
					std::cout << "The car (" << car.capacity << " boxes of " << productNames[car.product] << ")" << std::endl;
					roads[i].carQueue.push(car);
					roads[i].carQueue.pop();
				}
			}

			std::cout << "=========================== GENERAL ROAD ==============================" << std::endl;

			for (size_t i = 0; i < queue.generalQueues.size(); i++) {
				std::lock_guard<std::mutex> guard(coutMutex);

				for (size_t j = 0; j < queue.generalQueues[i].size(); j++) {
					Car car = queue.generalQueues[i].front();
					std::cout << "The car (" << car.capacity << " boxes of " << productNames[car.product] << ")" << std::endl;
					queue.generalQueues[i].push(car);
					queue.generalQueues[i].pop();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		}
			
 	}
private:
	static const int interval = 1000; // time interval through which the system status is displayed (in milliseconds)
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

	std::thread monitoringThread(&Monitoring::DisplayState, std::ref(sections),
		std::ref(roadsToSections), std::ref(queue));

//-------------------------------------------------------------------------------------------------------------

	queueThread.join();
	for (size_t i = 0; i < sectionCount; i++)
		roadsThreads[i].join();

	for (size_t i = 0; i < sectionCount; i++)
		sectionsThreads[i].join();

	monitoringThread.join();
}


