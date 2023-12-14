#pragma once


#ifndef _MINT_MATH_GEOMETRY_H_
#define _MINT_MATH_GEOMETRY_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintMath/Include/Float2.h>


namespace mint
{
	uint32 GrahamScan_FindStartPoint(const Vector<Float2>& points);
	void GrahamScan_SortPoints(Vector<Float2>& inoutPoints);
	void GrahamScan_Convexify(Vector<Float2>& inoutPoints);
}


#endif // !_MINT_MATH_GEOMETRY_H_
