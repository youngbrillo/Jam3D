#include "fileTools.hpp"

std::string OpenFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues);
std::string SaveFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues);

namespace jam::editor::tools
{
    std::string ShowOpenFileDialog(const char* fileName, std::vector<StringPair> filterValues)
    {
        return OpenFileDialogImpl(fileName, filterValues);
    }
    std::string ShowOpenFileDialog(const char* fileName)
    {
        std::vector<StringPair> filterPairs;
        return OpenFileDialogImpl(fileName, filterPairs);
    }

    std::string ShowSaveFileDialog(const char* fileName, std::vector<StringPair> filterValues)
    {
        return SaveFileDialogImpl(fileName, filterValues);
    }

    std::string ShowSaveFileDialog(const char* fileName)
    {
        std::vector<StringPair> filterPairs;
        return SaveFileDialogImpl(fileName, filterPairs);
    }
}