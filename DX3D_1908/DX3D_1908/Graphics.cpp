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

#pragma region InitCalls
#pragma region Mesh/Texture/File IO
void Graphics::CreateFloor(std::vector<gMesh*>& meshVec, UINT meshIndex, UINT floorWidth, UINT floorDepth)
{
	UINT quads = floorWidth * floorDepth;
	UINT indices = 6 * (quads * quads);

	gMesh* floor = new gMesh();
	meshVec.push_back(floor);

	meshVec[meshIndex]->isSkybox = false;
	meshVec[meshIndex]->verts = new gVertex[quads*quads*4];
	meshVec[meshIndex]->indices = new int[indices];

	UINT d = 0;
	for (UINT z = 0; z < quads; z++)
		for (UINT x = 0; x < quads; d += 4, x++)
		{
			meshVec[meshIndex]->verts[0 + d].pos = XMFLOAT4A(0.0f + x, -0.5f, 0.0f + z, 1.0f);	// TL
			meshVec[meshIndex]->verts[1 + d].pos = XMFLOAT4A(0.0f + x, -0.5f, 1.0f + z, 1.0f);	// TR
			meshVec[meshIndex]->verts[2 + d].pos = XMFLOAT4A(1.0f + x, -0.5f, 1.0f + z, 1.0f);	// BR
			meshVec[meshIndex]->verts[3 + d].pos = XMFLOAT4A(1.0f + x, -0.5f, 0.0f + z, 1.0f);	// BL

			meshVec[meshIndex]->verts[0 + d].uv = XMFLOAT2A(0.0f, 0.0f);	// TL
			meshVec[meshIndex]->verts[1 + d].uv = XMFLOAT2A(1.0f, 0.0f);	// TR
			meshVec[meshIndex]->verts[2 + d].uv = XMFLOAT2A(1.0f, 1.0f);	// BR
			meshVec[meshIndex]->verts[3 + d].uv = XMFLOAT2A(0.0f, 1.0f);	// BL

			meshVec[meshIndex]->verts[0 + d].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);	// TL
			meshVec[meshIndex]->verts[1 + d].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);	// TR
			meshVec[meshIndex]->verts[2 + d].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);	// BR
			meshVec[meshIndex]->verts[3 + d].color = XMFLOAT4A(0.0f, 1.0f, 0.0f, 1.0f);	// BL

			meshVec[meshIndex]->verts[0 + d].norm = XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f);	// TL
			meshVec[meshIndex]->verts[1 + d].norm = XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f);	// TR
			meshVec[meshIndex]->verts[2 + d].norm = XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f);	// BR
			meshVec[meshIndex]->verts[3 + d].norm = XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f);	// BL


		}


		for (UINT i = 0, v = 0; i < indices; i += 6, v+=4)
		{
			meshVec[meshIndex]->indices[0+i] = 0+v;
			meshVec[meshIndex]->indices[1+i] = 1+v;
			meshVec[meshIndex]->indices[2+i] = 3+v;
			meshVec[meshIndex]->indices[3+i] = 3+v;
			meshVec[meshIndex]->indices[4+i] = 1+v;
			meshVec[meshIndex]->indices[5+i] = 2+v;
		}
			meshVec[meshIndex]->numVertices = quads * quads * 4;
			meshVec[meshIndex]->numIndices = indices;

			HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"concrete.dds", nullptr, &meshVec[meshIndex]->shaderRV);
			if (FAILED(res))
				ToolBox::ThrowErrorMsg("CreateDDSTextureFromFile() Failed In LoadMesh!");
			//meshVec[meshIndex]->indices = new int[5];
			//meshVec[meshIndex]->verts = new gVertex[5];
			numOfMeshs++;
}
void Graphics::LoadMesh(std::string fileName, const wchar_t* textureFile, float mesh_scale, std::vector<gMesh*>& meshArr, UINT meshIndex, bool isSkybox)
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
		meshArr[meshIndex]->isSkybox = isSkybox;

		// TEXTURE LOADING///////////////////////
		HRESULT res = CreateDDSTextureFromFile(gDev.Get(), textureFile, nullptr, &meshArr[meshIndex]->shaderRV);
		if (FAILED(res))
			ToolBox::ThrowErrorMsg("CreateDDSTextureFromFile() Failed In LoadMesh!");
		numOfMeshs++;
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
				gmesh->verts[j].pos.x = (float)vert.mData[0] * gmesh->scale;
				gmesh->verts[j].pos.y = (float)vert.mData[1] * gmesh->scale;
				gmesh->verts[j].pos.z = (float)vert.mData[2] * gmesh->scale;
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
					HRESULT res = CreateDDSTextureFromFile(gDev.Get(), (const wchar_t*)txtname, nullptr, &gmesh->shaderRV);
					//HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"Crate.dds", nullptr, &gmesh->shaderRV);
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
HRESULT Graphics::CreateBuffers(std::vector<gMesh*>& meshArr)
{
	HRESULT hr;
	// ONE BUFFER DESCRIPTOR TO RULE THEM ALL
	D3D11_BUFFER_DESC buffdesc = {};

	for (UINT index = 0; index < meshArr.size(); index++)
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

		// TODO Set additional vertex buffer slot with per instance structured information.
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
HRESULT Graphics::CreateLightBuffers(ID3D11Device* gpDev,
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
	hr = gpDev->CreateBuffer(&buffdesc, nullptr, gpDLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	////////////////// Point Light Buffer //////////////////
	//buffdesc = {};
	//buffdesc.Usage = D3D11_USAGE_DEFAULT;
	//buffdesc.ByteWidth = sizeof(gLightBuff);
	//buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//buffdesc.CPUAccessFlags = 0;
	hr = gpDev->CreateBuffer(&buffdesc, nullptr, gpPLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	////////////////// Spot Light Buffer //////////////////
	//buffdesc = {};
	//buffdesc.Usage = D3D11_USAGE_DEFAULT;
	//buffdesc.ByteWidth = sizeof(gLightBuff);
	//buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//buffdesc.CPUAccessFlags = 0;
	hr = gpDev->CreateBuffer(&buffdesc, nullptr, gpSLightBuffer->GetAddressOf());
	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Graphics::CreateShaders(std::vector<gMesh*>& meshVec)
{
	HRESULT hr;
	for (UINT i = 0; i < meshVec.size(); i++)
	{
		//if (meshVec[i]->gBlob != nullptr)
		//{
		//	meshVec[i]->gBlob.Reset();
		//	meshVec[i]->gBlob = nullptr;
		//}
		if (meshVec[i]->isSkybox)
		{
			//////////////////// create pixel shader ////////////////////
			D3DReadFileToBlob(L"PixelShaderSkyBox.cso", meshVec[i]->gBlob.GetAddressOf());
			hr = gDev->CreatePixelShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gPixelShader.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreatePixelShader Failed in InitDevice");
				return hr;
			}
		}
		else
		{
			//////////////////// create pixel shader ////////////////////
			D3DReadFileToBlob(L"PixelShader.cso", meshVec[i]->gBlob.GetAddressOf());
			hr = gDev->CreatePixelShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gPixelShader.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreatePixelShader Failed in InitDevice");
				return hr;
			}
		}

		///////////////////// bind pixel shader /////////////////////
		gCon->PSSetShader(meshVec[i]->gPixelShader.Get(), nullptr, 0u);

		if (meshVec[i]->isSkybox)
		{
			/////////////////// create vertex shader ///////////////////
			D3DReadFileToBlob(L"VertexShaderSkybox.cso", meshVec[i]->gBlob.GetAddressOf());
			hr = gDev->CreateVertexShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gVertexShader.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreateVertexShader Failed in InitDevice");
				return hr;
			}
		}
		else
		{
			/////////////////// create vertex shader ///////////////////
			D3DReadFileToBlob(L"VertexShader.cso", meshVec[i]->gBlob.GetAddressOf());
			hr = gDev->CreateVertexShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gVertexShader.GetAddressOf());
			if (FAILED(hr))
			{
				ToolBox::ThrowErrorMsg("CreateVertexShader Failed in InitDevice");
				return hr;
			}
		}
		/////////////////////// bind vertex shader ///////////////////////
		gCon->VSSetShader(meshVec[i]->gVertexShader.Get(), nullptr, 0u);

		///////////////////// create geometry shader ///////////////////
		//D3DReadFileToBlob(L"GeometryShader.cso", meshVec[i]->gBlob.GetAddressOf());
		//hr = gDev->CreateGeometryShader(meshVec[i]->gBlob->GetBufferPointer(), meshVec[i]->gBlob->GetBufferSize(), nullptr, meshVec[i]->gGeometryShader.GetAddressOf());
		//if (FAILED(hr))
		//{
		//	ToolBox::ThrowErrorMsg("CreateGeometryShader Failed in InitDevice");
		//	return hr;
		//}
		//
		///////////////////////// bind geometry shader ///////////////////////
		//gCon->GSSetShader(meshVec[i]->gGeometryShader.Get(), nullptr, 0u);

	}
	return S_OK;
}
HRESULT Graphics::CreateInputLayout(std::vector<gMesh*>& meshVec)
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
		{
			ToolBox::ThrowErrorMsg("CreateInputLayout Failed in CreateInputLayout");
			return hr;
		}
		// bind vertex target
		gCon->IASetInputLayout(meshVec[i]->gInputLayout.Get());
	}
	return S_OK;
}
#pragma endregion

