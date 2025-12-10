#pragma once
#include <raylib.h>
#include <imgui.h>
#include <filesystem>
#include <string>
#include <map>

namespace jam::editor {
	bool EditColor(const char* label, Color* color, ImGuiColorEditFlags_ flags = ImGuiColorEditFlags_::ImGuiColorEditFlags_DisplayHex);

	typedef void (*ExternalFunction)(bool*, void*);

	struct ExternalFunctionWrapper
	{
		bool isOpen = true;
		void* userData = nullptr;
		ExternalFunction function = nullptr;
	};

	struct FolderInfo
	{
		std::string name;
		std::filesystem::path path;
		std::vector<std::filesystem::path> files;
		std::map<std::string, FolderInfo> folders;

		void refresh(int level, int = 10);
	};
}

