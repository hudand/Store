// Store.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "GlobalVariables.h"
#include "Car.h"
#include "CarGenerator.h"
#include "GeneralQueue.h"
#include "RoadToSection.h"
#include "StoreSection.h"
#include "Monitoring.h"
#include "Unloader.h"


int main()
{
	{
		std::ofstream log{ logFilePath }; // clear log file
	}
		
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
	for (size_t i = 0; i < sectionCount; i++) {
		roadsThreads.emplace_back(&RoadToSection::MovingFromQueueToRoad, &roadsToSections[i], std::ref(queue));
	}

	std::vector<std::thread> sectionsThreads;
	for (size_t i = 0; i < sectionCount; i++)
		sectionsThreads.emplace_back(&StoreSection::MovingFromRoadToSection, &sections[i], 
			                         std::ref(roadsToSections[i]));
	
	std::vector<std::thread> unloadersThreads;
	for (size_t i = 0; i < sectionCount; i++)
		unloadersThreads.emplace_back(&Unloader::Unload, std::ref(sections[i]));

	std::thread monitoringThread(&Monitoring::DisplayState, std::ref(sections),
		std::ref(roadsToSections), std::ref(queue));

//-------------------------------------------------------------------------------------------------------------

	queueThread.join();
	for (size_t i = 0; i < sectionCount; i++)
		roadsThreads[i].join();

	for (size_t i = 0; i < sectionCount; i++)
		sectionsThreads[i].join();

	for (size_t i = 0; i < sectionCount; i++)
		unloadersThreads[i].join();

	monitoringThread.join();
}


