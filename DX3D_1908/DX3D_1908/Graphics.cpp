#include "Graphics.h"


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib") // for shader loading function

Graphics::Graphics(HWND hWnd)
{
	if (hWnd)
	{
		DXGI_SWAP_CHAIN_DESC gSwapDes = {};
		ZeroMemory(&gSwapDes, sizeof(DXGI_SWAP_CHAIN_DESC));
		gSwapDes.BufferDesc.Width = gSwapDes.BufferDesc.Height = 0;
		gSwapDes.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		gSwapDes.BufferDesc.RefreshRate.Numerator = 0;
		gSwapDes.BufferDesc.RefreshRate.Denominator = 0;
		gSwapDes.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		gSwapDes.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		gSwapDes.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		gSwapDes.BufferCount = 1;
		gSwapDes.SampleDesc.Count = 1;
		gSwapDes.SampleDesc.Quality = 0;
		gSwapDes.OutputWindow = hWnd;
		gSwapDes.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		gSwapDes.Windowed = TRUE;
		gSwapDes.Flags = 0;

		HRESULT hr = D3D11CreateDeviceAndSwapChain
		(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&gSwapDes,
			&gSwap,
			&gDev,
			nullptr,
			&gCon
		);
		if (FAILED(hr))
			ToolBox::ThrowErrorMsg("Graphics Constuctor : CreateDeviceSwapchain Failed.");

		wrl::ComPtr<ID3D11Resource> gBackBuffer = nullptr;
		gSwap->GetBuffer(0, __uuidof(ID3D11Resource), &gBackBuffer);
		if (gBackBuffer)
		{
			hr = gDev->CreateRenderTargetView(gBackBuffer.Get(), nullptr, &gRtv);
			if (FAILED(hr))
				ToolBox::ThrowErrorMsg("Graphics Constuctor : CreateRenderTargetVew Failed.");
		}
	}
}

Graphics::~Graphics()
{
	delete gpMesh;
}

HRESULT Graphics::InitDevice()
{
	HRESULT hr;

#pragma region FBX Loading
	
	LoadMesh("NewDragon.fbx",25.0f, gpMesh);
#pragma endregion


	D3D11_BUFFER_DESC buffdesc = {};

	// create constant buffer
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gConstantBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gConstantBuffer);
	if (FAILED(hr))
		return hr;

	// create light buffer
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gDirLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gLightBuffer);
	if (FAILED(hr))
		return hr;

	// create vertex buffer
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffdesc.CPUAccessFlags = 0u;
	buffdesc.MiscFlags = 0u;
	buffdesc.ByteWidth = sizeof(gVertex) * gpMesh->numVertices;
	buffdesc.StructureByteStride = sizeof(gVertex);
	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = gpMesh->verts;
	hr = gDev->CreateBuffer(&buffdesc, &subData, gVertBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateVertexBuffer Failed in InitDevice");
		return hr;
	}

	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(int) * gpMesh->numIndices;
	buffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	subData = {};
	subData.pSysMem = gpMesh->indices;
	hr = gDev->CreateBuffer(&buffdesc, &subData, &gIndexBuffer);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateIndexBuffer Failed in InitDevice");
		return hr;
	}

	// Set index buffer
	gCon->IASetIndexBuffer(gIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// bind vertex buffers to pipeline
	const UINT strides = sizeof(gVertex);
	const UINT offset = 0u;
	gCon->IASetVertexBuffers(0u, 1u, gVertBuffer.GetAddressOf(), &strides, &offset);

	// create pixel shader
	D3DReadFileToBlob(L"PixelShader.cso", &gBlob);
	hr = gDev->CreatePixelShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gPixelShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreatePixelShader Failed in InitDevice");
		return hr;
	}

	// bind pixel shader
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);

	// create vertex shader.
	D3DReadFileToBlob(L"VertexShader.cso", &gBlob);
	hr = gDev->CreateVertexShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gVertexShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateVertexShader Failed in InitDevice");
		return hr;
	}

	// bind vertex.
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);

	// input vertex layout
	const D3D11_INPUT_ELEMENT_DESC ildes[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	//TextureFileFromFBX();
	HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"Crate.dds", nullptr, &shaderRV);
	//HRESULT res = CreateDDSTextureFromFile(g_pd3dDevice, (const wchar_t*)textName, nullptr,&shadRes);
	if (FAILED(res))
		return res;

	// Creating sample state
	D3D11_SAMPLER_DESC texDes;
	texDes.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	texDes.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	texDes.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	texDes.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	texDes.MaxAnisotropy = 0;
	texDes.MipLODBias = 0;
	texDes.MinLOD = 0;
	texDes.MaxLOD = 0;
	HRESULT sampleResult = gDev->CreateSamplerState(&texDes, &smplrState);
	if (FAILED(sampleResult))
		return sampleResult;

	hr = gDev->CreateInputLayout(ildes, (UINT)ARRAYSIZE(ildes), gBlob->GetBufferPointer(), gBlob->GetBufferSize(), &gInputLayout);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateInputLayout Failed in DrawTriangle");
		return hr;
	}
	// bind vertex target
	gCon->IASetInputLayout(gInputLayout.Get());

	// bind render target
	gCon->OMSetRenderTargets(1, gRtv.GetAddressOf(), nullptr);

	// Set primitive topology to triangle list ( group of 3 verts)
	gCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// configer viewport
	D3D11_VIEWPORT vp;
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gCon->RSSetViewports(1u, &vp);

	return hr;
}

