#pragma once


#ifndef FS_QUATERNION_H
#define FS_QUATERNION_H


#include <FsMath/Include/Float4.h>


namespace fs
{
	// Quaternion = a + bi + cj + dk
	class Quaternion final
	{
#pragma region Static functions
	private:
		// q*
		static Quaternion			conjugate(const Quaternion& q) noexcept;

		// ||q||
		static const float			norm(const Quaternion& q) noexcept;

	public:
		static Quaternion			makeRotationQuaternion(const fs::Float3& axis, float angle) noexcept;

		// q^(-1)
		static Quaternion			reciprocal(const Quaternion& q) noexcept;
#pragma endregion

	public:
									Quaternion();
									// Quaternion(a, b, c, d) = Float4(w, x, y, z)
									// a: real part
									// b, c, d: vector part
		explicit					Quaternion(const float a, const float b, const float c, const float d);
									Quaternion(const Quaternion& q)											= default;
									Quaternion(Quaternion&& q) noexcept										= default;
									~Quaternion()															= default;

	public:
		Quaternion&					operator=(const Quaternion& rhs)										= default;
		Quaternion&					operator=(Quaternion&& rhs) noexcept									= default;

	public:
		// Hamilton product
		Quaternion&					operator*=(const Quaternion& q) noexcept;
		Quaternion&					operator*=(const float s) noexcept;
		Quaternion&					operator/=(const float s) noexcept;

	public:
		// Hamilton product for vector rotation
		Quaternion&					operator*=(const fs::Float4& v) noexcept;

	public:
		// Hamilton product
		Quaternion					operator*(const Quaternion& q) const noexcept;
		Quaternion					operator*(const float s) const noexcept;
		Quaternion					operator/(const float s) const noexcept;

	public:
		Quaternion					conjugate() const noexcept;
		Quaternion					reciprocal() const noexcept;
		fs::Float4					rotateVector(const fs::Float4& inputVector);
		void						setAxisAngle(const fs::Float3& axis, float angle) noexcept;
		void						getAxisAngle(fs::Float3& axis, float& angle) const noexcept;

	public:
		union
		{
			fs::Float4				_data;
			struct
			{
				float				_x; // b (vector part)
				float				_y; // c (vector part)
				float				_z; // d (vector part)
				float				_w; // a  (real part)
			};
			struct
			{
				float				_b; // x (vector part)
				float				_c; // y (vector part)
				float				_d; // z (vector part)
				float				_a; // w (real part)
			};
		};
		
	};
}


#endif // !FS_QUATERNION_H
