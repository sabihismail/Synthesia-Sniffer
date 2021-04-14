#pragma once

#include <vector>
#include <functional>

struct VariableMessageBox
{
	const char* msg;
	std::vector<const char*> options;
	std::vector<std::function<void()>> callbacks;
};
