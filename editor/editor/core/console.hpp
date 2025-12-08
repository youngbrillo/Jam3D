#include "Jam/scene/iScene.hpp"
#include "Jam/scripting/script.hpp"
#include <imgui.h>

namespace jam::strawberry::editor
{
	class SceneConsole
	{
	public:
		SceneConsole();
		~SceneConsole();

		void ClearLog();
		void AddLog(const char* fmt, ...);
		void Draw(const char* title, bool* p_open, jam::iScene* scene);
		void ExecCommand(const char* command_line, scripting::Script& script);
		int TextEditCallback(ImGuiInputTextCallbackData* data);

		static void Render(bool* p_open, jam::iScene* scene);
	private:
		char                  InputBuf[256];
		ImVector<char*>       Items;
		ImVector<const char*> Commands;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImGuiTextFilter       Filter;
		bool                  AutoScroll;
		bool                  ScrollToBottom;
	};
}