#pragma region RenderCalls
void Graphics::CleanFrameBuffers(XMVECTORF32 DXCOLOR)
{
	gCon->ClearRenderTargetView(gRtv.Get(), DXCOLOR);
	gCon->ClearDepthStencilView(gDsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}
void Graphics::UpdateConstantBuffer(gMesh* mesh, XMMATRIX view, XMMATRIX cam, XMFLOAT4A cbTranslate, XMFLOAT4A cbRotate)
{
	//////////////////// Bind Index Buffer ////////////////////
	gCon->IASetIndexBuffer(mesh->gIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//////////////////// Bind Vertex Buffers ////////////////////
	const UINT strides = sizeof(gVertex);
	const UINT offset = 0u;
	gCon->IASetVertexBuffers(0u, 1u, mesh->gVertBuffer.GetAddressOf(), &strides, &offset);

	// This sends World,View,Proj,AmbientLight through the shaders.
	gConstantBuff gCB;
	globalWorld = XMMatrixTranslation(cbTranslate.x, cbTranslate.y, cbTranslate.z);
	if (cbRotate.x != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 1,0,0 }, degToRad(cbRotate.x)), globalWorld);
	if (cbRotate.y != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 0,1,0 }, degToRad(cbRotate.y)), globalWorld);
	if (cbRotate.z != 0)
		globalWorld = XMMatrixMultiply(XMMatrixRotationAxis({ 0,0,1 }, degToRad(cbRotate.z)), globalWorld);

	gCB.world = XMMatrixTranspose(globalWorld);
	gCB.view = XMMatrixTranspose(view);
	gCB.camera = XMMatrixTranspose(cam);
	gCB.perspProj = XMMatrixTranspose(globalProj);
	gCB.orthoProj = XMMatrixTranspose(globalOrthProj);
	gCB.ambientLight = XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f);
	gCB.dTime = (float)gTimer->deltaTime;
	gCon->UpdateSubresource(mesh->gConstantBuffer.Get(), 0, nullptr, &gCB, 0, 0);

	//////////////////////// Bind Shaders ////////////////////////
	// Bind buffers to pipeline so the Drawcall can access the information from setup.
	ID3D11Buffer* buffs[] = { *mesh->gConstantBuffer.GetAddressOf(),
							  *gDLightBuffer.GetAddressOf(),
							  *gPLightBuffer.GetAddressOf(),
							  *gSLightBuffer.GetAddressOf() };
	// Vertex Shader
	gCon->VSSetShader(mesh->gVertexShader.Get(), nullptr, 0u);
	gCon->VSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	// Geometry Shader
	//gCon->GSSetShader(mesh->gGeometryShader.Get(), nullptr, 0u);
	//gCon->GSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	// Pixel Shader
	gCon->PSSetShader(mesh->gPixelShader.Get(), nullptr, 0u);
	gCon->PSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShaderResources(0, 1, mesh->shaderRV.GetAddressOf());
	gCon->PSSetSamplers(0, 1, mesh->smplrState.GetAddressOf());
}
void Graphics::updateDirectionLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gDLightBuffer, XMFLOAT4A dir, XMFLOAT4A color)
{
	
	if (numOfLights == 0 || gDirectionalLights.size() == 0)
	{
		gLightBuff* gpLight = new gLightBuff();
		gpLight->dir = dir;
		gpLight->color = color;
		gDirectionalLights.push_back(*gpLight);
		gpCon->UpdateSubresource(gDLightBuffer, 0, nullptr, &gDirectionalLights[0], 0, 0);
		numOfDir_Lights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gDirectionalLights[i].dir = dir;
		gDirectionalLights[i].color = color;
		gpCon->UpdateSubresource(gDLightBuffer, 0, nullptr, &gDirectionalLights[i], 0, 0);
	}
}
void Graphics::updatePointLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gPLightBuffer, XMFLOAT4A pos, float radius, XMFLOAT4A color)
{
	if (numOfLights == 0 || gPointLights.size() == 0)
	{
		gLightBuff* gpLight = new gLightBuff();
		gpLight->pos = XMFLOAT4A(pos.x, pos.y, pos.z, radius);
		gpLight->color = color;
		gPointLights.push_back(*gpLight);
		gpCon->UpdateSubresource(gPLightBuffer, 0, nullptr, &gPointLights[0], 0, 0);
		numOfPointLights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gPointLights[i].pos = { pos.x,pos.y,pos.z,radius };
		gPointLights[i].color = color;
		gpCon->UpdateSubresource(gPLightBuffer, 0, nullptr, &gPointLights[i], 0, 0);
	}

}
void Graphics::updateSpotLight(ID3D11DeviceContext* gpCon, UINT startIndex, UINT numOfLights, ID3D11Buffer* gSLightBuffer, XMFLOAT4A pos, XMFLOAT4A dir, float width, XMFLOAT4A color)
{
	if (numOfLights == 0 || gSpotLights.size() == 0)
	{
		gLightBuff* gpLight = new gLightBuff();
		gpLight->pos = pos;
		gpLight->dir = { dir.x,dir.y,dir.z,width };
		gpLight->color = color;
		gSpotLights.push_back(*gpLight);
		gpCon->UpdateSubresource(gSLightBuffer, 0, nullptr, &gSpotLights[0], 0, 0);
		numOfSpotLights++;
		numOfTotalLights++;
		return;
	}

	for (UINT i = startIndex; i < numOfLights; i++)
	{
		gSpotLights[i].pos = pos;
		gSpotLights[i].dir = { dir.x,dir.y,dir.z,width };
		gSpotLights[i].color = color;
		gpCon->UpdateSubresource(gSLightBuffer, 0, nullptr, &gSpotLights[i], 0, 0);
	}


}
void Graphics::UpdateCamera(XMMATRIX CamToUpdate, bool listOfComs[], UINT numOfComs)
{
	for (UINT i = 0; i < numOfComs; i++)
	{

	}
}
#pragma endregion

