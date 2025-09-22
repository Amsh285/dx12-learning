#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#define NOGDI
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>

// has to be linked later
/*When using runtime compiled HLSL shaders using any of the D3DCompiler functions, do not forget to link against the d3dcompiler.lib library
and copy the D3dcompiler_47.dll to the same folder as the binary executable when distributing your project.

A redistributable version of the D3dcompiler_47.dll file can be found
in the Windows 10 SDK installation folder at C:\Program Files (x86)\Windows Kits\10\Redist\D3D\.

For more information, refer to the MSDN blog post at: https://blogs.msdn.microsoft.com/chuckw/2012/05/07/hlsl-fxc-and-d3dcompile/*/
#include <d3dcompiler.h>
#include <DirectXMath.h>

template <class T>
void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
        throw std::exception();
}


inline void ThrowIfFailed(HRESULT hr, const std::string& errormsg)
{
    if (FAILED(hr))
        throw std::exception(errormsg.c_str());
}
