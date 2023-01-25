#include <MintMath/Include/Test.h>
#include <MintMath/Include/AllHeaders.h>
#include <MintMath/Include/AllHpps.h>


#pragma optimize("", off)


namespace mint
{
	namespace TestMath
	{
		bool test_intTypes()
		{
			using namespace mint;
			Int2 ni;
			Int2 a{ 1, 2 };
			Int2 b{ 3, 4 };
			Int2 c = a + b;
			Int2 d;
			d = a;
			return true;
		}

		bool test_floatTypes()
		{
			using namespace mint;
			float a = 15.000001f;
			float b = 15.000002f;
			float c = 15000.0001f;
			float d = 15000.0005f;
			float e = 15000.0015f;

			struct
			{
				Float3 _a;
				Float3 _b;
			} st;
			auto sizeFloat2 = sizeof(Float2);
			auto sizeFloat3 = sizeof(Float3);
			auto sizeFloat4 = sizeof(Float4);
			auto sizeSt = sizeof(st);
			Float3 p{ 1, 0, 0 };
			Float3 q{ 0, 1, 0 };
			Float3 r = Float3::cross(p, q);

#if defined MINT_TEST_FAILURES
			Float2 t;
			t[3] = 1.0f;
#endif
			{
				/*
				static constexpr uint64 kTestCount = 500'000'000;
				{
					Profiler::ScopedCPUProfiler profiler("Profile - Float4");
					Float4 a(1.0f, 0.0f, 1.0f, 0.0f);
					Float4 b(0.0f, 1.0f, 0.0f, 1.0f);
					Float4 c;
					for (uint64 i = 0; i < kTestCount; ++i)
					{
						//a += b;
						//a -= b;
						//a *= 0.5f;
						//a *= 2.0f;
						c = a + b;
						//c = Float4::cross(a, b);
					}
				}
				{
					Profiler::ScopedCPUProfiler profiler("Profile - AffineVecF");
					AffineVecF a(1.0f, 0.0f, 1.0f, 0.0f);
					AffineVecF b(0.0f, 1.0f, 0.0f, 1.0f);
					AffineVecF c;
					for (uint64 i = 0; i < kTestCount; ++i)
					{
						//a += b;
						//a -= b;
						//a *= 0.5f;
						//a *= 2.0f;
						c = a + b;
						//c = a.cross(b);
					}
				}
				auto logArray = Profiler::ScopedCPUProfiler::getEntireLogArray();
				*/

				AffineVecF a;
				AffineVecF b;
				AffineVecF c = AffineVecF(0.0f, 2.0f, 3.0f, 4.0f);
				a == b;
				a == c;

				AffineVecD da;
				AffineVecD db;
				AffineVecD dc = AffineVecD(0, 2, 3, 4);
				da == db;
				da += dc;
				auto sizeA = sizeof(a);
				auto sizeB = sizeof(da);
				printf("");
			}
			return true;
		}

