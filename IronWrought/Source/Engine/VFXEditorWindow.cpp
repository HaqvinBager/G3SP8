#include "stdafx.h"
#include "VFXEditorWindow.h"
#include <widget_curve.h>
#include <curve_v122.hpp>
#include "JsonReader.h"
#include "JsonWriter.h"
#include "FolderUtility.h"

#include "Engine.h"
#include "Scene.h"
#include "VFXSystemComponent.h"

#include "rapidjson/filewritestream.h"
#include <fstream>

#define CURRENT_EFFECT myEffects[myCurrentEffectIndex]

ImGui::CVFXEditorWindow::CVFXEditorWindow(const char* aName)
	: CWindow(aName)
	, myCurrentMenu(EVFXEditorMenu::MainMenu)
{
	std::string saveDestination = "Assets/IronWrought/VFX/VFX_JSON/VFXSystem_Tester.json";
	ZeroMemory(mySaveDestination, 256);
	memcpy(&mySaveDestination[0], saveDestination.c_str(), strlen(saveDestination.c_str()));

	myMeshData = new SVFXMeshData();
	myEmitterData = new SParticleEmitterData();

	myCurrentCurveData = SCurveData();

	AddEffect(0);
}
ImGui::CVFXEditorWindow::~CVFXEditorWindow()
{
	delete myMeshData;
	myMeshData = nullptr;

	delete myEmitterData;
	myEmitterData = nullptr;
}

void ImGui::CVFXEditorWindow::OnEnable()
{
	std::vector<std::string> generatedJsonFiles = CFolderUtility::GetFileNamesInFolder(ASSETPATH("Assets/IronWrought/VFX/VFX_JSON/"), ".json", ".json");
	for (auto& file : generatedJsonFiles) {
		auto startIndex = file.find_first_of('_') + 1;
		std::string filePath = file.substr(startIndex, file.length() - 15);

		if (std::find(myEffectFilePaths.begin(), myEffectFilePaths.end(), filePath) == myEffectFilePaths.end()) {
			myEffectFilePaths.push_back(filePath);
		}
	}
	myEffectFilePaths.push_back("Empty");
}

void ImGui::CVFXEditorWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	switch (myCurrentMenu)
	{
	case ImGui::EVFXEditorMenu::MainMenu:
		ShowMainMenu();
		break;
	case ImGui::EVFXEditorMenu::VFXMeshView:
		ShowVFXMeshWindow();
		break;
	case ImGui::EVFXEditorMenu::ParticleEmitterView:
		ShowParticleEffectWindow();
		break;
	default:
		break;
	}

	ImGui::End();
}

void ImGui::CVFXEditorWindow::OnDisable()
{
}

void ImGui::CVFXEditorWindow::SaveToFile()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	writer.SetIndent(' ', 2);

	writer.StartObject();
	writer.Key("VFXEffects");
	writer.StartArray();

	for (auto& effect : myEffects)
	{
		effect.SerializeEffects(writer);
	}

	writer.EndArray();
	writer.EndObject();

	std::ofstream of(ASSETPATH(mySaveDestination));
	of << s.GetString();

	of.close();

	CGameObject* vfx = CEngine::GetInstance()->GetActiveScene().GetVFXTester();
	vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
	vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(myCurrentEffectIndex);
}

