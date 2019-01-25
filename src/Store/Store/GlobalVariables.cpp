#include "pch.h"
#include "GlobalVariables.h"

std::vector<size_t> capacities{ 2, 5, 8 };
const size_t capacityCount = capacities.size();

std::vector<productType> products{ productType::apples, productType::plums, productType::pears };
const size_t sectionCount = products.size();
std::vector<std::string> productNames{ "apples", "plums", "pears" };

std::mutex coutMutex;
std::mutex logMutex;

std::vector<std::mutex> isCarInQueue(sectionCount);
std::vector<std::mutex> isRoadFullMutexes(sectionCount);
std::vector<std::mutex> isCarOnRoadMutexes(sectionCount);
std::vector<std::mutex> isBusyMutexes(sectionCount);
std::vector<std::mutex> generalQueueMutexes(sectionCount);
std::vector<std::mutex> queueOnRoadMutexes(sectionCount);


std::vector<std::condition_variable> isCarInQueueCondVars(sectionCount);
std::vector<std::condition_variable> isRoadFullCondVars(sectionCount);
std::vector<std::condition_variable> isCarOnRoadCondVars(sectionCount);
std::vector<std::condition_variable> isBusyCondVars(sectionCount);