void Graphics::Render()
{

	// TIMER
	float deltaT = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	deltaT = (timeCur - timeStart) / 1000.0f;

	// Clear Buffers
	gCon->ClearRenderTargetView(gRtv.Get(), DirectX::Colors::Black);

	// from here
	// Setup Constant buffers to be used in shaders.
	gConstantBuff gCB;
	gCB.world = XMMatrixTranslation(-5.0f, 0.0f, 0.0f);
	gCB.world = XMMatrixRotationAxis({ 1,0,0 }, deltaT);
	gCB.world = XMMatrixTranspose(gCB.world);
	XMVECTOR tV = XMMatrixDeterminant(globalView);
	XMMATRIX tM = XMMatrixInverse(&tV, globalView);
	gCB.view = XMMatrixTranspose(tM);
	gCB.proj = XMMatrixTranspose(globalProj);
	gCB.ambientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	gCon->UpdateSubresource(gConstantBuffer.Get(), 0, nullptr, &gCB, 0, 0);

	// Light Buffer Setup
	gDirectional_1.dir = XMFLOAT4(-1.0f, 2.0f, -1.0f, 0.0f);
	DirectionLightSwitch(Graphics::DirectionLight_Red);
	gCon->UpdateSubresource(gLightBuffer.Get(), 0, nullptr, &gDirectional_1, 0, 0);
	gCon->DrawIndexed((UINT)gpMesh->numVertices, 0u, 0);

	// Light Buffer Setup
	gDirectional_1.dir = XMFLOAT4(1.0f, -2.0f, 1.0f, 0.0f);
	DirectionLightSwitch(Graphics::DirectionLight_Green);
	gCon->UpdateSubresource(gLightBuffer.Get(), 0, nullptr, &gDirectional_1, 0, 0);
	gCon->DrawIndexed((UINT)gpMesh->numVertices, 0u, 0);

	// Set Shaders and Constant Buffer to Shader and Draw
	ID3D11Buffer* buffs[] = { *gConstantBuffer.GetAddressOf(), *gLightBuffer.GetAddressOf() };
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);
	gCon->VSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);
	gCon->PSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShaderResources(0, 1, shaderRV.GetAddressOf());
	gCon->PSSetSamplers(0, 1, smplrState.GetAddressOf());
	//gCon->DrawIndexed(6u, 0u, 0u);
	gCon->DrawIndexed((UINT)gpMesh->numVertices, 0u, 0);
	// to here might be a function???

	// Present ""Finished"" buffer to screen
	gSwap->Present(1u, 0u);
}

