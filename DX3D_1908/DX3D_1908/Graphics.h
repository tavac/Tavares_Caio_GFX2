#include "ToolBox.h"

using namespace DirectX;

namespace wrl = Microsoft::WRL;
class Graphics
{
public:
	Timer* gTimer = new Timer();
	float PointLight_A = 1.0f;
	struct gVertex
	{
		XMFLOAT4 pos;
		XMFLOAT4 norm;
		XMFLOAT2 uv;
		XMFLOAT4 color;
	};
	struct gMesh
	{
		gVertex* verts = nullptr;
		int numVertices = 0;
		int* indices = nullptr;
		int numIndices = 0;
		float scale = 1.0f;
	};
	gMesh* gppMesh = nullptr;
	int numOfMeshs;
	struct gConstantBuff
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT4 ambientLight;
		float dTime;
	};
	struct gDirLightBuff
	{
		XMFLOAT4 dir[2];
		XMFLOAT4 color[2];
	};
	struct gPntLightBuff
	{
		XMFLOAT4 pos;
		XMFLOAT4 color;
	};
	float SpotLightWidth = 0.9f;
	struct gSptLightBuff
	{
		XMFLOAT4 pos;
		XMFLOAT4 coneDir;
		XMFLOAT4 color;
		XMFLOAT4 coneWidth_R;
	};

	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	HRESULT InitDevice();
	void Render();
	void LoadMesh(std::string fileName, float mesh_scale, gMesh** meshArr, UINT meshIndex);
	void ProcessFBXMesh(FbxNode* Node, gMesh* mesh); // Join with ProcessOBJMesh to make a template type mesh loader
	void LoadUVFromFBX(FbxMesh* pMesh, std::vector<XMFLOAT2>* pVecUV);
	void TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode); // This requires the model to have been made with a .dds file
	void ProcessOBJMesh(_OBJ_VERT_ ov[], int size); // Join with ProcessFBXMesh
	////
	void CleanFrameBuffers(XMVECTORF32 DXCOLOR = Colors::Silver);
	void UpdateConstantBuffer(float cbTranslate[3], float cbRotate[3]);
	enum LightType { Directional,Point,Spot };
	void AppendLight(LightType lType, XMVECTOR pos, XMVECTOR dir, XMFLOAT4A color);
private:
#pragma region Hointer Pell
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> gDsv = nullptr;
	wrl::ComPtr<ID3D11Texture2D> gDepthStencil = nullptr;
	wrl::ComPtr<ID3D11Buffer> gConstantBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gDLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gPLightBuffer = nullptr; 
	wrl::ComPtr<ID3D11Buffer> gSLightBuffer = nullptr; 
	wrl::ComPtr<ID3D11Buffer> gIndexBuffer = nullptr; //
	wrl::ComPtr<ID3D11InputLayout> gInputLayout = nullptr;
	wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
	wrl::ComPtr<ID3D11Buffer> gVertBuffer = nullptr; //
	wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
	wrl::ComPtr<ID3DBlob> gBlob = nullptr;
	wrl::ComPtr<ID3D11ShaderResourceView> shaderRV = nullptr; //
	wrl::ComPtr<ID3D11SamplerState> smplrState = nullptr;

#pragma endregion
public:
#pragma region Lights
	gDirLightBuff gDirectional = {};
	gPntLightBuff gPointLight = {};
	gSptLightBuff gSpotLight = {};
#pragma endregion

#pragma region WorldViewProjection
	XMMATRIX globalWorld = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -10.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX globalView = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX Camera = XMMatrixInverse(nullptr, globalView);
	float FoV_angle = 90.0f;
	float nearPlane = 0.001f;
	float farPlane = 1000.0f;
	XMMATRIX globalProj = XMMatrixPerspectiveFovLH((FoV_angle * (3.1415f / 180.0f)), 1280.0f / 720.0f, nearPlane, farPlane);
#pragma endregion
};

struct Lights
{
	enum LightType {
		Directional,
		Point,
		Spot
	};
	UINT numOfTotalLights = 0;
	UINT numOfDir_Lights = 0;
	UINT numOfPointLights = 0;
	UINT numOfSpotLights = 0;
	void setLight(LightType lt);
};