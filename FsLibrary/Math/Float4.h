#pragma once


#ifndef FS_FLOAT4_H
#define FS_FLOAT4_H


#include <CommonDefinitions.h>
#include <Math/MathCommon.h>
#include <Math/Float3.h>


namespace fs
{
	class Float4 final
	{
	public:
								Float4();
		explicit				Float4(const float s);
		explicit				Float4(const float x, const float y, const float z, const float w);
		explicit				Float4(const Float3& rhs);
								Float4(const Float4& rhs)											= default;
								Float4(Float4&& rhs)												= default;
								~Float4()															= default;

	public:
		Float4&					operator=(const Float4& rhs)										= default;
		Float4&					operator=(Float4&& rhs)												= default;

	public:
		Float4&					operator+=(const Float4& rhs);
		Float4&					operator-=(const Float4& rhs);
		Float4&					operator*=(const float s);
		Float4&					operator/=(const float s);

	public:
		const Float4			operator+(const Float4& rhs) const;
		const Float4			operator-(const Float4& rhs) const;
		const Float4			operator*(const float s) const;
		const Float4			operator/(const float s) const;

	public:
		float&					operator[](const uint32 index) noexcept;
		const float&			operator[](const uint32 index) const noexcept;

	public:
		const bool				operator==(const Float4& rhs) const noexcept;
		const bool				operator!=(const Float4& rhs) const noexcept;

	public:
		static const float		dotProductRaw(const float (&a)[4], const float (&b)[4]);

		// for matrix multiplication
		static const float		dotProductRaw(const float (&a)[4], const float bX, const float bY, const float bZ, const float bW);

	public:
		static const float		dot(const Float4& lhs, const Float4& rhs) noexcept;

		// in 3D affine space
		static const Float4		cross(const Float4& lhs, const Float4& rhs) noexcept;
		static const Float4		normalize(const Float4& float4) noexcept;

	public:
		const float				lengthSqaure() const noexcept;
		const float				length() const noexcept;

	public:
		void					set(const float x, const float y, const float z, const float w) noexcept;
		void					x(const float newX) noexcept;
		const float				x() const noexcept;
		void					y(const float newY) noexcept;
		const float				y() const noexcept;
		void					z(const float newZ) noexcept;
		const float				z() const noexcept;
		void					w(const float newW) noexcept;
		const float				w() const noexcept;

	private:
		float					_f[4];
	};
}


#endif // !FS_FLOAT4_H