void ImGui::CVFXEditorWindow::LoadFile(std::string aFilePath)
{
	rapidjson::Document document = CJsonReader::Get()->LoadDocument(ASSETPATH(aFilePath));
	ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(document), "Invalid Json document: %s", aFilePath.c_str());

	auto effectsArray = document["VFXEffects"].GetArray();

	myEffects.clear();
	myCurrentEffectIndex = 0;

	for (unsigned int i = 0; i < static_cast<unsigned int>(effectsArray.Size()); ++i)
	{
		myEffects.push_back(SVFXSerializable());

		std::string effectName = effectsArray[i]["Name"].GetString();
		ZeroMemory(myEffects.back().myName, 64);
		memcpy(&myEffects.back().myName[0], effectName.c_str(), strlen(effectName.c_str()));

		auto meshArray = effectsArray[i]["VFXMeshes"].GetArray();
		for (unsigned int j = 0; j < static_cast<unsigned int>(meshArray.Size()); ++j)
		{
			myEffects.back().myVFXMeshes.push_back(SVFXMeshTransformData());

			auto& vfxMesh = myEffects.back().myVFXMeshes.back();
			
			std::string path = meshArray[j]["Path"].GetString();
			ZeroMemory(vfxMesh.myPath, 128);
			memcpy(&vfxMesh.myPath[0], path.c_str(), strlen(path.c_str()));

			vfxMesh.myOffsetFromParent = { meshArray[j]["Offset X"].GetFloat(), meshArray[j]["Offset Y"].GetFloat(), meshArray[j]["Offset Z"].GetFloat() };
			vfxMesh.myRotationAroundParent = { meshArray[j]["Rotation X"].GetFloat(), meshArray[j]["Rotation Y"].GetFloat(), meshArray[j]["Rotation Z"].GetFloat() };
			vfxMesh.myScale = { meshArray[j]["Scale X"].GetFloat(), meshArray[j]["Scale Y"].GetFloat(), meshArray[j]["Scale Z"].GetFloat() };
			vfxMesh.myAngularSpeeds = { meshArray[j]["Rotate X per second"].GetFloat(), meshArray[j]["Rotate Y per second"].GetFloat(), meshArray[j]["Rotate Z per second"].GetFloat() };
			vfxMesh.myShouldOrbit = meshArray[j]["Orbit"].GetBool();
			vfxMesh.myDelay = meshArray[j]["Delay"].GetFloat();
			vfxMesh.myDuration = meshArray[j]["Duration"].GetFloat();
		}

		auto emitterArray = effectsArray[i]["ParticleSystems"].GetArray();
		for (unsigned int k = 0; k < static_cast<unsigned int>(emitterArray.Size()); ++k)
		{
			myEffects.back().myParticleEmitters.push_back(SParticleEmitterTransformData());

			auto& emitter = myEffects.back().myParticleEmitters.back();

			std::string path = emitterArray[k]["Path"].GetString();
			ZeroMemory(emitter.myPath, 128);
			memcpy(&emitter.myPath[0], path.c_str(), strlen(path.c_str()));

			emitter.myOffsetFromParent = { emitterArray[k]["Offset X"].GetFloat(), emitterArray[k]["Offset Y"].GetFloat(), emitterArray[k]["Offset Z"].GetFloat() };
			emitter.myRotationAroundParent = { emitterArray[k]["Rotation X"].GetFloat(), emitterArray[k]["Rotation Y"].GetFloat(), emitterArray[k]["Rotation Z"].GetFloat() };
			emitter.myUniformScale = emitterArray[k]["Scale"].GetFloat();
			emitter.myAngularSpeeds = { emitterArray[k]["Rotate X per second"].GetFloat(), emitterArray[k]["Rotate Y per second"].GetFloat(), emitterArray[k]["Rotate Z per second"].GetFloat() };
			emitter.myShouldOrbit = emitterArray[k]["Orbit"].GetBool();
			emitter.myDelay = emitterArray[k]["Delay"].GetFloat();
			emitter.myDuration = emitterArray[k]["Duration"].GetFloat();
		}
	}
}

