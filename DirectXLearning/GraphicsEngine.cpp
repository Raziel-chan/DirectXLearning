#include "GraphicsEngine.h"

// Constructor
GraphicsEngine::GraphicsEngine()
{
	// Initialize the pointers to nullptr
	// This is important because the ComPtr class will call Release() on the pointer when it goes out of scope
}

// Destructor
GraphicsEngine::~GraphicsEngine()
{
	// ComPtr will automatically release the resources when it goes out of scope
}

// Initialize the Directx for our window
bool GraphicsEngine::Initialize(HWND hwnd, int windowWidth, int windowHeight)
{
	UINT createDeviceFlags = 0;
	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	// First, we will set up the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit RGBA color
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // 60 Hz refresh rate
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // 60 Hz refresh rate
	swapChainDesc.SampleDesc.Count = 1; // No anti-aliasing
	swapChainDesc.SampleDesc.Quality = 0; // No anti-aliasing
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2; // Double buffering
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Log that we're starting device creation
	OutputDebugString(L"Creating Device and SwapChain...\n");

	// Create the device, device context and swap chain
	//UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG; // enable debugging
	
	// Try to create the device and swap chain
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr, // Use the default adapter
		D3D_DRIVER_TYPE_HARDWARE, // Use the GPU
		nullptr, // No software device
		createDeviceFlags, // enable debugging 
		nullptr, // use default feature level 
		0, // no feature levels
		D3D11_SDK_VERSION, // SDK version
		&swapChainDesc, // swap chain description
		m_swapChain.GetAddressOf(), // swap chain output
		m_device.GetAddressOf(), // device output
		nullptr, // don't care about feature level
		m_context.GetAddressOf() // device context output
	);
	// Check if the creation was successful
	if (FAILED(result))
	{
		// If the creation failed, we need to display an error message
		MessageBox(hwnd, L"Failed to create device and swap chain!", L"Error", MB_OK);
		return false;
	}

	OutputDebugString(L"Device and SwapChain created successfully\n");

	// Create the render target
	if (!CreateRenderTarget())
	{
		OutputDebugString(L"Failed to create render target\n");
		return false;
	}

	OutputDebugString(L"Render target created successfully\n");

	// set up the viewport
	D3D11_VIEWPORT viewport = {}; // create a viewport
	viewport.Width = static_cast<float>(windowWidth); // width of the viewport
	viewport.Height = static_cast<float>(windowHeight);	// height of the viewport
	viewport.MinDepth = 0.0f; // the closest an object can be on the screen
	viewport.MaxDepth = 1.0f; // the farthest an object can be on the screen

	if (m_context) {
		m_context->RSSetViewports(1, &viewport); // set the viewport
		OutputDebugString(L"Viewport set successfully\n");
	}
	else {
		OutputDebugString(L"Failed to set viewport\n");
		return false;
	}

	// Create the shaders
	if (!CreateShaders())
	{
		OutputDebugString(L"Failed to create shaders\n");
		return false;
	}

	// Create the triangle
	if (!CreateTriangle())
	{
		OutputDebugString(L"Failed to create triangle\n");
		return false;
	}

	// Create the constant buffer
	if (!CreateConstantBuffer())
	{
		OutputDebugString(L"Failed to create constant buffer\n");
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateRenderTarget()
{
	// Get the back buffer from the swap chain
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())); // get the back buffer
	// Check if we got the back buffer
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to get back buffer\n");
		return false;
	}

	// Create the render target view from the back buffer
	result = m_device->CreateRenderTargetView(
		backBuffer.Get(),  // The texture to create the view from
		nullptr,          // Use default view settings
		m_renderTarget.GetAddressOf()  // Store the created view
	);

	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create render target view\n");
		return false;
	}

	// set the render target
	m_context->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);

	return true;
}