void Graphics::ProcessFBXMesh(FbxNode* Node, gMesh* gmesh)
{
	// set up output console
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	//freopen("CONOUT$", "w", stderr);

	//FBX Mesh stuff
	int childrenCount = Node->GetChildCount();

	std::cout << "\nName:" << Node->GetName();

	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode* childNode = Node->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();

		if (mesh != NULL)
		{
			//loaduvs here
			std::vector<XMFLOAT2> UVvec; // array of UVs
			LoadUVFromFBX(mesh, &UVvec);
			//TextureFileFromFBX(mesh, childNode);
			std::cout << "\nMesh:" << childNode->GetName();

			// Get index count from mesh
			gmesh->numIndices = mesh->GetPolygonVertexCount();
			std::cout << "\nIndice Count:" << gmesh->numIndices;

			// No need to allocate int array, FBX does for us
			gmesh->indices = mesh->GetPolygonVertices();

			// Get vertex count from mesh
			gmesh->numVertices = mesh->GetControlPointsCount();
			std::cout << "\nVertex Count:" << gmesh->numVertices;

			// Create SimpleVertex array to size of this mesh
			gmesh->verts = new gVertex[gmesh->numVertices];

			//================= Process Vertices ===================
			for (int j = 0; j < gmesh->numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				gmesh->verts[j].pos.x = (float)vert.mData[0] / gmesh->scale;
				gmesh->verts[j].pos.y = (float)vert.mData[1] / gmesh->scale;
				gmesh->verts[j].pos.z = (float)vert.mData[2] / gmesh->scale;
				gmesh->verts[j].pos.w = 1.0f;
				// Generate random normal
				//verts[j].Normal = RAND_NORMAL;
			}

			// Get the Normals array from the mesh
			FbxArray<FbxVector4> normalsVec;
			mesh->GetPolygonVertexNormals(normalsVec);
			std::cout << "\nNormalVec Count:" << normalsVec.Size();

			// Declare a new array for the second vertex array
			// Note the size is numIndices not numVertices
			gVertex* vertices2 = new gVertex[gmesh->numIndices];

			// align (expand) vertex array and set the normals
			for (int j = 0; j < gmesh->numIndices; j++)
			{
				vertices2[j].pos = gmesh->verts[gmesh->indices[j]].pos;
				//vertices2[j].Normal = vertices[indices[j]].Normal;
				vertices2[j].norm.x = (float)normalsVec[j].mData[0];
				vertices2[j].norm.y = (float)normalsVec[j].mData[1];
				vertices2[j].norm.z = (float)normalsVec[j].mData[2];

				vertices2[j].uv.x =	UVvec[j].x;
				vertices2[j].uv.y = UVvec[j].y;
				//vertices2[j].Tex.x = vector[j].mData[0];
			}
			for (int hlk = 0; hlk < gmesh->numIndices; hlk++)
			{
				std::cout << std::endl << "new texture verts : ";
				std::cout << vertices2[hlk].uv.x << "," << vertices2[hlk].uv.y;
				std::cout << " at indice : " << hlk;
			}

			// vertices is an "out" var so make sure it points to the new array
			// and clean up first array
			delete gmesh->verts;
			gmesh->verts = vertices2;

			// make new indices to match the new vertex(2) array
			delete gmesh->indices;
			gmesh->indices = new int[gmesh->numIndices];
			for (int j = 0; j < gmesh->numIndices; j++)
			{
				gmesh->indices[j] = j;
			}
			//for (int k = 0; k < gmesh->numIndices; k++)
			//{
			//	std::cout << std::endl << "pos( ";
			//	std::cout << meshes[0].verts[k].pos.x << ", ";
			//	std::cout << meshes[0].verts[k].pos.y << ", ";
			//	std::cout << meshes[0].verts[k].pos.z << ", ";
			//	std::cout << meshes[0].verts[k].pos.w << ")\t nrm( ";
			//	std::cout << meshes[0].verts[k].norm.x << ", ";
			//	std::cout << meshes[0].verts[k].norm.y << ", ";
			//	std::cout << meshes[0].verts[k].norm.z << ", ";
			//	std::cout << meshes[0].verts[k].norm.w << ")\t tex( ";
			//	std::cout << meshes[0].verts[k].uv.x << ", ";
			//	std::cout << meshes[0].verts[k].uv.y << ")\t index: ";
			//	std::cout << meshes[0].indices[k];
			//
			//}

			//if (false)
			//{
			//	//Compactify();
			//}
			//else
			//{
				// numVertices is an "out" var so set to new size
				// this is used in the DrawIndexed functions to set the 
				// the right number of triangles
			gmesh->numVertices = gmesh->numIndices;
			//}
		}
		ProcessFBXMesh(childNode, gmesh);
	}

}
void Graphics::LoadUVFromFBX(FbxMesh* pMesh, std::vector<XMFLOAT2>* pVecUV)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file

					XMFLOAT2 temp = { static_cast<float>(lUVValue.mData[0]),static_cast<float>(lUVValue.mData[1]) };
					pVecUV->push_back(temp);

				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						/*if (lUVValue.mData[lVertIndex] <= 1 && lUVValue.mData[lVertIndex] >= 0)
						{
							pVecUV[lVertIndex].x = lUVValue.mData[lVertIndex];
						}*/
						//if (lVertIndex < 2)
						//	cout << lUVValue.mData[lVertIndex] << endl;
						//cout << endl;
						//cout << lUVValue.mData[0] << ", " << lUVValue.mData[1];
						//if ((lUVValue.mData[0] <= 1 && lUVValue.mData[0] >= 0 && lUVValue.mData[1] <= 1 && lUVValue.mData[1] >= 0))
						//{
							//cout << lUVValue.mData[lVertIndex] << endl;
						XMFLOAT2 temp = { static_cast<float>(lUVValue.mData[0]),static_cast<float>(lUVValue.mData[1]) };
						pVecUV->push_back(temp);
						//}
						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

