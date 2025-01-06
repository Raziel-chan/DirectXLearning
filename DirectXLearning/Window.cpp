#include <windows.h>
#include <string>
#include "Window.h"


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

bool Window::Initialize()
{
	//Setup the Window class
	WNDCLASSEX windowClass = {}; // Create an empty structure
	windowClass.cbSize = sizeof(WNDCLASSEX); // Set the size of the structure
	
	// Windows behavior
	windowClass.style = CS_HREDRAW | CS_VREDRAW; // Redraw the window if it's resized horizontally or vertically
	windowClass.lpfnWndProc = Window::WindowProc; // Set the window procedure to our static method
	windowClass.hInstance = GetModuleHandle(nullptr); // Set the instance

	// how our window will look
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // Load a standard arrow cursor
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Set the background color
	windowClass.lpszClassName = Window::WINDOW_CLASS_NAME; // Set the class name
	
	//try to register the windows class
	if (!RegisterClassEx(&windowClass))
	{
		return false;
	}

	// Calculate window size based on desired client region size
	RECT windowRect = { 0, 0, m_width, m_height }; // Set the size of the window
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // Adjust the size

	// Create the window
	m_handle = CreateWindowEx(
		0, // Optional window styles
		Window::WINDOW_CLASS_NAME, // Window class
		m_title.c_str(), // Window title
		WS_OVERLAPPEDWINDOW, // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, // Parent window
		nullptr, // Menu
		GetModuleHandle(nullptr), // Instance handle
		nullptr // Additional application data
	);

	// check if the window was created
	if (!m_handle)
	{
		return false;
	}

	// Show the window
	ShowWindow(m_handle, SW_SHOW);
	UpdateWindow(m_handle);

	return true;

	bool Window::ProcessMessages() {
		// create storage for message
		MSG message = {};
	}
}