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
	gCon->ClearRenderTargetView(gRtv.Get(), DirectX::Colors::DarkSeaGreen);

	// Set Shaders and Draw
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);
	gCon->Draw((UINT)numVerts, 0u);

	// Present ""Finished"" buffer to screen
	gSwap->Present(1u, 0u);
}


HRESULT Graphics::InitDevice()
{
	HRESULT hr;


	Graphics::gVertex tri[6] =
	{
		{XMFLOAT4(-0.5f, 0.5f, 0.0f, 1.0f)},
		{XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f)},
		{XMFLOAT4(-0.5f,-0.5f, 0.0f, 1.0f)},
		{XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f)},
		{XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f)},
		{XMFLOAT4(0.5f,-0.5f, 0.0f, 1.0f)},

	};

	UINT array_size = ARRAYSIZE(tri);
	numVerts = 6;

	// create vertex buffer
	D3D11_BUFFER_DESC buffdesc = {};
	buffdesc.Usage = D3D11_USAGE_DEFAULT;
	buffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffdesc.CPUAccessFlags = 0u;
	buffdesc.MiscFlags = 0u;
	buffdesc.ByteWidth = sizeof(gVertex) * array_size;
	buffdesc.StructureByteStride = sizeof(gVertex);
	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = tri;
	hr = gDev->CreateBuffer(&buffdesc, &subData, gVertBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateBuffer Failed in DrawTriangle");
		return hr;
	}

	// bind vertex buffers to pipeline
	const UINT strides = sizeof(gVertex);
	const UINT offset = 0u;
	gCon->IASetVertexBuffers(0u, 1u, gVertBuffer.GetAddressOf(), &strides, &offset);

	// create pixel shader
	D3DReadFileToBlob(L"PixelShader.cso", &gBlob);
	hr = gDev->CreatePixelShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gPixelShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreatePixelShader Failed in DrawTriangle");
		return hr;
	}

	// bind pixel shader
	gCon->PSSetShader(gPixelShader.Get(), nullptr, 0u);

	// create vertex shader.
	D3DReadFileToBlob(L"VertexShader.cso", &gBlob);
	hr = gDev->CreateVertexShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gVertexShader);
	if (FAILED(hr))
	{
		ToolBox::ThrowErrorMsg("CreateVertexShader Failed in DrawTriangle");
		return hr;
	}

	// bind vertex.
	gCon->VSSetShader(gVertexShader.Get(), nullptr, 0u);

	// input vertex layout
	const D3D11_INPUT_ELEMENT_DESC ildes[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
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