void Graphics::TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode)
{
	//================= Texture ========================================
	// GIVE ME UVs!
	FbxArray<FbxVector2> arrayOfUVs;
	FbxArray<int> arrayOfUVIDs;
	FbxStringList UV_Name;
	mesh->GetUVSetNames(UV_Name);
	const char* uvSetName = (char*)& UV_Name;
	mesh->GetPolygonVertexUVs(UV_Name.GetStringAt(0), arrayOfUVs);
	//mesh->GetPolygonVertexUVs(uvSetName,arrayOfUVs, &arrayOfUVIDs);


	int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();

	for (int index = 0; index < materialCount; index++)
	{
		FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(index);

		if (material != NULL)
		{
			std::cout << "\nmaterial: " << material->GetName() << std::endl;
			// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			// Check if it's layeredtextures
			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

			if (layeredTextureCount > 0)
			{
				for (int j = 0; j < layeredTextureCount; j++)
				{
					FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
					int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

					for (int k = 0; k < lcount; k++)
					{
						FbxFileTexture* texture = FbxCast<FbxFileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
						// Then, you can get all the properties of the texture, include its name
						material->ConnectSrcObject(texture);
						const wchar_t* textureName = (const wchar_t*)texture->GetFileName();
						HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureName, nullptr, &shaderRV);
						//HRESULT res = CreateDDSTextureFromFile(g_pd3dDevice, (const wchar_t*)textName, nullptr,&shadRes);
						//pTextureName = (char*)textureName;
						std::cout << "Texture Name " << textureName;
					}
				}
			}
			else
			{
				// Directly get textures
				int textureCount = prop.GetSrcObjectCount<FbxTexture>();
				for (int j = 0; j < textureCount; j++)
				{
					FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));
					// Then, you can get all the properties of the texture, include its name
					material->ConnectSrcObject(texture);
					//const char* textureName = texture->GetFileName();
					const wchar_t* textureName = (const wchar_t*)texture->GetFileName();
					HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureName, nullptr, &shaderRV);
					//pTextureName = (char*)textureName;
					std::cout << "Texture Name " << textureName;

					FbxProperty p = texture->RootProperty.Find("Filename");
					std::cout << p.Get<FbxString>() << std::endl;

				}
			}
		}
	}
}

