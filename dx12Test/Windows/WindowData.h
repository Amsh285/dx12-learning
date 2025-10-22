#pragma once

namespace windows
{
	struct WindowData
	{
		HWND windowHandle = nullptr;
		uint32_t clientWidth = 0;
		uint32_t clientHeight = 0;
	};
}
