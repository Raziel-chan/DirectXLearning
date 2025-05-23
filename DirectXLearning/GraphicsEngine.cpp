#include "GraphicsEngine.h"
#include "Window.h"

// Constructor
GraphicsEngine::GraphicsEngine()
{
	// Initialize the pointers to nullptr
	// This is important because the ComPtr class will call Release() on the pointer when it goes out of scope
	m_device = nullptr;
	m_context = nullptr;
	m_swapChain = nullptr;
	m_renderTarget = nullptr;
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_inputLayout = nullptr;
	m_vertexBuffer = nullptr;
	m_constantBuffer = nullptr;
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
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;  // Remove any extra flags that might cause issues

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

	// Create a simple blend state
D3D11_BLEND_DESC blendDesc = {};
blendDesc.RenderTarget[0].BlendEnable = FALSE;
blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
m_device->CreateBlendState(&blendDesc, blendState.GetAddressOf());

float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
m_context->OMSetBlendState(blendState.Get(), blendFactor, 0xFFFFFFFF);

	// Add this to your Initialize function after creating device
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;  // IMPORTANT: Don't cull any triangles
	rastDesc.FrontCounterClockwise = FALSE;  // This is the default
	rastDesc.DepthClipEnable = TRUE;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	HRESULT hr = m_device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());
	if (SUCCEEDED(hr)) {
		m_context->RSSetState(rastState.Get());
		OutputDebugString(L"Rasterizer state set\n");
	}


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

