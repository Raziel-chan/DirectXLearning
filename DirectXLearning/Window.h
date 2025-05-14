#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include "GraphicsEngine.h"

class Window {
public:
    Window(const std::wstring& title, int width, int height);
    ~Window();
    bool ProcessMessages();
    bool Initialize();
    void Render();
	// method to handle key press/release events
    void OnKeyDown(unsigned int key);
    void OnKeyUp(unsigned int key);

	// check if a key is pressed
    bool IsKeyPressed(unsigned int key) const;

    // Mouse handling methods
    void OnMouseMove(int x, int y);
    void OnMouseDown(int button);
    void OnMouseUp(int button);
    void OnMouseWheel(int delta);
    void OnMouseEnter();
    void OnMouseLeave();

    // Access methods for GraphicsEngine to use
    int GetMouseDeltaX() const { return m_mouseDeltaX; }
    int GetMouseDeltaY() const { return m_mouseDeltaY; }
    bool IsMouseButtonPressed(int button) const;

    // Update method to reset per-frame variables
    void UpdateInput();

	// Access methods width and height
	int GetWidth() const { return m_width; }
	void SetWidth(int width) { m_width = width; }
	int GetHeight() const { return m_height; }
	void SetHeight(int height) { m_height = height; }
private:
    // keyboard state
    bool m_keys[256] = {false};

    // Mouse state
    bool m_mouseButtons[3] = { false };  // Left, middle, right
    int m_mouseX = 0;
    int m_mouseY = 0;
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;
    int m_mouseDeltaX = 0;
    int m_mouseDeltaY = 0;
    bool m_mouseInWindow = false;

	std::unique_ptr<GraphicsEngine> m_graphicsEngine;
    // Message handlers
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    // Window properties
    HWND m_handle;
    std::wstring m_title;
    int m_width;
    int m_height;

    // Class name - make sure this is complete
    static constexpr const wchar_t* WINDOW_CLASS_NAME = L"DirectXLearningWindowClass";
};