#pragma region DeviceSetup/RenderPipeline
//vvv Init Device vvv//
HRESULT Graphics::InitDevice()
{
	gTimer->StartTimer();
	HRESULT hr;
#pragma region LOAD MODELS
	LoadMesh("ReverseCube.fbx", L"Skybox.dds", farPlane, gppMesh, numOfMeshs, true);
	LoadMesh("OfficeRoom_0.fbx", L"stainless_steel.dds", 1.0f, gppMesh, numOfMeshs, false);
	LoadMesh("Desk_1.fbx", L"stainless_steel.dds", 1.0f, gppMesh, numOfMeshs, false);
	LoadMesh("Ball_Lamp_0.fbx", L"stainless_steel.dds", 1.0f, gppMesh, numOfMeshs, false);
	LoadMesh("SpaceShip_3.fbx", L"stainless_steel.dds", 0.04f, gppMesh, numOfMeshs, false);
	CreateFloor(gppMesh, numOfMeshs,(floorW = 10u),(floorD = 10u));
	//LoadMesh("Cube.fbx", L"Crate.dds", .05f, gppMesh, numOfMeshs, false);
#pragma endregion

#pragma region Create_Buffers
	CreateBuffers(gppMesh);

	//////////////// Light Buffer ///////////////////
	hr = CreateLightBuffers(gDev.Get(), &gDLightBuffer, &gPLightBuffer, &gSLightBuffer);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateLightBuffers Failed in InitDevice");
		return hr;
	}