void GraphicsEngine::BeginFrame()
{
	if (!m_renderTarget)
	{
		OutputDebugString(L"Render target is null in BeginFrame\n");
		return;
	}

	// Clear the screen to a dark blue color
	float clearColor[4] = { 0.0f, 0.1f, 0.2f, 1.0f }; // dark blue
	m_context->ClearRenderTargetView(m_renderTarget.Get(), clearColor); // clear the render target

	// Update the constant buffer
	ConstantBufferData cbData;

	// create a simple rotation for the triangle
	static float rotationAngle = 0.0f;
	rotationAngle += 0.01f; // increment the rotation angle

	// set up the transformation matrices
	cbData.world = DirectX::XMMatrixRotationY(rotationAngle); // rotation matrix
	cbData.view = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f), // camera position
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), // look at point
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f) // up vector
	);

	cbData.projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV4, // 45 degree field of view
		static_cast<float>(800) / static_cast<float>(600), // aspect ratio
		0.1f, // near plane
		100.0f // far plane
	);

	// transpose the matrices for the shader
	cbData.world = DirectX::XMMatrixTranspose(cbData.world);
	cbData.view = DirectX::XMMatrixTranspose(cbData.view);
	cbData.projection = DirectX::XMMatrixTranspose(cbData.projection);

	// Update the constant buffer
	m_context->UpdateSubresource(
		m_constantBuffer.Get(), // the constant buffer
		0, // subresource index
		nullptr, // no box
		&cbData, // pointer to the data
		0, // no row pitch
		0 // no depth pitch
	);

	// Set the constant buffer to the vertex shader
	m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf()); // set the constant buffer

	// set the vertex buffer
	UINT stride = sizeof(Vertex); // size of each vertex
	UINT offset = 0; // no offset
	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset); // set the vertex buffer

	// set the input layout
	m_context->IASetInputLayout(m_inputLayout.Get()); // set the input layout

	// set the primitive topology
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	// set the shaders
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0); // set the vertex shader
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0); // set the pixel shader
}

void GraphicsEngine::EndFrame()
{
	//draw the triangle
	m_context->Draw(3, 0); // draw the triangle (3 vertices, starting at index 0)

	// Present the frame to the screen
	m_swapChain->Present(1, 0);
}

bool GraphicsEngine::CreateShaders() {
	// load and compile the vertex shader
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr, //optional defines
		nullptr, //optonal include 
		"main", // entry point function name
		"vs_5_0", // shader model
		D3DCOMPILE_DEBUG, // shader compile options
		0, // effect compile options
		vertexShaderBlob.GetAddressOf(), // compiled shader
		errorBlob.GetAddressOf() // error messages
	);

	if (FAILED(hr)) {
		// if the shader failed to compile, display an error message
		if (errorBlob) {
			OutputDebugStringA(reinterpret_cast<const char*>(
				errorBlob->GetBufferPointer()));
		}
		return false;
	}

	// create the shader object
	hr = m_device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		nullptr,
		m_vertexShader.GetAddressOf()
	);

	// check if the shader was created successfully
	if (FAILED(hr)) {
		return false;
	}

	// load and compile the pixel shader
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	hr = D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr, //optional defines
		nullptr, //optonal include 
		"main", // entry point function name
		"ps_5_0", // shader model
		D3DCOMPILE_DEBUG, // shader compile options
		0, // effect compile options
		pixelShaderBlob.GetAddressOf(), // compiled shader
		errorBlob.GetAddressOf() // error messages
	);

	if (FAILED(hr)) {
		// if the shader failed to compile, display an error message
		if (errorBlob) {
			OutputDebugStringA(reinterpret_cast<const char*>(
				errorBlob->GetBufferPointer()));
		}
		return false;
	}	

	// Define input layout (map to our vertex structure)
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	hr = m_device->CreateInputLayout(
		layout, // input layout description
		ARRAYSIZE(layout), // number of elements in the layout
		vertexShaderBlob->GetBufferPointer(), // compiled vertex shader
		vertexShaderBlob->GetBufferSize(), // size of the compiled shader
		m_inputLayout.GetAddressOf() // input layout output
	);

	// check if the shader was created successfully
	if (FAILED(hr)) {
		return false;
	}

	return SUCCEEDED(hr);
}

bool GraphicsEngine::CreateTriangle() {
	//define the vertices of our triangle
	Vertex triangleVertices[] = {
		{ DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // top vertex (red)
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // bottom left vertex (green)
		{ DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) } // bottom right vertex (blue)
	};

	// create the vertex buffer description
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // default usage
	vertexBufferDesc.ByteWidth = sizeof(triangleVertices); // size of the buffer
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // bind as a vertex buffer
	vertexBufferDesc.CPUAccessFlags = 0; // no CPU access

	//Create the initial data
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = triangleVertices; // pointer to the vertex data

	//Actually create the vertex buffer
	HRESULT hr = m_device->CreateBuffer(
		&vertexBufferDesc, // buffer description
		&vertexData, // initial data
		m_vertexBuffer.GetAddressOf() // vertex buffer output
	);



	// Check if the vertex buffer was created successfully
	return SUCCEEDED(hr);
}

bool GraphicsEngine::CreateConstantBuffer()
{
	// Create the constant buffer description
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT; // default usage
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferData); // size of the buffer
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // bind as a constant buffer
	constantBufferDesc.CPUAccessFlags = 0; // no CPU access

	// Create the constant buffer
	HRESULT hr = m_device->CreateBuffer(
		&constantBufferDesc, // buffer description
		nullptr, // no initial data
		m_constantBuffer.GetAddressOf() // constant buffer output
	);

	return SUCCEEDED(hr);
}