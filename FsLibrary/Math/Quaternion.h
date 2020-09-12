#pragma once


#ifndef FS_QUATERNION_H
#define FS_QUATERNION_H


#include <CommonDefinitions.h>
#include <Math/Float4.h>

namespace fs
{
	// Quaternion == a + bi + cj + dk
	//  (Float4)     w    x    y    z
	class Quaternion final
	{
#pragma region Static functions
	private:
		// q*
		static const Quaternion		conjugate(const Quaternion& q) noexcept;

		// ||q||
		static const float			norm(const Quaternion& q) noexcept;

	public:
		static const Quaternion		rotationQuaternion(const Float4& axis, float angle) noexcept;

		// q^(-1)
		static const Quaternion		reciprocal(const Quaternion& q) noexcept;
#pragma endregion

	public:
									Quaternion();
		explicit					Quaternion(const float a, const float b, const float c, const float d);
		explicit					Quaternion(const Float4& v);
									Quaternion(const Quaternion& q)											= default;
									Quaternion(Quaternion&& q) noexcept										= default;
									~Quaternion()															= default;

	public:
		Quaternion&					operator=(const Quaternion& rhs)										= default;
		Quaternion&					operator=(Quaternion&& rhs) noexcept									= default;

	public:
		// Hamilton product
		const Quaternion			operator*(const Quaternion& q) const noexcept;
		const Quaternion			operator/(const float s) const noexcept;

	public:
		const Quaternion			reciprocal() const noexcept;

	private:
		const float					a() const noexcept;
		const float					b() const noexcept;
		const float					c() const noexcept;
		const float					d() const noexcept;

	private:
		Float4						_data;
	};
}


#endif // !FS_QUATERNION_H
