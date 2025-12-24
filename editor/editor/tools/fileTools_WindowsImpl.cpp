//#include "fileTools.hpp"
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#include <shobjidl.h>
#include "fileTools.hpp"

HWND WindowHandle;
namespace jam::editor::tools
{
    void SetWindowHandle(void* handle)
	{
		WindowHandle = HWND(handle);
	}
}

static std::string wstrtostr(const std::wstring& wstr)
{
    std::string strTo;
    char* szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}
static std::wstring strtowstr(const std::string& str)
{
    std::wstring strTo;
    TCHAR* szTo = new TCHAR[str.length() + 1];
    szTo[str.size()] = '\0';
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, szTo, (int)str.length());
    strTo = szTo;
    delete[] szTo;
    return strTo;
}
static const char* BuildFilter(std::vector<std::pair<std::string, std::string>> filterValues)
{
    size_t count = 0;
    for (auto& filterValue : filterValues)
    {
        count += filterValue.first.size() + 1;
        count += filterValue.first.size() + 3;
    }
    count += 1;
    if (count == 1)
    {
        count += strlen("All Files\t*.*\t");
    }

    char* filter = new char[count];

    if (filterValues.size() == 0)
    {
        memcpy(filter, "All Files\0*.*\0\0", count);
    }
    else
    {
        size_t offset = 0;
        for (auto& filterValue : filterValues)
        {
            const std::string& name = filterValue.first;
            std::string extension = "*." + filterValue.second;

            memcpy(filter + offset, name.c_str(), name.size());
            offset += name.size();

            memcpy(filter + offset, "\0", 1);
            offset += 1;

            memcpy(filter + offset, extension.c_str(), extension.size());
            offset += extension.size();

            memcpy(filter + offset, "\0", 1);
            offset += 1;
        }
        memcpy(filter + offset, "\0", 1);
        offset += 1;
    }

    return filter;
}


std::string OpenFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues)
{
    std::string result;
    const char* filter = BuildFilter(filterValues);
#if true
    // common dialog box structure, setting all fields to 0 is important
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    if (filename != nullptr)
        wcscpy_s(szFile, 260, strtowstr(filename).c_str());

    // Initialize remaining fields of OPENFILENAME structure
    auto wfilter = strtowstr(filter);
    auto* _filter = wfilter.c_str();
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = WindowHandle;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    //ofn.lpstrFilter = _filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        // use ofn.lpstrFile here
        result = wstrtostr(ofn.lpstrFile);
    }

    delete[](filter);
#else
    OPENFILENAMEA ofn;				//common dialog box structure
    CHAR szFile[260] = { 0 };		//if using TCHAR macros
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);

    ofn.hwndOwner = WindowHandle; // glfwGetWin32Window(win);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        //Application::Global().SetTimeState((PlayState)originalState);
        return ofn.lpstrFile;
    }
#endif
    return result;
}
std::string SaveFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues)
{
    std::string result;
    const char* filter = BuildFilter(filterValues);



    return result;
}


#else
namespace jam::editor::tools
{
	void SetWindowHandle(void*)
	{
	}
}


std::string OpenFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues)
{
    return std::string();
}
std::string SaveFileDialogImpl(const char* filename, std::vector<std::pair<std::string, std::string>> filterValues)
{
    return std::string();
}
#endif