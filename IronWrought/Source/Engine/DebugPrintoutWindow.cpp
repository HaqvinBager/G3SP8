#include "stdafx.h"
#include "DebugPrintoutWindow.h"
#include <imgui.h>
#include <stdio.h>

ImGui::CDebugPrintoutWindow::CDebugPrintoutWindow(const char* aName) 
	: CWindow(aName)
{
	stream = std::ofstream("testOutput.txt");
	redir = new RedirectStdOutput(stream);

	std::cout << "Hejsan sverjsan" << std::endl;


	freopen_s(&outPut, "Log.txt", "w", stdout);
	if(outPut != nullptr)
		setvbuf(outPut, NULL, _IOFBF, 1024);

	read = std::ifstream("testOutput.txt", std::ios::in);
}

ImGui::CDebugPrintoutWindow::~CDebugPrintoutWindow()
{
}

void ImGui::CDebugPrintoutWindow::OnEnable()
{
	
}

void ImGui::CDebugPrintoutWindow::OnInspectorGUI()
{
	//ImGui::BeginChild(Name(), { 1000.0f, 250.0f }, false, ImGuiWindowFlags_NoDecoration);
	ImGui::SetNextWindowBgAlpha(0.1f);
	ImGui::SetNextWindowSize({ 1200.0f, 200.0f }, ImGuiCond_None);
	ImGui::SetNextWindowPos({ 0.0f, 256.0f });
	ImGui::Begin(Name(), Open(), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);		
	std::string data((std::istreambuf_iterator<char>(read)), std::istreambuf_iterator<char>());
	if(data.size() > 0)
	{
		print.append(data);
		if (print.size() > 4096)
			print.clear();
	}
	ImGui::Text(print.c_str());	
	ImGui::SetScrollHere(0.999f);
	ImGui::End();
	//ImGui::EndChild();

	if (Input::GetInstance()->IsKeyPressed('K'))
	{
		std::cout << "Hej Jag skriver ett langt meddelande" << std::endl;
	}
}

void ImGui::CDebugPrintoutWindow::OnDisable()
{
}
