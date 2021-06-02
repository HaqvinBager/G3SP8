#include "stdafx.h"
#include "LoadScene.h"
#include <imgui.h>

#include "JsonReader.h"
#include "Scene.h"
#include "Engine.h"
#include "SceneManager.h"
#include "CameraComponent.h"
#include "FolderUtility.h"
#include "MainSingleton.h"
#include "PostMaster.h"

ImGui::CLoadScene::CLoadScene(const char* aMenuName, const bool aIsMenuChild)
	: CWindow(aMenuName, aIsMenuChild)
	, myState(EState::DropDownMenu)
	, mySceneIndex(-1)
{
	OnEnable();

}

ImGui::CLoadScene::~CLoadScene()
{
}

void ImGui::CLoadScene::OnEnable()
{
	std::vector<std::string> generatedJsonFiles = CFolderUtility::GetFileNamesInFolder(ASSETPATH ("Assets/Generated"), ".json"/*, "Level"*/);
	for (auto& file : generatedJsonFiles) {
		//auto endIndex = file.find_last_of('_');
		//std::string sceneName = file.substr(0, endIndex);
		if (file.find("Bin") == std::string::npos)
		{
			if (std::find(myScenes.begin(), myScenes.end(), file.c_str()) == myScenes.end()) {
				myScenes.emplace_back(file.c_str());
			}
		}
	}
	myScenes.push_back("Empty");
	myState = EState::DropDownMenu;



}


bool ImGui::CLoadScene::OnMainMenuGUI()
{

	std::string previewName;
	if (mySceneIndex < 0)
		previewName = "Empty";
	else
		previewName = myScenes[mySceneIndex];

	if (ImGui::BeginCombo(Name(), previewName.c_str(), ImGuiComboFlags_NoPreview)) {
		int index = 0;
		for (const auto& scene : myScenes) {
			if (ImGui::Selectable(scene.c_str(), mySceneIndex == index)) {

				CCameraComponent* camera = CEngine::GetInstance()->GetActiveScene().FindFirstObjectWithComponent<CCameraComponent>();
				Vector3 camPos = camera->GameObject().myTransform->Position();

				CScene* newScene;
				if (scene == "Empty")
				{
					newScene = CSceneManager::CreateEmpty();
					CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, newScene);
					CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);
					OnComplete("Empty");
				}
				else
				{
					CSceneFactory::Get()->LoadSceneAsync(scene, CStateStack::EState::InGame, [this](std::string aJson) { CLoadScene::OnComplete(aJson); });
					
					// For Debug: Run without threading:
					//CSceneFactory::Get()->LoadScene(scene, CStateStack::EState::InGame);
				}
			}
			index++;
		}
		ImGui::EndCombo();
	}


	return true;
}

void ImGui::CLoadScene::OnInspectorGUI()
{
}

void ImGui::CLoadScene::OnDisable()
{
}

void ImGui::CLoadScene::OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded)
{
	SetConsoleColor(CONSOLE_DAQUA);
	std::cout << "Scene Load Complete!" << aSceneThatHasBeenSuccessfullyLoaded << std::endl;
	SetConsoleColor(CONSOLE_WHITE);

	CMainSingleton::PostMaster().Send({ "LoadScene", nullptr });
	myScenes.clear();
	OnEnable();
}


/*
//std::vector<std::string> files = CJsonReader::GetFilePathsInFolder(ASSETPATH + "Assets/Generated");
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, 18));
	float x = 10.f + ((ImGui::GetFontSize() + 5.5f) * static_cast<float>(myLevelsToSelectFrom.size()));
	ImGui::SetNextWindowSize({200.f,  x});

	ImGui::Begin("LevelSelect", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	/*if (ImGui::TreeNodeEx("Selection State: Single Selection", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
	{
static int selected = -1;
for (int n = 0; n < myLevelsToSelectFrom.size(); n++)
{
	char buf[512];
	sprintf_s(buf, "%s", myLevelsToSelectFrom[n].c_str());

	if (ImGui::Selectable(buf, selected == n, ImGuiSelectableFlags_AllowDoubleClick))
	{
		selected = n;

		if (ImGui::IsMouseDoubleClicked(0))
		{
			std::cout << "Load Level: " << buf << std::endl;
			CScene* myUnityScene = CSceneManager::CreateScene(buf);
			CEngine::GetInstance()->AddScene(CStateStack::EState::InGame, myUnityScene);
			CEngine::GetInstance()->SetActiveScene(CStateStack::EState::InGame);

			myIsEnabled = !myIsEnabled;
		}
	}
}
/*	ImGui::TreePop();
}


ImGui::End();
}


*/