void ImGui::CVFXEditorWindow::ShowMainMenu()
{
	if (ImGui::BeginCombo("Load Effect", "VFXSystem_Tester.json", ImGuiComboFlags_NoPreview)) 
	{
		unsigned int index = 0;
		for (const auto& filePath : myEffectFilePaths) 
		{
			if (ImGui::Selectable(filePath.c_str(), mySelectedIndex == index)) 
			{
				mySelectedIndex = index;

				if (myEffectFilePaths[mySelectedIndex].find("Empty") != std::string::npos)
				{
					ImGui::End();
					return;
				}

				std::string saveDestination = "Assets/IronWrought/VFX/VFX_JSON/VFXSystem_";
				saveDestination.append(myEffectFilePaths[mySelectedIndex] + ".json");
				ZeroMemory(mySaveDestination, 256);
				memcpy(&mySaveDestination[0], saveDestination.c_str(), strlen(saveDestination.c_str()));
				
				LoadFile(mySaveDestination);

				CGameObject* vfx = CEngine::GetInstance()->GetActiveScene().GetVFXTester();
				vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
				vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(myCurrentEffectIndex);
			}
			index++;
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	for (unsigned int i = 0; i < myEffects.size(); ++i)
	{
		ImGui::SameLine(0.0f, 20.0f);
		if (ImGui::Button(myEffects[i].myName))
		{
			myCurrentEffectIndex = i;
		}
	}
	ImGui::SameLine(0.0f, 20.f);
	if (ImGui::Button("Add Effect"))
	{
		AddEffect(static_cast<unsigned int>(myEffects.size()));
	}

	if (myEffects.empty())
	{
		ImGui::End();
		return;
	}

	ImGui::InputText("Effect Name", &CURRENT_EFFECT.myName[0], 64);
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "VFX Meshes");
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Add VFX Mesh"))
	{
		SVFXMeshTransformData meshData;
		CURRENT_EFFECT.myVFXMeshes.push_back(meshData);
	}
	for (unsigned int i = 0; i < CURRENT_EFFECT.myVFXMeshes.size(); ++i)
	{
		ShowVFXMeshTransformData(CURRENT_EFFECT.myVFXMeshes[i], i);
	}
	ImGui::Spacing();

	ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "ParticleSystems");
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button("Add Particle Emitter"))
	{
		SParticleEmitterTransformData emitterData;
		CURRENT_EFFECT.myParticleEmitters.push_back(emitterData);
	}
	for (unsigned int i = 0; i < CURRENT_EFFECT.myParticleEmitters.size(); ++i)
	{
		ShowParticleEmitterTransformData(CURRENT_EFFECT.myParticleEmitters[i], i);
	}
	ImGui::Spacing();

	ImGui::InputText("Save As", mySaveDestination, 256);
	if (ImGui::Button("Save")) {
		SaveToFile();
	}
}

void ImGui::CVFXEditorWindow::AddEffect(unsigned int anIndex)
{
	SVFXSerializable serializable;
	std::string name = "Effect " + std::to_string(anIndex);
	ZeroMemory(serializable.myName, 64);
	memcpy(&serializable.myName[0], name.c_str(), strlen(name.c_str()));

	SVFXMeshTransformData meshData;
	serializable.myVFXMeshes.push_back(meshData);

	SParticleEmitterTransformData emitterData;
	serializable.myParticleEmitters.push_back(emitterData);

	myEffects.push_back(serializable);
}

