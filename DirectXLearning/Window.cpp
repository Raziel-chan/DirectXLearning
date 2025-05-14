#include <windows.h>
#include <windowsx.h> // Add this at the top of Window.cpp
#include <string>
#include "Window.h"
#include "GraphicsEngine.h"

//Keyboard inputes
void Window::OnKeyDown(unsigned int key) {
	m_keys[key] = true;
}

void Window::OnKeyUp(unsigned int key) {
	m_keys[key] = false;
}

bool Window::IsKeyPressed(unsigned int key) const {
	return m_keys[key];
}

//Mouse inputes
void Window::OnMouseMove(int x, int y) {
	// Calculate mouse delta
	m_mouseDeltaX = x - m_lastMouseX;
	m_mouseDeltaY = y - m_lastMouseY;

	// Update current position
	m_lastMouseX = x;
	m_lastMouseY = y;
	m_mouseX = x;
	m_mouseY = y;
}

void Window::OnMouseDown(int button) {
	if (button >= 0 && button < 3)
		m_mouseButtons[button] = true;
}

void Window::OnMouseUp(int button) {
	if (button >= 0 && button < 3)
		m_mouseButtons[button] = false;
}

void Window::OnMouseWheel(int delta) {
	// We'll forward this to GraphicsEngine later
	// For now, just store it or handle it directly
}

void Window::OnMouseEnter() {
	m_mouseInWindow = true;
}

void Window::OnMouseLeave() {
	m_mouseInWindow = false;
	// Reset button states when mouse leaves window
	m_mouseButtons[0] = m_mouseButtons[1] = m_mouseButtons[2] = false;
}

bool Window::IsMouseButtonPressed(int button) const {
	if (button >= 0 && button < 3)
		return m_mouseButtons[button];
	return false;
}

