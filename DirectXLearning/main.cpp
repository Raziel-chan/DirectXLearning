#include "Window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    // Create our window
    Window window(L"DirectX Learning", 1280, 720);

    // Initialize the window
    if (!window.Initialize()) {
        MessageBox(nullptr, L"Window creation failed!", L"Error", MB_OK);
        return FALSE;
    }

    // Add this debug output
    OutputDebugString(L"Window initialized, entering message loop\n");

    // Main loop
    bool running = true;
    while (running) {
        // Use our Window class's ProcessMessages method
        running = window.ProcessMessages();

		// Render the frame
		window.Render();


        // Add a small sleep to prevent high CPU usage
        Sleep(1);  // Sleep for 1ms

    }

    OutputDebugString(L"Exiting program\n");
    return 0;
}