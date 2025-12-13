#pragma once
#include "editor/editorLayer.hpp"

namespace jam::editor
{
    bool renderToolBar(EditorLayer& editorLayer, EditorSettings& settings, Scene* scene);
    bool renderResourceInspector(EditorLayer& editorLayer, EditorSettings& settings, Scene* scene);
    bool renderSceneHierarchy(bool* isVisible, Scene* scene, Entity& selected);
    bool renderViewport(bool* isVisible, Scene* scene); 
}