#pragma endregion

#pragma region Create Geometry,Vertex,PixelShaders,InputLayout
	CreateShaders(gppMesh);

	CreateInputLayout(gppMesh);
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

	//// bind render target
	gCon->OMSetRenderTargets(1, gRtv.GetAddressOf(), gDsv.Get());

	// configer viewport
	D3D11_VIEWPORT port_one;
	port_one.Width = 640.0f;
	port_one.Height = 720.0f;
	port_one.MinDepth = 0.0f;
	port_one.MaxDepth = 1.0f;
	port_one.TopLeftX = 0.0f;
	port_one.TopLeftY = 0.0f;

	D3D11_VIEWPORT port_two;
	port_two.Width = 640.0f;
	port_two.Height = 720.0f;
	port_two.MinDepth = 0.0f;
	port_two.MaxDepth = 1.0f;
	port_two.TopLeftX = 640.0f;
	port_two.TopLeftY = 0.0f;

	vp[0] = port_one; //{ port_one, port_two };
	vp[1] = port_two;

	//D3D11_VIEWPORT vp;
	//vp.Width = 1280.0f;
	//vp.Height = 720.0f;
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//vp.TopLeftX = 0.0f;
	//vp.TopLeftY = 0.0f;
	//gCon->RSSetViewports(1u, &vp);
