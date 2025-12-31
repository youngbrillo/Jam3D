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

bool jam::editor::tools::DragAndDropMeshGive(jam::ModelResource& resource, const char* type)
{
    bool res = false;
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload(type, &resource, sizeof(ModelResource));
        ImGui::Text("Dragging ModelResource: %s", resource.name.c_str());
        ImGui::EndDragDropSource();
    }

    return res;
}

bool jam::editor::tools::DragAndDropMeshRecieve(Mesh** target, UUID& id, Material* material, int meshIndex, const char* type)
{
    bool res = false;
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type))
        {
            IM_ASSERT(payload->DataSize == sizeof(ModelResource));
            ModelResource* resource = (ModelResource*)payload->Data;

            *target = &resource->res.meshes[meshIndex];
            id = resource->id;

            if (material && resource->res.materials[0].maps[0].texture.id != 1)
            {
                //copy material ..
                material->maps[0].texture = resource->res.materials[0].maps[0].texture;
            }

            res = true;
        }

    }
    return res;
}
