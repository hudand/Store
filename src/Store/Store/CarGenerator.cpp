#include "pch.h"
#include "CarGenerator.h"
#include "Car.h"


Car CarGenerator::Generate() {
	size_t capacity = capacities[rand() % capacityCount];
	productType product = products[rand() % sectionCount];
	return Car(capacity, product);
}

