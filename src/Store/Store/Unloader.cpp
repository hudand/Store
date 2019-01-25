#include "pch.h"
#include "Unloader.h"
#include "StoreSection.h"

void Unloader::Unload(StoreSection& section) {
	productType product = section.product;
	while (true) {
		if (!section.isBusy)
		{
			std::unique_lock<std::mutex> lock(isBusyMutexes[static_cast<size_t>(section.product)]);
			isBusyCondVars[static_cast<size_t>(product)].wait(lock, [&] { return section.isBusy; });
		}

		LogUnloadStart(section.currentCar);
		std::this_thread::sleep_for(std::chrono::milliseconds(unloadingTimeOfBox * section.currentCar.capacity));
		LogUnloaded(section.currentCar);

		{
			std::lock_guard<std::mutex> lock(isBusyMutexes[static_cast<size_t>(product)]/**road.ptrMutex*/);
			section.isBusy = false;
		}
		isBusyCondVars[static_cast<size_t>(product)].notify_one();
	}
}


void Unloader::LogUnloadStart(const Car& car) {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "began to unload");
}


void Unloader::LogUnloaded(const Car& car) {
	std::ofstream file{ logFilePath, std::ios::app };
	car.Log(file, "is unloaded");
}

