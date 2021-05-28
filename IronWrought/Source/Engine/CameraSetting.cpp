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
	, myCameraFov(70.0f)
	, myCameraShakeSpeed(20.0f)
	, myShakeDecayInSeconds(0.5f)
	, myTestTrauma(1.5f)
{
	myShakeMaxRotation = { 2.0f, 2.0f, 2.0f };
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
	
	ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "Camera Shake");
	if (ImGui::SliderFloat("Camera Shake Speed", &myCameraShakeSpeed, 0.01f, 20.0f))
	{
		CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
		camera->SetShakeSpeed(myCameraShakeSpeed);
	}

	if (ImGui::SliderFloat("Camera Decay Speed", &myShakeDecayInSeconds, 0.01f, 10.0f))
	{
		CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
		camera->SetShakeDecay(myShakeDecayInSeconds);
	}

	if (ImGui::SliderFloat("Test Trauma Strength", &myTestTrauma, 0.01f, 10.0f))
	{
		CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
		camera->SetTestTrauma(myTestTrauma);
	}

	float imguiVector[3];
	memcpy(&imguiVector[0], &myShakeMaxRotation, sizeof(DirectX::SimpleMath::Vector3));
	if (ImGui::InputFloat3("Max Shake Rotation", imguiVector, "%.2f"))
	{
		memcpy(&myShakeMaxRotation, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));
		CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
		camera->SetMaxShakeRotation(myShakeMaxRotation);
	}

	ImGui::End();
}

void ImGui::CCameraSetting::OnDisable()
{
}