void Graphics::LoadMesh(std::string fileName, float mesh_scale, gMesh* mesh)
{
	mesh->scale = mesh_scale;

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(fileName.c_str(), -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	// Process the scene and build DirectX Arrays
	ProcessFBXMesh(lScene->GetRootNode(),mesh);
}

void Graphics::CameraMove(XMVECTOR E, Graphics::Axis axi)
{
	// Take dot of each axis to identity
	XMVECTOR dAngle = XMVector4Dot(globalView.r[3], E);
	XMFLOAT4 dAngleF; 
	XMStoreFloat4(&dAngleF, dAngle);

	XMMATRIX rotationMtx;
	switch (axi)
	{
	case Graphics::x:
		rotationMtx = XMMatrixRotationAxis({ 1,0,0 }, dAngleF.x);
		break;
	case Graphics::y:
		rotationMtx = XMMatrixRotationAxis({ 0,1,0 }, dAngleF.y);
		break;
	case Graphics::z:
		rotationMtx = XMMatrixRotationAxis({ 0,0,1 }, dAngleF.z);
		break;
	}
	globalView = XMMatrixMultiply(rotationMtx, globalView);

	// mult dot to inputVector E
	// inputVector E into globalView


	//globalView += XMMatrixTranslationFromVector(E);
	//XMVECTOR currEye = globalView.r[2];
	//XMVECTOR currAt = At + E;
	//currEye += XMVector4Transform(E, globalView);
	//XMMATRIX nMtx = XMMatrixLookAtLH(currEye, currAt, Up);
	//XMVECTOR detmnt = XMMatrixDeterminant(nMtx);
	//globalView = XMMatrixInverse(&detmnt, nMtx);
	
	

	//XMMATRIX nMtx = XMMatrixLookAtLH(Eye, At, Up);
	//detmnt = XMMatrixDeterminant(globalView);
	////XMVECTOR tmpEye = (XMVECTOR)globalView.r[3];
	////tmpEye += XMVectorSet(0.0f, 0.0f, loc_z, 0.0f);
	////XMMATRIX tmpp = XMMatrixLookAtLH(tmpEye, At, Up);
	////XMVECTOR tmpDet = XMMatrixDeterminant(tmpp);
	//globalView = XMMatrixInverse(&detmnt, nMtx);
}
void Graphics::CameraRotate(XMVECTOR axis, float angle)
{
	
	At += (axis * angle);
	XMMATRIX nMtx = XMMatrixLookAtLH(Eye, At, Up);
	XMVECTOR detmnt = XMMatrixDeterminant(nMtx);
	//XMVECTOR tmpEye = (XMVECTOR)globalView.r[3];
	//tmpEye += XMVectorSet(0.0f, 0.0f, loc_z, 0.0f);
	//XMMATRIX tmpp = XMMatrixLookAtLH(tmpEye, At, Up);
	//XMVECTOR tmpDet = XMMatrixDeterminant(tmpp);
	globalView = XMMatrixInverse(&detmnt, nMtx);
	//globalView *= XMMatrixRotationAxis(axis, angle);
}


bool Graphics::DirectionLightSwitch(DirLightComs cmd)
{
	switch (cmd)
	{
	case Graphics::DirectionLight_Red:
	{
		gDirectional_1.color = XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f);
	}
	break;
	case Graphics::DirectionLight_Green:
	{
		gDirectional_1.color = XMFLOAT4(0.1f, 1.0f, 0.1f, 1.0f);
	}
	break;
	case Graphics::DirectionLight_Blue:
	{
		gDirectional_1.color = XMFLOAT4(0.1f, 0.1f, 1.0f, 1.0f);
	}
	break;
	case Graphics::DirectionLight_Off:
	{
		gDirectional_1.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	break;
	case Graphics::DirectionLight_Default:
	{
		gDirectional_1.color = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	}
	break;
	}
	return false;
}
