#include "Graphics.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib") // for shader loading function

#pragma region GraphicsConstructor/Destructor
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
	for (int i = 0; i < numOfMeshs; i++)
	{
		delete gppMesh[i];
	}
	gppMesh.clear();
}
#pragma endregion

#pragma region Mesh/Texture/File IO
HRESULT Graphics::CreateBuffers(std::vector<gMesh*>& meshArr, UINT size)
{
	HRESULT hr;
	// ONE BUFFER DESCRIPTOR TO RULE THEM ALL
	D3D11_BUFFER_DESC buffdesc = {};

	for (UINT index = 0; index < size; index++)
	{
		if (meshArr[index]->gConstantBuffer == nullptr)
		{
			//////////////////// Constant Buffer ////////////////////
			buffdesc = {};
			buffdesc.Usage = D3D11_USAGE_DEFAULT;
			buffdesc.ByteWidth = sizeof(gConstantBuff);
			buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			buffdesc.CPUAccessFlags = 0;
			hr = gDev->CreateBuffer(&buffdesc, nullptr, meshArr[index]->gConstantBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("Create Constant Buffer failed in initdevice.");
				return hr;
			}
		}

		if (meshArr[index]->gVertBuffer == nullptr)
		{
			//////////////////// Vertex Buffer ////////////////////
			buffdesc = {};
			buffdesc.Usage = D3D11_USAGE_DEFAULT;
			buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffdesc.CPUAccessFlags = 0u;
			buffdesc.MiscFlags = 0u;
			buffdesc.ByteWidth = sizeof(gVertex) * meshArr[index]->numVertices;
			buffdesc.StructureByteStride = sizeof(gVertex);
			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = gppMesh[index]->verts;
			hr = gDev->CreateBuffer(&buffdesc, &subData, meshArr[index]->gVertBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreateVertexBuffer Failed in InitDevice");
				return hr;
			}
		}

		if (meshArr[index]->gIndexBuffer == nullptr)
		{
			///////////////////// Index Buffer /////////////////////
			buffdesc = {};
			buffdesc.Usage = D3D11_USAGE_DEFAULT;
			buffdesc.ByteWidth = sizeof(int) * meshArr[index]->numIndices;
			buffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffdesc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA subData = {};
			subData.pSysMem = gppMesh[index]->indices;
			hr = gDev->CreateBuffer(&buffdesc, &subData, meshArr[index]->gIndexBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreateIndexBuffer Failed in InitDevice");
				return hr;
			}
		}
		//////////////////// Bind Index Buffer ////////////////////
		gCon->IASetIndexBuffer(meshArr[index]->gIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//////////////////// Bind Vertex Buffers ////////////////////
		const UINT strides = sizeof(gVertex);
		const UINT offset = 0u;
		gCon->IASetVertexBuffers(0u, 1u, meshArr[index]->gVertBuffer.GetAddressOf(), &strides, &offset);

		/////////////////// Creating sample state ///////////////////
		D3D11_SAMPLER_DESC texDes;
		texDes.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		texDes.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		texDes.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		texDes.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		texDes.MaxAnisotropy = 0;
		texDes.MipLODBias = 0;
		texDes.MinLOD = 0;
		texDes.MaxLOD = 0;
		HRESULT sampleResult = gDev->CreateSamplerState(&texDes, &meshArr[index]->smplrState);
		if (FAILED(sampleResult))
			return sampleResult;
	}
	return S_OK;
}
void Graphics::LoadMesh(std::string fileName, const wchar_t* textureFile, float mesh_scale, std::vector<gMesh*>& meshArr, UINT meshIndex)
{
	if (meshArr.size() <= meshIndex) // If empty, fill.
	{
		// LIFE OF A MODEL BEGINS. . .
		gMesh* tmpMesh = new gMesh();
		tmpMesh->scale = mesh_scale;
		//meshArr[meshIndex] = new gMesh();
		//meshArr[meshIndex]->scale = mesh_scale;
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
		//ProcessFBXMesh(lScene->GetRootNode(), meshArr[meshIndex]);
		ProcessFBXMesh(lScene->GetRootNode(), tmpMesh);
		meshArr.push_back(tmpMesh);
		numOfMeshs++;

		// TEXTURE LOADING///////////////////////
		HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureFile, nullptr, &meshArr[meshIndex]->shaderRV);
		if (FAILED(res))
			ToolBox::ThrowErrorMsg("CreateDDSTextureFromFile() Failed In LoadMesh!");
	}
	else
	{
		ToolBox::ThrowErrorMsg("LoadMesh() failed::meshArr was not nullptr.\nWe do not overwrite memory in this house!");
	}
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
				vertices2[j].norm.x = (float)normalsVec[j].mData[0];
				vertices2[j].norm.y = (float)normalsVec[j].mData[1];
				vertices2[j].norm.z = (float)normalsVec[j].mData[2];

				vertices2[j].uv.x = UVvec[j].x;
				vertices2[j].uv.y = UVvec[j].y;
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
void Graphics::TextureFileFromFBX(FbxMesh* mesh, FbxNode* childNode, gMesh* gmesh)
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
						HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureName, nullptr, &gmesh->shaderRV);
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
					const char* txtname = texture->GetFileName();
					int i = 0;
					std::string t;
					while (txtname[i] != NULL)
					{
						t.push_back(txtname[i]);
						i++;
					}
					OutputDebugString(t.c_str());
					//HRESULT res = CreateDDSTextureFromFile(gDev.Get(), (const wchar_t*)txtname, nullptr, &shaderRV);
					HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"Crate.dds", nullptr, &gmesh->shaderRV);
					//const wchar_t* textureName = (const wchar_t*)texture->GetFileName();
					//HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureName, nullptr, &shaderRV);
					//pTextureName = (char*)textureName;
					std::cout << "Texture Name " << txtname;

					FbxProperty p = texture->RootProperty.Find("Filename");
					std::cout << p.Get<FbxString>() << std::endl;

				}
			}
		}
	}
}
#pragma endregion


