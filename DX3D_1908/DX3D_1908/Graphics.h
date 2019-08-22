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
		wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
		//wrl::ComPtr<ID3D11GeometryShader> gGeometryShader = nullptr;
		wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
		wrl::ComPtr<ID3DBlob> gBlob = nullptr;

		wrl::ComPtr<ID3D11Buffer> gConstantBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> gIndexBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> gVertBuffer = nullptr;
		//wrl::ComPtr<ID3D11Buffer> gGeoBuffer = nullptr;

		wrl::ComPtr<ID3D11ShaderResourceView> shaderRV = nullptr;
		wrl::ComPtr<ID3D11SamplerState> smplrState = nullptr;

		wrl::ComPtr<ID3D11InputLayout> gInputLayout = nullptr;



		gVertex* verts = nullptr;
		int numVertices = 0;
		int* indices = nullptr;
		int numIndices = 0;
		float scale = 1.0f;
	};
	std::vector<gMesh*> gppMesh;
	int numOfMeshs = 0;

#pragma region Lights

	UINT numOfTotalLights = 0;
	UINT numOfDir_Lights = 0;
	UINT numOfPointLights = 0;
	UINT numOfSpotLights = 0;

	struct gLightBuff
	{
		XMFLOAT4A pos;
		XMFLOAT4A dir;
		XMFLOAT4A color;
	};
	
	std::vector<gLightBuff*> gDirectionalLights = {};
	std::vector<gLightBuff*> gPointLights = {};
	std::vector<gLightBuff*> gSpotLights = {};

	enum LightType {
		Directional,
		Point,
		Spot
	};

	HRESULT CreateLightBuffers(ID3D11Device* gpDev,
		wrl::ComPtr<ID3D11Buffer>* gpDLightBuffer,
		wrl::ComPtr<ID3D11Buffer>* gpPLightBuffer,
		wrl::ComPtr<ID3D11Buffer>* gpSLightBuffer);

	void updateDirectionLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gDLightBuffer, XMFLOAT4A dir, XMFLOAT4A color);

	void updatePointLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gPLightBuffer, XMFLOAT4A pos, float radius, XMFLOAT4A color);

	void updateSpotLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gSLightBuffer, XMFLOAT4A pos, XMFLOAT4A dir, float width, XMFLOAT4A color);

#pragma endregion
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	HRESULT InitDevice();
	void Render();
	void ProcessFBXMesh(FbxNode* Node, gMesh* mesh); // Join with ProcessOBJMesh to make a template type mesh loader
	void LoadUVFromFBX(FbxMesh* pMesh, std::vector<XMFLOAT2>* pVecUV);
	void TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode, gMesh* gmesh); // This requires the model to have been made with a .dds file

	void CreateSkyBox(ID3D11Device* gpDev, std::vector<gMesh*>& meshArr);
	void CleanFrameBuffers(XMVECTORF32 DXCOLOR = Colors::Silver);
	void UpdateConstantBuffer(gMesh* mesh, XMFLOAT4A cbTranslate, XMFLOAT4A cbRotate);

	void LoadMesh(std::string fileName, const wchar_t* textureFile, float mesh_scale, std::vector<gMesh*>& meshArr, UINT meshIndex);
	HRESULT CreateShaders(std::vector<gMesh*>& meshVec);
	HRESULT CreateBuffers(std::vector<gMesh*>& meshVec);
	HRESULT CreateInputLayout(std::vector<gMesh*>& meshVec);
private:
#pragma region Hointer Pell
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> gDsv = nullptr;
	wrl::ComPtr<ID3D11Texture2D> gDepthStencil = nullptr;
	wrl::ComPtr<ID3D11Buffer> gDLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gPLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gSLightBuffer = nullptr;


#pragma endregion
public:
	struct gConstantBuff
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT4 ambientLight;
		float dTime;
	};

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
	XMMATRIX globalProj = XMMatrixPerspectiveFovLH(degToRad(FoV_angle), hWndWidth / hWndHeight, nearPlane, farPlane);
#pragma endregion
};