void ImGui::CVFXEditorWindow::ShowVFXMeshTransformData(SVFXMeshTransformData& someData, unsigned int anIndex)
{
	std::string index = std::to_string(anIndex);

	std::string label = "VM Path " + index;
	ImGui::InputText(label.c_str(), &someData.myPath[0], 128);

	label = "VM Modify " + index;
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button(label.c_str()))
	{
		OpenVFXMeshWindow(someData.myPath);
	}

	float imguiVector[3];
	label = "VM Offset From Parent " + index;
	memcpy(&imguiVector[0], &someData.myOffsetFromParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.2f");
	memcpy(&someData.myOffsetFromParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Rotation Around Parent " + index;
	memcpy(&imguiVector[0], &someData.myRotationAroundParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myRotationAroundParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Angular Speeds " + index;
	memcpy(&imguiVector[0], &someData.myAngularSpeeds, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myAngularSpeeds, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Scale " + index;
	memcpy(&imguiVector[0], &someData.myScale, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myScale, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "VM Orbit Around Parent " + index;
	ImGui::Checkbox(label.c_str(), &someData.myShouldOrbit);

	label = "VM Delay " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDelay, 0.1f, 0.0f, 10.0f, "%.1f");

	label = "VM Duration " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDuration, 0.2f, 0.1f, 10.0f, "%.1f");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

void ImGui::CVFXEditorWindow::ShowParticleEmitterTransformData(SParticleEmitterTransformData& someData, unsigned int anIndex)
{
	std::string index = std::to_string(anIndex);

	std::string label = "PS Path " + index;
	ImGui::InputText(label.c_str(), &someData.myPath[0], 128);

	label = "PS Modify " + index;
	ImGui::SameLine(0.0f, 20.0f);
	if (ImGui::Button(label.c_str()))
	{
		OpenParticleEffectWindow(someData.myPath);
	}

	float imguiVector[3];
	label = "PS Offset From Parent " + index;
	memcpy(&imguiVector[0], &someData.myOffsetFromParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.2f");
	memcpy(&someData.myOffsetFromParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Rotation Around Parent " + index;
	memcpy(&imguiVector[0], &someData.myRotationAroundParent, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myRotationAroundParent, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Angular Speeds " + index;
	memcpy(&imguiVector[0], &someData.myAngularSpeeds, sizeof(DirectX::SimpleMath::Vector3));
	ImGui::InputFloat3(label.c_str(), &imguiVector[0], "%.0f");
	memcpy(&someData.myAngularSpeeds, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector3));

	label = "PS Uniform Scale " + index;
	ImGui::DragFloat(label.c_str(), &someData.myUniformScale, 0.01f, 0.01f, 10.0f, "%.2f");

	label = "PS Orbit Around Parent " + index;
	ImGui::Checkbox(label.c_str(), &someData.myShouldOrbit);

	label = "PS Delay " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDelay, 0.1f, 0.0f, 10.0f, "%.1f");

	label = "PS Duration " + index;
	ImGui::DragFloat(label.c_str(), &someData.myDuration, 0.2f, 0.1f, 10.0f, "%.1f");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}

void ImGui::CVFXEditorWindow::ShowVFXMeshWindow()
{
	ImGui::TextColored({ 1.0f, 0.0f, 1.0f, 1.0f }, "VFX Mesh File");
	ImGui::Spacing();

	float imguiVector[2];
	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed1, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 1", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed1, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed2, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 2", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed2, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed3, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 3", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed3, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	memcpy(&imguiVector[0], &myMeshData->myScrollSpeed4, sizeof(DirectX::SimpleMath::Vector2));
	ImGui::InputFloat2("Scroll Speed Texture 4", &imguiVector[0], "%.2f");
	memcpy(&myMeshData->myScrollSpeed4, &imguiVector[0], sizeof(DirectX::SimpleMath::Vector2));

	ImGui::DragFloat("UV Scale Texture 1", &myMeshData->myUVScale1, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 2", &myMeshData->myUVScale2, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 3", &myMeshData->myUVScale3, 0.1f, 0.1f, 10.0f, "%.2f");
	ImGui::DragFloat("UV Scale Texture 4", &myMeshData->myUVScale4, 0.1f, 0.1f, 10.0f, "%.2f");

	ImGui::DragFloat("Opacity Strength", &myMeshData->myOpacityStrength, 0.1f, 0.1f, 10.0f, "%.2f");

	ImGui::InputText("Texture 1 Path", &myMeshData->myTexturePath1[0], 64);
	ImGui::InputText("Texture 2 Path", &myMeshData->myTexturePath2[0], 64);
	ImGui::InputText("Texture 3 Path", &myMeshData->myTexturePath3[0], 64);
	ImGui::InputText("Texture Mask Path", &myMeshData->myTextureMaskPath[0], 64);
	ImGui::InputText("Mesh Path", &myMeshData->myMeshPath[0], 64);

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::InputText("Save As", myVFXMeshDataDestination, 256);
	if (ImGui::Button("Save"))
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		writer.SetIndent(' ', 2);

		myMeshData->Serialize(writer);

		std::ofstream of(ASSETPATH(myVFXMeshDataDestination));
		of << s.GetString();

		of.close();

		CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
		vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
		vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(myCurrentEffectIndex);
	}
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	if (ImGui::Button("Back to VFX Effect"))
	{
		myCurrentMenu = EVFXEditorMenu::MainMenu;
	}
}

void ImGui::CVFXEditorWindow::ShowParticleEffectWindow()
{
	ImGui::TextColored({ 1.0f, 0.0f, 1.0f, 1.0f }, "Particle Emitter File");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::InputText("Texture Path", myEmitterData->myTexturePath, 64);
	ImGui::Spacing();

	ImGui::DragInt("Max Number Of Particles", &myEmitterData->myMaxNumberOfParticles, 0.1f, 1, 200);
	ImGui::DragFloat("Spawn Rate (Particles / Second)", &myEmitterData->mySpawnRate, 0.1f, 0.1f, 200.0f, "%.1f");
	ImGui::DragFloat("Base Lifetime", &myEmitterData->myLifetime, 0.01f, 0.01f, 10.0f, "%.2f");
	ImGui::DragFloat("Base Speed", &myEmitterData->myDefaultSpeed, 0.01f, 0.01f, 20.0f, "%.2f");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("Interpolator Ranges");

	float imguiVector4[4];
	memcpy(&imguiVector4[0], &myEmitterData->myStartColor, sizeof(Vector4));
	ImGui::ColorEdit4("Start Color", &imguiVector4[0]);
	memcpy(&myEmitterData->myStartColor, &imguiVector4[0], sizeof(Vector4));

	memcpy(&imguiVector4[0], &myEmitterData->myEndColor, sizeof(Vector4));
	ImGui::ColorEdit4("End Color", &imguiVector4[0]);
	memcpy(&myEmitterData->myEndColor, &imguiVector4[0], sizeof(Vector4));

	ImGui::DragFloat("Start Uniform Size ", &myEmitterData->myUniformStartSize, 0.01f, 0.01f, 20.0f, "%.2f");
	ImGui::DragFloat("End Uniform Size ", &myEmitterData->myUniformEndSize, 0.01f, 0.01f, 20.0f, "%.2f");

	float imguiVector3[3];
	memcpy(&imguiVector3[0], &myEmitterData->myStartDirection, sizeof(Vector3));
	ImGui::InputFloat3("Start Direction", &imguiVector3[0]);
	memcpy(&myEmitterData->myStartDirection, &imguiVector3[0], sizeof(Vector3));

	memcpy(&imguiVector3[0], &myEmitterData->myEndDirection, sizeof(Vector3));
	ImGui::InputFloat3("End Direction", &imguiVector3[0]);
	memcpy(&myEmitterData->myEndDirection, &imguiVector3[0], sizeof(Vector3));

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("Offset Randomization");

	ImGui::DragFloat("Lifetime Randomize Offset From", &myEmitterData->myLifetimeRandomizeFrom, 0.01f, -10.0f, myEmitterData->myLifetimeRandomizeTo, "%.2f");
	ImGui::DragFloat("Lifetime Randomize Offset To", &myEmitterData->myLifetimeRandomizeTo, 0.01f, myEmitterData->myLifetimeRandomizeFrom, 10.0f, "%.2f");

	ImGui::DragFloat("Speed Randomize Offset From", &myEmitterData->mySpeedRandomizeFrom, 0.01f, -10.0f, myEmitterData->mySpeedRandomizeTo, "%.2f");
	ImGui::DragFloat("Speed Randomize Offset To", &myEmitterData->mySpeedRandomizeTo, 0.01f, myEmitterData->mySpeedRandomizeFrom, 10.0f, "%.2f");

	memcpy(&imguiVector3[0], &myEmitterData->mySpawnPositionOffsetRandomizeFrom, sizeof(Vector3));
	ImGui::DragFloat3("Offset Spawn Position Randomize From", &imguiVector3[0], 0.1f, -10.0f, 10.0f, "%.1f");
	memcpy(&myEmitterData->mySpawnPositionOffsetRandomizeFrom, &imguiVector3[0], sizeof(Vector3));

	memcpy(&imguiVector3[0], &myEmitterData->mySpawnPositionOffsetRandomizeTo, sizeof(Vector3));
	ImGui::DragFloat3("Offset Spawn Position Randomize To", &imguiVector3[0], 0.1f, -10.0f, 10.0f, "%.1f");
	memcpy(&myEmitterData->mySpawnPositionOffsetRandomizeTo, &imguiVector3[0], sizeof(Vector3));

	memcpy(&imguiVector3[0], &myEmitterData->mySpawnDirectionRandomizeFrom, sizeof(Vector3));
	ImGui::DragFloat3("Spawn Direction Randomize From", &imguiVector3[0], 0.1f, -10.0f, 10.0f, "%.1f");
	memcpy(&myEmitterData->mySpawnDirectionRandomizeFrom, &imguiVector3[0], sizeof(Vector3));

	memcpy(&imguiVector3[0], &myEmitterData->mySpawnDirectionRandomizeTo, sizeof(Vector3));
	ImGui::DragFloat3("Spawn Direction Randomize To", &imguiVector3[0], 0.1f, -10.0f, 10.0f, "%.1f");
	memcpy(&myEmitterData->mySpawnDirectionRandomizeTo, &imguiVector3[0], sizeof(Vector3));

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("Curves");

	// Curves

	if (ImGui::Button("Edit Color Curve"))
	{
		myCurrentCurveData = myEmitterData->myColorCurve;
		myCurrentCurveData.myShouldShowWindow = true;
	}
	ImGui::Spacing();

	if (ImGui::Button("Edit Size Curve"))
	{
		myCurrentCurveData = myEmitterData->mySizeCurve;
		myCurrentCurveData.myShouldShowWindow = true;
	}
	ImGui::Spacing();

	if (ImGui::Button("Edit Direction Curve"))
	{
		myCurrentCurveData = myEmitterData->myDirectionCurve;
		myCurrentCurveData.myShouldShowWindow = true;
	}

	if (myCurrentCurveData.myShouldShowWindow)
	{
		ImGui::Begin("Curve Editor", Open());
		
		ImGui::IronCurve(myCurrentCurveData.myLabel, myCurrentCurveData.myPoints);
		
		if (ImGui::Button("Save Curve"))
		{
			myCurrentCurveData.myShouldShowWindow = false;
			std::string currentLabel = myCurrentCurveData.myLabel;

			if (currentLabel.find("Color Curve") != std::string::npos)
			{
				myEmitterData->myColorCurve = myCurrentCurveData;
			}
			else if (currentLabel.find("Size Curve") != std::string::npos)
			{
				myEmitterData->mySizeCurve = myCurrentCurveData;
			}
			else if (currentLabel.find("Direction Curve") != std::string::npos)
			{
				myEmitterData->myDirectionCurve = myCurrentCurveData;
			}

			rapidjson::StringBuffer s;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
			writer.SetIndent(' ', 2);

			myEmitterData->Serialize(writer);

			std::ofstream of(ASSETPATH(myParticleEmitterDataDestination));
			of << s.GetString();

			of.close();

			CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
			vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
			vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(myCurrentEffectIndex);
		}

		ImGui::End();
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::InputText("Save As", myParticleEmitterDataDestination, 256);
	if (ImGui::Button("Save"))
	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		writer.SetIndent(' ', 2);

		myEmitterData->Serialize(writer);

		std::ofstream of(ASSETPATH(myParticleEmitterDataDestination));
		of << s.GetString();

		of.close();

		CGameObject* vfx = IRONWROUGHT->GetActiveScene().GetVFXTester();
		vfx->GetComponent<CVFXSystemComponent>()->Init(ASSETPATH(mySaveDestination));
		vfx->GetComponent<CVFXSystemComponent>()->EnableEffect(myCurrentEffectIndex);
	}
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	if (ImGui::Button("Back to VFX Effect"))
	{
		myCurrentMenu = EVFXEditorMenu::MainMenu;
	}
}

void ImGui::CVFXEditorWindow::OpenVFXMeshWindow(const std::string& aPath)
{
	if (myMeshData)
		myMeshData->Init(aPath);

	myCurrentMenu = EVFXEditorMenu::VFXMeshView;

	ZeroMemory(myVFXMeshDataDestination, 256);
	memcpy(&myVFXMeshDataDestination[0], aPath.c_str(), strlen(aPath.c_str()));
}

void ImGui::CVFXEditorWindow::OpenParticleEffectWindow(const std::string& aPath)
{
	if (myEmitterData)
		myEmitterData->Init(aPath);

	myCurrentMenu = EVFXEditorMenu::ParticleEmitterView;

	ZeroMemory(myParticleEmitterDataDestination, 256);
	memcpy(&myParticleEmitterDataDestination[0], aPath.c_str(), strlen(aPath.c_str()));
}
