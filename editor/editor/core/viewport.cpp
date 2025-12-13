#include "modules.hpp"

bool jam::editor::renderViewport(bool* _viewPortVisible, Scene* scene)
{
	if (!*_viewPortVisible) return false;

	ImGui::Begin("viewport", _viewPortVisible, ImGuiWindowFlags_::ImGuiWindowFlags_None);
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	ImVec2 winSize = ImGui::GetWindowSize();
	ImTextureID tid = scene->renderTarget.target.texture.id;

	float imageAspectRatio = scene->renderTarget.resolution.x / scene->renderTarget.resolution.y;
	float viewportAspectRatio = viewportSize.x / viewportSize.y;

	//scall horizontally if region is > than the image
	if (viewportAspectRatio > imageAspectRatio)
	{
		float imgWidth = viewportSize.y / imageAspectRatio;
		float xPadding = (viewportSize.x - imgWidth) / 2;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xPadding);
		ImGui::Image(tid, ImVec2{ imgWidth, viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1,0 });
	}
	else
	{
		float imgHeight = viewportSize.x / imageAspectRatio;
		float yPadding = (viewportSize.y - imgHeight) / 2;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yPadding);
		ImGui::Image(tid, ImVec2{ viewportSize.x, imgHeight }, ImVec2{ 0, 1 }, ImVec2{ 1,0 });
	}

	ImGui::End();

	return *_viewPortVisible == false;
}