#pragma region InitCalls
HRESULT Graphics::CreateLightBuffers(ID3D11Device& gpDev,
	wrl::ComPtr<ID3D11Buffer>* gpDLightBuffer,
	wrl::ComPtr<ID3D11Buffer>* gpPLightBuffer,
	wrl::ComPtr<ID3D11Buffer>* gpSLightBuffer)
{
	HRESULT hr;
	D3D11_BUFFER_DESC buffdesc = {};

	/////////////// Directional Light Buffer ///////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gpDev.CreateBuffer(&buffdesc, nullptr, gpDLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	////////////////// Point Light Buffer //////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gpDev.CreateBuffer(&buffdesc, nullptr, gpPLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	////////////////// Spot Light Buffer //////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gpDev.CreateBuffer(&buffdesc, nullptr, gpSLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Graphics::CreateShaders(std::vector<gMesh*>& meshVec)
{
	HRESULT hr;
	for (UINT i = 0; i < meshVec.size(); i++)
	{
		if (meshVec[i]->gBlob != nullptr)
		{
			meshVec[i]->gBlob.Reset();
			meshVec[i]->gBlob = nullptr;
		}
		//////////////////// create pixel shader ////////////////////
		D3DReadFileToBlob(L"PixelShader.cso", meshVec[i]->gBlob.GetAddressOf());
		hr = gDev->CreatePixelShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gPixelShader.GetAddressOf());
		if (FAILED(hr))
		{
			ToolBox::ThrowErrorMsg("CreatePixelShader Failed in InitDevice");
			return hr;
		}

		///////////////////// bind pixel shader /////////////////////
		gCon->PSSetShader(meshVec[i]->gPixelShader.Get(), nullptr, 0u);

		/////////////////// create vertex shader ///////////////////
		D3DReadFileToBlob(L"VertexShader.cso", meshVec[i]->gBlob.GetAddressOf());
		hr = gDev->CreateVertexShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gVertexShader.GetAddressOf());
		if (FAILED(hr))
		{
			ToolBox::ThrowErrorMsg("CreateVertexShader Failed in InitDevice");
			return hr;
		}

		/////////////////////// bind vertex ///////////////////////
		gCon->VSSetShader(meshVec[i]->gVertexShader.Get(), nullptr, 0u);

		///////////////////// create geometry shader ///////////////////
		//D3DReadFileToBlob(L"GeometryShader.cso", &gBlob);
		//hr = gDev->CreateGeometryShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gGeometryShader);
		//if (FAILED(hr))
		//{
		//	ToolBox::ThrowErrorMsg("CreateGeometryShader Failed in InitDevice");
		//	return hr;
		//}

		//gCon->GSSetShader(meshVec[i]->gVertexShader.Get(), nullptr, 0u);


	}
	return S_OK;
}
#pragma endregion

