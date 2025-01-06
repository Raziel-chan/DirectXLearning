#pragma once
#include <windows.h>

class Window
{
public:
	Window(const std::wstring& title, int width, int height);
	~Window(); // Destructor to clean up when our window is destroyed
	bool ProcessMessages();
	bool Initialize(); // This will handle the registration and creation

private:
	// This is our window procedure - it handles messages
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND m_handle; // Handle de la fen�tre
	std::wstring m_title; // Titre de la fen�tre
	int m_width; // Largeur de la fen�tre
	int m_height; // Hauteur de la fen�tre

	// We'll use this name to register our window class
	static constexpr const wchar_t* WINDOW_CLASS_NAME = L"OurWindowClass";
};