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
	//for (int i = 0; i < numOfMeshs; i++)
	//{
	//	delete gppMesh;
	//}
	delete gppMesh;
}
#pragma endregion

#pragma region DeviceSetup/RenderPipeline
HRESULT Graphics::InitDevice()
{
	HRESULT hr;

#pragma region Create_Buffers
	// ONE BUFFER TO RULE THEM ALL
	D3D11_BUFFER_DESC buffdesc = {};

	//////////////////// Constant Buffer ////////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gConstantBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gConstantBuffer);
	if (FAILED(hr))
		return hr;

	/////////////// Directional Light Buffer ///////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gDirLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gDLightBuffer);
	if (FAILED(hr))
		return hr;

	////////////////// Point Light Buffer //////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gPntLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gPLightBuffer);
	if (FAILED(hr))
		return hr;

	////////////////// Spot Light Buffer //////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(gSptLightBuff);
	buffdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	hr = gDev->CreateBuffer(&buffdesc, nullptr, &gSLightBuffer);
	if (FAILED(hr))
		return hr;


	//////////////////// Vertex Buffer ////////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffdesc.CPUAccessFlags = 0u;
	buffdesc.MiscFlags = 0u;
	buffdesc.ByteWidth = sizeof(gVertex) * gppMesh->numVertices;
	buffdesc.StructureByteStride = sizeof(gVertex);
	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = gppMesh->verts;
	hr = gDev->CreateBuffer(&buffdesc, &subData, gVertBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateVertexBuffer Failed in InitDevice");
		return hr;
	}

	///////////////////// Index Buffer /////////////////////
	buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.ByteWidth = sizeof(int) * gppMesh->numIndices;
	buffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffdesc.CPUAccessFlags = 0;
	subData = {};
	subData.pSysMem = gppMesh->indices;
	hr = gDev->CreateBuffer(&buffdesc, &subData, &gIndexBuffer);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateIndexBuffer Failed in InitDevice");
		return hr;
	}

	//////////////////// Bind Index Buffer ////////////////////
	gCon->IASetIndexBuffer(gIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//////////////////// Bind Vertex Buffers ////////////////////
	const UINT strides = sizeof(gVertex);
	const UINT offset = 0u;
	gCon->IASetVertexBuffers(0u, 1u, gVertBuffer.GetAddressOf(), &strides, &offset);
#pragma endregion

#pragma region Vertex/PixelShaders
	//////////////////// create pixel shader ////////////////////
	D3DReadFileToBlob(L"PixelShader.cso", &gBlob);
	hr = gDev->CreatePixelShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gPixelShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreatePixelShader Failed in InitDevice");
		return hr;
	}

	///////////////////// bind pixel shader /////////////////////
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);

	/////////////////// create vertex shader ///////////////////
	D3DReadFileToBlob(L"VertexShader.cso", &gBlob);
	hr = gDev->CreateVertexShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gVertexShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateVertexShader Failed in InitDevice");
		return hr;
	}

	/////////////////////// bind vertex ///////////////////////
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);

	/////////////////// input vertex layout ///////////////////
	const D3D11_INPUT_ELEMENT_DESC ildes[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"crate.dds", nullptr, &shaderRV);
	HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"carbonfiber.dds", nullptr, &shaderRV);
	//HRESULT res = CreateDDSTextureFromFile(g_pd3dDevice, (const wchar_t*)textName, nullptr,&shadRes);
	if (FAILED(res))
		return res;

	// Creating sample state
	D3D11_SAMPLER_DESC texDes;
	texDes.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	texDes.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	texDes.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
		ToolBox::ThrowErrorMsg("CreateInputLayout Failed in InitDevice");
		return hr;
	}
	// bind vertex target
	gCon->IASetInputLayout(gInputLayout.Get());
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
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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
	//gCon->OMSetRenderTargets(1, gRtv.GetAddressOf(), nullptr);

	// configer viewport
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
	//////////////// TIMER ////////////////
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	deltaT = (timeCur - timeStart) / 1000.0f;

	/////////////////////////// Clear Buffers ///////////////////////////
	gCon->ClearRenderTargetView(gRtv.Get(), DirectX::Colors::DarkGray);
	gCon->ClearDepthStencilView(gDsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	///////////////////// Constant Buffer Setup /////////////////////
	// This sends World,View,Proj,AmbientLight through the shaders.
	gConstantBuff gCB;
	gCB.dTime = deltaT;
	gCB.world = XMMatrixTranslation(0.0f, -10.0f, 50.0f);
	gCB.world = XMMatrixMultiply(XMMatrixRotationAxis({ 0,1,0 }, degToRad(deltaT * 15.0f)), gCB.world);
	gCB.world = XMMatrixTranspose(gCB.world);
	gCB.view = XMMatrixTranspose(globalView);
	gCB.proj = XMMatrixTranspose(globalProj);
	gCB.ambientLight = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	gCon->UpdateSubresource(gConstantBuffer.Get(), 0, nullptr, &gCB, 0, 0);

	///////////////// Directional Light Buffer Setup /////////////////
	gDirectional.dir[0] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	gDirectional.color[0] = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	gCon->UpdateSubresource(gDLightBuffer.Get(), 0, nullptr, &gDirectional, 0, 0);

	///////////////// Directional Light Buffer Setup /////////////////
	gDirectional.dir[1] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	gDirectional.color[1] = XMFLOAT4(0.8f, 0.4f, 0.2f, 1.0f);
	gCon->UpdateSubresource(gDLightBuffer.Get(), 0, nullptr, &gDirectional, 0, 0);

	/////////////////// Point Light Buffer Setup /////////////////////
	XMVECTOR nx = XMVectorSet(sin(deltaT * 3.0f) * 50.0f, 10.0f, (sin(deltaT * 1.5f) * 50.0f) + 10.0f, 0.0f);
	XMStoreFloat4(&gPointLight.pos, nx);
	gPointLight.color = XMFLOAT4(0.0f, 0.5f, 0.0f, PointLight_A);
	gCon->UpdateSubresource(gPLightBuffer.Get(), 0, nullptr, &gPointLight, 0, 0);

	/////////////////// Spot Light Buffer Setup /////////////////////
	//XMVECTOR nx = XMVectorSet(sin(deltaT * 3) * 50, 10, (sin(deltaT * 1.5) * 50) + 10, 0.0f);
	XMStoreFloat4(&gSpotLight.pos, Camera.r[3]);
	//gSpotLight.pos = XMFLOAT4(0.0f, 70.0f, 40.0f, 0.0f);
	//gSpotLight.pos.y += 5.0f;
	gSpotLight.pos.z += 10.0f;
	XMVECTOR tmp = XMVectorSet(0.0f, 0.0f, 1.0f,0.0f);
	tmp = XMVector4Transform(tmp, Camera);
	XMStoreFloat4(&gSpotLight.coneDir, tmp);
	gSpotLight.coneWidth_R = XMFLOAT4(SpotLightWidth,0.0f,0.0f,0.0f);
	gSpotLight.color = XMFLOAT4(1.0f, 0.1f, 0.1f, 1.0f);
	gCon->UpdateSubresource(gSLightBuffer.Get(), 0, nullptr, &gSpotLight, 0, 0);

	//////////////////////// Bind Shaders ////////////////////////
	// Bind buffers to pipeline so the Drawcall can access the information from setup.
	ID3D11Buffer* buffs[] = { *gConstantBuffer.GetAddressOf(),
							  *gDLightBuffer.GetAddressOf(),
							  *gPLightBuffer.GetAddressOf(),
							  *gSLightBuffer.GetAddressOf() };
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);
	gCon->VSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);
	gCon->PSSetConstantBuffers(0u, (UINT)ARRAYSIZE(buffs), buffs);
	gCon->PSSetShaderResources(0, 1, shaderRV.GetAddressOf());
	gCon->PSSetSamplers(0, 1, smplrState.GetAddressOf());

	gCon->DrawIndexed((UINT)gppMesh->numVertices, 0u, 0);

	gSwap->Present(1u, 0u);
}
#pragma endregion

#pragma region Mesh / Texture / File IO
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
					HRESULT res = CreateDDSTextureFromFile(gDev.Get(), L"Crate.dds", nullptr, &shaderRV);
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
void Graphics::LoadMesh(std::string fileName, float mesh_scale, gMesh** meshArr, UINT meshIndex)
{
	if (meshArr[meshIndex] == nullptr) // If empty, fill.
	{
		meshArr[meshIndex] = new gMesh();
		meshArr[meshIndex]->scale = mesh_scale;
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
		ProcessFBXMesh(lScene->GetRootNode(), meshArr[meshIndex]);
		numOfMeshs++;
		return;
	}
	else
	{
		ToolBox::ThrowErrorMsg("LoadMesh() failed::meshArr was not nullptr.\nWe do not overwrite memory in this house!");
	}
}
#pragma endregion