#include "ToolBox.h"

using namespace DirectX;

namespace wrl = Microsoft::WRL;
class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();



	Timer* gTimer = new Timer();

#pragma region Mesh Stuff
	struct gVertex
	{
		XMFLOAT4 pos;
		XMFLOAT4 norm;
		XMFLOAT2 uv;
		XMFLOAT4 color;
	};
	struct gMesh
	{
		bool isSkybox = false;
		wrl::ComPtr<ID3D11VertexShader> gVertexShader = nullptr;
		wrl::ComPtr<ID3D11GeometryShader> gGeometryShader = nullptr;
		wrl::ComPtr<ID3D11PixelShader> gPixelShader = nullptr;
		wrl::ComPtr<ID3DBlob> gBlob = nullptr;

		wrl::ComPtr<ID3D11Buffer> gConstantBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> gIndexBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> gVertBuffer = nullptr;
		wrl::ComPtr<ID3D11Buffer> gGeoBuffer = nullptr;

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
	void LoadMesh(std::string fileName, const wchar_t* textureFile, float mesh_scale, std::vector<gMesh*>& meshArr, UINT meshIndex, bool isSkybox);
	void ProcessFBXMesh(FbxNode* Node, gMesh* mesh); // Join with ProcessOBJMesh to make a template type mesh loader
	void LoadUVFromFBX(FbxMesh* pMesh, std::vector<XMFLOAT2>* pVecUV);
	void TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode, gMesh* gmesh); // This requires the model to have been made with a .dds file

	UINT floorW, floorD;
	void CreateFloor(std::vector<gMesh*>& meshVec, UINT meshIndex, UINT floorWidth, UINT floorDepth);
#pragma endregion

#pragma region Lights
	wrl::ComPtr<ID3D11Buffer> gDLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gPLightBuffer = nullptr;
	wrl::ComPtr<ID3D11Buffer> gSLightBuffer = nullptr;
	// Position of directional light in degrees. Converted to radians in updatelight section of render.
	float sunPos = 0;
	float randomLightPos = 0;
	UINT numOfTotalLights = 0;
	UINT numOfDir_Lights = 0;
	UINT numOfPointLights = 0;
	UINT numOfSpotLights = 0;

	struct gLightBuff
	{
		XMMATRIX lightSpace;
		XMMATRIX lightOrtho;
		//float ZBuff[hWndHeight * hWndWidth];
		XMFLOAT4A pos;
		XMFLOAT4A dir;
		XMFLOAT4A color;
	};
	
	std::vector<gLightBuff> gDirectionalLights = {};

	std::vector<gLightBuff> gPointLights = {};
	float PointLight_A = 1.0f;

	std::vector<gLightBuff> gSpotLights = {};

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






private:
#pragma region Hointer Pell
	wrl::ComPtr<ID3D11Device> gDev = nullptr;
	wrl::ComPtr<IDXGISwapChain> gSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> gCon = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> gRtv = nullptr;
	wrl::ComPtr<ID3D11DepthStencilView> gDsv = nullptr;
	wrl::ComPtr<ID3D11Texture2D> gDepthStencil = nullptr;

	// RTT
	wrl::ComPtr<ID3D11RenderTargetView> goffRtv = nullptr; // color
	wrl::ComPtr<ID3D11DepthStencilView> gOffDsv = nullptr; // depth
	wrl::ComPtr<ID3D11Texture2D> gOffStencilTexture2D = nullptr; 
	wrl::ComPtr<ID3D11Texture2D> gOffTexture2D = nullptr; // read
	wrl::ComPtr<ID3D11ShaderResourceView> gOffSRV = nullptr; // read





#pragma endregion
public:
	void UpdateCamera(XMMATRIX CamToUpdate, bool listOfComs[], UINT numOfComs);
	struct gConstantBuff
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX camera;
		XMMATRIX perspProj;
		XMMATRIX orthoProj;
		XMFLOAT4 ambientLight;
		float dTime;
	};

#pragma region WorldViewProjection
	XMMATRIX globalWorld = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(24.0f, 5.0f, -0.5f, 0.0f);
	XMVECTOR At = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX globalView_1 = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX globalView_2 = XMMatrixLookAtLH(Eye, At, Up);
	XMMATRIX Camera_1 = XMMatrixInverse(nullptr, globalView_1);
	XMMATRIX Camera_2 = XMMatrixInverse(nullptr, globalView_2);
	float FoV_angle = 60.0f;
	float nearPlane = 0.001f;
	float farPlane = 1000.0f;
	XMMATRIX globalProj = XMMatrixPerspectiveFovLH(degToRad(FoV_angle), hWndWidth / hWndHeight, nearPlane, farPlane);
	XMMATRIX globalOrthProj = XMMatrixOrthographicLH(hWndWidth, hWndHeight, nearPlane, farPlane);
#pragma endregion

#pragma region Initializations and Draws
	HRESULT InitDevice();
	HRESULT CreateShaders(std::vector<gMesh*>& meshVec);
	HRESULT CreateBuffers(std::vector<gMesh*>& meshVec);
	HRESULT CreateInputLayout(std::vector<gMesh*>& meshVec);

	void Render();
	D3D11_VIEWPORT vp[2];
	void CleanFrameBuffers(XMVECTORF32 DXCOLOR = Colors::Silver);
	void UpdateConstantBuffer(gMesh* mesh,XMMATRIX view, XMMATRIX cam, XMFLOAT4A cbTranslate, XMFLOAT4A cbRotate);

#pragma endregion
};