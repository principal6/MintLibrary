#pragma once


#ifndef _MINT_PHYSICS_INTERSECTION_H_
#define _MINT_PHYSICS_INTERSECTION_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	class Float2;
	class Rect;

	namespace Physics
	{
		bool intersect2D_circle_point(const Float2& circleCenter, const float circleRadius, const Float2& point);
		bool intersect2D_aabb_point(const Rect& aabb, const Float2& point);
	}
}


#endif // !_MINT_PHYSICS_INTERSECTION_H_
