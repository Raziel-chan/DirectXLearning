#pragma once
#include <windows.h> // Windows header
#include <d3d11.h> // Main DirectX 11 header
#include <wrl.h> // ComPtr smart pointers

// We need to link with the DirectX libraries
#pragma comment(lib, "d3d11.lib")

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

	// Helper function to create the render target 
	bool CreateRenderTarget();
};