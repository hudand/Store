#pragma once
#include "GlobalVariables.h"

struct Car {

	size_t capacity;
	productType product;
	size_t number;
	static size_t counter;

	Car() = default;

	Car(size_t pCapacity, productType pProduct);

	void Log(std::ostream& stream, const std::string& processMessage) const;

private:

	bool IsSuchCapacity(size_t pCapacity) const;
};

