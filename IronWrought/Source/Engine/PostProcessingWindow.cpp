#include "stdafx.h"
#include "PostProcessingWindow.h"
#include "FullscreenRenderer.h"
#include "Engine.h"
#include <imgui.h>

ImGui::CPostProcessingWindow::CPostProcessingWindow(const char* aName)
	: CWindow(aName)
	, mySSAORadius(0.6f)
	, mySSAOSampleBias(0.005f)
	, mySSAOMagnitude(1.1f)
	, mySSAOContrast(1.5f)
	, mySSAOConstantBias(0.2f)
	, myWhitePointColor(1.0f)
	, myWhitePointIntensity(10.0f)
	, myExposure(1.0f)
	, myIsReinhard(false)
	, myIsUncharted(true)
	, myIsACES(false)
	, myVignetteStrength(1.0f/9.0f)
	, myVignetteColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
{
}

ImGui::CPostProcessingWindow::~CPostProcessingWindow()
{
}

void ImGui::CPostProcessingWindow::OnEnable()
{
	CFullscreenRenderer::SPostProcessingBufferData bufferData = IRONWROUGHT->GetPostProcessingBufferData();
	myWhitePointColor = bufferData.myWhitePointColor;
	myWhitePointIntensity = bufferData.myWhitePointIntensity;
	myExposure = bufferData.myExposure;
	myIsReinhard = bufferData.myIsReinhard;
	myIsUncharted = bufferData.myIsUncharted;
	myIsACES = bufferData.myIsACES;
	
	mySSAORadius = bufferData.mySSAORadius;
	mySSAOSampleBias = bufferData.mySSAOSampleBias;
	mySSAOMagnitude = bufferData.mySSAOMagnitude;
	mySSAOContrast = bufferData.mySSAOContrast;
	mySSAOConstantBias = 0.2f;

	myEmissiveStrength = bufferData.myEmissiveStrength;

	myVignetteStrength = bufferData.myVignetteStrength;
	myVignetteColor = bufferData.myVignetteColor;
}

void ImGui::CPostProcessingWindow::OnInspectorGUI()
{
	CFullscreenRenderer::SPostProcessingBufferData bufferData = IRONWROUGHT->GetPostProcessingBufferData();

	ImGui::Begin(Name(), Open());

	ImVec4 tonemapColor = { 0.1f, 0.2f, 1.0f, 1.0f };
	ImGui::TextColored(tonemapColor, "Tonemapping");

	ImGui::Text("Algorithm: ");
	ImGui::SameLine();
	
	if (ImGui::Checkbox("Reinhard", &myIsReinhard))
	{
		myIsUncharted = false;
		myIsACES = false;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Uncharted", &myIsUncharted))
	{
		myIsReinhard = false;
		myIsACES = false;

	}
	ImGui::SameLine();
	if (ImGui::Checkbox("ACES", &myIsACES))
	{
		myIsReinhard = false;
		myIsUncharted = false;
	}

	bufferData.myIsReinhard = myIsReinhard;
	bufferData.myIsUncharted = myIsUncharted;
	bufferData.myIsACES = myIsACES;

	float imguiVector4[4];
	memcpy(&imguiVector4[0], &myWhitePointColor, sizeof(Vector4));
	ImGui::ColorEdit4("White Point Color", &imguiVector4[0]);
	memcpy(&myWhitePointColor, &imguiVector4[0], sizeof(Vector4));

	bufferData.myWhitePointColor = myWhitePointColor;

	if (ImGui::SliderFloat("White Point Intensity", &myWhitePointIntensity, 0.0f, 20.0f, "%0.3f"))
	{
		bufferData.myWhitePointIntensity = myWhitePointIntensity;
	}

	if (ImGui::SliderFloat("Exposure", &myExposure, -5.0f, 10.0f, "%.1f"))
	{
		bufferData.myExposure = myExposure;
	}

	ImGui::Dummy({ 0.0f, 10.0f });

	ImVec4 bloomColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	ImGui::TextColored(bloomColor, "Bloom");

	if (ImGui::SliderFloat("Emissive Strength", &myEmissiveStrength, 0.0f, 40.0f, "%.1f"))
	{
		bufferData.myEmissiveStrength = myEmissiveStrength;
	}

	ImGui::Dummy({ 0.0f, 10.0f });

	ImVec4 ssaoColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	ImGui::TextColored(ssaoColor, "SSAO");

	if (ImGui::SliderFloat("Sample Kernel Radius", &mySSAORadius, 0.1f, 10.0f, "%.1f"))
	{
		bufferData.mySSAORadius = mySSAORadius;	
	}

	if (ImGui::SliderFloat("Sample Bias", &mySSAOSampleBias, 0.0001f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOSampleBias = mySSAOSampleBias;
	}

	if (ImGui::SliderFloat("Magnitude", &mySSAOMagnitude, 0.1f, 5.0f, "%.1f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOMagnitude = mySSAOMagnitude;
	}

	if (ImGui::SliderFloat("Contrast", &mySSAOContrast, 0.1f, 5.0f, "%.1f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.mySSAOContrast = mySSAOContrast;
	}

	ImGui::Dummy({ 0.0f, 10.0f });

	ImVec4 vignetteTextColor = { 1.0f, 1.0f, 0.0f, 1.0f };
	ImGui::TextColored(vignetteTextColor, "Vignette");

	if (ImGui::SliderFloat("Vignette Strength", &myVignetteStrength, 0.0625f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic))
	{
		bufferData.myVignetteStrength = myVignetteStrength;
	}

	memcpy(&imguiVector4[0], &myVignetteColor, sizeof(Vector4));
	ImGui::ColorEdit4("Vignette Color", &imguiVector4[0]);
	memcpy(&myVignetteColor, &imguiVector4[0], sizeof(Vector4));

	bufferData.myVignetteColor = myVignetteColor;

	ImGui::End();

	IRONWROUGHT->SetPostProcessingBufferData(bufferData);
}

void ImGui::CPostProcessingWindow::OnDisable()
{
}
