#pragma once


#include <MintMath/Include/VectorR.h>

#include <MintMath/Include/MathCommon.h>

#include <MintMath/Include/Matrix.hpp>

#include <initializer_list>


namespace mint
{
	namespace Math
	{
		template<int32 N, typename T>
		MINT_INLINE bool Equals(const T(&lhs)[N], const T(&rhs)[N], const T epsilon) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				const T absDiff = ::abs(lhs[i] - rhs[i]);
				if (epsilon < absDiff)
				{
					return false;
				}
			}
			return true;
		}

		template<int32 N, typename T>
		MINT_INLINE void CopyVec(const T(&src)[N], T(&dest)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				dest[i] = src[i];
			}
		}

		template<int32 N, typename T>
		MINT_INLINE T Dot(const T(&lhs)[N], const T(&rhs)[N]) noexcept
		{
			T result{};
			for (int32 i = 0; i < N; ++i)
			{
				result += lhs[i] * rhs[i];
			}
			return result;
		}

		template<typename T>
		MINT_INLINE void Cross(const T(&lhs)[3], const T(&rhs)[3], T(&out)[3]) noexcept
		{
			out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
			out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
			out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
		}

		template<typename T>
		void Cross(const T(&lhs)[4], const T(&rhs)[4], T(&out)[4]) noexcept
		{
			out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
			out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
			out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
			out[3] = static_cast<T>(0); // It is a vector, not point!
		}

		template<int32 N, typename T>
		MINT_INLINE T NormSq(const T(&vec)[N]) noexcept
		{
			return Dot(vec, vec);
		}

		template<int32 N, typename T>
		MINT_INLINE T Norm(const T(&vec)[N]) noexcept
		{
			return ::sqrt(NormSq(vec));
		}

		template<int32 N, typename T>
		MINT_INLINE void Normalize(T(&inOut)[N]) noexcept
		{
			const T norm_ = Norm(inOut);
			Math::SetDivVec(inOut, norm_);
		}

		template<int32 N, typename T>
		MINT_INLINE void SetZeroVec(T(&vec)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				vec[i] = 0;
			}
		}

		template<int32 N, typename T>
		MINT_INLINE void SetNAN(T(&vec)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				vec[i] = Math::getNAN();
			}
		}

		template<int32 N, typename T>
		MINT_INLINE bool IsNAN(const T(&vec)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				if (Math::IsNAN(vec[i]))
				{
					return true;
				}
			}
			return false;
		}

		template<int32 N, typename T>
		MINT_INLINE void SetAddVec(T(&lhs)[N], const T(&rhs)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				lhs[i] += rhs[i];
			}
		}

		template<int32 N, typename T>
		MINT_INLINE void SetSubVec(T(&lhs)[N], const T(&rhs)[N]) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				lhs[i] -= rhs[i];
			}
		}

		template<int32 N, typename T>
		MINT_INLINE void SetMulVec(T(&vec)[N], const float scalar) noexcept
		{
			for (int32 i = 0; i < N; ++i)
			{
				vec[i] *= scalar;
			}
		}

		template<int32 N, typename T>
		MINT_INLINE void SetDivVec(T(&vec)[N], const float scalar) noexcept
		{
			MINT_ASSERT(scalar != 0.0, "0 으로 나누려 합니다!");
			for (int32 i = 0; i < N; ++i)
			{
				vec[i] /= scalar;
			}
		}
	}


	template<int32 N, typename T>
	inline VectorR<N, T> VectorR<N, T>::StandardUnitVector(const int32 math_i) noexcept
	{
		VectorR<N, T> result;
		result.SetComponent(math_i - 1, 1.0);
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Dot(const VectorR& lhs, const VectorR& rhs) noexcept
	{
		return Math::Dot(lhs._c, rhs._c);
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Distance(const VectorR& lhs, const VectorR& rhs) noexcept
	{
		return (rhs - lhs).Norm();
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Angle(const VectorR& lhs, const VectorR& rhs) noexcept
	{
		return ::acos(lhs.Normalize().Dot(rhs.Normalize()));
	}

	template<int32 N, typename T>
	MINT_INLINE bool VectorR<N, T>::IsOrthogonal(const VectorR& lhs, const VectorR& rhs) noexcept
	{
		return Math::Equals(lhs.Dot(rhs), 0.0);
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::ProjectUOntoV(const VectorR& u, const VectorR& v) noexcept
	{
		return (u.Dot(v) / u.Dot(u)) * u;
	}

	template<int32 N, typename T>
	inline constexpr VectorR<N, T>::VectorR()
		: _c{}
	{
		__noop;
	}

	template<int32 N, typename T>
	template <class ...Args>
	inline constexpr VectorR<N, T>::VectorR(Args... args)
		: _c{ args... }
	{
		__noop;
	}

	template<int32 N, typename T>
	inline constexpr VectorR<N, T>::VectorR(const std::initializer_list<T>& initializerList)
	{
		const int32 count = Min(static_cast<int32>(initializerList.size()), N);
		const T* const first = initializerList.begin();
		for (int32 index = 0; index < count; ++index)
		{
			_c[index] = *(first + index);
		}
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator*=(const T scalar) noexcept
	{
		for (int32 index = 0; index < N; ++index)
		{
			_c[index] *= scalar;
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator/=(const T scalar) noexcept
	{
		MINT_ASSERT(scalar != 0.0, "0 으로 나누려 합니다!");

		for (int32 index = 0; index < N; ++index)
		{
			_c[index] /= scalar;
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator+=(const VectorR& rhs) noexcept
	{
		for (int32 index = 0; index < N; ++index)
		{
			_c[index] += rhs._c[index];
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::operator-=(const VectorR& rhs) noexcept
	{
		for (int32 index = 0; index < N; ++index)
		{
			_c[index] -= rhs._c[index];
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::operator*(const T scalar) const noexcept
	{
		VectorR result = *this;
		result *= scalar;
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::operator/(const T scalar) const noexcept
	{
		VectorR result = *this;
		result /= scalar;
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::operator+(const VectorR& rhs) const noexcept
	{
		VectorR result = *this;
		result += rhs;
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::operator-(const VectorR& rhs) const noexcept
	{
		VectorR result = *this;
		result -= rhs;
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE const VectorR<N, T>& VectorR<N, T>::operator+() const noexcept
	{
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::operator-() const noexcept
	{
		VectorR<N, T> result;
		for (int32 i = 0; i < N; ++i)
		{
			result._c[i] = -_c[i];
		}
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE T& VectorR<N, T>::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
		return _c[index];
	}

	template<int32 N, typename T>
	MINT_INLINE const T& VectorR<N, T>::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
		return _c[index];
	}

	template<int32 N, typename T>
	MINT_INLINE bool VectorR<N, T>::operator==(const VectorR& rhs) const noexcept
	{
		for (int32 i = 0; i < N; ++i)
		{
			if (rhs._c[i] != _c[i])
			{
				return false;
			}
		}
		return true;
	}

	template<int32 N, typename T>
	MINT_INLINE bool VectorR<N, T>::operator!=(const VectorR& rhs) const noexcept
	{
		return !(rhs == *this);
	}

	template<int32 N, typename T>
	MINT_INLINE void VectorR<N, T>::SetZero() noexcept
	{
		for (int32 index = 0; index < N; ++index)
		{
			_c[index] = 0.0;
		}
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::SetComponent(const uint32 index, const T value) noexcept
	{
		if (index < static_cast<uint32>(N))
		{
			_c[index] = value;
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::GetComponent(const uint32 index) const noexcept
	{
		MINT_ASSERT(index < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");
		return _c[index];
	}

	template<int32 N, typename T>
	MINT_INLINE T& VectorR<N, T>::X() noexcept
	{
		return _c[0];
	}

	template<int32 N, typename T>
	MINT_INLINE T& VectorR<N, T>::Y() noexcept
	{
		static_assert(N >= 2, "Vector dimension is not enough!");
		return _c[1];
	}

	template<int32 N, typename T>
	MINT_INLINE T& VectorR<N, T>::Z() noexcept
	{
		static_assert(N >= 3, "Vector dimension is not enough!");
		return _c[2];
	}

	template<int32 N, typename T>
	MINT_INLINE T& VectorR<N, T>::W() noexcept
	{
		static_assert(N >= 4, "Vector dimension is not enough!");
		return _c[3];
	}

	template<int32 N, typename T>
	MINT_INLINE const T& VectorR<N, T>::X() const noexcept
	{
		return _c[0];
	}

	template<int32 N, typename T>
	MINT_INLINE const T& VectorR<N, T>::Y() const noexcept
	{
		static_assert(N >= 2, "Vector dimension is not enough!");
		return _c[1];
	}

	template<int32 N, typename T>
	MINT_INLINE const T& VectorR<N, T>::Z() const noexcept
	{
		static_assert(N >= 3, "Vector dimension is not enough!");
		return _c[2];
	}

	template<int32 N, typename T>
	MINT_INLINE const T& VectorR<N, T>::W() const noexcept
	{
		static_assert(N >= 4, "Vector dimension is not enough!");
		return _c[3];
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::GetMaxComponent() const noexcept
	{
		T result = 1.0e-323;
		for (int32 index = 0; index < N; ++index)
		{
			if (result < _c[index])
			{
				result = _c[index];
			}
		}
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::GetMinComponent() const noexcept
	{
		T result = 1.0e+308;
		for (int32 index = 0; index < N; ++index)
		{
			if (_c[index] < result)
			{
				result = _c[index];
			}
		}
		return result;
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::NormSq() const noexcept
	{
		return Math::NormSq(_c);
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Norm() const noexcept
	{
		return Math::Norm(_c);
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T>& VectorR<N, T>::SetNormalized() noexcept
	{
		const T norm_ = Norm();
		if (norm_ != 0)
		{
			*this /= norm_;
		}
		return *this;
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::Normalize() const noexcept
	{
		VectorR<N, T> result = *this;
		return result.SetNormalized();
	}

	template<int32 N, typename T>
	MINT_INLINE bool VectorR<N, T>::IsUnitVector() const noexcept
	{
		return Math::Equals(NormSq(), static_cast<T>(1.0));
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Dot(const VectorR& rhs) const noexcept
	{
		return Dot(*this, rhs);
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Distance(const VectorR& rhs) const noexcept
	{
		return Distance(*this, rhs);
	}

	template<int32 N, typename T>
	MINT_INLINE T VectorR<N, T>::Angle(const VectorR& rhs) const noexcept
	{
		return Angle(*this, rhs);
	}

	template<int32 N, typename T>
	MINT_INLINE bool VectorR<N, T>::IsOrthogonalTo(const VectorR& rhs) const noexcept
	{
		return IsOrthogonal(*this, rhs);
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> VectorR<N, T>::ProjectOnto(const VectorR& rhs) const noexcept
	{
		return ProjectUOntoV(*this, rhs);
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N - 1, T> VectorR<N, T>::Shrink() const noexcept
	{
		MINT_ASSERT(N > 1, "N must be greater than 1!!!");

		VectorR<N - 1, T> result;
		for (uint32 i = 0; i < N - 1; ++i)
		{
			result.SetComponent(0, _c[i]);
		}
		return result;
	}


	template<typename T>
	VectorR<3, T> Cross(const VectorR<3, T>& lhs, const VectorR<3, T>& rhs) noexcept
	{
		return VectorR<3, T>
			(
				{
					lhs.GetComponent(1) * rhs.GetComponent(2) - lhs.GetComponent(2) * rhs.GetComponent(1),
					lhs.GetComponent(2) * rhs.GetComponent(0) - lhs.GetComponent(0) * rhs.GetComponent(2),
					lhs.GetComponent(0) * rhs.GetComponent(1) - lhs.GetComponent(1) * rhs.GetComponent(0)
				}
		);
	}

	template<int32 N, typename T>
	MINT_INLINE VectorR<N, T> operator*(const T scalar, const VectorR<N, T>& vector) noexcept
	{
		return (vector * scalar);
	}


	namespace VectorUtils
	{
		template<int32 N, typename T>
		void SetNAN(VectorR<N, T>& in) noexcept
		{
			for (uint32 i = 0; i < N; ++i)
			{
				in.SetComponent(i, Math::getNAN());
			}
		}

		template<int32 N, typename T>
		bool IsNAN(const VectorR<N, T>& in) noexcept
		{
			for (uint32 i = 0; i < N; ++i)
			{
				if (Math::IsNAN(in.GetComponent(i)))
				{
					return true;
				}
			}
			return false;
		}
	}
}
