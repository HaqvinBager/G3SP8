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
		setvbuf(outPut, NULL, _IOFBF, 1024);

	read = std::ifstream("DebugLog.txt", std::ios::in);
	*Open() = true;

	print.reserve(50000);
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
		print.append(data);
		//if (print.size() > myBufferSize)
		//	print.clear();
	}
	ImGui::Text(print.c_str());	
	ImGui::SetScrollHere(0.999f);
	ImGui::End();
}

void ImGui::CDebugPrintoutWindow::OnDisable()
{
}
