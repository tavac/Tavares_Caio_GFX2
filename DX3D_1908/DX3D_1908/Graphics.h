#pragma once
#if defined(DEBUG)|defined(_DEBUG)
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif
#include "ToolBox.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

namespace wrl = Microsoft::WRL;
class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void Render();
private:
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11InputLayout> gInputLayout = nullptr;
	wrl::ComPtr<ID3D11Buffer> vertBuffer = nullptr;
	wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
	wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
	wrl::ComPtr<ID3DBlob> gBlob = nullptr;
public:
	struct gVertex
	{
		float x, y, z, w;
		//DirectX::XMFLOAT4 Pos;
		//DirectX::XMFLOAT4 Color;
	};
	int numVerts = 0;
	HRESULT InitDevice();
};