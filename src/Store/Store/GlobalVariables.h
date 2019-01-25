#pragma once

const int unloadingTimeOfBox = 1000; // in milliseconds
const int moveTimeOfCar = 1000; // in milliseconds
const std::filesystem::path logFilePath{ "Log.log" };

enum class productType { apples, plums, pears };

extern std::vector<size_t> capacities;
extern const size_t capacityCount;

extern std::vector<productType> products;
extern const size_t sectionCount;
extern std::vector<std::string> productNames;

extern std::mutex coutMutex;
extern std::mutex logMutex;
extern std::vector<std::mutex> isCarInQueue;
extern std::vector<std::mutex> isRoadFullMutexes;
extern std::vector<std::mutex> isCarOnRoadMutexes;
extern std::vector<std::mutex> isBusyMutexes;
extern std::vector<std::mutex> generalQueueMutexes;
extern std::vector<std::mutex> queueOnRoadMutexes;

extern std::vector<std::condition_variable> isCarInQueueCondVars;
extern std::vector<std::condition_variable> isRoadFullCondVars;
extern std::vector<std::condition_variable> isCarOnRoadCondVars;
extern std::vector<std::condition_variable> isBusyCondVars;