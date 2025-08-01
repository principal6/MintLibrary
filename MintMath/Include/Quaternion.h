﻿#pragma once


#ifndef _MINT_MATH_QUATERNION_H_
#define _MINT_MATH_QUATERNION_H_


#include <MintMath/Include/Float4.h>
#include <MintMath/Include/Vec.h>


namespace mint
{
	// Quaternion = a + bi + cj + dk
	template<typename T>
	class Quaternion final
	{
#pragma region Static functions
	private:
		// q*
		static Quaternion Conjugate(const Quaternion& q) noexcept;

		// ||q||
		static T Norm(const Quaternion& q) noexcept;

	public:
		static Quaternion MakeRotationQuaternion(const Float3& axis, const T angle) noexcept;
		static Quaternion MakeRotationQuaternion(const Vec3<T>& axis, const T angle) noexcept;

		// q^(-1)
		static Quaternion Reciprocal(const Quaternion& q) noexcept;
#pragma endregion

	public:
		Quaternion();
		// Quaternion(a, b, c, d) = Float4(w, x, y, z)
		// a: real part
		// b, c, d: vector part
		explicit Quaternion(const T a, const T b, const T c, const T d);
		Quaternion(const Quaternion& q) = default;
		Quaternion(Quaternion&& q) noexcept = default;
		~Quaternion() = default;

	public:
		Quaternion& operator=(const Quaternion& rhs) = default;
		Quaternion& operator=(Quaternion&& rhs) noexcept = default;

	public:
		// Hamilton product
		Quaternion& operator*=(const Quaternion& q) noexcept;
		Quaternion& operator*=(const T s) noexcept;
		Quaternion& operator/=(const T s) noexcept;

	public:
		// Hamilton product for vector rotation
		Quaternion& operator*=(const Float4& v) noexcept;
		// Hamilton product for vector rotation
		Quaternion& operator*=(const Vec4<T>& v) noexcept;

	public:
		// Hamilton product
		Quaternion operator*(const Quaternion& q) const noexcept;
		Quaternion operator*(const T s) const noexcept;
		Quaternion operator/(const T s) const noexcept;

	public:
		Quaternion Conjugate() const noexcept;
		Quaternion Reciprocal() const noexcept;
		Float4 RotateVector(const Float4& inputVector) const noexcept;
		Vec4<T> RotateVector(const Vec4<T>& inputVector) const noexcept;

	public:
		void SetAxisAngle(const Float3& axis, const T angle) noexcept;
		void SetAxisAngle(const Vec3<T>& axis, const T angle) noexcept;
		void GetAxisAngle(T(&axis)[3], T& angle) const noexcept;
		void GetAxisAngle(Float3& axis, T& angle) const noexcept;
		void GetAxisAngle(Vec3<T>& axis, T& angle) const noexcept;

	public:
		union
		{
			struct
			{
				T _x; // b (vector part)
				T _y; // c (vector part)
				T _z; // d (vector part)
				T _w; // a (real part)
			};
			struct
			{
				T _b; // x (vector part)
				T _c; // y (vector part)
				T _d; // z (vector part)
				T _a; // w (real part)
			};
		};
	};


	using QuaternionF = Quaternion<float>;
	using QuaternionD = Quaternion<double>;
}


#include <MintMath/Include/Quaternion.inl>


#endif // !_MINT_MATH_QUATERNION_H_