#pragma endregion

	return hr;
}
//^^^ Init Device ^^^//

//vvv Render vvv///
void Graphics::Render()
{
	CleanFrameBuffers();
	float deltaT = (float)gTimer->TimeSinceStart();

/*TODO	
	figure out why i cant change the lightbuffers
	maybe its because of the buffer size that has to match vertex but that should be right.
*/
#pragma region Update Lights

	// SUN DIRECTIONAL LIGHT
	sunPos += 0.1f;
	updateDirectionLight(gCon.Get(), 0u, 1u, gDLightBuffer.Get(),
		XMFLOAT4A(0.0f, cosf(degToRad(sunPos)), sinf(degToRad(sunPos)), 0.0f), // direction
		XMFLOAT4A(0.65f, 0.55f, 0.35f, 1.0f)); // color

	// LAMP POINT LIGHT
	updatePointLight(gCon.Get(), 0u, 1u, gPLightBuffer.Get(),
		XMFLOAT4A(10.5f, 5.25f, 4.25f, 0.0f), // position
		15.0f, // radius
		XMFLOAT4A(0.95f, 0.95f, 0.55f, PointLight_A)); // color

	// CAMREA SPOT LIGHT
	//XMFLOAT4A camPos;
	//XMStoreFloat4A(&camPos, Camera_1.r[3]);
	//XMFLOAT4A camForwDir;
	//XMStoreFloat4A(&camForwDir, XMVector4Transform(XMVectorSet(0, 0, 1, 0), Camera_1));
	//updateSpotLight(gCon.Get(), 0u, 1u, gSLightBuffer.Get(),
	//	camPos, // position
	//	camForwDir, // direction
	//	0.99f, // cone ratio
	//	XMFLOAT4A(1.0f, 0.0f, 0.0f, 1.0f)); // color
#pragma endregion

#pragma region Update Constant Buffer and draw objects
	XMFLOAT4A move, rotate;
#pragma region Draw 1
	// Viewport 1
	gCon->RSSetViewports(1u, &vp[0]);

	// TO DO: UpdateConstantBuffer will take a camera from the array of cameras who's index will be based on the last inputed value from WinMain.cpp
	// Skybox
	XMFLOAT4A camPos_1;
	XMStoreFloat4A(&camPos_1, Camera_1.r[3]);
	XMFLOAT4A camForwDir_1;
	XMStoreFloat4A(&camForwDir_1, XMVector4Transform(XMVectorSet(0, 0, 1, 0), Camera_1));
	move = camPos_1;
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[0],globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[0]->numIndices, 0u, 0);

	// room
	move = XMFLOAT4A(10.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[1],globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[1]->numIndices, 0u, 0);

	// Desk
	move = XMFLOAT4A(10.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[2], globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[2]->numIndices, 0u, 0);

	// Ball Lamp on desk
	move = XMFLOAT4A(10.5f, 4.25f, 4.25f, 0.0f);
	rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[3],globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[3]->numIndices, 0u, 0);

	// Spaceship on desk
	move = XMFLOAT4A(9.5f, 3.65f, -3.5f, 0.0f);
	rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[4],globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[4]->numIndices, 0u, 0);

	// Box for testing
	//move = XMFLOAT4A(-5.0f, 0.0f, 2.0f, 0.0f);
	//rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	//UpdateConstantBuffer(gppMesh[4], move, rotate);
	//gCon->DrawIndexed((UINT)gppMesh[4]->numIndices, 0u, 0);

	// floor
	move = XMFLOAT4A(-((floorD*floorW)/2.0f), 0.0f, -((floorD * floorW) / 2.0f), 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[5],globalView_1, Camera_1, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[5]->numIndices, 0u, 0);

