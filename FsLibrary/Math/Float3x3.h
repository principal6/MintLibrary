#pragma once


#ifndef FS_FLOAT3X3_H
#define FS_FLOAT3X3_H


#include <CommonDefinitions.h>


namespace fs
{
#pragma region Forward declaration
	class Float2x2;
	class Float4x4;
#pragma endregion


	// 3x3 floating point matrix
	class Float3x3 final
	{
		friend Float4x4;

	public:
							Float3x3();
		explicit			Float3x3(const float s);
		explicit			Float3x3(
								const float m00, const float m01, const float m02,
								const float m10, const float m11, const float m12,
								const float m20, const float m21, const float m22);
							Float3x3(const Float3x3& rhs)								= default;
							Float3x3(Float3x3&& rhs) noexcept							= default;
							~Float3x3()													= default;

	public:
		Float3x3&			operator=(const Float3x3& rhs)								= default;
		Float3x3&			operator=(Float3x3&& rhs) noexcept							= default;

	public:
		const Float3x3		operator*(const Float3x3& rhs) const noexcept;
		const Float3x3		operator*(const float s) const noexcept;
		const Float3x3		operator/(const float s) const noexcept;
		
	public:
		void				setZero() noexcept;
		void				setIdentity() noexcept;

	public:
		const Float2x2		minor(const uint32 row, const uint32 col) const noexcept;
		const float			determinant() const noexcept;
		const Float3x3		transpose() const noexcept;
		const Float3x3		cofactor() const noexcept;
		const Float3x3		adjugate() const noexcept;
		const Float3x3		inverse() const noexcept;

	private:
		float				_m[3][3]{};
	};
}


#endif // !FS_FLOAT3X3_H
