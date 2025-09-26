#include "pch.h"

#include "Logger.h"
#include "Directx12/Runtime/Dx12Setup.h"
#include "Directx12/Dx12Renderer.h"
#include "Windows/WindowData.h"

using namespace Microsoft::WRL;

using namespace directx12;
using namespace directx12::runtime;
using namespace windows;

// By default, enable V-Sync.
// Can be toggled with the V key.
bool g_VSync = true;
bool g_TearingSupported = false;
// By default, use windowed mode.
// Can be toggled with the Alt+Enter or F11
bool g_Fullscreen = false;

// number of backbuffers
const uint8_t g_NumFrames = 3;

bool g_UseWarp = false;

uint32_t g_ClientWidth = 1280;
uint32_t g_ClientHeight = 720;

bool g_IsInitialized = false;
uint32_t g_RTVDescriptorSize;
uint32_t g_CurrentBackBufferIndex;

HWND g_WindowHandle;
RECT g_WindowRect;

ComPtr<IDXGISwapChain4> g_SwapChain;

ComPtr<ID3D12CommandQueue> g_CommandQueue;
ComPtr<ID3D12GraphicsCommandList> g_CommandList;
ComPtr<ID3D12CommandAllocator> g_CommandAllocator[g_NumFrames];

ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
ComPtr<ID3D12Resource> g_BackBuffers[g_NumFrames];

//Synchronization
ComPtr<ID3D12Fence> g_Fence;

uint64_t g_FenceValue = 0;
uint64_t g_FrameFenceValues[g_NumFrames] = {};
HANDLE g_FenceEvent;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PWSTR cmdline, _In_ int cmdshow)
{
	Logger::Configure({ "logs/dx12Test.log", 1024 * 1024 * 50, 3 });
	Logger runtimeLogger("Dx12Test runtime");
	runtimeLogger.Info("Start Dx12Test runtime");


	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	// windowClass.style = CS_OWNDC; Todo: try this when dx12 renderer works!!!
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"TestDx12Window";
	windowClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	ATOM registerResult = RegisterClassExW(&windowClass);

	if (registerResult == 0)
	{
		OutputDebugString(L"Failed to register Windowclass.");
		return EXIT_FAILURE;
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	g_WindowRect = { 0, 0, static_cast<LONG>(g_ClientWidth), static_cast<LONG>(g_ClientHeight) };
	AdjustWindowRect(&g_WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = g_WindowRect.right - g_WindowRect.left;
	int windowHeight = g_WindowRect.bottom - g_WindowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

	g_WindowHandle = CreateWindowExW(
		NULL,
		L"TestDx12Window",
		L"Dx12 Test",
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInst,
		nullptr
	);

	if (g_WindowHandle == nullptr)
	{
		//see: https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
		uint32_t errorCode = GetLastError();
		std::wstring errorMessage = L"Error creating Window. Errorcode: ";
		errorMessage += std::to_wstring(errorCode);
		OutputDebugString(errorMessage.c_str());

		return EXIT_FAILURE;
	}

	WindowData windowData = { g_WindowHandle, g_ClientWidth, g_ClientHeight };

	Dx12SetupResult runtimeSetupResult = directx12::runtime::Setup();

	Dx12Renderer renderer(windowData);
	Dx12RendererSetupResult renderSetupResult = renderer.Setup();

	/*DXGI_ADAPTER_DESC1 desc = {};
	adapter->GetDesc1(&desc);
	OutputDebugStringW(desc.Description);
	OutputDebugStringW(L"\n");*/

	ShowWindow(g_WindowHandle, SW_SHOW);

	MSG message = {};

	runtimeLogger.Info("Startup complete - application running.");

	while (message.message != WM_QUIT)
	{
		if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
		else
		{
			//update here
		}
	}

	DestroyWindow(g_WindowHandle);
	return EXIT_SUCCESS;
}


LRESULT _stdcall WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wparam, lparam);
}