void GraphicsEngine::BeginFrame(const Window& window)
{
	if (!m_renderTarget)
	{
		OutputDebugString(L"Render target is null in BeginFrame\n");
		return;
	}

	// Clear with a VERY different color - bright purple for visibility 
	float clearColor[4] = { 0.5f, 0.0f, 0.5f, 1.0f }; // Bright purple
	m_context->ClearRenderTargetView(m_renderTarget.Get(), clearColor); // clear the render target

	// *** IMPORTANT: Re-bind render target every frame ***
	m_context->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);

	// Set up pipeline
	// Set up vertex buffer with proper stride
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	m_context->IASetInputLayout(m_inputLayout.Get());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	
	// Update the constant buffer
	// create a simple rotation for the triangle
	ConstantBufferData cbData;
	static float rotationAngle = 0.0f;
	rotationAngle += 0.01f; // increment the rotation angle

	// Create world matrix with rotation
	cbData.world = DirectX::XMMatrixRotationRollPitchYaw(m_rotationX, m_rotationY, 0.0f);

	// Create view matrix from camera position
	cbData.view = DirectX::XMMatrixLookAtLH(
		m_cameraPosition,
		m_cameraTarget,
		m_upVector
	);

	// Create projection matrix
	cbData.projection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XM_PIDIV4, // 45 degrees
		static_cast<float>(window.GetWidth()) / static_cast<float>(window.GetHeight()),
		0.1f, // Near plane
		100.0f // Far plane
	);

	// transpose the matrices for the shader
	cbData.world = DirectX::XMMatrixTranspose(cbData.world);
	cbData.view = DirectX::XMMatrixTranspose(cbData.view);
	cbData.projection = DirectX::XMMatrixTranspose(cbData.projection);

	m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
	// Update the constant buffer
	m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cbData, 0, 0);
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

	// Create the pixel shader object
	hr = m_device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr,
		m_pixelShader.GetAddressOf()
	);

	if (FAILED(hr)) {
		return false;
	}

	// Define input layout (map to our vertex structure)
	D3D11_INPUT_ELEMENT_DESC layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
		{ DirectX::XMFLOAT3(0.0f, 0.9f, 0.5f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },  // Top (YELLOW)
		{ DirectX::XMFLOAT3(-0.9f, -0.9f, 0.5f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // Bottom left (CYAN)
		{ DirectX::XMFLOAT3(0.9f, -0.9f, 0.5f), DirectX::XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f) }   // Bottom right (LIGHT RED)
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

	for (int i = 0; i < 3; i++) {
		std::ostringstream oss;
		oss << "Vertex " << i << " Position: ("
			<< triangleVertices[i].position.x << ", "
			<< triangleVertices[i].position.y << ", "
			<< triangleVertices[i].position.z << ") Color: ("
			<< triangleVertices[i].color.x << ", "
			<< triangleVertices[i].color.y << ", "
			<< triangleVertices[i].color.z << ", "
			<< triangleVertices[i].color.w << ")" << std::endl;
		OutputDebugStringA(oss.str().c_str());
	}

	D3D11_BUFFER_DESC desc;
	m_vertexBuffer->GetDesc(&desc);
	std::ostringstream oss;
	oss << "Vertex buffer size: " << desc.ByteWidth << " bytes, for "
		<< (desc.ByteWidth / sizeof(Vertex)) << " vertices" << std::endl;
	OutputDebugStringA(oss.str().c_str());



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

void GraphicsEngine::ProcessKeyboardInput(const Window& window, float deltaTime) {
	// Rotate triangle with arrow keys
	if (window.IsKeyPressed(VK_UP)) {
		m_rotationX += m_rotationSpeed * deltaTime;
	}
	if (window.IsKeyPressed(VK_DOWN)) {
		m_rotationX -= m_rotationSpeed * deltaTime;
	}
	if (window.IsKeyPressed(VK_LEFT)) {
		m_rotationY -= m_rotationSpeed * deltaTime;
	}
	if (window.IsKeyPressed(VK_RIGHT)) {
		m_rotationY += m_rotationSpeed * deltaTime;
	}
	// Camera movement with WASD
	DirectX::XMVECTOR forward = DirectX::XMVector3Normalize(
		DirectX::XMVectorSubtract(m_cameraTarget, m_cameraPosition)
	);

	DirectX::XMVECTOR right = DirectX::XMVector3Normalize(
		DirectX::XMVector3Cross(forward, m_upVector)
	);

	if (window.IsKeyPressed('W')) {
		m_cameraPosition = DirectX::XMVectorAdd(
			m_cameraPosition,
			DirectX::XMVectorScale(forward, m_moveSpeed * deltaTime)
		);
		m_cameraTarget = DirectX::XMVectorAdd(
			m_cameraTarget,
			DirectX::XMVectorScale(forward, m_moveSpeed * deltaTime)
		);
	}
	if (window.IsKeyPressed('S')) {
		m_cameraPosition = DirectX::XMVectorSubtract(
			m_cameraPosition,
			DirectX::XMVectorScale(forward, m_moveSpeed * deltaTime)
		);
		m_cameraTarget = DirectX::XMVectorSubtract(
			m_cameraTarget,
			DirectX::XMVectorScale(forward, m_moveSpeed * deltaTime)
		);
	}
	if (window.IsKeyPressed('A')) {
		m_cameraPosition = DirectX::XMVectorSubtract(
			m_cameraPosition,
			DirectX::XMVectorScale(right, m_moveSpeed * deltaTime)
		);
		m_cameraTarget = DirectX::XMVectorSubtract(
			m_cameraTarget,
			DirectX::XMVectorScale(right, m_moveSpeed * deltaTime)
		);
	}
	if (window.IsKeyPressed('D')) {
		m_cameraPosition = DirectX::XMVectorAdd(
			m_cameraPosition,
			DirectX::XMVectorScale(right, m_moveSpeed * deltaTime)
		);
		m_cameraTarget = DirectX::XMVectorAdd(
			m_cameraTarget,
			DirectX::XMVectorScale(right, m_moveSpeed * deltaTime)
		);
	}
}

void GraphicsEngine::ProcessMouseInput(const Window& window, float deltaTime) {
	// Use right mouse button to rotate the view
	if (window.IsMouseButtonPressed(1)) { // 1 = right button
		float mouseSensitivity = 0.005f;
		float yawChange = window.GetMouseDeltaX() * mouseSensitivity;
		float pitchChange = window.GetMouseDeltaY() * mouseSensitivity;

		// Create rotation matrices
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitchChange, yawChange, 0);

		// Get the vector from camera to target
		DirectX::XMVECTOR directionVector = DirectX::XMVectorSubtract(m_cameraTarget, m_cameraPosition);

		// Apply rotation to this vector
		directionVector = DirectX::XMVector3Transform(directionVector, rotationMatrix);

		// Set new target position based on rotated vector
		m_cameraTarget = DirectX::XMVectorAdd(m_cameraPosition, directionVector);
	}
}