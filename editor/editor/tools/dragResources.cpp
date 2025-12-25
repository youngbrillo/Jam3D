#include "dragResources.hpp"

bool jam::editor::tools::DragAndDropTextureGive(jam::TextureResource& resource, const char* type)
{
    bool res = false;
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload(type, &resource, sizeof(TextureResource));
        ImGui::Text("Dragging Texture: %s", resource.name.c_str());
        ImGui::EndDragDropSource();
    }

    return res;
}

bool jam::editor::tools::DragAndDropTextureRecieve(Texture& texture, UUID& id, const char* type)
{
    bool res = false;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
        {
            IM_ASSERT(payload->DataSize == sizeof(TextureResource));
            TextureResource* resource = (TextureResource*)payload->Data;

            texture = resource->res;
            id = resource->id;

            res = true;
        }

    }
    return res;
}
