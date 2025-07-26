#pragma once


#ifndef _MINT_MATH_VEC_H_
#define _MINT_MATH_VEC_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace std
{
	template <class _Elem>
	class initializer_list;
}


namespace mint
{
	namespace Math
	{
		template <int32 N, typename T>
		bool Equals(const T(&lhs)[N], const T(&rhs)[N], const T epsilon = 0) noexcept;

		template <int32 N, typename T>
		void CopyVec(const T(&src)[N], T(&dest)[N]) noexcept;

		template <int32 N, typename T>
		T Dot(const T(&lhs)[N], const T(&rhs)[N]) noexcept;

		template <typename T>
		void Cross(const T(&lhs)[3], const T(&rhs)[3], T(&out)[3]) noexcept;

		template <typename T>
		void Cross(const T(&lhs)[4], const T(&rhs)[4], T(&out)[4]) noexcept;

		template <int32 N, typename T>
		T NormSq(const T(&vec)[N]) noexcept;

		template <int32 N, typename T>
		T Norm(const T(&vec)[N]) noexcept;

		template <int32 N, typename T>
		void Normalize(T(&inOut)[N]) noexcept;

		template <int32 N, typename T>
		void SetZeroVec(T(&vec)[N]) noexcept;

		template <int32 N, typename T>
		void SetNAN(T(&vec)[N]) noexcept;

		template <int32 N, typename T>
		bool IsNAN(const T(&vec)[N]) noexcept;

		template <int32 N, typename T>
		void SetAddVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;

		template <int32 N, typename T>
		void SetSubVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;
		
		template <int32 N, typename T>
		void SetMulVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;

		template <int32 N, typename T>
		void SetMulVec(T(&vec)[N], const float scalar) noexcept;

		template <int32 N, typename T>
		void SetDivVec(T(&vec)[N], const float scalar) noexcept;

		template <int32 N, typename T>
		void SetDivVec(T(&lhs)[N], const T(&rhs)[N]) noexcept;
	}


	template <int32 N, typename T>
	class Vec
	{
		static_assert(N > 0, "N must be greater than 0!");

	public:
		constexpr Vec();
		template <class ...Args>
		constexpr Vec(Args ... args);
		constexpr Vec(const std::initializer_list<T>& initializerList);
		constexpr Vec(const Vec& rhs) = default;
		constexpr Vec(Vec&& rhs) noexcept = default;
		~Vec() = default;

	public:
		static Vec<N, T> StandardUnitVector(const int32 math_i) noexcept;
		static T Dot(const Vec& lhs, const Vec& rhs) noexcept;
		static T Distance(const Vec& lhs, const Vec& rhs) noexcept;
		static T Angle(const Vec& lhs, const Vec& rhs) noexcept;
		static bool IsOrthogonal(const Vec& lhs, const Vec& rhs) noexcept;
		static Vec<N, T> ProjectUOntoV(const Vec& u, const Vec& v) noexcept;

	public:
		operator float() const requires (N == 1);
		
	public:
		Vec& operator=(const Vec& rhs) = default;
		Vec& operator=(Vec&& rhs) noexcept = default;

	public:
		Vec& operator*=(const T scalar) noexcept;
		Vec& operator/=(const T scalar) noexcept;
		Vec& operator+=(const Vec& rhs) noexcept;
		Vec& operator-=(const Vec& rhs) noexcept;
		Vec operator*(const T scalar) const noexcept;
		Vec operator/(const T scalar) const noexcept;
		Vec operator+(const Vec& rhs) const noexcept;
		Vec operator-(const Vec& rhs) const noexcept;
		const Vec& operator+() const noexcept;
		Vec operator-() const noexcept;

	public:
		bool operator==(const Vec& rhs) const noexcept;
		bool operator!=(const Vec& rhs) const noexcept;

	public:
		T& operator[](const uint32 index) noexcept;
		const T& operator[](const uint32 index) const noexcept;

	public:
		T& X() noexcept;
		T& Y() noexcept;
		T& Z() noexcept;
		T& W() noexcept;
		const T& X() const noexcept;
		const T& Y() const noexcept;
		const T& Z() const noexcept;
		const T& W() const noexcept;

	public:
		void SetZero() noexcept;
		Vec<N, T>& SetComponent(const uint32 index, const T value) noexcept;
		Vec<N, T>& SetNormalized() noexcept;

	public:
		T GetComponent(const uint32 index) const noexcept;
		T GetMaxComponent() const noexcept;
		T GetMinComponent() const noexcept;
		T NormSq() const noexcept;
		T Norm() const noexcept;
		Vec<N, T> Normalize() const noexcept;
		bool IsUnitVector() const noexcept;

	public:
		T Dot(const Vec& rhs) const noexcept;
		T Distance(const Vec& rhs) const noexcept;
		T Angle(const Vec& rhs) const noexcept;
		bool IsOrthogonalTo(const Vec& rhs) const noexcept;
		Vec<N, T> ProjectOnto(const Vec& rhs) const noexcept;
		Vec<N - 1, T> Shrink() const noexcept;

	public:
		T _c[N];
	};


	template <typename T>
	Vec<3, T> Cross(const Vec<3, T>& lhs, const Vec<3, T>& rhs) noexcept;

	template <int32 N, typename T>
	Vec<N, T> operator*(const T scalar, const Vec<N, T>& vector) noexcept;


	template <int32 N>
	using VectorF = Vec<N, float>;

	template <int32 N>
	using VectorD = Vec<N, double>;


	template <typename T>
	using Vec3 = Vec<3, T>;

	template <typename T>
	using Vec4 = Vec<4, T>;


	namespace VectorUtils
	{
		template <int32 N, typename T>
		void SetNAN(Vec<N, T>& in) noexcept;

		template <int32 N, typename T>
		bool IsNAN(const Vec<N, T>& in) noexcept;
	}
}


#endif // !_MINT_MATH_VEC_H_
