#include "bindings.hpp"
#include <raylib.h>
#include <raymath.h>

void jam::scripting::BindRaylib(sol::state& lua)
{
	lua.new_usertype<Vector2>("Vector2",
		sol::meta_function::construct,
		sol::factories(
			//Vector2.new()
			[]() {return Vector2{ 0.0f, 0.0f }; }, [](float x, float y) {return Vector2{ x, y }; },
			//Vector2:new()
			[](sol::object) {return Vector2{ 0.0f, 0.0f }; }, [](sol::object, float x, float y) {return Vector2{ x, y }; }
		),
		//Vector(x,y) syntax, only
		sol::call_constructor,
		sol::factories([](float x, float y) {return Vector2{ x, y }; }),
		"x", &Vector2::x,
		"y", &Vector2::y,
		"set", [](Vector2& v, float x, float y)  -> Vector2& {v.x = x; v.y = y; return v; },
		"normalize", [](Vector2& v) -> Vector2& {v = Vector2Normalize(v); return v; },
		"scale", [](Vector2& v, float s) -> Vector2& {v = Vector2Scale(v, s); return v; },
		"DirectionTowards", [](const Vector2& v, Vector2 other) -> Vector2 { Vector2 r = Vector2Normalize(Vector2Subtract(other, v)); return r; },
		"DistanceTo", [](const Vector2& v, Vector2 other) -> float {  float r = Vector2DistanceSqr(other, v); return r; }
	);

	lua.new_usertype<Vector3>("Vector3",
		sol::meta_function::construct,
		sol::factories(
			//Vector3.new()
			[]() {return Vector3{ 0.0f, 0.0f, 0.0f }; }, [](float x, float y) {return Vector3{ x, y }; },
			//Vector3:new()
			[](sol::object) {return Vector3{ 0.0f, 0.0f }; }, [](sol::object, float x, float y) {return Vector3{ x, y }; }
		),
		//Vector(x,y,z) syntax, only
		sol::call_constructor,
		sol::factories([](float x, float y, float z) {return Vector3{ x, y,z }; }),
		"x", &Vector3::x,
		"y", &Vector3::y,
		"z", &Vector3::z,
		"set", [](Vector3& v, float x, float y)  -> Vector3& {v.x = x; v.y = y; return v; },
		"normalize", [](Vector3& v) -> Vector3& {v = Vector3Normalize(v); return v; },
		"scale", [](Vector3& v, float s) -> Vector3& {v = Vector3Scale(v, s); return v; },
		"DirectionTowards", [](const Vector3& v, Vector3 other) -> Vector3 { Vector3 r = Vector3Normalize(Vector3Subtract(other, v)); return r; },
		"DistanceTo", [](const Vector3& v, Vector3 other) -> float {  float r = Vector3DistanceSqr(other, v); return r; }
	);
	lua.new_usertype<Rectangle>("Rectangle",
		sol::constructors<Rectangle()>()
		, sol::call_constructor
		, sol::factories([](float x, float y, float w, float h) {return Rectangle{ x, y, w, h }; })
		, "x", &Rectangle::x
		, "y", &Rectangle::y
		, "width", &Rectangle::width
		, "height", &Rectangle::height
		, "set", [](Rectangle& v, float x, float y, float w, float h) {v = Rectangle{ x, y, w, h }; }
	);

	lua.new_usertype<Color>("Color",
		sol::constructors<Color()>(),
		sol::call_constructor,
		sol::factories(
			[]() {return Color{ 255,255,255,255 }; }
	, [](unsigned int hex) {return GetColor(hex); }
		),
		"r", &Color::r,
		"g", &Color::g,
		"b", &Color::b,
		"a", &Color::a
		, "set", [](Color& c, unsigned int hex) { c = GetColor(hex); }
		, "alpha", [](Color& c, float alpha) { c = ColorAlpha(c, alpha); }
		, "lerp", [](Color& c, Color b, float factor) { c = ColorLerp(c, b, factor); }
		);


	lua.new_usertype<Camera2D>("Camera2D"
		, "offset", &Camera2D::offset
		, "target", &Camera2D::target
		, "rotation", &Camera2D::rotation
		, "zoom", &Camera2D::zoom
	);

	lua.new_usertype<Texture>("Texture"
		, "id", &Texture::id
		, "width", &Texture::width
		, "height", &Texture::height
		, "mipmaps", &Texture::mipmaps
		, "format", &Texture::format
	);

	lua.new_usertype<Font>("Font"
		, "texture", &Font::texture
	);
	lua.new_usertype<Sound>("Sound"
		, "frameCount", &Sound::frameCount
	);
	lua.new_usertype<Music>("Music"
		, "frameCount", &Music::frameCount
		, "looping", &Music::looping
	);
	//input: keyboard
	lua["IsKeyPressed"] = IsKeyPressed;
	lua["IsKeyPressedRepeat"] = IsKeyPressedRepeat;
	lua["IsKeyDown"] = IsKeyDown;
	lua["IsKeyReleased"] = IsKeyReleased;
	lua["IsKeyUp"] = IsKeyUp;
	lua["GetKeyPressed"] = GetKeyPressed;
	lua["GetScreenWidth"] = GetScreenWidth;
	lua["GetScreenHeight"] = GetScreenHeight;
	lua["Log"] = [](const char* message) {TraceLog(LOG_INFO, message); };
	lua["Debug"] = [](const char* message) {TraceLog(LOG_DEBUG, message); };
	lua["Warn"] = [](const char* message) {TraceLog(LOG_WARNING, message); };
	lua["Error"] = [](const char* message) {TraceLog(LOG_ERROR, message); };
}
