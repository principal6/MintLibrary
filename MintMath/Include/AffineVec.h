#pragma once


#ifndef _MINT_MATH_AFFINE_VEC_H_
#define _MINT_MATH_AFFINE_VEC_H_


#include <MintMath/Include/MathCommon.h>

#include <immintrin.h>


namespace mint
{
	template<typename T>
	class AffineVec
	{
		AffineVec() = default;
	};


	template<>
	class AffineVec<float>
	{
	public:
		static const AffineVec<float> kZero;
		static const AffineVec<float> kNan;
		static const AffineVec<float> k1000;
		static const AffineVec<float> k0100;
		static const AffineVec<float> k0010;
		static const AffineVec<float> k0001;

	public:
		AffineVec();
		AffineVec(const float scalar);
		AffineVec(const float x, const float y, const float z, const float w);
		AffineVec(const __m128 raw);
		AffineVec(const AffineVec& rhs);
		AffineVec(AffineVec&& rhs) noexcept;
		~AffineVec() = default;

	public:
		AffineVec& operator=(const AffineVec& rhs);
		AffineVec& operator=(AffineVec&& rhs) noexcept;

	public:
		bool operator==(const AffineVec& rhs) const noexcept;
		bool operator!=(const AffineVec& rhs) const noexcept;

	public:
		AffineVec& operator+() noexcept;
		AffineVec operator-() const noexcept;

		AffineVec operator+(const AffineVec& rhs) const noexcept;
		AffineVec operator-(const AffineVec& rhs) const noexcept;
		AffineVec operator*(const AffineVec& rhs) const noexcept;
		AffineVec operator/(const AffineVec& rhs) const noexcept;
		AffineVec operator*(const float scalar) const noexcept;
		AffineVec operator/(const float scalar) const noexcept;

		AffineVec& operator+=(const AffineVec& rhs) noexcept;
		AffineVec& operator-=(const AffineVec& rhs) noexcept;
		AffineVec& operator*=(const AffineVec& rhs) noexcept;
		AffineVec& operator/=(const AffineVec& rhs) noexcept;
		AffineVec& operator*=(const float scalar) noexcept;
		AffineVec& operator/=(const float scalar) noexcept;

	public:
		void Set(const float x, const float y, const float z, const float w) noexcept;
		void SetComponent(const int32 i, const float scalar) noexcept;
		void AddComponent(const int32 i, const float scalar) noexcept;

	public:
		void Get(float(&vec)[4]) const noexcept;
		float GetComponent(const int32 i) const noexcept;
		const __m128& GetRaw() const noexcept;

	public:
		float Dot(const AffineVec& rhs) const noexcept;
		float NormSq() const noexcept;
		float Norm() const noexcept;
		void Normalize() noexcept;
		AffineVec<float> Cross(const AffineVec<float>& rhs) const noexcept;

	private:
		__m128 _raw;
	};


	template<>
	class AffineVec<double>
	{
	public:
		AffineVec();
		AffineVec(const double scalar);
		AffineVec(const double x, const double y, const double z, const double w);
		AffineVec(const __m256d raw);
		AffineVec(const AffineVec& rhs);
		AffineVec(AffineVec&& rhs) noexcept;
		~AffineVec() = default;

	public:
		AffineVec& operator=(const AffineVec& rhs);
		AffineVec& operator=(AffineVec&& rhs) noexcept;

	public:
		bool operator==(const AffineVec& rhs) const noexcept;
		bool operator!=(const AffineVec& rhs) const noexcept;

	public:
		AffineVec& operator+() noexcept;
		AffineVec operator-() const noexcept;

		AffineVec operator+(const AffineVec& rhs) const noexcept;
		AffineVec operator-(const AffineVec& rhs) const noexcept;
		AffineVec operator*(const AffineVec& rhs) const noexcept;
		AffineVec operator/(const AffineVec& rhs) const noexcept;
		AffineVec operator*(const double scalar) const noexcept;
		AffineVec operator/(const double scalar) const noexcept;

		AffineVec& operator+=(const AffineVec& rhs) noexcept;
		AffineVec& operator-=(const AffineVec& rhs) noexcept;
		AffineVec& operator*=(const AffineVec& rhs) noexcept;
		AffineVec& operator/=(const AffineVec& rhs) noexcept;
		AffineVec& operator*=(const double scalar) noexcept;
		AffineVec& operator/=(const double scalar) noexcept;

	public:
		void Set(const double x, const double y, const double z, const double w) noexcept;
		void SetComponent(const int32 i, const double scalar) noexcept;
		void AddComponent(const int32 i, const double scalar) noexcept;

	public:
		void Get(double(&vec)[4]) const noexcept;
		double GetComponent(const int32 i) const noexcept;
		const __m256d& GetRaw() const noexcept;

	public:
		double Dot(const AffineVec& rhs) const noexcept;
		double NormSq() const noexcept;
		double Norm() const noexcept;
		void Normalize() noexcept;
		AffineVec<double> Cross(const AffineVec<double>& rhs) const noexcept;

	private:
		__m256d _raw;
	};


#pragma region Free functions
	template<typename T>
	AffineVec<T> operator*(const T scalar, const AffineVec<T>& vec) noexcept;

	template<typename T>
	AffineVec<T> Normalize(const AffineVec<T>& in) noexcept;

	template<typename T>
	void Normalize(AffineVec<T>& inOut) noexcept;

	float Dot(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept;
	AffineVec<float> Cross(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept;

	double Dot(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept;
	AffineVec<double> Cross(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept;
#pragma endregion


	using AffineVecF = AffineVec<float>;
	using AffineVecD = AffineVec<double>;
}


#include <MintMath/Include/AffineVec.inl>


#endif // !_MINT_MATH_AFFINE_VEC_H_