void Window::UpdateInput() {
	// Reset per-frame delta values
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

Window::Window(const std::wstring& title, int width,int height)
	: m_title(title)
	, m_width(width)
	, m_height(height)
	, m_handle(nullptr) //we start with no handle
{
}

Window::~Window()
{
	if (m_handle)
	{
		DestroyWindow(m_handle); // Destroy the window
		UnregisterClass(Window::WINDOW_CLASS_NAME, GetModuleHandle(nullptr)); // Unregister the window class
	}
}

bool Window::Initialize() {
	OutputDebugString(L"Starting window initialization...\n");

	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszClassName = WINDOW_CLASS_NAME;
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Unregister any existing class first
	UnregisterClass(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));

	if (!RegisterClassEx(&windowClass)) {
		DWORD error = GetLastError();
		wchar_t msg[256];
		swprintf_s(msg, L"RegisterClassEx failed with error: %lu\n", error);
		OutputDebugString(msg);
		return false;
	}

	OutputDebugString(L"Window class registered successfully\n");

	RECT windowRect = { 0, 0, m_width, m_height };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_handle = CreateWindowEx(
		0,                              // Extended style
		WINDOW_CLASS_NAME,              // Class name
		m_title.c_str(),               // Window title
		WS_OVERLAPPEDWINDOW,           // Window style
		CW_USEDEFAULT,                 // X position
		CW_USEDEFAULT,                 // Y position
		windowRect.right - windowRect.left,  // Width
		windowRect.bottom - windowRect.top,  // Height
		nullptr,                       // Parent window
		nullptr,                       // Menu
		GetModuleHandle(nullptr),      // Instance handle
		this                          // Additional data
	);

	if (!m_handle) {
		DWORD error = GetLastError();
		wchar_t msg[256];
		swprintf_s(msg, L"CreateWindowEx failed with error: %lu\n", error);
		OutputDebugString(msg);
		return false;
	}

	OutputDebugString(L"Window created successfully\n");

	ShowWindow(m_handle, SW_SHOW);
	UpdateWindow(m_handle);

	// Create the graphics engine
	m_graphicsEngine = std::make_unique<GraphicsEngine>();
	if (!m_graphicsEngine->Initialize(m_handle, m_width, m_height))
	{
		return false;
	}

	// Set up mouse tracking
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_handle;
	tme.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&tme);

	return true;
}

	bool Window::ProcessMessages() {
		// create storage for message
		MSG message = {};

		// check if there are any messages waiting
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			//check if it's time to quit
			if (message.message == WM_QUIT) {
				OutputDebugString(L"Received WM_QUIT message\n");
				return false; // return false to tell the main loop to stop
			}

			// translate virtual key messages into character messages
			TranslateMessage(&message);

			// send the message to the window procedure
			DispatchMessage(&message);
		}

		return true; // return true to tell the main loop to continue
	}

	LRESULT Window::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		Window* window = nullptr;

		if (message == WM_NCCREATE) {
			OutputDebugString(L"WM_NCCREATE received\n");
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(pCreate->lpCreateParams);

			if (SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window)) == 0) {
				DWORD error = GetLastError();
				if (error != 0) {
					wchar_t msg[256];
					swprintf_s(msg, L"SetWindowLongPtr failed with error: %lu\n", error);
					OutputDebugString(msg);
				}
			}

			window->m_handle = hwnd;  // Add this line
			return window->HandleMessage(message, wParam, lParam);
		}

		window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (window) {
			return window->HandleMessage(message, wParam, lParam);
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	void Window::Render()
	{
		Window* window = (Window*)GetWindowLongPtr(m_handle, GWLP_USERDATA);
		// Calculate delta time
		static float lastTime = 0.0f;
		float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;
		float deltaTime = (lastTime == 0.0f) ? 0.016f : currentTime - lastTime;
		lastTime = currentTime;

		// Process input
		m_graphicsEngine->ProcessKeyboardInput(*this, deltaTime);
		m_graphicsEngine->ProcessMouseInput(*this, deltaTime);

		// begin rendering
		m_graphicsEngine->BeginFrame(*this);
		// render the scene
		m_graphicsEngine->EndFrame();
	}

	LRESULT Window::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {

		// Get pointer to your application instance
		Window* window = (Window*)GetWindowLongPtr(m_handle, GWLP_USERDATA);

		switch (message) {

		case WM_KEYDOWN:
			if (window) window->OnKeyDown((unsigned int)wParam);
			return 0;

		case WM_KEYUP:
			if (window) window->OnKeyUp((unsigned int)wParam);
			return 0;

			// Add mouse handling
		case WM_MOUSEMOVE:
			if (window) window->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;

		case WM_LBUTTONDOWN:
			if (window) window->OnMouseDown(0); // 0 = left button
			SetCapture(m_handle); // Capture mouse
			return 0;

		case WM_RBUTTONDOWN:
			if (window) window->OnMouseDown(1); // 1 = right button
			SetCapture(m_handle); // Capture mouse
			return 0;

		case WM_MBUTTONDOWN:
			if (window) window->OnMouseDown(2); // 2 = middle button
			SetCapture(m_handle); // Capture mouse
			return 0;

		case WM_LBUTTONUP:
			if (window) window->OnMouseUp(0);
			ReleaseCapture(); // Release mouse capture
			return 0;

		case WM_RBUTTONUP:
			if (window) window->OnMouseUp(1);
			ReleaseCapture(); // Release mouse capture
			return 0;

		case WM_MBUTTONUP:
			if (window) window->OnMouseUp(2);
			ReleaseCapture(); // Release mouse capture
			return 0;

		case WM_MOUSEWHEEL:
			if (window) window->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;

		case WM_MOUSELEAVE:
			if (window) window->OnMouseLeave();
			return 0;

		case WM_CREATE:
			OutputDebugString(L"Window Created\n");
			return 0;

		case WM_CLOSE:
			OutputDebugString(L"Window Closing\n");
			DestroyWindow(m_handle);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			m_width = LOWORD(lParam);   // Now this is valid
			m_height = HIWORD(lParam);  // Now this is valid
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_handle, &ps);
			EndPaint(m_handle, &ps);
			return 0;
		}
		return DefWindowProc(m_handle, message, wParam, lParam);
	}