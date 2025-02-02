#pragma once
#include <windows.h> // Windows header
#include <d3d11.h> // Main DirectX 11 header
#include <wrl.h> // ComPtr smart pointers
#include <d3dcompiler.h> // for shader compilation
#include <sstream> // for error messages

// We need to link with the DirectX libraries
#pragma comment(lib, "d3d11.lib")
// We also need to link with the DirectX shader compiler
#pragma comment(lib, "d3dcompiler.lib")

// Class to handle the DirectX rendering
class GraphicsEngine
{
public:
	GraphicsEngine();
	~GraphicsEngine();

	// Initialise the Directx for our window
	bool Initialize(HWND hwnd, int windowWidth, int windowHeight);

	// Clear the screen and prepare for drawing
	void BeginFrame();
	// Present the finished frame to the screen
	void EndFrame();

private:
	// Smart pointers for DirectX resources
	// These will automatically release the resources when they go out of scope
	Microsoft::WRL::ComPtr<ID3D11Device> m_device; // Creates ressources (textures, buffers, shaders)
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context; // Sends commands to the GPU (Set up rendering pipeline)
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain; // Presents the rendered image to the screen (screen buffering)
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget; // our canvas (where we draw)

	// Add shader-related members
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader; // Vertex shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader; // Pixel shader
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout; // Input layout

	bool CreateShaders(); // Helper function to create the shaders

	// Helper function to create the render target 
	bool CreateRenderTarget();
};