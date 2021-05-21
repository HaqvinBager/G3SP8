#include "stdafx.h"
#include "CameraSetting.h"
#include "CameraControllerComponent.h"
#include "CameraComponent.h"
#include "Engine.h"
#include "Scene.h"
#include <iostream>
#include <imgui.h>

ImGui::CCameraSetting::CCameraSetting(const char* aName)
	: CWindow(aName)
	, myCameraSpeed(2.0f)
{
}

ImGui::CCameraSetting::~CCameraSetting()
{
}

void ImGui::CCameraSetting::OnEnable()
{
}

void ImGui::CCameraSetting::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	/*if (ImGui::BeginCombo(Name(), "Speed", ImGuiComboFlags_NoPreview)) {*/
		if (ImGui::SliderFloat("Camera Speed", &myCameraSpeed, 1.0f, 20.0f)) {
			CCameraControllerComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraControllerComponent>();
			camera->SetCameraMoveSpeed(myCameraSpeed);
			
		}
	/*	
		ImGui::EndCombo();
	}*/
	

	if (ImGui::SliderFloat("Camera FoV", &myCameraFov, 1.0f, 100.0f)) {
		CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
		camera->SetFoV(myCameraFov);
		
	}

	ImGui::End();
}

void ImGui::CCameraSetting::OnDisable()
{
}