		bool test_linearAlgebra()
		{
			using namespace mint;

			VectorD<3> vec0(1.0, 1.0, 0.0);
			vec0 = 5.0 * vec0;
			VectorD<3> vec1(0.0, 3.0, 0.0);
			VectorD<3> vec2 = cross(vec0, vec1).setNormalized();
			const bool trueValue = vec2.isUnitVector();
			const bool falseValue = Math::equals(1.00002f, 1.0f);
			const double distance = vec1.normalize().distance(vec2);
			const double theta = vec1.angle(vec2);
			const bool orthogonality = vec1.isOrthogonalTo(vec2);

			VectorD<1> vec3(3.0);
			MatrixD<1, 3> mat0;
			mat0.setRow(0, VectorD<3>(4.0, 5.0, 6.0));
			constexpr bool isMat0Square = mat0.isSquareMatrix();

			MatrixD<3, 3> mat1;
			mat1.setRow(0, VectorD<3>(3.0, 0.0, 0.0));
			mat1.setRow(1, VectorD<3>(0.0, 3.0, 0.0));
			mat1.setRow(2, VectorD<3>(0.0, 0.0, 3.0));
			const bool isMat1Scalar = mat1.isScalarMatrix();
			mat1.setIdentity();
			const bool isMat1Identity = mat1.isIdentityMatrix();
			mat1.setZero();
			const bool isMat1Zero = mat1.isZeroMatrix();

			VectorD<3> a = VectorD<3>(1.0, 2.0, 3.0);
			mat1.setRow(0, VectorD<3>(1.0, 2.0, 3.0));
			mat1.setRow(1, VectorD<3>(4.0, 5.0, 6.0));
			mat1.setRow(2, VectorD<3>(7.0, 8.0, 9.0));
			VectorD<3> e1 = VectorD<3>::standardUnitVector(1);
			VectorD<3> row1 = e1 * mat1;
			VectorD<3> col1 = mat1 * e1;
			vec0 = vec3 * mat0;

			mat1.setRow(1, VectorD<3>(2.0, 5.0, 6.0));
			mat1.setRow(2, VectorD<3>(3.0, 6.0, 9.0));
			const bool isMat1Symmetric = mat1.isSymmetricMatrix();

			mat1.setRow(1, VectorD<3>(-2.0, 5.0, 6.0));
			mat1.setRow(2, VectorD<3>(-3.0, -6.0, 9.0));
			const bool isMat1SkewSymmetric = mat1.isSkewSymmetricMatrix();

			MatrixD<2, 3> mat2;
			mat2.setRow(0, VectorD<3>(0.0, 1.0, 2.0));
			mat2.setRow(1, VectorD<3>(3.0, 4.0, 5.0));
			MatrixD<3, 2> mat2Transpose = mat2.transpose();

			MatrixD<2, 2> mat3;
			const bool isMat3Idempotent = mat3.isIdempotentMatrix();


			Float4x4 testFloat4x4;
			testFloat4x4.set(1, 1, 1, 0, 0, 3, 1, 2, 2, 3, 1, 0, 1, 0, 2, 1);
			Float4x4 testFloat4x4Inverse{ testFloat4x4.inverse() };

			Matrix<4, 4, float> testMatrix4x4;
			testMatrix4x4.setRow(0, { 1, 1, 1, 0 });
			testMatrix4x4.setRow(1, { 0, 3, 1, 2 });
			testMatrix4x4.setRow(2, { 2, 3, 1, 0 });
			testMatrix4x4.setRow(3, { 1, 0, 2, 1 });
			const bool testEquals = testMatrix4x4 == testMatrix4x4;
			testMatrix4x4 *= testMatrix4x4;

			const Matrix<4, 4, float> testIdentity(MatrixUtils::identity<4, float>());

			// Affine
			{
				float v[4];
				AffineVecF vec0 = AffineVecF(1, 0, 0, 0);
				AffineVecF vec1 = AffineVecF(0, 1, 0, 0);
				AffineVecF vec2 = vec0.cross(vec1);
				vec0.setComponent(3, 1);
				vec0.get(v);

				AffineMat<float> mat0 = translationMatrix(AffineVecF(4, 5, 6, 1));
				AffineMat<float> mat1 = scalarMatrix(AffineVecF(2, 3, 4, 0));
				mat1 *= mat0;
				AffineVecF vec3 = mat1 * vec0;
				AffineMat<float> mat1Inv = mat1.inverse();
				mat1 *= mat1Inv;

				AffineMat<float> mat2 = rotationMatrixAxisAngle(AffineVecF(1, 0, 0, 0), 1.0f);
				printf("");
			}

			return true;
		}

		bool test()
		{
			MINT_ASSURE(test_intTypes());
			MINT_ASSURE(test_floatTypes());
			MINT_ASSURE(test_linearAlgebra());
			return true;
		}
	}
}
