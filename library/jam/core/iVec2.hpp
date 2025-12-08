#pragma once
#include <iostream>
#ifdef RAYLIB_H
#include <raylib.h>
#endif
namespace jam
{
	struct iVec2
	{
		int x, y;

		iVec2() : x(0), y(0) {}
		iVec2(int X, int Y) : x(X), y(Y) {}
		iVec2(const iVec2& other) = default;
#ifdef RAYLIB_H
		iVec2(const Vector2 o) : x(o.x), y(o.y) {}
		Vector2 toVector2() const { return Vector2{ (float)x, (float)y }; }
#endif
	};


	inline iVec2 operator + (const iVec2& lhs, const iVec2& rhs)
	{
		return iVec2{ lhs.x + rhs.x, lhs.y + rhs.y };
	}

	inline iVec2 operator - (const iVec2& lhs, const iVec2& rhs)
	{
		return iVec2{ lhs.x - rhs.x, lhs.y - rhs.y };
	}

	inline const iVec2& operator += (iVec2& lhs, const iVec2& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	inline const iVec2& operator -= (iVec2& lhs, const iVec2& rhs)
	{
		lhs = lhs - rhs;
		return lhs;
	}

	inline bool operator == (const iVec2& lhs, const iVec2& rhs)
	{
		return (lhs.x == rhs.x) && (lhs.y == rhs.y);
	}

	inline bool operator != (const iVec2& lhs, const iVec2& rhs)
	{
		return (lhs.x != rhs.x) || (lhs.y != rhs.y);
	}
	inline bool operator < (const iVec2& lhs, const iVec2& rhs)
	{
		return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
	}

	inline float iVec2DistanceSquared(const iVec2& v1, const iVec2& v2) {
		float res = float(v1.x - v2.x) * float(v1.x - v2.x) + float(v1.y - v2.y) * float(v1.y - v2.y);

		return res;
	}

	inline float iVec2Distance(const iVec2& lhs, const iVec2& rhs) {
		float res = iVec2DistanceSquared(lhs, rhs);
		res = sqrtf(res);
		return res;
	}

}
#include <xhash>

namespace std
{
	template <>
	struct hash<jam::iVec2>
	{
		std::size_t operator()(const jam::iVec2& vec) const
		{
			return std::hash<int>()(vec.x ^ (vec.y << 16));
		}
	};
}