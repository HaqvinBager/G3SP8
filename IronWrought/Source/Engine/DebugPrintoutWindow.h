#pragma once
#include "Window.h"
#include <fstream>

namespace ImGui
{
	class RedirectStdOutput {
	public:
		RedirectStdOutput(std::ofstream& file)
			: _psbuf{ file.rdbuf() }, _backup{ std::cout.rdbuf() }
		{
			std::cout.rdbuf(_psbuf);
		}

		~RedirectStdOutput()
		{
			std::cout.rdbuf(_backup);
		}

	private:
		std::streambuf* _psbuf;
		std::streambuf* _backup;
	};

	class CDebugPrintoutWindow : public CWindow
	{
	public:
		CDebugPrintoutWindow(const char* aName);
		~CDebugPrintoutWindow();
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;
	private:
		FILE* outPut;
		std::ofstream stream;
		std::ifstream read;
		std::streampos lastPos;
		std::string print;
		char* myLastPos;

		RedirectStdOutput* redir;
		//char buf[2056];
	};
}

