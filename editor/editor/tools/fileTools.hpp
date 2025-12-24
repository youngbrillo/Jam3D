#pragma once
#include <vector>
#include <string>

using StringPair = std::pair<std::string, std::string>;

namespace jam::editor::tools
{
	std::string ShowOpenFileDialog(const char* fileName, std::vector<StringPair> filterValues);
	std::string ShowOpenFileDialog(const char* fileName);

	std::string ShowSaveFileDialog(const char* fileName, std::vector<StringPair> filterValues);
	std::string ShowSaveFileDialog(const char* fileName);

	void SetWindowHandle(void* handle);
}