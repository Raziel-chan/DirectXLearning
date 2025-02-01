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

private:
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