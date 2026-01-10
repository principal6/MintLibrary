#include <MintRendering/Include/GraphicsDevice.h>

#include <d3dcompiler.h>
#include <functional>
#include <typeinfo>

#include <MintPlatform/Include/Window.h>

#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/HashMap.hpp>
#include <MintContainer/Include/Algorithm.hpp>

#include <MintRendering/Include/LowLevelRenderer.hpp>
#include <MintRendering/Include/ShapeRenderer.h>
#include <MintRendering/Include/SpriteRenderer.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


namespace mint
{
	namespace Rendering
	{
#define MINT_COMPARE_SET_OR_RETURN(a, b) if (a == b) { return; } a = b;
#define MINT_CHECK_TWO_STATES(a, aa, b, bb) if ((a == aa) && (b == bb)) { return; } a = aa; b = bb;


#pragma region SafeResourceMapper
		SafeResourceMapper::SafeResourceMapper(GraphicsDevice& graphicsDevice, ID3D11Resource* const resource, const uint32 subresource)
			: _graphicsDevice{ graphicsDevice }
			, _resource{ resource }
			, _subresource{ subresource }
			, _mappedSubresource{}
		{
			if (FAILED(_graphicsDevice._deviceContext->Map(_resource, _subresource, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &_mappedSubresource)))
			{
				_mappedSubresource.pData = nullptr;
				_mappedSubresource.DepthPitch = 0;
				_mappedSubresource.RowPitch = 0;
			}
		}

		SafeResourceMapper::~SafeResourceMapper()
		{
			if (IsValid() == true)
			{
				_graphicsDevice._deviceContext->Unmap(_resource, _subresource);
			}
		}

		bool SafeResourceMapper::IsValid() const noexcept
		{
			return _mappedSubresource.pData != nullptr;
		}

		void SafeResourceMapper::Set(const void* const data, const uint32 size) noexcept
		{
			memcpy(_mappedSubresource.pData, data, size);
		}
#pragma endregion


#pragma region ScopedRenderPhase, RenderPhaseIterator
		ScopedRenderPhase::ScopedRenderPhase(RenderPhaseIterator& renderPhaseIterator)
			: _renderPhaseSequence{ renderPhaseIterator }
			, _renderPhaseLabel{ renderPhaseIterator._renderPhaseLabel }
		{
			MINT_ASSERT(_renderPhaseLabel != RenderPhaseLabel::COUNT, "This condition MUST be assured!!!");

			GraphicsDevice& graphicsDevice = _renderPhaseSequence._graphicsDevice;
			switch (_renderPhaseLabel)
			{
			case RenderPhaseLabel::WorldSpace:
			{
				graphicsDevice.GetFontRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::World);
				graphicsDevice.GetShapeRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::World);
				graphicsDevice.GetSpriteRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::World);
				break;
			}
			case RenderPhaseLabel::ScreenSpace:
			{
				graphicsDevice.GetFontRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::Screen);
				graphicsDevice.GetShapeRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::Screen);
				graphicsDevice.GetSpriteRenderer().SetCoordinateSpace(Rendering::CoordinateSpace::Screen);
				break;
			}
			case RenderPhaseLabel::COUNT:
				break;
			default:
				MINT_NEVER;
				break;
			}

			const GraphicsDevice::DelegateExecuteRenderPhase& delegateExecuteRenderPhase = graphicsDevice.GetDelegateExecuteRenderPhase();
			delegateExecuteRenderPhase.Invoke(*this);
		}
		
		ScopedRenderPhase::~ScopedRenderPhase()
		{
			GraphicsDevice& graphicsDevice = _renderPhaseSequence._graphicsDevice;

			switch (_renderPhaseLabel)
			{
			case RenderPhaseLabel::WorldSpace:
			{
				graphicsDevice.Render();
				break;
			}
			case RenderPhaseLabel::ScreenSpace:
			{
				const float4x4 cachedViewMatrix = graphicsDevice.GetViewMatrix();
				const float4x4 cachedProjectionMatrix = graphicsDevice.GetProjectionMatrix();
				// This view projection matrix is forced!
				graphicsDevice.SetViewProjectionMatrix(Float4x4::kIdentity, graphicsDevice.GetScreenSpace2DProjectionMatrix());

				graphicsDevice.Render();

				graphicsDevice.SetViewProjectionMatrix(cachedViewMatrix, cachedProjectionMatrix);
				break;
			}
			case RenderPhaseLabel::COUNT:
				break;
			default:
				MINT_NEVER;
				break;
			}
		}

		bool ScopedRenderPhase::IsPhase(const RenderPhaseLabel& renderPhaseLabel) const
		{
			return _renderPhaseLabel == renderPhaseLabel;
		}

		RenderPhaseIterator::RenderPhaseIterator(GraphicsDevice& graphicsDevice, const RenderPhaseLabel& renderPhaseLabel, bool isCreatedByThis)
			: _graphicsDevice{ graphicsDevice }
			, _renderPhaseLabel{ renderPhaseLabel }
			, _isCreatedByThis{ isCreatedByThis }
		{
			if (_isCreatedByThis == false)
			{
				_graphicsDevice.BeginRendering();
			}
		}

		RenderPhaseIterator::~RenderPhaseIterator()
		{
			if (_isCreatedByThis == false)
			{
				_graphicsDevice.EndRendering();
			}
		}

		bool RenderPhaseIterator::operator!=(const RenderPhaseIterator& rhs) const
		{
			if (&_graphicsDevice != &rhs._graphicsDevice)
				return true;

			if (_renderPhaseLabel != rhs._renderPhaseLabel)
				return true;

			return false;
		}

		RenderPhaseIterator RenderPhaseIterator::begin()
		{
			return RenderPhaseIterator(_graphicsDevice, RenderPhaseLabel::WorldSpace, true);
		}
		
		RenderPhaseIterator RenderPhaseIterator::end()
		{
			return RenderPhaseIterator(_graphicsDevice, RenderPhaseLabel::COUNT, true);
		}

		bool RenderPhaseIterator::IsValidPhase() const
		{
			return _renderPhaseLabel < RenderPhaseLabel::COUNT;
		}

		ScopedRenderPhase RenderPhaseIterator::GetScopedRenderPhase()
		{
			return ScopedRenderPhase(*this);
		}

		void RenderPhaseIterator::ToNextPhase()
		{
			_renderPhaseLabel = static_cast<RenderPhaseLabel>(static_cast<uint8>(_renderPhaseLabel) + 1);
			
			if (_renderPhaseLabel >= RenderPhaseLabel::COUNT)
			{
				_renderPhaseLabel = RenderPhaseLabel::COUNT;
			}
		}
