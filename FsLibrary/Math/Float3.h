#pragma once


#ifndef FS_FLOAT3_H
#define FS_FLOAT3_H


#include <CommonDefinitions.h>


namespace fs
{
	class Float3 final
	{
	public:
								Float3();
		explicit				Float3(const float s);
		explicit				Float3(const float x, const float y, const float z);
								Float3(const Float3& rhs)								= default;
								Float3(Float3&& rhs)									= default;
								~Float3()												= default;

	public:
		Float3&					operator=(const Float3& rhs)							= default;
		Float3&					operator=(Float3&& rhs)									= default;

	public:
		Float3&					operator+=(const Float3& rhs);
		Float3&					operator-=(const Float3& rhs);
		Float3&					operator*=(const float s);
		Float3&					operator/=(const float s);

	public:
		const Float3			operator+(const Float3& rhs) const;
		const Float3			operator-(const Float3& rhs) const;
		const Float3			operator*(const float s) const;
		const Float3			operator/(const float s) const;

	public:
		float&					operator[](const uint32 index) noexcept;
		const float&			operator[](const uint32 index) const noexcept;

	public:
		const bool				operator==(const Float3& rhs) const noexcept;
		const bool				operator!=(const Float3& rhs) const noexcept;

	public:
		static const float		dotProductRaw(const float (&a)[3], const float (&b)[3]) noexcept;

		// for matrix multiplication
		static const float		dotProductRaw(const float (&a)[3], const float bX, const float bY, const float bZ) noexcept;

	public:
		static const float		dot(const Float3& lhs, const Float3& rhs) noexcept;
		static const Float3		cross(const Float3& lhs, const Float3& rhs) noexcept;
		static const Float3		normalize(const Float3& float3) noexcept;

	public:
		const float				lengthSqaure() const noexcept;
		const float				length() const noexcept;

	public:
		void					set(const float x, const float y, const float z) noexcept;
		void					x(const float newX) noexcept;
		const float				x() const noexcept;
		void					y(const float newY) noexcept;
		const float				y() const noexcept;
		void					z(const float newZ) noexcept;
		const float				z() const noexcept;

	private:
		float					_f[3];
	};
}


#endif // !FS_FLOAT3_H
