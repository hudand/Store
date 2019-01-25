#pragma once
#include "GlobalVariables.h"
#include "StoreSection.h"

namespace Unloader
{
	void Unload(StoreSection& section);
	void LogUnloadStart(const Car& car);
	void LogUnloaded(const Car& car);
};

