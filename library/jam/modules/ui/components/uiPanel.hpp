#pragma once
#include <raylib.h>
#include <jam/core/uuid.hpp>
namespace jam::components
{
	struct Transform2D;
	struct UIPanel
	{
		UUID id = 0;
		Texture2D texture = { 1,1,1 };

		NPatchInfo npatch = NPatchInfo{
			.left = 0,
			.top = 0,
			.right = 0,
			.bottom = 0,
			.layout = NPATCH_NINE_PATCH
		};
		Color tint = WHITE;
		bool visible = true;

		void Render(const Transform2D& transform) const;
	};
}