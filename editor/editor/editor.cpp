#include "editor.hpp"
namespace fs = std::filesystem;

bool jam::editor::EditColor(const char* label, Color* color, ImGuiColorEditFlags_ flags)
{
    Vector4 c = ColorNormalize(*color);

    bool changed = ImGui::ColorEdit4(label, &c.x, flags);
    if (changed)
    {
        *color = ColorFromNormalized(c);
    }
    return changed;
}

void jam::editor::FolderInfo::refresh(int level, int maxLevel)
{
	FilePathList fpl = LoadDirectoryFilesEx(path.string().c_str(), ".yml;.yaml", false);
	fs::path curr_dir = path;
	std::string curr_folder = curr_dir.parent_path().stem().string();

	//only go one-level deep
	for (int i = 0; i < fpl.count; i++)
	{
		fs::path filepath = fpl.paths[i];
		fs::path name = filepath.stem();
		std::string folder = filepath.parent_path().stem().string();

		if (folder == curr_folder)
			this->files.push_back(filepath);
		else
			this->folders.emplace(folder, FolderInfo{});
	}

	UnloadDirectoryFiles(fpl);

	if (level <= maxLevel)
	{
		for (auto&& [name, fi] : folders)
		{
			fi.refresh(level + 1, maxLevel);
		}
	}
}