#pragma region RenderCalls
void Graphics::CleanFrameBuffers(XMVECTORF32 DXCOLOR)
{
	gCon->ClearRenderTargetView(gRtv.Get(), DXCOLOR);
	gCon->ClearDepthStencilView(gDsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	for (int i = 0; i < gppMesh.size(); i++)
	{
		//gppMesh[i]->gConstantBuffer.Reset();
		gppMesh[i]->gIndexBuffer.Reset();
		gppMesh[i]->gInputLayout.Reset();
		gppMesh[i]->gVertBuffer.Reset();
		//gppMesh[i]->gVertexShader.Reset();
		//gppMesh[i]->gConstantBuffer = nullptr;
		gppMesh[i]->gIndexBuffer = nullptr;
		gppMesh[i]->gInputLayout = nullptr;
		gppMesh[i]->gVertBuffer = nullptr;
		//gppMesh[i]->gVertexShader = nullptr;
	}
}
void Graphics::UpdateConstantBuffer(gMesh* mesh, float cbTranslate[3], float cbRotate[3])
{
	// This sends World,View,Proj,AmbientLight through the shaders.
	gConstantBuff gCB;
	globalWorld = XMMatrixTranslation(cbTranslate[0], cbTranslate[1], cbTranslate[2]);
	if (cbRotate[0] != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 1,0,0 }, degToRad(cbRotate[0])), globalWorld);
	if (cbRotate[1] != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 0,1,0 }, degToRad(cbRotate[1])), globalWorld);
	if (cbRotate[2] != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 0,0,1 }, degToRad(cbRotate[2])), globalWorld);

	gCB.world = XMMatrixTranspose(globalWorld);
	gCB.view = XMMatrixTranspose(globalView);
	gCB.proj = XMMatrixTranspose(globalProj);
	gCB.dTime = (float)gTimer->deltaTime;
	gCB.ambientLight = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	gCon->UpdateSubresource(mesh->gConstantBuffer.Get(), 0, nullptr, &gCB, 0, 0);

	//////////////////////// Bind Shaders ////////////////////////
	// Bind buffers to pipeline so the Drawcall can access the information from setup.
	ID3D11Buffer* buffs[] = { *mesh->gConstantBuffer.GetAddressOf(),
							  *gDLightBuffer.GetAddressOf(),
							  *gPLightBuffer.GetAddressOf(),
							  *gSLightBuffer.GetAddressOf() };
	gCon->VSSetShader(mesh->gVertexShader.Get(), nullptr, 0u);
	gCon->VSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShader(mesh->gPixelShader.Get(), nullptr, 0u);
	gCon->PSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShaderResources(0, 1, mesh->shaderRV.GetAddressOf());
	gCon->PSSetSamplers(0, 1, mesh->smplrState.GetAddressOf());
}
void Graphics::updateDirectionLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gDLightBuffer, XMFLOAT4A dir, XMFLOAT4A color)
{

	if (numOfLights == 0 || gDirectionalLights.size() == 0)
	{
		gLightBuff tmp;
		tmp.dir = dir;
		tmp.color = color;
		gDirectionalLights.push_back(&tmp);
		gpCon->UpdateSubresource(gDLightBuffer, 0, nullptr, gDirectionalLights[0], 0, 0);
		numOfDir_Lights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gDirectionalLights[i]->dir = dir;
		gDirectionalLights[i]->color = color;
		gpCon->UpdateSubresource(gDLightBuffer, 0, nullptr, gDirectionalLights[i], 0, 0);
	}
}
void Graphics::updatePointLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gPLightBuffer, XMFLOAT4A pos, float radius, XMFLOAT4A color)
{
	if (numOfLights == 0 || gPointLights.size() == 0)
	{
		gLightBuff tmp;
		tmp.pos = XMFLOAT4A(pos.x, pos.y, pos.z, radius);
		tmp.color = color;
		gPointLights.push_back(&tmp);
		gpCon->UpdateSubresource(gPLightBuffer, 0, nullptr, gPointLights[0], 0, 0);
		numOfPointLights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gPointLights[i]->pos = { pos.x,pos.y,pos.z,radius };
		gPointLights[i]->color = color;
		gpCon->UpdateSubresource(gPLightBuffer, 0, nullptr, gPointLights[i], 0, 0);
	}

}
void Graphics::updateSpotLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gSLightBuffer, XMFLOAT4A pos, XMFLOAT4A dir, float width, XMFLOAT4A color)
{
	if (numOfLights == 0 || gSpotLights.size() == 0)
	{
		gLightBuff tmp;
		tmp.pos = pos;
		tmp.dir = { dir.x,dir.y,dir.z,width };
		tmp.color = color;
		gSpotLights.push_back(&tmp);
		gpCon->UpdateSubresource(gSLightBuffer, 0, nullptr, gSpotLights[0], 0, 0);
		numOfSpotLights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gSpotLights[i]->pos = pos;
		gSpotLights[i]->dir = { dir.x,dir.y,dir.z,width };
		gSpotLights[i]->color = color;
		gpCon->UpdateSubresource(gSLightBuffer, 0, nullptr, gSpotLights[i], 0, 0);
	}


}
void Graphics::CreateInputLayout(std::vector<gMesh*>& meshVec)
{
	HRESULT hr;
	for (UINT i = 0; i < meshVec.size(); i++)
	{
		/////////////////// input vertex layout ///////////////////
		const D3D11_INPUT_ELEMENT_DESC ildes[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"NORMAL",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};

		hr = gDev->CreateInputLayout(ildes, (UINT)ARRAYSIZE(ildes), meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), &meshVec[i]->gInputLayout);
		if (FAILED(hr))
			ToolBox::ThrowErrorMsg("CreateInputLayout Failed in CreateInputLayout");
		// bind vertex target
		gCon->IASetInputLayout(meshVec[i]->gInputLayout.Get());
	}
}
#pragma endregion

