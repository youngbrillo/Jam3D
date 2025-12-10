#include "uiPanel.hpp"
#include "transform2d.hpp"

void jam::components::UIPanel::Render(const Transform2D& transform) const
{
	if (!visible)
		return;

	Rectangle dest = transform.getRectangle();
	Vector2 origin = transform.getOrigin();
	DrawTextureNPatch(
		texture,
		npatch,
		dest,//transform.getRectangle(),
		origin,//transform.getOrigin(),
		transform.angle,
		tint
	);
}
