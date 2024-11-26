#include <MintMath/Include/TestMath.h>
#include <MintMath/Include/AllHeaders.h>
#include <MintMath/Include/AllHpps.h>


#pragma optimize("", off)


namespace mint
{
	namespace TestMath
	{
		bool Test_IntTypes()
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

		bool Test_FloatTypes()
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
			Float3 r = Float3::Cross(p, q);

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
						//c = Float4::Cross(a, b);
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
						//c = a.Cross(b);
					}
				}
				auto logArray = Profiler::ScopedCPUProfiler::GetEntireLogData();
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

		bool Test_Transforms()
		{
			{
				Transform2D a(Float2::kOne, Math::kPiOverTwo, Float2(2, 0));
				Float3x3 ma = a.ToMatrix();
				Float3 v0{ 2, 1, 1 };
				Float3 v1 = a * v0;
				Float3 v1cmp = ma * v0;
				MINT_ASSURE(v1 == v1cmp);
			}
			{
				Transform2D a(Float2(2.0f, 3.0f), Math::kPiOverTwo, Float2(0, 0));
				Transform2D a2Cmp = a * a;
				Float3x3 ma = a.ToMatrix();
				Float3x3 ma2 = a2Cmp.ToMatrix();
				Float3x3 ma2Cmp = ma * ma;
				MINT_ASSURE(Math::Equals(ma2._m, ma2Cmp._m, Math::kFloatEpsilon) == false);
			}
			{
				Transform2D a(Float2(2.0f, 2.0f), Math::kPiOverTwo, Float2(3, 5));
				Transform2D b(Float2(3.0f, 5.0f), Math::kPiOverTwo, Float2(5, 7));
				Transform2D c = a * b;
				Float3x3 ma = a.ToMatrix();
				Float3x3 mb = b.ToMatrix();
				Float3x3 mc = ma * mb;
				Float3x3 mcCmp = c.ToMatrix();
				MINT_ASSURE(Math::Equals(mc._m, mcCmp._m, Math::kFloatEpsilon));
			}
			return true;
		}

		bool Test_LinearAlgebra()
		{
			using namespace mint;

			VectorD<3> vec0(1.0, 1.0, 0.0);
			vec0 = 5.0 * vec0;
			VectorD<3> vec1(0.0, 3.0, 0.0);
			VectorD<3> vec2 = Cross(vec0, vec1).SetNormalized();
			const bool trueValue = vec2.IsUnitVector();
			const bool falseValue = Math::Equals(1.00002f, 1.0f);
			const double distance = vec1.Normalize().Distance(vec2);
			const double theta = vec1.Angle(vec2);
			const bool orthogonality = vec1.IsOrthogonalTo(vec2);

			VectorD<1> vec3(3.0);
			MatrixD<1, 3> mat0;
			mat0.SetRow(0, VectorD<3>(4.0, 5.0, 6.0));
			constexpr bool isMat0Square = mat0.IsSquareMatrix();

			MatrixD<3, 3> mat1;
			mat1.SetRow(0, VectorD<3>(3.0, 0.0, 0.0));
			mat1.SetRow(1, VectorD<3>(0.0, 3.0, 0.0));
			mat1.SetRow(2, VectorD<3>(0.0, 0.0, 3.0));
			const bool isMat1Scalar = mat1.IsScalarMatrix();
			mat1.SetIdentity();
			const bool isMat1Identity = mat1.IsIdentityMatrix();
			mat1.SetZero();
			const bool isMat1Zero = mat1.IsZeroMatrix();

			VectorD<3> a = VectorD<3>(1.0, 2.0, 3.0);
			mat1.SetRow(0, VectorD<3>(1.0, 2.0, 3.0));
			mat1.SetRow(1, VectorD<3>(4.0, 5.0, 6.0));
			mat1.SetRow(2, VectorD<3>(7.0, 8.0, 9.0));
			VectorD<3> e1 = VectorD<3>::StandardUnitVector(1);
			VectorD<3> row1 = e1 * mat1;
			VectorD<3> col1 = mat1 * e1;
			vec0 = vec3 * mat0;

			mat1.SetRow(1, VectorD<3>(2.0, 5.0, 6.0));
			mat1.SetRow(2, VectorD<3>(3.0, 6.0, 9.0));
			const bool isMat1Symmetric = mat1.IsSymmetricMatrix();

			mat1.SetRow(1, VectorD<3>(-2.0, 5.0, 6.0));
			mat1.SetRow(2, VectorD<3>(-3.0, -6.0, 9.0));
			const bool isMat1SkewSymmetric = mat1.IsSkewSymmetricMatrix();

			MatrixD<2, 3> mat2;
			mat2.SetRow(0, VectorD<3>(0.0, 1.0, 2.0));
			mat2.SetRow(1, VectorD<3>(3.0, 4.0, 5.0));
			MatrixD<3, 2> mat2Transpose = mat2.Transpose();

			MatrixD<2, 2> mat3;
			const bool isMat3Idempotent = mat3.IsIdempotentMatrix();


			Float4x4 testFloat4x4;
			testFloat4x4.Set(1, 1, 1, 0, 0, 3, 1, 2, 2, 3, 1, 0, 1, 0, 2, 1);
			Float4x4 testFloat4x4Inverse{ testFloat4x4.Inverse() };

			Matrix<4, 4, float> testMatrix4x4;
			testMatrix4x4.SetRow(0, { 1, 1, 1, 0 });
			testMatrix4x4.SetRow(1, { 0, 3, 1, 2 });
			testMatrix4x4.SetRow(2, { 2, 3, 1, 0 });
			testMatrix4x4.SetRow(3, { 1, 0, 2, 1 });
			const bool testEquals = testMatrix4x4 == testMatrix4x4;
			testMatrix4x4 *= testMatrix4x4;

			const Matrix<4, 4, float> testIdentity(MatrixUtils::Identity<4, float>());

			// Affine
			{
				float v[4];
				AffineVecF vec0 = AffineVecF(1, 0, 0, 0);
				AffineVecF vec1 = AffineVecF(0, 1, 0, 0);
				AffineVecF vec2 = vec0.Cross(vec1);
				vec0.SetComponent(3, 1);
				vec0.Get(v);

				AffineMat<float> mat0 = TranslationMatrix(AffineVecF(4, 5, 6, 1));
				AffineMat<float> mat1 = ScalarMatrix(AffineVecF(2, 3, 4, 0));
				mat1 *= mat0;
				AffineVecF vec3 = mat1 * vec0;
				AffineMat<float> mat1Inv = mat1.Inverse();
				mat1 *= mat1Inv;

				AffineMat<float> mat2 = RotationMatrixAxisAngle(AffineVecF(1, 0, 0, 0), 1.0f);
				printf("");
			}

			return true;
		}

		bool Test()
		{
			MINT_ASSURE(Test_IntTypes());
			MINT_ASSURE(Test_FloatTypes());
			MINT_ASSURE(Test_Transforms());
			MINT_ASSURE(Test_LinearAlgebra());
			return true;
		}
	}
}