#pragma endregion


#pragma region GraphicsDevice::StateManager
		GraphicsDevice::StateManager::StateManager(GraphicsDevice& graphicsDevice)
			: _graphicsDevice{ graphicsDevice }
			, _iaRenderingPrimitive{ RenderingPrimitive::INVALID }
			, _iaInputLayout{ nullptr }
			, _rsRasterizerState{ nullptr }
			, _rsViewport{}
			, _rsScissorRectangle{}
			, _vsShader{ nullptr }
			, _gsShader{ nullptr }
			, _psShader{ nullptr }
		{
			__noop;
		}

		void GraphicsDevice::StateManager::SetIAInputLayout(ID3D11InputLayout* const iaInputLayout) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_iaInputLayout, iaInputLayout);

			_graphicsDevice._deviceContext->IASetInputLayout(_iaInputLayout);
		}

		void GraphicsDevice::StateManager::SetIARenderingPrimitive(const RenderingPrimitive iaRenderingPrimitive) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_iaRenderingPrimitive, iaRenderingPrimitive);

			switch (iaRenderingPrimitive)
			{
			case RenderingPrimitive::INVALID:
				MINT_NEVER;
				break;
			case RenderingPrimitive::LineList:
				_graphicsDevice._deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			case RenderingPrimitive::TriangleList:
				_graphicsDevice._deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			default:
				break;
			}
		}

		void GraphicsDevice::StateManager::SetIAVertexBuffers(const int32 bindingStartSlot, const uint32 bufferCount, ID3D11Buffer* const* const buffers, const uint32* const strides, const uint32* const offsets) noexcept
		{
			_graphicsDevice._deviceContext->IASetVertexBuffers(bindingStartSlot, bufferCount, buffers, strides, offsets);
		}

		void GraphicsDevice::StateManager::SetIAIndexBuffer(ID3D11Buffer* const buffer, const DXGI_FORMAT format, const uint32 offset) noexcept
		{
			_graphicsDevice._deviceContext->IASetIndexBuffer(buffer, format, offset);
		}

		void GraphicsDevice::StateManager::SetRSRasterizerState(ID3D11RasterizerState* const rsRasterizerState) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsRasterizerState, rsRasterizerState);

			_graphicsDevice._deviceContext->RSSetState(_rsRasterizerState);
		}

		void GraphicsDevice::StateManager::SetRSViewport(const D3D11_VIEWPORT rsViewport) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsViewport, rsViewport);

			_graphicsDevice._deviceContext->RSSetViewports(1, &rsViewport);
		}

		void GraphicsDevice::StateManager::SetRSScissorRectangle(const D3D11_RECT rsScissorRectangle) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_rsScissorRectangle, rsScissorRectangle);

			_graphicsDevice._deviceContext->RSSetScissorRects(1, &rsScissorRectangle);
		}

		void GraphicsDevice::StateManager::SetVSShader(ID3D11VertexShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_vsShader, shader);

			_graphicsDevice._deviceContext->VSSetShader(shader, nullptr, 0);
		}

		void GraphicsDevice::StateManager::SetGSShader(ID3D11GeometryShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_gsShader, shader);

			_graphicsDevice._deviceContext->GSSetShader(shader, nullptr, 0);
		}

		void GraphicsDevice::StateManager::SetPSShader(ID3D11PixelShader* const shader) noexcept
		{
			MINT_COMPARE_SET_OR_RETURN(_psShader, shader);

			_graphicsDevice._deviceContext->PSSetShader(shader, nullptr, 0);
		}

		void GraphicsDevice::StateManager::SetShaderResources(GraphicsShaderType graphicsShaderType, const GraphicsResource* resource, uint32 bindingSlot) noexcept
		{
			Vector<GraphicsObjectID>* shaderResourceIDs = nullptr;
			if (graphicsShaderType == GraphicsShaderType::VertexShader)
			{
				shaderResourceIDs = &_vsShaderResourceIDs;
			}
			else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
			{
				shaderResourceIDs = &_gsShaderResourceIDs;
			}
			else if (graphicsShaderType == GraphicsShaderType::PixelShader)
			{
				shaderResourceIDs = &_psShaderResourceIDs;
			}
			MINT_ASSERT(shaderResourceIDs != nullptr, "로직 상 반드시 보장되어야 합니다!");

			if (resource == nullptr)
			{
				MINT_ASSERT(bindingSlot < shaderResourceIDs->Size(), "반드시 호출자에서 보장해 주세요.");

				(*shaderResourceIDs)[bindingSlot] = GraphicsObjectID();

				ID3D11ShaderResourceView* const nullSRVs[1]{ nullptr };
				if (graphicsShaderType == GraphicsShaderType::VertexShader)
				{
					_graphicsDevice._deviceContext->VSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
				{
					_graphicsDevice._deviceContext->GSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				else if (graphicsShaderType == GraphicsShaderType::PixelShader)
				{
					_graphicsDevice._deviceContext->PSSetShaderResources(bindingSlot, 1, nullSRVs);
				}
				return;
			}

			if (bindingSlot == kUint32Max)
			{
				bindingSlot = resource->GetRegisterIndex();
			}

			if (shaderResourceIDs->Size() <= bindingSlot)
			{
				shaderResourceIDs->Resize(bindingSlot + 1);
			}
			if (resource->NeedsToBind() == true)
			{
				(*shaderResourceIDs)[bindingSlot] = GraphicsObjectID();
			}
			MINT_COMPARE_SET_OR_RETURN((*shaderResourceIDs)[bindingSlot], resource->GetID());

			if (graphicsShaderType == GraphicsShaderType::VertexShader)
			{
				_graphicsDevice._deviceContext->VSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
			else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
			{
				_graphicsDevice._deviceContext->GSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
			else if (graphicsShaderType == GraphicsShaderType::PixelShader)
			{
				_graphicsDevice._deviceContext->PSSetShaderResources(bindingSlot, 1, resource->GetResourceView());
			}
		}

		void GraphicsDevice::StateManager::SetConstantBuffers(GraphicsShaderType graphicsShaderType, const GraphicsResource* constantBuffer, uint32 bindingSlot)
		{
			Vector<GraphicsObjectID>* constantBufferIDs = nullptr;
			if (graphicsShaderType == GraphicsShaderType::VertexShader)
			{
				constantBufferIDs = &_vsConstantBufferIDs;
			}
			else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
			{
				constantBufferIDs = &_gsConstantBufferIDs;
			}
			else if (graphicsShaderType == GraphicsShaderType::PixelShader)
			{
				constantBufferIDs = &_psConstantBufferIDs;
			}
			MINT_ASSERT(constantBufferIDs != nullptr, "로직 상 반드시 보장되어야 합니다!");

			if (constantBuffer == nullptr)
			{
				MINT_ASSERT(bindingSlot < constantBufferIDs->Size(), "반드시 호출자에서 보장해 주세요.");

				(*constantBufferIDs)[bindingSlot] = GraphicsObjectID();

				ID3D11Buffer* const nullBuffers[1]{ nullptr };
				if (graphicsShaderType == GraphicsShaderType::VertexShader)
				{
					_graphicsDevice._deviceContext->VSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
				{
					_graphicsDevice._deviceContext->GSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				else if (graphicsShaderType == GraphicsShaderType::PixelShader)
				{
					_graphicsDevice._deviceContext->PSSetConstantBuffers(bindingSlot, 1, nullBuffers);
				}
				return;
			}

			if (bindingSlot == kUint32Max)
			{
				bindingSlot = constantBuffer->GetRegisterIndex();
			}

			if (constantBufferIDs->Size() <= bindingSlot)
			{
				constantBufferIDs->Resize(bindingSlot + 1);
			}
			if (constantBuffer->NeedsToBind() == true)
			{
				(*constantBufferIDs)[bindingSlot] = GraphicsObjectID();
			}
			MINT_COMPARE_SET_OR_RETURN((*constantBufferIDs)[bindingSlot], constantBuffer->GetID());

			if (graphicsShaderType == GraphicsShaderType::VertexShader)
			{
				_graphicsDevice._deviceContext->VSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
			else if (graphicsShaderType == GraphicsShaderType::GeometryShader)
			{
				_graphicsDevice._deviceContext->GSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
			else if (graphicsShaderType == GraphicsShaderType::PixelShader)
			{
				_graphicsDevice._deviceContext->PSSetConstantBuffers(bindingSlot, 1, constantBuffer->GetBuffer());
			}
		}
#pragma endregion


#pragma region GraphicsDevice
		GraphicsDevice& GraphicsDevice::GetInvalidInstance()
		{
			static Window invalidWindow;
			static GraphicsDevice invalidInstance{ invalidWindow, false };
			return invalidInstance;
		}

		GraphicsDevice::GraphicsDevice(Window& window, bool usesMSAA)
			: _window{ window }
			, _usesMSAA{ usesMSAA }
			, _clearColor{ 0.875f, 0.875f, 0.875f, 1.0f }
			, _currentRasterizerFor3D{ nullptr }
			, _fullScreenViewport{}
			, _shaderPool{ *this, &_shaderHeaderMemory, ShaderVersion::v_5_0 }
			, _shaderPipelinePool{ *this }
			, _resourcePool{ *this }
			, _materialPool{ *this }
			, _stateManager{ *this }
			, _isInRenderingScope{ false }
		{
			__noop;
		}

		GraphicsDevice::~GraphicsDevice()
		{
			Terminate();
		}

		bool GraphicsDevice::Initialize()
		{
			_clearColor = _window.GetBackgroundColor();
			_cachedWindowSize = _window.GetSize();

			if (CreateDxDevice() == false)
			{
				return false;
			}

			CreateDefaultShaderPipelines();

			_lowLevelRendererForShapeAndFont.Assign(MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>));
			_fontRenderer.Assign(MINT_NEW(FontRenderer, *this, *_lowLevelRendererForShapeAndFont, _sbTransformDataForShapeAndFont));
			_fontRenderer->Initialize();
			_shapeRenderer.Assign(MINT_NEW(ShapeRenderer, *this, *_lowLevelRendererForShapeAndFont, _sbTransformDataForShapeAndFont));
			_shapeRenderer->Initialize();

			_lowLevelRendererForSprite.Assign(MINT_NEW(LowLevelRenderer<VS_INPUT_SHAPE>));
			_spriteRenderer.Assign(MINT_NEW(SpriteRenderer, *this, *_lowLevelRendererForSprite, _sbTransformDataForSprite, 1, ByteColor(0, 0, 0, 0)));
			_spriteRenderer->Initialize();

			if (LoadFontData() == false)
			{
				return false;
			}
			return true;
		}

		void GraphicsDevice::Terminate()
		{
			if (_shaderPipelineTriangleID.IsValid())
			{
				_shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineTriangleID);
			}

			if (_shaderPipelineTriangleDrawNormalsID.IsValid())
			{
				_shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineTriangleDrawNormalsID);
			}

			if (_shaderPipelineTriangleDrawEdgesID.IsValid())
			{
				_shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineTriangleDrawEdgesID);
			}

			if (_shaderPipelineLineID.IsValid())
			{
				_shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineLineID);
			}
		}

		void GraphicsDevice::UpdateScreenSize()
		{
			if (_window.GetSize() == _cachedWindowSize)
			{
				return;
			}

			_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			_deviceContext->OMSetDepthStencilState(nullptr, 0);

			_backBufferRtv.Reset();

			if (FAILED(_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0)))
			{
				return;
			}

			InitializeBackBuffer();
			InitializeDepthStencilBufferAndView(_window.GetSize());
			InitializeFullScreenData(_window.GetSize());
			SetViewProjectionMatrix(Float4x4::kIdentity, GetScreenSpace2DProjectionMatrix());

			SetDefaultRenderTargetsAndDepthStencil();

			_cachedWindowSize = _window.GetSize();
		}

		bool GraphicsDevice::CreateDxDevice()
		{
			const Int2& windowSize = _window.GetSize();

			if (CreateSwapChain(windowSize, _window.GetHandle()) == false)
			{
				return false;
			}

			if (InitializeBackBuffer() == false)
			{
				return false;
			}

			if (InitializeDepthStencilBufferAndView(windowSize) == false)
			{
				return false;
			}

			if (InitializeDepthStencilStates() == false)
			{
				return false;
			}

			InitializeFullScreenData(windowSize);
			InitializeDxShaderHeaderMemory();
			InitializeSamplerStates();
			InitializeBlendStates();

			// Rasterizer states and viewport
			{
				D3D11_RASTERIZER_DESC rasterizerDescriptor;
				rasterizerDescriptor.AntialiasedLineEnable = TRUE;
				rasterizerDescriptor.DepthBias = 0;
				rasterizerDescriptor.DepthBiasClamp = 0.0f;
				rasterizerDescriptor.DepthClipEnable = TRUE;
				rasterizerDescriptor.FrontCounterClockwise = TRUE; // 오른손 좌표계
				rasterizerDescriptor.MultisampleEnable = TRUE;
				rasterizerDescriptor.ScissorEnable = FALSE;
				rasterizerDescriptor.SlopeScaledDepthBias = 0.0f;

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateWireFrameNoCulling.ReleaseAndGetAddressOf());

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateWireFrameCullBack.ReleaseAndGetAddressOf());

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateSolidCullBack.ReleaseAndGetAddressOf());

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateSolidCullFront.ReleaseAndGetAddressOf());

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateSolidCullNone.ReleaseAndGetAddressOf());

				rasterizerDescriptor.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
				rasterizerDescriptor.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
				rasterizerDescriptor.ScissorEnable = TRUE;
				_device->CreateRasterizerState(&rasterizerDescriptor, _rasterizerStateScissorRectangles.ReleaseAndGetAddressOf());

				_currentRasterizerFor3D = _rasterizerStateSolidCullBack.Get();
			}

			SetDefaultRenderTargetsAndDepthStencil();
			return true;
		}

		void GraphicsDevice::CreateDefaultShaderPipelines()
		{
			Rendering::GraphicsObjectID inputLayoutDefaultID;
			Rendering::GraphicsObjectID vsDefaultID;
			Rendering::GraphicsObjectID gsNormalID;
			Rendering::GraphicsObjectID gsTriangleEdgeID;
			Rendering::GraphicsObjectID psDefaultID;
			Rendering::GraphicsObjectID psTexCoordAsColorID;
			Rendering::GraphicsObjectID psColorID;
			vsDefaultID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "VsDefault.hlsl", "main", GraphicsShaderType::VertexShader, Path::MakeIncludeAssetPath("HlslBinary/"));

			const Language::TypeMetaData<Language::CppHlsl::TypeCustomData>& vsInputTypeMetaData = _cppHlslStreamData.GetTypeMetaData(typeid(VS_INPUT));
			inputLayoutDefaultID = _shaderPool.CreateInputLayout(vsDefaultID, vsInputTypeMetaData);

			gsNormalID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsNormal.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			gsTriangleEdgeID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "GsTriangleEdge.hlsl", "main", GraphicsShaderType::GeometryShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			psDefaultID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsDefault.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			psTexCoordAsColorID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsTexCoordAsColor.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			psColorID = _shaderPool.CreateShader(Path::MakeIncludeAssetPath("Hlsl/"), "PsColor.hlsl", "main", GraphicsShaderType::PixelShader, Path::MakeIncludeAssetPath("HlslBinary/"));
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
				shaderPipelineDesc._vertexShaderID = vsDefaultID;
				shaderPipelineDesc._pixelShaderID = psDefaultID;
				_shaderPipelineTriangleID = _shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
				shaderPipelineDesc._vertexShaderID = vsDefaultID;
				shaderPipelineDesc._geometryShaderID = gsNormalID;
				shaderPipelineDesc._pixelShaderID = psTexCoordAsColorID;
				_shaderPipelineTriangleDrawNormalsID = _shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
				shaderPipelineDesc._vertexShaderID = vsDefaultID;
				shaderPipelineDesc._geometryShaderID = gsTriangleEdgeID;
				shaderPipelineDesc._pixelShaderID = psTexCoordAsColorID;
				_shaderPipelineTriangleDrawEdgesID = _shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutDefaultID;
				shaderPipelineDesc._vertexShaderID = vsDefaultID;
				shaderPipelineDesc._pixelShaderID = psColorID;
				_shaderPipelineLineID = _shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}
		}

		bool GraphicsDevice::LoadFontData()
		{
			static const Path kDefaultFontPath = Path::MakeAssetPath("noto_sans_kr_medium");
			FontLoader fontLoader;
			if (FontLoader::ExistsFont(kDefaultFontPath) == false)
			{
				fontLoader.PushGlyphRange(GlyphRange(0, 0x33DD));
				fontLoader.PushGlyphRange(GlyphRange(L'가', L'힣'));
				if (fontLoader.BakeFontData(kDefaultFontPath, kDefaultFontSize, kDefaultFontPath, 2048, 1, 1) == false)
				{
					MINT_LOG_ERROR("폰트 데이터를 Bake 하는 데 실패했습니다!");
					return false;
				}
			}

			if (fontLoader.LoadFont(kDefaultFontPath, *this) == false)
			{
				MINT_LOG_ERROR("폰트 데이터를 로드하지 못했습니다!");
				return false;
			}

			_fontRenderer->InitializeFontData(fontLoader.GetFontData());

			return true;
		}

		bool GraphicsDevice::CreateSwapChain(const Int2& windowSize, const HWND windowHandle)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDescriptor{};
			swapChainDescriptor.BufferCount = 1;
			swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDescriptor.BufferDesc.Width = static_cast<UINT>(windowSize._x);
			swapChainDescriptor.BufferDesc.Height = static_cast<UINT>(windowSize._y);
			swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
			swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
			swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDescriptor.Flags = 0;
			swapChainDescriptor.OutputWindow = windowHandle;
			swapChainDescriptor.SampleDesc.Count = (_usesMSAA ? 4 : 1);
			swapChainDescriptor.SampleDesc.Quality = 0;
			swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
			swapChainDescriptor.Windowed = TRUE;

			if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
				&swapChainDescriptor, _swapChain.ReleaseAndGetAddressOf(), _device.ReleaseAndGetAddressOf(), nullptr, _deviceContext.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("SwapChain 생성에 실패했습니다!");
				return false;
			}
			return true;
		}

		bool GraphicsDevice::InitializeBackBuffer()
		{
			ComPtr<ID3D11Texture2D> backBuffer;
			if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.ReleaseAndGetAddressOf()))))
			{
				MINT_LOG_ERROR("BackBuffer 를 얻어오는 데 실패했습니다!");
				return false;
			}

			if (FAILED(_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _backBufferRtv.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("BackBuffer 초기화에 실패했습니다!");
				return false;
			}
			return true;
		}

		bool GraphicsDevice::InitializeDepthStencilBufferAndView(const Int2& windowSize)
		{
			D3D11_TEXTURE2D_DESC depthStencilBufferDescriptor;
			depthStencilBufferDescriptor.Width = static_cast<UINT>(windowSize._x);
			depthStencilBufferDescriptor.Height = static_cast<UINT>(windowSize._y);
			depthStencilBufferDescriptor.MipLevels = 1;
			depthStencilBufferDescriptor.ArraySize = 1;
			depthStencilBufferDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilBufferDescriptor.SampleDesc.Count = (_usesMSAA ? 4 : 1);
			depthStencilBufferDescriptor.SampleDesc.Quality = 0;
			depthStencilBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
			depthStencilBufferDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthStencilBufferDescriptor.CPUAccessFlags = 0;
			depthStencilBufferDescriptor.MiscFlags = 0;

			if (FAILED(_device->CreateTexture2D(&depthStencilBufferDescriptor, nullptr, _depthStencilBuffer.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("DepthStencil 텍스쳐 생성에 실패했습니다.");
				return false;
			}

			if (FAILED(_device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("DepthStencil 뷰 생성에 실패했습니다.");
				return false;
			}

			return true;
		}

		bool GraphicsDevice::InitializeDepthStencilStates()
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDescriptor;
			depthStencilDescriptor.DepthEnable = TRUE;
			depthStencilDescriptor.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
			depthStencilDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDescriptor.StencilEnable = FALSE;
			if (FAILED(_device->CreateDepthStencilState(&depthStencilDescriptor, _depthStencilStateLessEqual.ReleaseAndGetAddressOf())))
			{
				MINT_LOG_ERROR("DepthStencil State 생성에 실패했습니다.");
				return false;
			}

			return true;
		}

		void GraphicsDevice::InitializeDxShaderHeaderMemory()
		{
			const Int2 windowSize = _window.GetSize();

			// Stream data
			{
				_cppHlslStreamData.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslStreamData.h"));
				_cppHlslStreamData.GenerateHlslString(Language::CppHlsl::CppHlslFileType::StreamData);
				_shaderHeaderMemory.PushHeader("ShaderStructDefinitions", _cppHlslStreamData.GetHlslString());
			}

			using namespace Language;

			// Constant buffers
			{
				_cppHlslConstantBuffers.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslConstantBuffers.h"));
				_cppHlslConstantBuffers.GenerateHlslString(Language::CppHlsl::CppHlslFileType::ConstantBuffers);
				_shaderHeaderMemory.PushHeader("ShaderConstantBuffers", _cppHlslConstantBuffers.GetHlslString());

				{
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.GetTypeMetaData(typeid(_cbViewData));
					_cbViewID = _resourcePool.AddConstantBuffer(&_cbViewData, sizeof(_cbViewData), typeMetaData._customData.GetRegisterIndex());

					GraphicsResource& cbView = _resourcePool.GetResource(_cbViewID);
					cbView.BindToShader(GraphicsShaderType::VertexShader, cbView.GetRegisterIndex());
					cbView.BindToShader(GraphicsShaderType::GeometryShader, cbView.GetRegisterIndex());
					cbView.BindToShader(GraphicsShaderType::PixelShader, cbView.GetRegisterIndex());
				}

				{
					CB_Transform cbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.GetTypeMetaData(typeid(cbTransformData));
					_cbTransformID = _resourcePool.AddConstantBuffer(&cbTransformData, sizeof(cbTransformData), typeMetaData._customData.GetRegisterIndex());
				}

				{
					CB_MaterialData cbMaterialData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslConstantBuffers.GetTypeMetaData(typeid(cbMaterialData));
					_cbMaterialID = _resourcePool.AddConstantBuffer(&cbMaterialData, sizeof(cbMaterialData), typeMetaData._customData.GetRegisterIndex());
				}

				SetViewProjectionMatrix(Float4x4::kIdentity, GetScreenSpace2DProjectionMatrix());
			}

			// Structured buffers
			{
				_cppHlslStructuredBuffers.ParseCppHlslFile(Path::MakeIncludeAssetPath("CppHlsl/CppHlslStructuredBuffers.h"));
				_cppHlslStructuredBuffers.GenerateHlslString(Language::CppHlsl::CppHlslFileType::StructuredBuffers);
				_shaderHeaderMemory.PushHeader("ShaderStructuredBufferDefinitions", _cppHlslStructuredBuffers.GetHlslString());

				{
					SB_Transform sbTransformData;
					const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _cppHlslStructuredBuffers.GetTypeMetaData(typeid(sbTransformData));
					_sbTransformID = _resourcePool.AddStructuredBuffer(&sbTransformData, sizeof(sbTransformData), 1, typeMetaData._customData.GetRegisterIndex());
				}
			}
		}

		void GraphicsDevice::InitializeSamplerStates()
		{
			{
				D3D11_SAMPLER_DESC samplerDescriptor{};
				samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.MipLODBias = 0.0f;
				samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
				samplerDescriptor.MinLOD = 0.0f;
				samplerDescriptor.MaxLOD = 0.0f;
				_device->CreateSamplerState(&samplerDescriptor, _linearSamplerState.ReleaseAndGetAddressOf());
				_deviceContext->PSSetSamplers(0, 1, _linearSamplerState.GetAddressOf());
			}
			{
				D3D11_SAMPLER_DESC samplerDescriptor{};
				samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
				samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
				samplerDescriptor.MipLODBias = 0.0f;
				samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
				samplerDescriptor.MinLOD = 0.0f;
				samplerDescriptor.MaxLOD = 0.0f;
				_device->CreateSamplerState(&samplerDescriptor, _pointSamplerState.ReleaseAndGetAddressOf());
				_deviceContext->PSSetSamplers(1, 1, _pointSamplerState.GetAddressOf());
			}
		}

		void GraphicsDevice::InitializeBlendStates()
		{
			{
				D3D11_BLEND_DESC blendDescriptor{};
				blendDescriptor.AlphaToCoverageEnable = false;
				blendDescriptor.RenderTarget[0].BlendEnable = true;
				blendDescriptor.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
				blendDescriptor.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				blendDescriptor.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				blendDescriptor.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
				blendDescriptor.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
				blendDescriptor.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
				blendDescriptor.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
				_device->CreateBlendState(&blendDescriptor, _blendState.ReleaseAndGetAddressOf());

				const float kBlendFactor[4]{ 0, 0, 0, 0 };
				_deviceContext->OMSetBlendState(_blendState.Get(), kBlendFactor, 0xFFFFFFFF);
			}
		}

		void GraphicsDevice::InitializeFullScreenData(const Int2& windowSize)
		{
			_fullScreenViewport.Width = static_cast<FLOAT>(windowSize._x);
			_fullScreenViewport.Height = static_cast<FLOAT>(windowSize._y);
			_fullScreenViewport.MinDepth = 0.0f;
			_fullScreenViewport.MaxDepth = 1.0f;
			_fullScreenViewport.TopLeftX = 0.0f;
			_fullScreenViewport.TopLeftY = 0.0f;

			_fullScreenClipRect.Left(0);
			_fullScreenClipRect.Right(static_cast<float>(windowSize._x));
			_fullScreenClipRect.Top(0);
			_fullScreenClipRect.Bottom(static_cast<float>(windowSize._y));
		}

		void GraphicsDevice::SetDefaultRenderTargetsAndDepthStencil()
		{
			_deviceContext->OMSetRenderTargets(1, _backBufferRtv.GetAddressOf(), _depthStencilView.Get());
			_deviceContext->OMSetDepthStencilState(_depthStencilStateLessEqual.Get(), 0);
		}

		RenderPhaseIterator GraphicsDevice::IterateRenderPhases() noexcept
		{
			return RenderPhaseIterator(*this, RenderPhaseLabel::WorldSpace, false);
		}

		void GraphicsDevice::Draw(const uint32 vertexCount, const uint32 vertexOffset) noexcept
		{
			MINT_ASSERT(_isInRenderingScope == true, "This function must be called in ScopedRenderPhase!");
			_deviceContext->Draw(vertexCount, vertexOffset);
		}

		void GraphicsDevice::DrawIndexed(const uint32 indexCount, const uint32 indexOffset, const uint32 vertexOffset) noexcept
		{
			MINT_ASSERT(_isInRenderingScope == true, "This function must be called in ScopedRenderPhase!");
			_deviceContext->DrawIndexed(indexCount, indexOffset, vertexOffset);
		}

		void GraphicsDevice::Render() noexcept
		{
			MINT_ASSERT(_isInRenderingScope == true, "This function must be called in ScopedRenderPhase!");
			GraphicsResourcePool& resourcePool = GetResourcePool();
			GraphicsResource& sbTransformBuffer = resourcePool.GetResource(GetCommonSBTransformID());
			sbTransformBuffer.BindToShader(GraphicsShaderType::VertexShader, sbTransformBuffer.GetRegisterIndex());

			if (_lowLevelRendererForSprite->IsRenderable() == true)
			{
				if (_sbTransformDataForSprite.IsEmpty() == false)
				{
					sbTransformBuffer.UpdateBuffer(&_sbTransformDataForSprite[0], _sbTransformDataForSprite.Size());
				}

				_lowLevelRendererForSprite->ExecuteRenderCommands(*this);
				_lowLevelRendererForSprite->Flush();
				_sbTransformDataForSprite.Clear();
			}

			if (_lowLevelRendererForShapeAndFont->IsRenderable() == true)
			{
				if (_sbTransformDataForShapeAndFont.IsEmpty() == false)
				{
					sbTransformBuffer.UpdateBuffer(&_sbTransformDataForShapeAndFont[0], _sbTransformDataForShapeAndFont.Size());
				}

				_lowLevelRendererForShapeAndFont->ExecuteRenderCommands(*this);
				_lowLevelRendererForShapeAndFont->Flush();
				_sbTransformDataForShapeAndFont.Clear();
			}
		}
		
		void GraphicsDevice::SetDelegateExecuteRenderPhase(const DelegateExecuteRenderPhase& delegate) noexcept
		{
			_renderPhaseDelegate = delegate;
		}

		const GraphicsDevice::DelegateExecuteRenderPhase& GraphicsDevice::GetDelegateExecuteRenderPhase() const noexcept
		{
			return _renderPhaseDelegate;
		}

		void GraphicsDevice::BeginRendering()
		{
			MINT_ASSERT(_isInRenderingScope == false, "BeginRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 EndRendering() 을 호출해 주세요!");
			MINT_ASSERT(_fontRenderer->IsEmpty(), "BeginRendering() 호출 전에 채우면 안 됩니다!");
			MINT_ASSERT(_shapeRenderer->IsEmpty(), "BeginRendering() 호출 전에 채우면 안 됩니다!");
			MINT_ASSERT(_spriteRenderer->IsEmpty(), "BeginRendering() 호출 전에 채우면 안 됩니다!");

			if (_window.IsResized())
			{
				UpdateScreenSize();
			}

			_deviceContext->ClearRenderTargetView(_backBufferRtv.Get(), _clearColor);
			_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			UseFullScreenViewport();

			_isInRenderingScope = true;
		}

		void GraphicsDevice::EndRendering()
		{
			MINT_ASSERT(_isInRenderingScope == true, "BeginRendering() 을 두 번 연달아 호출할 수 없습니다. 먼저 EndRendering() 을 호출해 주세요!");
			MINT_ASSERT(_fontRenderer->IsEmpty(), "EndRendering() 호출 전에 Flush() 해야 합니다!");
			MINT_ASSERT(_shapeRenderer->IsEmpty(), "EndRendering() 호출 전에 Flush() 해야 합니다!");
			MINT_ASSERT(_spriteRenderer->IsEmpty(), "EndRendering() 호출 전에 Flush() 해야 합니다!");

			_swapChain->Present(0, 0);

			_isInRenderingScope = false;
		}

		void GraphicsDevice::UseScissorRectangles() noexcept
		{
			_stateManager.SetRSRasterizerState(_rasterizerStateScissorRectangles.Get());
			_stateManager.SetRSViewport(_fullScreenViewport);
		}

		void GraphicsDevice::UseFullScreenViewport() noexcept
		{
			_stateManager.SetRSRasterizerState(_currentRasterizerFor3D);
			_stateManager.SetRSViewport(_fullScreenViewport);
		}

		void GraphicsDevice::UseWireFrameNoCullingRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateWireFrameNoCulling.Get();
		}

		void GraphicsDevice::UseWireFrameCullBackRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateWireFrameCullBack.Get();
		}

		void GraphicsDevice::UseSolidCullBackRasterizer() noexcept
		{
			_currentRasterizerFor3D = _rasterizerStateSolidCullBack.Get();
		}

		void GraphicsDevice::SetSolidCullBackRasterizer() noexcept
		{
			_stateManager.SetRSRasterizerState(_rasterizerStateSolidCullBack.Get());
		}

		void GraphicsDevice::SetSolidCullFrontRasterizer() noexcept
		{
			_stateManager.SetRSRasterizerState(_rasterizerStateSolidCullFront.Get());
		}

		void GraphicsDevice::SetSolidCullNoneRasterizer() noexcept
		{
			_stateManager.SetRSRasterizerState(_rasterizerStateSolidCullNone.Get());
		}

		const Rect& GraphicsDevice::GetFullScreenClipRect() const noexcept
		{
			return _fullScreenClipRect;
		}

		ShaderPool& GraphicsDevice::GetShaderPool() noexcept
		{
			return _shaderPool;
		}

		ShaderPipelinePool& GraphicsDevice::GetShaderPipelinePool() noexcept
		{
			return _shaderPipelinePool;
		}

		GraphicsResourcePool& GraphicsDevice::GetResourcePool() noexcept
		{
			return _resourcePool;
		}

		MaterialPool& GraphicsDevice::GetMaterialPool() noexcept
		{
			return _materialPool;
		}

		FontRenderer& GraphicsDevice::GetFontRenderer() noexcept
		{
			return *_fontRenderer;
		}
		
		ShapeRenderer& GraphicsDevice::GetShapeRenderer() noexcept
		{
			return *_shapeRenderer;
		}

		SpriteRenderer& GraphicsDevice::GetSpriteRenderer() noexcept
		{
			return *_spriteRenderer;
		}

		Float4x4 GraphicsDevice::GetScreenSpace2DProjectionMatrix() const noexcept
		{
			const Float2 windowSize{ GetWindowSize() };
			return Float4x4::ProjectionMatrix2DOffCenter(0.0f, windowSize._x, 0.0f, windowSize._y);
		}

		void GraphicsDevice::SetViewProjectionMatrix(const Float4x4& viewMatrix, const Float4x4& projectionMatrix) noexcept
		{
			_cbViewData._cbViewMatrix = viewMatrix;
			_cbViewData._cbProjectionMatrix = projectionMatrix;
			_cbViewData._cbViewProjectionMatrix = _cbViewData._cbProjectionMatrix * _cbViewData._cbViewMatrix;

			GraphicsResource& cbView = _resourcePool.GetResource(_cbViewID);
			cbView.UpdateBuffer(&_cbViewData, 1);
		}

		const Float4x4& GraphicsDevice::GetViewMatrix() const noexcept
		{
			return _cbViewData._cbViewMatrix;
		}

		const Float4x4& GraphicsDevice::GetProjectionMatrix() const noexcept
		{
			return _cbViewData._cbProjectionMatrix;
		}

		const Float4x4& GraphicsDevice::GetViewProjectionMatrix() const noexcept
		{
			return _cbViewData._cbViewProjectionMatrix;
		}

		Window& GraphicsDevice::AccessWindow() noexcept
		{
			return _window;
		}

		const Window& GraphicsDevice::GetWindow() const noexcept
		{
			return _window;
		}
#pragma endregion
	}
}
