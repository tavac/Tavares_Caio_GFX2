#include "ToolBox.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <fbxsdk.h>
#include <vector>
#include "DDSTextureLoader.h"

using namespace DirectX;

namespace wrl = Microsoft::WRL;
class Graphics
{
public:
	struct gVertex
	{
		XMFLOAT4 pos;
		XMFLOAT4 norm;
		XMFLOAT2 uv;
	};
	struct gMesh
	{
		gVertex* verts = nullptr;
		int numVertices = 0;
		int* indices = nullptr;
		int numIndices = 0;
		float scale = 1.0f;
	};
	gMesh* gpMesh = new gMesh();
	struct gConstantBuff
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		// Lights and such
		XMFLOAT4 ambientLight;
	};
	struct gDirLightBuff
	{
		XMFLOAT4 dir;
		XMFLOAT4 color;
	};

	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	HRESULT InitDevice();
	void Render();
	void LoadMesh(std::string fileName, float mesh_scale, gMesh* mesh);
	void ProcessFBXMesh(FbxNode* Node, gMesh* mesh);
	void LoadUVFromFBX(FbxMesh* pMesh, std::vector<XMFLOAT2>* pVecUV);
	void TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode);
private:
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11InputLayout> gInputLayout = nullptr;
	wrl::ComPtr<ID3D11Buffer> gConstantBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gIndexBuffer = nullptr;
	wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
	wrl::ComPtr<ID3D11Buffer> gVertBuffer = nullptr;
	wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
	wrl::ComPtr<ID3DBlob> gBlob = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> shaderRV = nullptr;
	wrl::ComPtr<ID3D11SamplerState> smplrState = nullptr;
public:
#pragma region Lights
	gDirLightBuff gDirectional_1 = {};
#pragma endregion
	// Setting up Matrices
	XMMATRIX globalWorld = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 0.5f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX tmpVw = XMMatrixLookAtLH(Eye, At, Up);
	XMVECTOR detVw = XMMatrixDeterminant(tmpVw);
	XMMATRIX globalView = XMMatrixInverse(&detVw, tmpVw);
	XMMATRIX globalProj = XMMatrixPerspectiveFovLH((90.0f * (3.1415f / 180.0f)), 1280.0f / 720.0f, 0.01f, 100.0f);

	// Win32 + DirectX = gobeldegooks
	enum Axis
	{
		x,y,z,w,u,v
	};
	void CameraMove(XMVECTOR E, Axis axi);
	void CameraRotate(XMVECTOR axis, float angle);
	enum DirLightComs
	{
		DirectionLight_Default,
		DirectionLight_Red,
		DirectionLight_Green,
		DirectionLight_Blue,
		DirectionLight_Off,
	};
	DirLightComs LightState = DirectionLight_Default;
	bool DirectionLightSwitch(DirLightComs cmd);
};