#pragma region DeviceSetup/RenderPipeline
HRESULT Graphics::InitDevice()
{
	gTimer->StartTimer(gTimer);
	HRESULT hr;
#pragma region LOAD MODELS
	//ModelDraw_Switch(currModel); // Default Model set to draw, SPACE BAR to cycle.
	//std::string modelName[MODEL_COUNT] = { "Tester.fbx","NewDragon.fbx","Cube.fbx" }; // convert to array or vector of strings to store multiple mesh directories.
	//LoadMesh("Tester.fbx", 1.0f, gppMesh, 0);
	//LoadMesh("NewDragon.fbx",10.0f, gppMesh, 0);
	//LoadMesh("SpaceShip_1.fbx", 1.0f, gppMesh, 0);
	//LoadMesh("SpaceShip_3.fbx", 0.5f, gppMesh, 0);
	//LoadMesh("Desk_0.fbx", L"carbonfiber.dds", 1.0f, &gppMesh, 0);
	LoadMesh("Desk_1.fbx", L"carbonfiber.dds", 1.0f, gppMesh, 0);
	LoadMesh("Cube.fbx", L"Crate.dds", 50.0f, gppMesh, 1);
#pragma endregion

#pragma region Create_Buffers
	CreateBuffers(gppMesh, 2);
	//CreateBuffers(gppMesh, 1);

	//////////////// Light Buffer ///////////////////
	hr = CreateLightBuffers(*gDev.Get(), &gDLightBuffer, &gPLightBuffer, &gSLightBuffer);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateLightBuffers Failed in InitDevice");
		return hr;
	}
