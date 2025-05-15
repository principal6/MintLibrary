#include <MintRendering/Include/ShapeRenderer.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StackVector.hpp>

#include <MintRendering/Include/GraphicsDevice.h>
#include <MintRendering/Include/LowLevelRenderer.hpp>
#include <MintRendering/Include/ShapeGenerator.h>

#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Float3x3.h>


namespace mint
{
	namespace Rendering
	{
		ShapeRenderer::ShapeRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData)
			: IRenderer(graphicsDevice, lowLevelRenderer, sbTransformData)
		{
			__noop;
		}

		ShapeRenderer::~ShapeRenderer()
		{
			Terminate();
		}

		const char* ShapeRenderer::GetDefaultVertexShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
				#include <ShaderConstantBuffers>
				#include <ShaderStructuredBufferDefinitions>
					
				StructuredBuffer<SB_Transform> sbTransform : register(t0);
					
				VS_OUTPUT_SHAPE main_shape(VS_INPUT_SHAPE input)
				{
					uint transformIndex = input._info & 0x3FFFFFFF;
					uint info = input._info >> 30;
					
					float4 transformedPosition = float4(input._position.xyz, 1.0);
					transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
						
					VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
					result._position = mul(transformedPosition, _cbViewProjectionMatrix);
					result._color = input._color;
					result._texCoord = input._texCoord;
					result._info = info;
					result._viewportIndex = 0;
					return result;
				}
				)"
			};
			return kShaderString;
		}

		const char* ShapeRenderer::GetDefaultGeometryShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
					
				[maxvertexcount(3)]
				void main_shape(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
				{
					for (int i = 0; i < 3; ++i)
					{
						OutputStream.Append(input[i]);
					}
					OutputStream.RestartStrip();
				}
				)"
			};
			return kShaderString;
		}

		const char* ShapeRenderer::GetDefaultPixelShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					
					sampler g_sampler0;
					Texture2D<float4> g_texture0;
					
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
					{
						if (input._info == 1)
						{
							// Textured triangle
							return g_texture0.Sample(g_sampler0, input._texCoord.xy);
						}
						// Just use color
						return input._color;
					}
					)"
			};
			return kShaderString;
		}

		void ShapeRenderer::Initialize() noexcept
		{
			InitializeCommon();

			MaterialDesc materialDesc;
			materialDesc._materialName = "DefaultShapeMaterial";
			materialDesc._shaderPipelineID = GetDefaultShaderPipelineID();
			MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();
			_defaultMaterialID = materialPool.CreateMaterial(materialDesc);

			SetMaterial(_defaultMaterialID);
		}

		void ShapeRenderer::InitializeCommon() noexcept
		{
			SetClipRect(_graphicsDevice.GetFullScreenClipRect());

			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			GraphicsObjectID vertexShaderID = shaderPool.CreateShaderFromMemory("ShapeRendererVS", GetDefaultVertexShaderString(), "main_shape", GraphicsShaderType::VertexShader);
			using namespace Language;
			const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicsDevice.GetCppHlslSteamData().GetTypeMetaData(typeid(VS_INPUT_SHAPE));
			GraphicsObjectID inputLayoutID = shaderPool.CreateInputLayout(vertexShaderID, typeMetaData);
			GraphicsObjectID geometryShaderID = shaderPool.CreateShaderFromMemory("ShapeRendererGS", GetDefaultGeometryShaderString(), "main_shape", GraphicsShaderType::GeometryShader);
			GraphicsObjectID pixelShaderID = shaderPool.CreateShaderFromMemory(GetPixelShaderName(), GetPixelShaderString(), GetPixelShaderEntryPoint(), GraphicsShaderType::PixelShader);

			ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutID;
				shaderPipelineDesc._vertexShaderID = vertexShaderID;
				shaderPipelineDesc._geometryShaderID = geometryShaderID;
				shaderPipelineDesc._pixelShaderID = pixelShaderID;
				_shaderPipelineMultipleViewportID = shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}
		}

		void ShapeRenderer::Terminate() noexcept
		{
			ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
			if (_shaderPipelineMultipleViewportID.IsValid())
			{
				shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineMultipleViewportID);
			}
		}

		bool ShapeRenderer::IsEmpty() const noexcept
		{
			return _lowLevelRenderer.IsRenderable() == false;
		}

		void ShapeRenderer::SetMaterial(const GraphicsObjectID& materialID) noexcept
		{
			_currentMaterialID = materialID;
		}

		void ShapeRenderer::AddShape(const Shape& shape, const Transform& transform)
		{
			AddShape_Internal(shape);
			PushTransformToBuffer(transform);
		}

		void ShapeRenderer::AddShape(const Shape& shape, const Transform2D& transform2D)
		{
			AddShape_Internal(shape);
			PushTransformToBuffer(transform2D);
		}

		void ShapeRenderer::AddShape(const Shape& shape)
		{
			AddShape(shape, Transform2D::GetIdentity());
		}

		void ShapeRenderer::DrawLine(const Float2& p0, const Float2& p1, const float thickness)
		{
			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			ShapeGenerator::GenerateLine(p0, p1, thickness, 8, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());

			const uint32 deltaVertexCount = _lowLevelRenderer.GetVertexCount() - vertexOffset;
			for (uint32 i = 0; i < deltaVertexCount; i++)
			{
				_lowLevelRenderer.Vertices()[vertexOffset + i]._info = ComputeVertexInfo(transformIndex, 0);
			}
			const uint32 deltaIndexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, deltaIndexCount, GetClipRect(), _currentMaterialID);

			PushTransformToBuffer(Float2::kOne, 0.0f, Float3::kZero);
		}

		void ShapeRenderer::DrawLineStrip(const Vector<Float2>& points, const float thickness)
		{
			const uint32 pointCount = points.Size();
			for (uint32 i = 1; i < pointCount; i++)
			{
				DrawLine(points[i - 1], points[i], thickness);
			}
		}

		void ShapeRenderer::DrawArrow(const Float2& begin, const Float2& end, const float thickness, float headLengthRatio, float headWidthRatio)
		{
			const Float2 difference = end - begin;
			const float length = difference.Length();
			if (length == 0.0f)
			{
				return;
			}

			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			ShapeGenerator::GenerateLine(begin, end, thickness, 8, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());
			{
				const float headLength = length * headLengthRatio;
				const float headWidth = thickness * headWidthRatio;
				if (headLength > 0.0f && headWidth > 0.0f)
				{
					const Float2 direction = Float2::Normalize(difference);
					float angle = ::atan2(direction._y, direction._x);
					angle -= Math::kPiOverTwo;

					const Transform2D transform2D{ angle, begin };
					Float2 point0{ -headWidth * 0.5f, length - headLength };
					Float2 point1{ +headWidth * 0.5f, length - headLength };
					Float2 point2{ 0.0f, length };
					point0 = transform2D * point0;
					point1 = transform2D * point1;
					point2 = transform2D * point2;
					ShapeGenerator::GenerateConvexShape({ point0, point1, point2 }, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());
				}
			}

			const uint32 deltaVertexCount = _lowLevelRenderer.GetVertexCount() - vertexOffset;
			for (uint32 i = 0; i < deltaVertexCount; i++)
			{
				_lowLevelRenderer.Vertices()[vertexOffset + i]._info = ComputeVertexInfo(transformIndex, 0);
			}
			const uint32 deltaIndexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, deltaIndexCount, GetClipRect(), _currentMaterialID);

			PushTransformToBuffer(Float2::kOne, 0.0f, Float3::kZero);
		}

		void ShapeRenderer::DrawTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC)
		{
			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			ShapeGenerator::GenerateConvexShape({ pointA, pointB, pointC }, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());

			const uint32 deltaVertexCount = _lowLevelRenderer.GetVertexCount() - vertexOffset;
			for (uint32 i = 0; i < deltaVertexCount; i++)
			{
				_lowLevelRenderer.Vertices()[vertexOffset + i]._info = ComputeVertexInfo(transformIndex, 0);
			}
			const uint32 deltaIndexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, deltaIndexCount, GetClipRect(), _currentMaterialID);

			PushTransformToBuffer(Float2::kOne, 0.0f, Float3::kZero);
		}

		void ShapeRenderer::DrawRectangle(const Float3& position, const Float2& size)
		{
			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			ShapeGenerator::GenerateRectangle(size, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());

			const uint32 deltaVertexCount = _lowLevelRenderer.GetVertexCount() - vertexOffset;
			for (uint32 i = 0; i < deltaVertexCount; i++)
			{
				_lowLevelRenderer.Vertices()[vertexOffset + i]._info = ComputeVertexInfo(transformIndex, 0);
			}

			_lowLevelRenderer.Vertices()[vertexOffset + 0]._texCoord = Float2(_uv0._x, _uv0._y);
			_lowLevelRenderer.Vertices()[vertexOffset + 1]._texCoord = Float2(_uv0._x, _uv1._y);
			_lowLevelRenderer.Vertices()[vertexOffset + 2]._texCoord = Float2(_uv1._x, _uv1._y);
			_lowLevelRenderer.Vertices()[vertexOffset + 3]._texCoord = Float2(_uv1._x, _uv0._y);

			const uint32 deltaIndexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, deltaIndexCount, GetClipRect(), _currentMaterialID);

			PushTransformToBuffer(Float2::kOne, 0.0f, position);
		}

		void ShapeRenderer::DrawCircle(const Float3& position, const float radius)
		{
			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			ShapeGenerator::GenerateCircle(radius, 32, _color, _lowLevelRenderer.Vertices(), _lowLevelRenderer.Indices(), Transform2D::GetIdentity());

			const uint32 deltaVertexCount = _lowLevelRenderer.GetVertexCount() - vertexOffset;
			for (uint32 i = 0; i < deltaVertexCount; i++)
			{
				_lowLevelRenderer.Vertices()[vertexOffset + i]._info = ComputeVertexInfo(transformIndex, 0);
			}
			const uint32 deltaIndexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, deltaIndexCount, GetClipRect(), _currentMaterialID);

			PushTransformToBuffer(Float2::kOne, 0.0f, position);
		}

		const char* ShapeRenderer::GetPixelShaderName() const noexcept
		{
			return "ShapeRendererPS";
		}

		const char* ShapeRenderer::GetPixelShaderString() const noexcept
		{
			return GetDefaultPixelShaderString();
		}

		const char* ShapeRenderer::GetPixelShaderEntryPoint() const noexcept
		{
			return "main_shape";
		}

		uint32 ShapeRenderer::ComputeVertexInfo(uint32 transformIndex, uint8 type) const
		{
			return (type << 30) | (transformIndex & 0x3FFFFFFF);
		}

		void ShapeRenderer::PushTransformToBuffer(const Transform2D& transform2D)
		{
			PushTransformToBuffer(transform2D._scale, transform2D._rotation, Float3(transform2D._translation));
		}

		void ShapeRenderer::PushTransformToBuffer(const Float2& scale, const float rotationAngle, const Float3& position)
		{
			const Float3& transformedPosition = ApplyCoordinateSpace(position);
			SB_Transform sbTransform;
			sbTransform._transformMatrix = Float4x4::SRTMatrix(Float3(scale._x, scale._y, 1.0f), QuaternionF::MakeRotationQuaternion(Float3::kAxisZ, -rotationAngle), transformedPosition);
			_sbTransformData.PushBack(sbTransform);
		}

		void ShapeRenderer::PushTransformToBuffer(const Transform& transform)
		{
			const Float3& transformedPosition = ApplyCoordinateSpace(transform._translation);
			SB_Transform sbTransform;
			sbTransform._transformMatrix = transform.ToMatrix();
			sbTransform._transformMatrix.SetTranslation(transformedPosition);
			_sbTransformData.PushBack(sbTransform);
		}

		void ShapeRenderer::PushManualTransformToBuffer(const Float3& preTranslation, const Float4x4& transformMatrix, const Float3& postTranslation)
		{
			SB_Transform sbTransform;
			sbTransform._transformMatrix.PreTranslate(preTranslation);
			sbTransform._transformMatrix *= transformMatrix;
			sbTransform._transformMatrix.PostTranslate(postTranslation);
			_sbTransformData.PushBack(sbTransform);
		}

		void ShapeRenderer::AddShape_Internal(const Shape& shape)
		{
			const uint32 vertexOffset = _lowLevelRenderer.GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			VS_INPUT_SHAPE v;
			v._info = ComputeVertexInfo(transformIndex, 0);
			auto& vertices = _lowLevelRenderer.Vertices();
			for (const VS_INPUT_SHAPE& vertex : shape._vertices)
			{
				v._color = vertex._color;
				v._position = vertex._position;
				vertices.PushBack(v);
			}

			auto& indices = _lowLevelRenderer.Indices();
			for (const IndexElementType index : shape._indices)
			{
				indices.PushBack(vertexOffset + index);
			}

			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, shape._indices.Size(), GetClipRect(), _currentMaterialID);
		}
	}
}
