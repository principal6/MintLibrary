#pragma once


#ifndef _MINT_MATH_VECTOR_R_H_
#define _MINT_MATH_VECTOR_R_H_


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
	}


	template <int32 N, typename T>
	class VectorR
	{
		static_assert(N > 0, "N must be greater than 0!");

	public:
		static VectorR<N, T> StandardUnitVector(const int32 math_i) noexcept;
		static T Dot(const VectorR& lhs, const VectorR& rhs) noexcept;
		static T Distance(const VectorR& lhs, const VectorR& rhs) noexcept;
		static T Angle(const VectorR& lhs, const VectorR& rhs) noexcept;
		static bool IsOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept;
		static VectorR<N, T> ProjectUOntoV(const VectorR& u, const VectorR& v) noexcept;

	public:
		constexpr VectorR();
		template <class ...Args>
		constexpr VectorR(Args ... args);
		constexpr VectorR(const std::initializer_list<T>& initializerList);
		constexpr VectorR(const VectorR& rhs) = default;
		constexpr VectorR(VectorR&& rhs) noexcept = default;
		~VectorR() = default;

	public:
		VectorR& operator=(const VectorR& rhs) = default;
		VectorR& operator=(VectorR&& rhs) noexcept = default;

	public:
		VectorR& operator*=(const T scalar) noexcept;
		VectorR& operator/=(const T scalar) noexcept;
		VectorR& operator+=(const VectorR& rhs) noexcept;
		VectorR& operator-=(const VectorR& rhs) noexcept;

	public:
		VectorR operator*(const T scalar) const noexcept;
		VectorR operator/(const T scalar) const noexcept;
		VectorR operator+(const VectorR& rhs) const noexcept;
		VectorR operator-(const VectorR& rhs) const noexcept;

	public:
		const VectorR& operator+() const noexcept;
		VectorR operator-() const noexcept;

	public:
		T& operator[](const uint32 index) noexcept;
		const T& operator[](const uint32 index) const noexcept;

	public:
		bool operator==(const VectorR& rhs) const noexcept;
		bool operator!=(const VectorR& rhs) const noexcept;

	public:
		void SetZero() noexcept;

	public:
		VectorR<N, T>& SetComponent(const uint32 index, const T value) noexcept;
		T GetComponent(const uint32 index) const noexcept;
		T& X() noexcept;
		T& Y() noexcept;
		T& Z() noexcept;
		T& W() noexcept;
		const T& X() const noexcept;
		const T& Y() const noexcept;
		const T& Z() const noexcept;
		const T& W() const noexcept;

	public:
		T GetMaxComponent() const noexcept;
		T GetMinComponent() const noexcept;

	public:
		T NormSq() const noexcept;
		T Norm() const noexcept;
		VectorR<N, T>& SetNormalized() noexcept;
		VectorR<N, T> Normalize() const noexcept;
		bool IsUnitVector() const noexcept;

	public:
		T Dot(const VectorR& rhs) const noexcept;
		T Distance(const VectorR& rhs) const noexcept;
		T Angle(const VectorR& rhs) const noexcept;
		bool IsOrthogonalTo(const VectorR& rhs) const noexcept;
		VectorR<N, T> ProjectOnto(const VectorR& rhs) const noexcept;

	public:
		VectorR<N - 1, T> Shrink() const noexcept;

	public:
		T _c[N];
	};


	template <typename T>
	VectorR<3, T> Cross(const VectorR<3, T>& lhs, const VectorR<3, T>& rhs) noexcept;

	template <int32 N, typename T>
	VectorR<N, T> operator*(const T scalar, const VectorR<N, T>& vector) noexcept;


	template <int32 N>
	using VectorF = VectorR<N, float>;

	template <int32 N>
	using VectorD = VectorR<N, double>;


	template <typename T>
	using VectorR3 = VectorR<3, T>;

	template <typename T>
	using VectorR4 = VectorR<4, T>;


	namespace VectorUtils
	{
		template <int32 N, typename T>
		void SetNAN(VectorR<N, T>& in) noexcept;

		template <int32 N, typename T>
		bool IsNAN(const VectorR<N, T>& in) noexcept;
	}
}


#endif // !_MINT_MATH_VECTOR_R_H_