#pragma endregion

#pragma region Create Geometry / Vertex / PixelShaders

	CreateShaders(gppMesh);

#pragma endregion

#pragma region Views


	// Set primitive topology to triangle list ( group of 3 verts)
	gCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = hWndWidth;
	descDepth.Height = hWndHeight;
	descDepth.MipLevels = 0;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = gDev->CreateTexture2D(&descDepth, nullptr, gDepthStencil.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = gDev->CreateDepthStencilView(gDepthStencil.Get(), &descDSV, gDsv.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// bind render target
	gCon->OMSetRenderTargets(1, gRtv.GetAddressOf(), gDsv.Get());

	// configer viewport
	//D3D11_VIEWPORT port_one;
	//port_one.Width = 640.0f;
	//port_one.Height = 720.0f;
	//port_one.MinDepth = 0.0f;
	//port_one.MaxDepth = 1.0f;
	//port_one.TopLeftX = 0.0f;
	//port_one.TopLeftY = 0.0f;
	//  
	//D3D11_VIEWPORT port_two;
	//port_two.Width = 640.0f;
	//port_two.Height = 720.0f;
	//port_two.MinDepth = 0.0f;
	//port_two.MaxDepth = 1.0f;
	//port_two.TopLeftX = 640.0f;
	//port_two.TopLeftY = 0.0f;

	//D3D11_VIEWPORT vp[2] = { port_one, port_two };
	D3D11_VIEWPORT vp;
	vp.Width = 1280.0f;
	vp.Height = 720.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	gCon->RSSetViewports(1u, &vp);
#pragma endregion

	return hr;
}
void Graphics::Render()
{
	float deltaT = (float)gTimer->TimeSinceTick(gTimer);
	CleanFrameBuffers();

	CreateInputLayout(gppMesh);

#pragma region Lights
	// SUN DIRECTIONAL LIGHT
	updateDirectionLight(gCon.Get(), 0u, 1u, gDLightBuffer.Get(),
		XMFLOAT4A(0.55f, 0.0f, 0.45f, 0.0f), XMFLOAT4A(0.65f, 0.45f, 0.0f, 1.0f));

	// LAMP POINT LIGHT
	updatePointLight(gCon.Get(), 0u, 1u, gPLightBuffer.Get(),
		XMFLOAT4A(7.0f, 5.0f, 40.0f, 0.0f),
		10.0f,
		XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f));
	//XMFLOAT4A(sin(degToRad(deltaT) + 10), 45.0f, sin(degToRad(deltaT * 50.0f) + 10)

	// CAMREA SPOT LIGHT
	XMFLOAT4A tmp_pos;
	XMStoreFloat4A(&tmp_pos, Camera.r[3]);
	XMFLOAT4A tmp_dir;
	XMStoreFloat4A(&tmp_dir, XMVector4Transform(XMVectorSet(0, 0, 1, 0), Camera));
	updateSpotLight(gCon.Get(), 0u, 1u, gSLightBuffer.Get(),
		tmp_pos, tmp_dir, 0.9f, XMFLOAT4A(0.75f, 0.5f, 0.5f, 1.0f));
#pragma endregion

#pragma region Update Constant Buffer
	float move[] = { 10.0f, 0.0f, 0.0f };
	float rotate[] = { 0.0f, -90.0f, 0.0f };
	UpdateConstantBuffer(gppMesh[0], move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[0]->numIndices, 0u, 0);

	float move1[] = { -10.0f, 0.0f, 0.0f };
	float rotate1[] = { 0.0f, 0.0f, 0.0f };
	UpdateConstantBuffer(gppMesh[1], move1, rotate1);
	gCon->DrawIndexed((UINT)gppMesh[1]->numIndices, 0u, 0);
#pragma endregion

	gSwap->Present(1u, 0u);
}
#pragma endregion