#include "ToolBox.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
using namespace DirectX;

namespace wrl = Microsoft::WRL;
class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void Render();
private:
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11InputLayout> gInputLayout = nullptr;
	wrl::ComPtr<ID3D11Buffer> gConstantBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gIndexBuffer = nullptr;
	wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
	wrl::ComPtr<ID3D11Buffer> gVertBuffer = nullptr;
	wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
	wrl::ComPtr<ID3DBlob> gBlob = nullptr;
public:
	struct gVertex
	{
		XMFLOAT4 Pos;
		//DirectX::XMFLOAT4 Norm;
		//DirectX::XMFLOAT4 Color;
	};
	struct gConstBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gView;
		XMMATRIX gProj;
		// Lights and such
	};
	// Setting up Matrices
	XMMATRIX globalWorld = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -3.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX tmpVw = XMMatrixLookAtLH(Eye, At, Up);
	XMVECTOR detVw = XMMatrixDeterminant(tmpVw);
	XMMATRIX globalView = XMMatrixInverse(&detVw, tmpVw);
	XMMATRIX globalProj = XMMatrixPerspectiveFovLH((90.0f*(3.1415f / 180.0f)), 720.0f / 1280.0f, 0.01f, 10.0f);

	int numVerts = 0;
	HRESULT InitDevice();
};