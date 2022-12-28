#pragma once


#ifndef _MINT_MATH_TRANSFORM_INL_
#define _MINT_MATH_TRANSFORM_INL_


namespace mint
{
	MINT_INLINE Transform::Transform()
		: Transform(Float3::kZero)
	{
		__noop;
	}

	MINT_INLINE Transform::Transform(const Float3& translation)
		: _scale{ Float3::kUnitScale }
		, _translation{ translation }
	{
		__noop;
	}
}


#endif // !_MINT_MATH_TRANSFORM_INL_
