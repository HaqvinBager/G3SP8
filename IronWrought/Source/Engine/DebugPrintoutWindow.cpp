#include "stdafx.h"
#include "DebugPrintoutWindow.h"
#include <imgui.h>
#include <stdio.h>

ImGui::CDebugPrintoutWindow::CDebugPrintoutWindow(const char* aName) 
	: CWindow(aName)
{
	stream = std::ofstream("DebugLog.txt");
	redir = new RedirectStdOutput(stream);

	freopen_s(&outPut, "DebugLog.txt", "w", stdout);
	if(outPut != nullptr)
		setvbuf(outPut, NULL, _IOFBF, myBufferSize);

	read = std::ifstream("DebugLog.txt", std::ios::in);
	*Open() = true;
}

ImGui::CDebugPrintoutWindow::~CDebugPrintoutWindow()
{
}

void ImGui::CDebugPrintoutWindow::OnEnable()
{
}

void ImGui::CDebugPrintoutWindow::OnInspectorGUI()
{	
	ImGui::SetNextWindowBgAlpha(0.1f);
	ImGui::SetNextWindowPos({ 0.0f, 900.0f }, ImGuiCond_None, {0.0f, 1.0f});
	ImGui::SetNextWindowSize({ 1200.0f, 350.0f }, ImGuiCond_None);
	ImGui::Begin(Name(), Open(), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);		
	std::string data((std::istreambuf_iterator<char>(read)), std::istreambuf_iterator<char>());
	if(data.size() > 0)
	{
		if (print.size() >= myBufferSize)
		{
			std::string temp = print.substr(print.size() / 2, (print.size() / 2 - print.size()));
			print.clear();
			print.reserve(temp.size());
			print = std::move(temp);
		}
		print.append(data);
	}

	ImGui::Text(print.c_str());	
	ImGui::SetScrollHere(0.999f);
	ImGui::End();
}

void ImGui::CDebugPrintoutWindow::OnDisable()
{
}
