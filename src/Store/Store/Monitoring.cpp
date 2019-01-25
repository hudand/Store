#include "pch.h"
#include "Monitoring.h"


void Monitoring::DisplayState(const std::vector<StoreSection>& sections, std::vector<RoadToSection>& roads, GeneralQueue& queue) {
	while (true) {
		system("cls");

		std::cout << "============================== STORE ==================================" << std::endl;
		for (size_t i = 0; i < sectionCount; i++) {
			if (!sections[i].isBusy)
				std::cout << "The section " << i + 1 << " is free" << std::endl;
			else
				std::cout << "On the section " << i + 1 << " the car No." << sections[i].currentCar.number << " ("
				<< sections[i].currentCar.capacity << " boxes of "
				<< productNames[static_cast<size_t>(sections[i].currentCar.product)] << ") is unloading" << std::endl;
		}

		std::cout << "============================== ROADS ==================================" << std::endl;

		for (size_t i = 0; i < sectionCount; i++) {
			std::cout << "The road to section " << i + 1 << ":" << std::endl;

			std::lock_guard<std::mutex> lock(queueOnRoadMutexes[i]);
			for (size_t j = 0; j < roads[i].carQueue.size(); j++) {
				Car car = roads[i].carQueue.front();
				std::cout << "The car No." << car.number << " (" << car.capacity << " boxes of "
					<< productNames[static_cast<size_t>(car.product)] << ")" << std::endl;
				roads[i].carQueue.push(car);
				roads[i].carQueue.pop();
			}
		}

		std::cout << "=========================== GENERAL QUEUE =============================" << std::endl;

		for (size_t i = 0; i < sectionCount; i++) {

			std::lock_guard<std::mutex> lock(generalQueueMutexes[i]);
			for (size_t j = 0; j < queue.generalQueues[i].size(); j++) {
				Car car = queue.generalQueues[i].front();
				std::cout << "The car No." << car.number << " (" << car.capacity << " boxes of "
					<< productNames[static_cast<size_t>(car.product)] << ")" << std::endl;
				queue.generalQueues[i].push(car);
				queue.generalQueues[i].pop();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}


	

