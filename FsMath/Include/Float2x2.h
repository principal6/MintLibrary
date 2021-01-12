#pragma once


#ifndef FS_FLOAT2X2_H
#define FS_FLOAT2X2_H


#include <CommonDefinitions.h>


namespace fs
{
#pragma region Forward declaration
	class Float3x3;
#pragma endregion


	// 2x2 floating point matrix
	class Float2x2 final
	{
		friend Float3x3;

		static constexpr uint32		kMaxIndex = 1;

	public:
									Float2x2();
		explicit					Float2x2(const float s);
		explicit					Float2x2(
										const float m00, const float m01, 
										const float m10, const float m11);
									Float2x2(const Float2x2& rhs)				= default;
									Float2x2(Float2x2&& rhs) noexcept			= default;
									~Float2x2()									= default;
		
	public:
		Float2x2&					operator=(const Float2x2& rhs)				= default;
		Float2x2&					operator=(Float2x2&& rhs) noexcept			= default;
	public:
		Float2x2					operator*(const float s) const noexcept;
		Float2x2					operator/(const float s) const noexcept;

	public:
		Float2x2					operator*(const Float2x2& rhs) const noexcept;

	public:
		void						set(const uint32 row, const uint32 col, const float newValue) noexcept;
		const float					get(const uint32 row, const uint32 col) const noexcept;
		void						setZero() noexcept;
		void						setIdentity() noexcept;

	public:
		const float					determinant() const noexcept;
		Float2x2					inverse() const noexcept;

	private:
		float						_m[2][2]{};
	};
}


#endif // !FS_FLOAT2X2_H

