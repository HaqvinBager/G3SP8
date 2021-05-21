#pragma once
#include "Window.h"

namespace ImGui
{
	class CDebugPrintoutWindow :	public CWindow
	{
	public:
		CDebugPrintoutWindow();
		~CDebugPrintoutWindow();
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;
	private:

	};
}