#pragma endregion

#pragma region draw 2
	// Viewport 2
	gCon->RSSetViewports(1u, &vp[1]);

	// TO DO: UpdateConstantBuffer will take a camera from the array of cameras who's index will be based on the last inputed value from WinMain.cpp
	XMFLOAT4A camPos_2;
	XMStoreFloat4A(&camPos_2, Camera_2.r[3]);
	XMFLOAT4A camForwDir_2;
	XMStoreFloat4A(&camForwDir_2, XMVector4Transform(XMVectorSet(0, 0, 1, 0), Camera_2));
	move = camPos_2;
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[0],globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[0]->numIndices, 0u, 0);

	// room
	move = XMFLOAT4A(10.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[1],globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[1]->numIndices, 0u, 0);

	// Desk
	move = XMFLOAT4A(10.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[2], globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[2]->numIndices, 0u, 0);

	// Ball Lamp on desk
	move = XMFLOAT4A(10.5f, 4.23f, 4.25f, 0.0f);
	rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[3],globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[3]->numIndices, 0u, 0);

	// Spaceship on desk
	move = XMFLOAT4A(9.5f, 3.65f, -3.5f, 0.0f);
	rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[4],globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[4]->numIndices, 0u, 0);

	// Box for testing
	//move = XMFLOAT4A(-5.0f, 0.0f, 2.0f, 0.0f);
	//rotate = XMFLOAT4A(0.0f, -30.0f, 0.0f, 0.0f);
	//UpdateConstantBuffer(gppMesh[4], move, rotate);
	//gCon->DrawIndexed((UINT)gppMesh[4]->numIndices, 0u, 0);

	// floor
	move = XMFLOAT4A(-((floorD * floorW) / 2.0f), 0.0f, -((floorD * floorW) / 2.0f), 0.0f);
	rotate = XMFLOAT4A(0.0f, 0.0f, 0.0f, 0.0f);
	UpdateConstantBuffer(gppMesh[5],globalView_2, Camera_2, move, rotate);
	gCon->DrawIndexed((UINT)gppMesh[5]->numIndices, 0u, 0);

#pragma endregion

#pragma endregion

	gSwap->Present(1u, 0u);
}
//^^^ Render ^^^//
#pragma endregion