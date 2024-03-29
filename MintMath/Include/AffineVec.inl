﻿#pragma once


namespace mint
{
#pragma region Free functions
	template<typename T>
	MINT_INLINE AffineVec<T> operator*(const T scalar, const AffineVec<T>& vec) noexcept
	{
		return vec * scalar;
	}

	template<typename T>
	MINT_INLINE AffineVec<T> Normalize(const AffineVec<T>& in) noexcept
	{
		const T Norm = in.Norm();
		return AffineVec<T>(in) / Norm;
	}

	template<typename T>
	MINT_INLINE void Normalize(AffineVec<T>& inOut) noexcept
	{
		const T Norm = inOut.Norm();
		inOut /= Norm;
	}

	MINT_INLINE float Dot(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept
	{
		const __m128 result = _mm_mul_ps(lhs.GetRaw(), rhs.GetRaw());
		return result.m128_f32[0] + result.m128_f32[1] + result.m128_f32[2] + result.m128_f32[3];
	}

	MINT_INLINE AffineVec<float> Cross(const AffineVec<float>& lhs, const AffineVec<float>& rhs) noexcept
	{
		float l[4];
		lhs.Get(l);
		float r[4];
		rhs.Get(r);
		const __m128 a = _mm_mul_ps(_mm_set_ps(0.0f, l[0], l[2], l[1]), _mm_set_ps(0.0f, r[1], r[0], r[2])); // 순서에 주의!!!
		const __m128 b = _mm_mul_ps(_mm_set_ps(0.0f, l[1], l[0], l[2]), _mm_set_ps(0.0f, r[0], r[2], r[1])); // 순서에 주의!!!
		// x == ly * rz - lz * ry
		// y == lz * rx - lx * rz
		// z == lx * ry - ly * rx
		return AffineVec<float>(_mm_sub_ps(a, b));
	}

	MINT_INLINE double Dot(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept
	{
		const __m256d result = _mm256_mul_pd(lhs.GetRaw(), rhs.GetRaw());
		return result.m256d_f64[0] + result.m256d_f64[1] + result.m256d_f64[2] + result.m256d_f64[3];
	}

	MINT_INLINE AffineVec<double> Cross(const AffineVec<double>& lhs, const AffineVec<double>& rhs) noexcept
	{
		double l[4];
		lhs.Get(l);
		double r[4];
		rhs.Get(r);
		const __m256d a = _mm256_mul_pd(_mm256_set_pd(0.0f, l[0], l[2], l[1]), _mm256_set_pd(0.0f, r[1], r[0], r[2])); // 순서에 주의!!!
		const __m256d b = _mm256_mul_pd(_mm256_set_pd(0.0f, l[1], l[0], l[2]), _mm256_set_pd(0.0f, r[0], r[2], r[1])); // 순서에 주의!!!
		// x == ly * rz - lz * ry
		// y == lz * rx - lx * rz
		// z == lx * ry - ly * rx
		return AffineVec<double>(_mm256_sub_pd(a, b));
	}
#pragma endregion


	inline AffineVec<float>::AffineVec()
		: _raw{ _mm_setzero_ps() }
	{
		__noop;
	}

	inline AffineVec<float>::AffineVec(const float scalar)
		: _raw{ _mm_set_ps(scalar, scalar, scalar, scalar) }
	{
		__noop;
	}

	inline AffineVec<float>::AffineVec(const float x, const float y, const float z, const float w)
		: _raw{ _mm_set_ps(w, z, y, x) } // 순서에 주의!!!
	{
		__noop;
	}

	inline AffineVec<float>::AffineVec(const __m128 raw)
		: _raw{ raw }
	{
		__noop;
	}

	inline AffineVec<float>::AffineVec(const AffineVec& rhs)
		: _raw{ rhs._raw }
	{
		__noop;
	}

	inline AffineVec<float>::AffineVec(AffineVec&& rhs) noexcept
		: _raw{ rhs._raw }
	{
		rhs._raw = _mm_setzero_ps();
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator=(const AffineVec& rhs)
	{
		if (this != &rhs)
		{
			_raw = rhs._raw;
		}
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator=(AffineVec&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_raw = rhs._raw;
			rhs._raw = _mm_setzero_ps();
		}
		return *this;
	}

	MINT_INLINE bool AffineVec<float>::operator==(const AffineVec& rhs) const noexcept
	{
		const __m128 cmpResult = _mm_cmpeq_ps(_raw, rhs._raw);
		return (cmpResult.m128_i32[0] < 0) && (cmpResult.m128_i32[1] < 0) && (cmpResult.m128_i32[2] < 0) && (cmpResult.m128_i32[3] < 0);
	}

	MINT_INLINE bool AffineVec<float>::operator!=(const AffineVec& rhs) const noexcept
	{
		const __m128 cmpResult = _mm_cmpeq_ps(_raw, rhs._raw);
		return (cmpResult.m128_i32[0] == 0) || (cmpResult.m128_i32[1] == 0) || (cmpResult.m128_i32[2] == 0) || (cmpResult.m128_i32[3] == 0);
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator+() noexcept
	{
		return *this;
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator-() const noexcept
	{
		return AffineVec(_mm_mul_ps(_raw, _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f)));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator+(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm_add_ps(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator-(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm_sub_ps(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator*(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm_mul_ps(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator/(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm_div_ps(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator*(const float scalar) const noexcept
	{
		return AffineVec(_mm_mul_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar)));
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::operator/(const float scalar) const noexcept
	{
		return AffineVec(_mm_div_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar)));
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator+=(const AffineVec& rhs) noexcept
	{
		_raw = _mm_add_ps(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator-=(const AffineVec& rhs) noexcept
	{
		_raw = _mm_sub_ps(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator*=(const AffineVec& rhs) noexcept
	{
		_raw = _mm_mul_ps(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator/=(const AffineVec& rhs) noexcept
	{
		_raw = _mm_div_ps(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator*=(const float scalar) noexcept
	{
		_raw = _mm_mul_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar));
		return *this;
	}

	MINT_INLINE AffineVec<float>& AffineVec<float>::operator/=(const float scalar) noexcept
	{
		_raw = _mm_div_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar));
		return *this;
	}

	MINT_INLINE void AffineVec<float>::Set(const float x, const float y, const float z, const float w) noexcept
	{
		_raw = _mm_set_ps(w, z, y, x); // 순서에 주의!!!
	}

	MINT_INLINE void AffineVec<float>::SetComponent(const int32 i, const float scalar) noexcept
	{
		_raw.m128_f32[i] = scalar;
	}

	MINT_INLINE void AffineVec<float>::AddComponent(const int32 i, const float scalar) noexcept
	{
		_raw.m128_f32[i] += scalar;
	}

	MINT_INLINE void AffineVec<float>::Get(float(&vec)[4]) const noexcept
	{
		_mm_store_ps(vec, _raw);
	}

	MINT_INLINE float AffineVec<float>::GetComponent(const int32 i) const noexcept
	{
		return _raw.m128_f32[i];
	}

	MINT_INLINE const __m128& AffineVec<float>::GetRaw() const noexcept
	{
		return _raw;
	}

	MINT_INLINE float AffineVec<float>::Dot(const AffineVec& rhs) const noexcept
	{
		return mint::Dot(*this, rhs);
	}

	MINT_INLINE float AffineVec<float>::NormSq() const noexcept
	{
		return Dot(*this);
	}

	MINT_INLINE float AffineVec<float>::Norm() const noexcept
	{
		return ::sqrt(NormSq());
	}

	MINT_INLINE void AffineVec<float>::Normalize() noexcept
	{
		mint::Normalize(*this);
	}

	MINT_INLINE AffineVec<float> AffineVec<float>::Cross(const AffineVec<float>& rhs) const noexcept
	{
		return mint::Cross(*this, rhs);
	}




	inline AffineVec<double>::AffineVec()
		: _raw{ _mm256_setzero_pd() }
	{
		__noop;
	}

	inline AffineVec<double>::AffineVec(const double scalar)
		: _raw{ _mm256_set_pd(scalar, scalar, scalar, scalar) }
	{
		__noop;
	}

	inline AffineVec<double>::AffineVec(const double x, const double y, const double z, const double w)
		: _raw{ _mm256_set_pd(w, z, y, x) } // 순서에 주의!!!
	{
		__noop;
	}

	inline AffineVec<double>::AffineVec(const __m256d raw)
		: _raw{ raw }
	{
		__noop;
	}

	inline AffineVec<double>::AffineVec(const AffineVec& rhs)
		: _raw{ rhs._raw }
	{
		__noop;
	}

	inline AffineVec<double>::AffineVec(AffineVec&& rhs) noexcept
		: _raw{ rhs._raw }
	{
		rhs._raw = _mm256_setzero_pd();
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator=(const AffineVec& rhs)
	{
		if (this != &rhs)
		{
			_raw = rhs._raw;
		}
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator=(AffineVec&& rhs) noexcept
	{
		if (this != &rhs)
		{
			_raw = rhs._raw;
			rhs._raw = _mm256_setzero_pd();
		}
		return *this;
	}

	MINT_INLINE bool AffineVec<double>::operator==(const AffineVec& rhs) const noexcept
	{
		const __m256d cmpResult = _mm256_cmp_pd(_raw, rhs._raw, _CMP_EQ_OQ);
		return (cmpResult.m256d_f64[0] < 0) && (cmpResult.m256d_f64[1] < 0) && (cmpResult.m256d_f64[2] < 0) && (cmpResult.m256d_f64[3] < 0);
	}

	MINT_INLINE bool AffineVec<double>::operator!=(const AffineVec& rhs) const noexcept
	{
		const __m256d cmpResult = _mm256_cmp_pd(_raw, rhs._raw, _CMP_EQ_OQ);
		return (cmpResult.m256d_f64[0] == 0) || (cmpResult.m256d_f64[1] == 0) || (cmpResult.m256d_f64[2] == 0) || (cmpResult.m256d_f64[3] == 0);
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator+() noexcept
	{
		return *this;
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator-() const noexcept
	{
		return AffineVec(_mm256_mul_pd(_raw, _mm256_set_pd(-1.0f, -1.0f, -1.0f, -1.0f)));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator+(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm256_add_pd(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator-(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm256_sub_pd(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator*(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm256_mul_pd(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator/(const AffineVec& rhs) const noexcept
	{
		return AffineVec(_mm256_div_pd(_raw, rhs._raw));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator*(const double scalar) const noexcept
	{
		return AffineVec(_mm256_mul_pd(_raw, _mm256_set_pd(scalar, scalar, scalar, scalar)));
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::operator/(const double scalar) const noexcept
	{
		return AffineVec(_mm256_div_pd(_raw, _mm256_set_pd(scalar, scalar, scalar, scalar)));
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator+=(const AffineVec& rhs) noexcept
	{
		_raw = _mm256_add_pd(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator-=(const AffineVec& rhs) noexcept
	{
		_raw = _mm256_sub_pd(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator*=(const AffineVec& rhs) noexcept
	{
		_raw = _mm256_mul_pd(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator/=(const AffineVec& rhs) noexcept
	{
		_raw = _mm256_div_pd(_raw, rhs._raw);
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator*=(const double scalar) noexcept
	{
		_raw = _mm256_mul_pd(_raw, _mm256_set_pd(scalar, scalar, scalar, scalar));
		return *this;
	}

	MINT_INLINE AffineVec<double>& AffineVec<double>::operator/=(const double scalar) noexcept
	{
		_raw = _mm256_div_pd(_raw, _mm256_set_pd(scalar, scalar, scalar, scalar));
		return *this;
	}

	MINT_INLINE void AffineVec<double>::Set(const double x, const double y, const double z, const double w) noexcept
	{
		_raw = _mm256_set_pd(w, z, y, x); // 순서에 주의!!!
	}

	MINT_INLINE void AffineVec<double>::SetComponent(const int32 i, const double scalar) noexcept
	{
		_raw.m256d_f64[i] = scalar;
	}

	MINT_INLINE void AffineVec<double>::AddComponent(const int32 i, const double scalar) noexcept
	{
		_raw.m256d_f64[i] += scalar;
	}

	MINT_INLINE void AffineVec<double>::Get(double(&vec)[4]) const noexcept
	{
		_mm256_store_pd(vec, _raw);
	}

	MINT_INLINE double AffineVec<double>::GetComponent(const int32 i) const noexcept
	{
		return _raw.m256d_f64[i];
	}

	MINT_INLINE const __m256d& AffineVec<double>::GetRaw() const noexcept
	{
		return _raw;
	}

	MINT_INLINE double AffineVec<double>::Dot(const AffineVec& rhs) const noexcept
	{
		return mint::Dot(*this, rhs);
	}

	MINT_INLINE double AffineVec<double>::NormSq() const noexcept
	{
		return Dot(*this);
	}

	MINT_INLINE double AffineVec<double>::Norm() const noexcept
	{
		return ::sqrt(NormSq());
	}

	MINT_INLINE void AffineVec<double>::Normalize() noexcept
	{
		mint::Normalize(*this);
	}

	MINT_INLINE AffineVec<double> AffineVec<double>::Cross(const AffineVec<double>& rhs) const noexcept
	{
		return mint::Cross(*this, rhs);
	}
}
