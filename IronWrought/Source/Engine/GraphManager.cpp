#include "stdafx.h"
#include "GraphManager.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "JsonReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "NodeInstance.h"
#include "NodeType.h"
#include "NodeTypes.h"
#ifdef _DEBUG
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "Interop.h"
#include <imgui_impl_dx11.h>
#endif
#include "Input.h"
#include <filesystem>
#include "Scene.h"
#include "Engine.h"
#include "GameObject.h"
#include "GraphNodeTimerManager.h"
#include "FolderUtility.h"
#include "NodeDataManager.h"

using namespace rapidjson;
#ifdef _DEBUG

namespace ed = ax::NodeEditor;

using namespace ax::Drawing;
static ed::EditorContext* g_Context = nullptr;
#endif

CGraphManager::~CGraphManager()
{
#ifdef _DEBUG
	delete myHeaderTextureID;
	myHeaderTextureID = nullptr;
	ed::DestroyEditor(g_Context);
#endif
}

void CGraphManager::SGraph::Clear()
{
	for (size_t i = 0; i < myNodeInstances.size(); ++i)
	{
		delete myNodeInstances[i];
		myNodeInstances[i] = nullptr;
	}
	myNodeInstances.clear();
	myBluePrintInstances.clear();
	CNodeTypeCollector::Clear();
}

void CGraphManager::Load(const std::string& aSceneName)
{
	//Global = Kan alltid n�s om programmet k�r
	//Scene = Data som relaterar till Just denna Scen, kan alltid n�s n�r Scene �r ig�ng
	//Script = Data som relaterar till just detta script

	if (!CNodeDataManager::Get())
	{
		CNodeDataManager::Create();
		myInstantiableVariables.push_back("Float");
		myInstantiableVariables.push_back("Int");
		myInstantiableVariables.push_back("Bool");
		myInstantiableVariables.push_back("Start");
		myInstantiableVariables.push_back("Vector 3");
	}

	const std::string sceneJson = ASSETPATH("Assets/Generated/" + aSceneName + "/" + aSceneName + ".json");
	CGraphNodeTimerManager::Create();
	const auto doc = CJsonReader::Get()->LoadDocument(sceneJson);
	if (doc.HasParseError())
		return;

	// Create Scene folder.
	mySceneFolder = "Imgui/NodeScripts/" + aSceneName + "/";
	//Om denna Blueprint redan finns ska vi bara spara undan den som nyckel
	if (!std::filesystem::exists(mySceneFolder))
	{
		if (!std::filesystem::create_directory(mySceneFolder.c_str()))
		{
			ENGINE_BOOL_POPUP("Failed to create Directory: %s", mySceneFolder.c_str());
			return;
		}
	}
	// !Create Scene folder.

	for (auto& scene : doc["Scenes"].GetArray())
	{
		if (scene.HasMember("bluePrints"))
		{
			const auto& bluePrints = scene["bluePrints"].GetArray();
			for (auto& bluePrint : bluePrints)
			{
				if (!bluePrint.HasMember("type"))
					continue;
				if (!bluePrint.HasMember("instances"))
					continue;
				if (!(bluePrint["instances"].GetArray().Size() > 0))
					continue;

				std::string key = bluePrint["type"].GetString();
				SGraph graph;
				myGraphs.push_back(graph);

				for (const auto& jsonGameObjectID : bluePrint["instances"].GetArray())
				{
					if (!jsonGameObjectID.HasMember("instanceID") && jsonGameObjectID.HasMember("childrenInstanceIDs"))
					{
						continue;
					}
					BluePrintInstance bpInstance;
					bpInstance.rootID = jsonGameObjectID["instanceID"].GetInt();
					bool firstLoop = true;
					int counter = 1;
					for (const auto& childID : jsonGameObjectID["childrenInstanceIDs"].GetArray())
					{
						bpInstance.childrenIDs.emplace_back(childID.GetInt());
						if (firstLoop)
						{

							firstLoop = false;
							continue;
						}
						myGraphs.back().myChildrenKey = key;
						CNodeTypeCollector::RegisterChildNodeTypes(key, counter++, childID.GetInt());
					}
					myGraphs.back().myBluePrintInstances.emplace_back(bpInstance);
				}

				std::string scriptFolder = mySceneFolder + key + "/";
				myGraphs.back().myFolderPath = scriptFolder;
				if (std::filesystem::exists(scriptFolder))
					continue;

				if (!std::filesystem::create_directory(scriptFolder.c_str()))
				{
					ENGINE_BOOL_POPUP("Failed to create Directory: %s", scriptFolder.c_str());
					continue;
				}
				else
				{
					myCurrentGraph = &myGraphs.back();
					SaveTreeToFile();
				}
			}
		}
	}

	CNodeDataManager::Get()->SetFolderPath(mySceneFolder);

	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];
	CNodeTypeCollector::PopulateTypes();
#ifdef _DEBUG
	myHeaderTextureID = nullptr;
	ed::Config config;
	std::string simple = "Imgui/NodeScripts/Simple.json";
	config.SettingsFile = simple.c_str();
	g_Context = ed::CreateEditor(&config);
#endif
	LoadDataNodesFromFile();
	myMenuSearchField = new char[127];
	memset(&myMenuSearchField[0], 0, sizeof(myMenuSearchField));
	LoadTreeFromFile();

	myRenderGraph = false;
#ifdef _DEBUG
	myRunScripts = false;
#else
	myRunScripts = true;
#endif // _DEBUG
}

void CGraphManager::Clear()
{
	if (myGraphs.size() <= 0)
		return;

	SaveTreeToFile();
	CUID::ClearUIDS();
	for (auto& sGraph : myGraphs)
	{
		sGraph.Clear();
	}
	myGraphs.clear();
	myCurrentGraph = nullptr;
	CNodeDataManager::Get()->ClearStoredData();
}

void CGraphManager::ReTriggerUpdatingTrees()
{
	//Locate start nodes, we support N start nodes, we might want to remove this, as we dont "support" different trees with different starrtnodes to be connected. It might work, might not
	if (myRunScripts)
	{
		for (const auto& graph : myGraphs)
		{
			const auto& bluePrintInstances = graph.myBluePrintInstances;

			for (unsigned int i = 0; i < bluePrintInstances.size(); ++i)
			{
				myCurrentBluePrintInstance = bluePrintInstances[i];
				for (auto& nodeInstance : graph.myNodeInstances)
				{
					if (nodeInstance->myNodeType->IsStartNode())
					{
						nodeInstance->Enter();
					}
				}
			}
		}
	}
}

void CGraphManager::SaveTreeToFile()
{
#ifdef _DEBUG
	for (const auto& graph : myGraphs)
	{

		{
			rapidjson::StringBuffer s;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

			writer1.StartObject();
			writer1.Key("UID_MAX");

			writer1.StartObject();
			writer1.Key("Num");
			writer1.Int(CUID::myGlobalUID);
			writer1.EndObject();

			writer1.Key("NodeInstances");
			writer1.StartArray();
			for (auto& nodeInstance : graph.myNodeInstances)
			{
				nodeInstance->Serialize(writer1);
			}
			writer1.EndArray();
			writer1.EndObject();

			std::ofstream of(graph.myFolderPath + "/nodeinstances.json");
			of << s.GetString();
		}
		//Links
		{
			rapidjson::StringBuffer s;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

			writer1.StartObject();
			writer1.Key("Links");
			writer1.StartArray();
			for (auto& link : graph.myLinks)
			{
				writer1.StartObject();
				writer1.Key("ID");
				writer1.Int(static_cast<int>(link.myID.Get()));
				writer1.Key("Input");
				writer1.Int(static_cast<int>(link.myInputID.Get()));
				writer1.Key("Output");
				writer1.Int(static_cast<int>(link.myOutputID.Get()));
				writer1.EndObject();

			}
			writer1.EndArray();
			writer1.EndObject();


			std::ofstream of(graph.myFolderPath + "/links.json");
			of << s.GetString();
		}
	}
#endif
}

SPin::EPinType LoadPinData(NodeDataPtr& someDataToCopy, rapidjson::Value& someData)
{
	if (someData.IsBool())
	{
		someDataToCopy = new bool;
		bool test = someData.GetBool();
		memcpy(someDataToCopy, &test, sizeof(bool));
		return SPin::EPinType::EBool;
	}
	else if (someData.IsDouble())
	{
		someDataToCopy = new float;
		float test = static_cast<float>(someData.GetDouble());
		memcpy(someDataToCopy, &test, sizeof(float));
		return SPin::EPinType::EFloat;
	}
	else if (someData.IsString())
	{
		if (someData.GetStringLength() > 0)
		{
			int length = someData.GetStringLength();
			someDataToCopy = new char[length];
			const char* data = someData.GetString();
			memcpy(someDataToCopy, data, sizeof(char) * length);
			((char*)someDataToCopy)[length] = '\0';
			return SPin::EPinType::EString;
		}

	}
	else if (someData.IsInt())
	{
		someDataToCopy = new int;
		int test = someData.GetInt();
		memcpy(someDataToCopy, &test, sizeof(int));
		return SPin::EPinType::EInt;
	}
	else if (someData.IsArray())
	{
		someDataToCopy = new Vector3;
		Vector3 test;
		auto values = someData.GetArray();

		test.x = values[0].GetFloat();
		test.y = values[1].GetFloat();
		test.z = values[2].GetFloat();

		//int test = someData.GetInt();
		memcpy(someDataToCopy, &test, sizeof(Vector3));
		return SPin::EPinType::EVector3;
	}
	return SPin::EPinType::EUnknown;
}

void CGraphManager::LoadTreeFromFile()
{
	CUID::myAllUIDs.clear();
	CUID::myGlobalUID = 0;
	for (auto& graph : myGraphs)
	{
		myCurrentGraph = &graph;
		Document document;
		{
			std::string path = graph.myFolderPath + "nodeinstances.json";

			document = CJsonReader::Get()->LoadDocument(path);
			if (document.HasMember("UID_MAX"))
			{
				auto uIDMax = document["UID_MAX"]["Num"].GetInt();
				CUID::myGlobalUID = uIDMax;
			}
			if (document.HasMember("NodeInstances"))
			{
				auto nodeInstances = document["NodeInstances"].GetArray();

				for (unsigned int i = 0; i < nodeInstances.Size(); ++i)
				{
					auto nodeInstance = nodeInstances[i].GetObjectW();
					CNodeInstance* object = new CNodeInstance(this, false);
					int nodeTypeID = nodeInstance["NodeType ID"].GetInt();
					int UID = nodeInstance["UID"].GetInt();
					object->myUID.SetUID(UID);
					object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeTypeID, static_cast<CNodeType::ENodeType>(nodeInstance["NodeType"].GetInt()));

					if (object->myNodeType)
						object->CheckIfInputNode();

					object->myEditorPosition[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
					object->myEditorPosition[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());

					if (object->myEditorPosition[0] <= -100000 || object->myEditorPosition[0] >= 100000)
						object->myEditorPosition[0] = 0;
					if (object->myEditorPosition[1] <= -100000 || object->myEditorPosition[1] >= 100000)
						object->myEditorPosition[1] = 0;

					object->ConstructUniquePins();

					for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
					{
						int index = nodeInstance["Pins"][j]["Index"].GetInt();
						object->myPins[index].myUID.SetUID(nodeInstance["Pins"][j]["UID"].GetInt());
						SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"]);
						if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
						{
							object->ChangePinTypes(newType);
						}
					}
					graph.myNodeInstances.push_back(object);
				}
			}
		}
		{
			document = CJsonReader::Get()->LoadDocument(graph.myFolderPath + "links.json");
			if (document.HasMember("Links"))
			{
				auto links = document["Links"].GetArray();
				graph.myNextLinkIdCounter = 0;
				for (unsigned int i = 0; i < links.Size(); i++)
				{
					unsigned int id = document["Links"][i]["ID"].GetInt();
					int inputID = document["Links"][i]["Input"].GetInt();
					int Output = document["Links"][i]["Output"].GetInt();

					CNodeInstance* firstNode = GetNodeFromPinID(inputID);
					if (!firstNode)
						continue;
					CNodeInstance* secondNode = GetNodeFromPinID(Output);
					if (!secondNode)
						continue;

					firstNode->AddLinkToVia(secondNode, inputID, Output, id);
					secondNode->AddLinkToVia(firstNode, Output, inputID, id);
#ifdef _DEBUG
					graph.myLinks.push_back({ ed::LinkId(id), ed::PinId(inputID), ed::PinId(Output) });
					if (graph.myNextLinkIdCounter < id + 1)
					{
						graph.myNextLinkIdCounter = id + 1;
					}
#endif
				}
			}
		}
	}
	if (myGraphs.size() > 0)
		myCurrentGraph = &myGraphs[0];
}

void CGraphManager::SaveNodesToClipboard()
{
	int selectedObjectCount = ed::GetSelectedObjectCount();
	ed::NodeId* selectedNodeIDs = new ed::NodeId[selectedObjectCount];
	ed::GetSelectedNodes(selectedNodeIDs, selectedObjectCount);
	std::vector<CNodeInstance*> nodeInstances;
	for (int i = 0; i < selectedObjectCount; ++i)
		nodeInstances.push_back(GetNodeFromNodeID(static_cast<unsigned int>(selectedNodeIDs[i].Get())));

	{
		rapidjson::StringBuffer s;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

		writer1.StartObject();
		writer1.Key("UID_MAX");

		writer1.StartObject();
		writer1.Key("Num");
		writer1.Int(CUID::myGlobalUID);
		writer1.EndObject();

		writer1.Key("NodeInstances");
		writer1.StartArray();
		for (auto& nodeInstance : nodeInstances)
		{
			nodeInstance->Serialize(writer1);
		}
		writer1.EndArray();
		writer1.EndObject();



		std::ofstream of("Imgui/NodeScripts/clipboard.json");
		of << s.GetString();
	}
}

void CGraphManager::LoadNodesFromClipboard()
{
	std::ifstream inputFile("Imgui/NodeScripts/clipboard.json");
	std::stringstream jsonDocumentBuffer;
	std::string inputLine;

	while (std::getline(inputFile, inputLine))
	{
		jsonDocumentBuffer << inputLine << "\n";
	}
	rapidjson::Document document;
	document.Parse(jsonDocumentBuffer.str().c_str());

	rapidjson::Value& uidmax = document["UID_MAX"];
	int test = uidmax["Num"].GetInt();
	CUID::myGlobalUID = test;

	rapidjson::Value& results = document["NodeInstances"];

	float firstNodePos[2];

	for (rapidjson::SizeType i = 0; i < results.Size(); i++)
	{
		rapidjson::Value& nodeInstance = results[i];


		CNodeInstance* object = new CNodeInstance(this, true);
		int nodeTypeID = nodeInstance["NodeType ID"].GetInt();
		object->myNodeType = CNodeTypeCollector::GetNodeTypeFromID(nodeTypeID, static_cast<CNodeType::ENodeType>(nodeInstance["NodeType"].GetInt()));

		if (object->myNodeType)
			object->CheckIfInputNode();

		if (i == 0)
		{
			firstNodePos[0] = static_cast<float>(nodeInstance["Position"]["X"].GetInt());
			firstNodePos[1] = static_cast<float>(nodeInstance["Position"]["Y"].GetInt());
		}

		ImVec2 position;
		position.x = nodeInstance["Position"]["X"].GetInt() - firstNodePos[0] + ImGui::GetMousePos().x;
		position.y = nodeInstance["Position"]["Y"].GetInt() - firstNodePos[1] + ImGui::GetMousePos().y;

		object->ConstructUniquePins();

		for (unsigned int j = 0; j < nodeInstance["Pins"].Size(); j++)
		{
			int index = nodeInstance["Pins"][j]["Index"].GetInt();
			object->myPins[index].myUID.SetUID(CUID().AsInt());
			SPin::EPinType newType = LoadPinData(object->myPins[index].myData, nodeInstance["Pins"][j]["DATA"]);
			if (object->myPins[index].myVariableType == SPin::EPinType::EUnknown)
			{
				object->ChangePinTypes(newType);
			}
		}

		ed::SetNodePosition(object->myUID.AsInt(), position);
		object->myHasSetEditorPosition = true;

		myCurrentGraph->myNodeInstances.push_back(object);
	}
}

void CGraphManager::ShowFlow(int aLinkID)
{
	if (aLinkID == 0)
		return;
	myFlowsToBeShown.push_back(aLinkID);
}

void CGraphManager::Update()
{
	if (myGraphs.size() > 0)
	{
		CGraphNodeTimerManager::Get()->Update();

		PreFrame();
#ifdef _DEBUG

		if (myRenderGraph)
		{
			ConstructEditorTreeAndConnectLinks();
			PostFrame();
			ImGui::End();
		}
#endif // _DEBUG
	}
}

void CGraphManager::ToggleShouldRenderGraph()
{
	myRenderGraph = !myRenderGraph;
}

bool CGraphManager::ToggleShouldRunScripts()
{
	myRunScripts = !myRunScripts;
	return myRunScripts;
}

CGameObject* CGraphManager::GetCurrentGameObject()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	CGameObject* gameObject = scene.FindObjectWithID(myCurrentBluePrintInstance.childrenIDs[0]);

	return gameObject;
}

std::vector<CGameObject*> CGraphManager::GetCurrentGameObjectChildren()
{
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	std::vector<CGameObject*> gameObjects = {};
	for (int i = 1; i < myCurrentBluePrintInstance.childrenIDs.size(); ++i)
		gameObjects.push_back(scene.FindObjectWithID(myCurrentBluePrintInstance.childrenIDs[i]));

	return gameObjects;
}

const int CGraphManager::GetCurrentBlueprintInstanceID() const
{
	return myCurrentBluePrintInstance.childrenIDs[0];
}

ImColor GetIconColor(SPin::EPinType type)
{
	switch (type)
	{
	default:
	case SPin::EPinType::EFlow:
		return ImColor(255, 255, 255);
	case SPin::EPinType::EBool:
		return ImColor(220, 48, 48);
	case SPin::EPinType::EInt:
		return ImColor(68, 201, 156);
	case SPin::EPinType::EFloat:
		return ImColor(147, 226, 74);
	case SPin::EPinType::EString:
		return ImColor(124, 21, 153);
	case SPin::EPinType::EVector3:
		return ImColor(255, 166, 0);
	case SPin::EPinType::EStringListIndexed:
		return ImColor(0, 255, 0);
	case SPin::EPinType::EUnknown:
		return ImColor(255, 0, 0);
	}
};

#ifdef _DEBUG
void DrawPinIcon(const SPin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor  color = GetIconColor(pin.myVariableType);
	color.Value.w = alpha / 255.0f;
	switch (pin.myVariableType)
	{
	case SPin::EPinType::EFlow:
		iconType = IconType::Flow;
		break;
	case SPin::EPinType::EBool:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EInt:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EFloat:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EString:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EVector3:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EStringListIndexed:
		iconType = IconType::Circle;
		break;
	case SPin::EPinType::EUnknown:
		iconType = IconType::Circle;
		break;
	default:
		return;
	}
	const int s_PinIconSize = 24;
	ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};
#endif

CNodeInstance* CGraphManager::GetNodeFromNodeID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	auto it = myCurrentGraph->myNodeInstances.begin();
	while (it != myCurrentGraph->myNodeInstances.end())
	{
		if ((*it)->myUID.AsInt() == anID)
		{
			return *it;
		}
		else
		{
			++it;
		}
	}

	return nullptr;
}


CNodeInstance* CGraphManager::GetNodeFromPinID(unsigned int anID)
{
	if (anID == 0)
		return nullptr;

	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		std::vector<SPin>& pins = nodeInstance->GetPins();

		for (auto& pin : pins)
		{
			if (pin.myUID.AsInt() == anID)
			{
				return nodeInstance;
			}
		}

	}
	return nullptr;
}

#ifdef _DEBUG
void CGraphManager::DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance)
{
	switch (aPin.myVariableType)
	{
	case SPin::EPinType::EString:
	{
		if (!aPin.myData)
		{
			aPin.myData = new char[128];
			static_cast<char*>(aPin.myData)[0] = '\0';
		}

		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			if (!myRunScripts)
			{
				ImGui::InputText("##edit", (char*)aPin.myData, 127);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EInt:
	{
		if (!aPin.myData)
		{
			aPin.myData = new int;
			int* c = ((int*)aPin.myData);
			*c = 0;
		}
		int* c = ((int*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			if (!myRunScripts)
			{
				ImGui::InputInt("##edit", c);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EBool:
	{
		if (!aPin.myData)
		{
			aPin.myData = new bool;
			bool* c = ((bool*)aPin.myData);
			*c = false;
		}
		bool* c = ((bool*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			if (!myRunScripts)
			{
				ImGui::Checkbox("##edit", c);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EFloat:
	{
		if (!aPin.myData)
		{
			aPin.myData = new float;
			float* c = ((float*)aPin.myData);
			*c = 1.0f;
		}
		float* c = ((float*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(70.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			if (!myRunScripts)
			{
				ImGui::InputFloat("##edit", c);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EVector3:
	{
		if (!aPin.myData)
		{
			aPin.myData = new DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
		}
		DirectX::SimpleMath::Vector3* c;
		c = static_cast<DirectX::SimpleMath::Vector3*>(aPin.myData);

		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(35.0f);
		if (aNodeInstance->IsPinConnected(aPin))
		{
			DrawPinIcon(aPin, true, 255);
		}
		else
		{
			if (!myRunScripts)
			{
				ImGui::InputFloat("##edit", &c->x);
				ImGui::SameLine();
				ImGui::InputFloat("##edit1", &c->y);
				ImGui::SameLine();
				ImGui::InputFloat("##edit2", &c->z);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EUnknown:
	{
		if (!myRunScripts)
		{
			ImGui::PushID(aPin.myUID.AsInt());
			ImGui::PushItemWidth(100.0f);

			int selectedIndex = -1;
			if (ImGui::RadioButton("Bool", false))
			{
				selectedIndex = (int)SPin::EPinType::EBool;
			}
			if (ImGui::RadioButton("Int", false))
			{
				selectedIndex = (int)SPin::EPinType::EInt;
			}
			if (ImGui::RadioButton("Float", false))
			{
				selectedIndex = (int)SPin::EPinType::EFloat;
			}
			if (ImGui::RadioButton("Vector3", false))
			{
				selectedIndex = (int)SPin::EPinType::EVector3;
			}
			if (ImGui::RadioButton("String", false))
			{
				selectedIndex = (int)SPin::EPinType::EString;
			}

			if (selectedIndex != -1)
			{
				CNodeInstance* instance = GetNodeFromPinID(aPin.myUID.AsInt());
				instance->ChangePinTypes((SPin::EPinType)selectedIndex);
			}

			ImGui::PopItemWidth();
			ImGui::PopID();
		}
		break;
	}
	default:
		assert(0);
	}

}
#endif // _DEBUG

void CGraphManager::CreateNewDataNode()
{
	if (myEnteringNodeName)
	{
		if (!mySetPosition)
		{
			ImGui::SetNextWindowPos({ ImGui::GetIO().MousePos.x,ImGui::GetIO().MousePos.y });
			mySetPosition = true;
		}
		ImGui::SetNextWindowSize({ 225, 60 });
		ImGui::Begin(myNewVariableType.c_str());
		static char buffer[64] = "";
		ImGui::InputText("Name", buffer, 64);

		if (Input::GetInstance()->IsKeyPressed(VK_RETURN))
		{
			mySetPosition = false;
			myEnteringNodeName = false;
			bool hasCreatedNewVariable = false;
			if (strlen(buffer) == 0)
				memcpy(buffer, myNewVariableType.c_str(), 10);

			for (unsigned int i = 0; i < CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EChild); ++i)
			{
				if (CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EChild)[i]->NodeName() == buffer)
				{
					hasCreatedNewVariable = true;
					break;
				}
			}
			if (!hasCreatedNewVariable)
			{
				if (myNewVariableType == "Float")
				{
					float nullValue = 0.0f;
					CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EFloat));
					CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EFloat, nullValue);
				}
				else if (myNewVariableType == "Int")
				{
					int nullValue = 0;
					CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EInt));
					CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EInt, nullValue);
				}
				else if (myNewVariableType == "Bool")
				{
					bool nullValue = false;
					CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EBool));
					CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EBool, nullValue);
				}
				else if (myNewVariableType == "Start")
				{
					bool nullValue = false;
					CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EStart));
					CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EStart, nullValue);
				}
				else if (myNewVariableType == "Vector 3")
				{
					Vector3 nullValue = { 0.0f,0.0f,0.0f };
					CNodeTypeCollector::RegisterNewDataType(buffer, static_cast<int>(CNodeDataManager::EDataType::EVector3));
					CNodeDataManager::Get()->SetData(buffer, CNodeDataManager::EDataType::EVector3, nullValue);
				}
				myCustomDataNodes.push_back(buffer);
				CNodeDataManager::Get()->SaveDataTypesToJson();
				ZeroMemory(buffer, 64);
				hasCreatedNewVariable = false;
			}
		}
		ImGui::End();
	}
}

void CGraphManager::LoadDataNodesFromFile()
{
	Document document;
	{
		std::string path = mySceneFolder + "CustomDataNodes.json";
		document = CJsonReader::Get()->LoadDocument(path);
		if (CJsonReader::IsValid(document, { "Custom Data" }))
		{
			auto nodeInstances = document["Custom Data"].GetArray();

			for (unsigned int i = 0; i < nodeInstances.Size(); ++i)
			{
				if (nodeInstances[i].HasMember("Type"))
				{
					myNewVariableType = nodeInstances[i]["Type"].GetString();

					if (myNewVariableType == "Float")
					{
						float value = 0.0f;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EFloat));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EFloat, value);
					}
					else if (myNewVariableType == "Int")
					{
						int value = 0;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EInt));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EInt, value);
					}
					else if (myNewVariableType == "Bool")
					{
						bool value = false;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EBool));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EBool, value);
					}
					else if (myNewVariableType == "Start")
					{
						bool value = false;
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EStart));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EStart, value);
					}
					else if (myNewVariableType == "Vector 3")
					{
						Vector3 value = { 0.0f,0.0f,0.0f };
						CNodeTypeCollector::RegisterNewDataType(nodeInstances[i]["Data key"].GetString(), static_cast<int>(CNodeDataManager::EDataType::EVector3));
						CNodeDataManager::Get()->SetData(nodeInstances[i]["Data key"].GetString(), CNodeDataManager::EDataType::EVector3, value);
					}
					myCustomDataNodes.push_back(nodeInstances[i]["Data key"].GetString());
				}
			}
		}
	}
}

#ifdef _DEBUG
ImTextureID CGraphManager::HeaderTextureID()
{
	if (!myHeaderTextureID)
	{
		myHeaderTextureID = ImGui_LoadTexture("Imgui/Sprites/BlueprintBackground.png");
	}
	return myHeaderTextureID;
}
#endif

void CGraphManager::WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* /*aSecond*/, bool& aIsCyclic, CNodeInstance* aBase)
{
	if (aFirst == nullptr)
		return;

	if (aBase == nullptr)
		return;

	if (aIsCyclic)
		return;

	std::vector<SPin>& pins = aFirst->GetPins();
	for (auto& pin : pins)
	{
		if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
		{
			std::vector< SNodeInstanceLink*> links = aFirst->GetLinkFromPin(pin.myUID.AsInt());
			if (links.size() == 0)
			{
				return;
			}
			for (auto& link : links)
			{
				CNodeInstance* connectedNodeToOutPut = GetNodeFromPinID(link->myToPinID);

				if (connectedNodeToOutPut == aBase)
				{
					aIsCyclic |= true;
					return;
				}
				WillBeCyclic(connectedNodeToOutPut, nullptr, aIsCyclic, aBase);
			}
		}
	}
}

void CGraphManager::PreFrame()
{
#ifdef _DEBUG
	if (myRenderGraph)
	{
		auto& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 18));
		ImGui::SetNextWindowSize({ io.DisplaySize.x,  io.DisplaySize.y });
		ImGui::SetNextWindowBgAlpha(0.5f);

		ImGui::Begin(myCurrentGraph->myFolderPath.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::SameLine();
		if (ImGui::Button("Save"))
			mySave = true;

		ImGui::SetNextWindowPos(ImVec2(8.5f, 69.5f));
		ImGui::SetNextWindowSize({ 200.f,  io.DisplaySize.y - 69.5f });

		ImGui::Begin("Scripts:", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		static int selected = -1;

		for (int i = 0; i < myGraphs.size(); i++)
		{
			char buf[512];
			sprintf_s(buf, "%s", myGraphs[i].myFolderPath.c_str());

			if (ImGui::Selectable(buf, selected == i, ImGuiSelectableFlags_AllowDoubleClick))
			{
				selected = i;

				if (ImGui::IsMouseClicked(0))
				{
					myCurrentGraph = &myGraphs[i];
				}
			}
		}
		ImGui::End();
	}
	CreateNewDataNode();

	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		nodeInstance->DebugUpdate();
		nodeInstance->VisualUpdate(CTimer::Dt());
	}
#endif

	ReTriggerUpdatingTrees();

#ifdef _DEBUG
	if (myRenderGraph)
	{
		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
	}
#endif
}

bool ArePinTypesCompatible(SPin& aFirst, SPin& aSecond)
{
	if ((aFirst.myVariableType == SPin::EPinType::EFlow && aSecond.myVariableType != SPin::EPinType::EFlow))
	{
		return false;
	}
	if ((aSecond.myVariableType == SPin::EPinType::EFlow && aFirst.myVariableType != SPin::EPinType::EFlow))
	{
		return false;
	}
	return true;
}

void CGraphManager::ConstructEditorTreeAndConnectLinks()
{
#ifdef _DEBUG
	for (auto& nodeInstance : myCurrentGraph->myNodeInstances)
	{
		if (!nodeInstance->myHasSetEditorPosition)
		{
			ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPosition[0], nodeInstance->myEditorPosition[1]));
			nodeInstance->myHasSetEditorPosition = true;
		}

		// Start drawing nodes.
		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
		ed::BeginNode(nodeInstance->myUID.AsInt());
		ImGui::PushID(nodeInstance->myUID.AsInt());
		ImGui::BeginVertical("node");

		ImGui::BeginHorizontal("header");
		ImGui::Spring(0);
		ImGui::TextUnformatted(nodeInstance->GetNodeName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		ImGui::Spring(0);

		ImGui::EndHorizontal();
		ax::rect HeaderRect = ImGui_GetItemRect();
		ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

		bool previusWasOut = false;
		bool isFirstInput = true;
		bool isFirstIteration = true;
		for (auto& pin : nodeInstance->GetPins())
		{
			if (isFirstIteration)
			{
				if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
				{
					isFirstInput = false;
				}
				isFirstIteration = false;
			}

			if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
			{
				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);
				if (pin.myVariableType == SPin::EPinType::EFlow)
				{
					DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				}
				else
				{
					DrawTypeSpecificPin(pin, nodeInstance);

				}


				ed::EndPin();
				previusWasOut = false;

			}
			else
			{
				if (isFirstInput)
				{
					ImGui::SameLine(100, 0);
				}

				ImGui::Indent(150.0f);


				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Output);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);

				DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				ed::EndPin();
				previusWasOut = true;
				ImGui::Unindent(150.0f);
				isFirstInput = false;
			}
		}

		ImGui::EndVertical();
		auto ContentRect = ImGui_GetItemRect();
		ed::EndNode();

		if (ImGui::IsItemVisible())
		{
			auto drawList = ed::GetNodeBackgroundDrawList(nodeInstance->myUID.AsInt());

			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;
			auto headerColor = nodeInstance->GetColor();
			const auto uv = ImVec2(
				HeaderRect.w / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureID())),
				HeaderRect.h / (float)(4.0f * ImGui_GetTextureHeight(HeaderTextureID())));

			drawList->AddImageRounded(HeaderTextureID(),
				to_imvec(HeaderRect.top_left()) - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
				to_imvec(HeaderRect.bottom_right()) + ImVec2(8 - halfBorderWidth, 0),
				ImVec2(0.0f, 0.0f), uv,
				headerColor, ed::GetStyle().NodeRounding, 1 | 2);


			auto headerSeparatorRect = ax::rect(HeaderRect.bottom_left(), ContentRect.top_right());
			drawList->AddLine(
				to_imvec(headerSeparatorRect.top_left()) + ImVec2(-(8 - halfBorderWidth), -0.5f),
				to_imvec(headerSeparatorRect.top_right()) + ImVec2((8 - halfBorderWidth), -0.5f),
				ImColor(255, 255, 255, 255), 1.0f);
		}
		ImGui::PopID();
		ed::PopStyleVar();
	}


	for (auto& linkInfo : myCurrentGraph->myLinks)
		ed::Link(linkInfo.myID, linkInfo.myInputID, linkInfo.myOutputID);

	if (!myRunScripts)
	{
		// Handle creation action, returns true if editor want to create new object (node or link)
		if (ed::BeginCreate())
		{
			ed::PinId inputPinId, outputPinId;
			if (ed::QueryNewLink(&inputPinId, &outputPinId))
			{
				if (inputPinId && outputPinId)
				{
					if (ed::AcceptNewItem())
					{
						unsigned int inputPinID = static_cast<unsigned int>(inputPinId.Get());
						unsigned int outputPinID = static_cast<unsigned int>(outputPinId.Get());
						CNodeInstance* firstNode = GetNodeFromPinID(inputPinID);
						CNodeInstance* secondNode = GetNodeFromPinID(outputPinID);
						assert(firstNode);
						assert(secondNode);

						if (firstNode == secondNode)
						{
							// User trying connect input and output on the same node :/, who does this!?!
							// SetBlueScreenOnUserComputer(true)
						}
						else
						{
							{
								SPin* firstPin = firstNode->GetPinFromID(inputPinID);
								SPin* secondPin = secondNode->GetPinFromID(outputPinID);

								bool canAddlink = true;
								if (firstPin && secondPin)
								{
									if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN && secondPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
									{
										canAddlink = false;
									}
								}

								if (!ArePinTypesCompatible(*firstPin, *secondPin))
								{
									canAddlink = false;
								}

								if (!firstNode->CanAddLink(inputPinID))
								{
									canAddlink = false;
								}
								if (!secondNode->CanAddLink(outputPinID))
								{
									canAddlink = false;
								}

								if (firstNode->HasLinkBetween(inputPinID, outputPinID))
								{
									canAddlink = false;
								}


								if (canAddlink)
								{
									if (secondPin->myVariableType == SPin::EPinType::EUnknown)
									{
										secondNode->ChangePinTypes(firstPin->myVariableType);
									}
									unsigned int linkId = ++myCurrentGraph->myNextLinkIdCounter;
									firstNode->AddLinkToVia(secondNode, inputPinID, outputPinID, linkId);
									secondNode->AddLinkToVia(firstNode, outputPinID, inputPinID, linkId);

									bool aIsCyclic = false;
									WillBeCyclic(firstNode, secondNode, aIsCyclic, firstNode);
									if (aIsCyclic || !canAddlink)
									{
										firstNode->RemoveLinkToVia(secondNode, inputPinID);
										secondNode->RemoveLinkToVia(firstNode, outputPinID);
									}
									else
									{
										// Depending on if you drew the new link from the output to the input we need to create the link as the flow FROM->TO to visualize the correct flow
										if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
										{
											myCurrentGraph->myLinks.push_back({ ed::LinkId(linkId), outputPinId, inputPinId });
										}
										else
										{
											myCurrentGraph->myLinks.push_back({ ed::LinkId(linkId), inputPinId, outputPinId });
										}

										myUndoCommands.push({ ECommandAction::EAddLink, firstNode, secondNode, myCurrentGraph->myLinks.back(), 0 });

										mySave = true;
									}
								}
							}
						}
					}
				}
			}
		}
		ed::EndCreate();

		// Handle deletion action
		if (ed::BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			ed::LinkId deletedLinkId;
			while (ed::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ed::AcceptDeletedItem())
				{
					// Then remove link from your data.
					for (auto& link : myCurrentGraph->myLinks)
					{
						if (link.myID == deletedLinkId)
						{
							CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myInputID.Get()));
							CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myOutputID.Get()));
							assert(firstNode);
							assert(secondNode);

							firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myInputID.Get()));
							secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myOutputID.Get()));

							if (myPushCommand)
							{
								myUndoCommands.push({ ECommandAction::ERemoveLink, firstNode, secondNode, link, 0 });
							}

							myCurrentGraph->myLinks.erase(&link);
							mySave = true;

							break;
						}
					}
				}
			}
			ed::NodeId nodeId = 0;
			while (ed::QueryDeletedNode(&nodeId))
			{
				if (ed::AcceptDeletedItem())
				{

					auto it = myCurrentGraph->myNodeInstances.begin();
					while (it != myCurrentGraph->myNodeInstances.end())
					{
						if ((*it)->myUID.AsInt() == nodeId.Get())
						{

							(*it)->myNodeType->ClearNodeInstanceFromMap((*it));
							if (myPushCommand)
							{
								mySave = true;
								myUndoCommands.push({ ECommandAction::EDelete, (*it), nullptr,  {0,0,0}, (*it)->myUID.AsInt() });
							}

							it = myCurrentGraph->myNodeInstances.erase(it);
						}
						else
						{
							++it;
						}
					}

				}
			}
		}
		ed::EndDelete();

		auto openPopupPosition = ImGui::GetMousePos();
		ed::Suspend();

		if (ed::ShowBackgroundContextMenu())
		{
			ImGui::OpenPopup("Create New Node");
		}
		ed::Resume();

		ed::Suspend();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginPopup("Create New Node"))
		{
			auto newNodePostion = openPopupPosition;
			CNodeType** defaultTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EDefault);
			CNodeType** customTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::ECustom);
			CNodeType** childTypes = CNodeTypeCollector::GetAllNodeTypes(CNodeType::ENodeType::EChild);
			unsigned short noOfDefaultTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EDefault);
			unsigned short noOfCustomTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::ECustom);
			unsigned short noOfChildTypes = CNodeTypeCollector::GetNodeTypeCount(CNodeType::ENodeType::EChild);

			std::map< std::string, std::vector<CNodeType*>> cats;
			for (int i = 0; i < noOfDefaultTypes; i++)
			{
				cats[defaultTypes[i]->GetNodeTypeCategory()].push_back(defaultTypes[i]);
			}

			static bool noVariablesCreated = true;
			for (int i = 0; i < noOfCustomTypes; i++)
			{
				cats[customTypes[i]->GetNodeTypeCategory()].push_back(customTypes[i]);
				if (customTypes[i]->GetNodeTypeCategory() == "New Node Type")
					noVariablesCreated = false;
			}

			for (int i = 0; i < noOfChildTypes; i++)
			{
				cats[childTypes[i]->GetNodeTypeCategory()].push_back(childTypes[i]);
			}

			if (noVariablesCreated)
			{
				cats["New Node Type"].push_back(nullptr);
				cats["Delete Node Type"].push_back(nullptr);
			}

			ImGui::PushItemWidth(100.0f);
			ImGui::InputText("##edit", (char*)myMenuSearchField, 127);
			if (mySearchFokus)
			{
				ImGui::SetKeyboardFocusHere(0);
			}
			mySearchFokus = false;
			ImGui::PopItemWidth();

			if (myMenuSearchField[0] != '\0')
			{
				std::vector<CNodeType*> found;
				for (int i = 0; i < noOfDefaultTypes; i++)
				{
					std::string first = defaultTypes[i]->NodeName();
					std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
					std::string second = myMenuSearchField;
					std::transform(second.begin(), second.end(), second.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

					if (first.find(second) != std::string::npos)
						found.push_back(defaultTypes[i]);
				}

				for (int i = 0; i < noOfCustomTypes; i++)
				{
					std::string first = customTypes[i]->NodeName();
					std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
					std::string second = myMenuSearchField;
					std::transform(second.begin(), second.end(), second.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

					if (first.find(second) != std::string::npos)
						found.push_back(customTypes[i]);
				}

				for (int i = 0; i < noOfChildTypes; i++)
				{
					std::string first = childTypes[i]->NodeName();
					std::transform(first.begin(), first.end(), first.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
					std::string second = myMenuSearchField;
					std::transform(second.begin(), second.end(), second.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });

					if (first.find(second) != std::string::npos)
						found.push_back(childTypes[i]);
				}

				for (int i = 0; i < found.size(); i++)
				{
					CNodeInstance* node = nullptr;
					if (ImGui::MenuItem(found[i]->NodeName().c_str()))
					{
						node = new CNodeInstance(this);

						node->myNodeType = found[i];
						node->CheckIfInputNode();
						node->ConstructUniquePins();
						ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
						node->myHasSetEditorPosition = true;

						myCurrentGraph->myNodeInstances.push_back(node);

						if (myPushCommand)
						{
							mySave = true;
							myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
						}
					}
				}
			}
			else
			{
				for (auto& category : cats)
				{
					std::string theCatName = category.first;
					if (theCatName.empty())
					{
						theCatName = "General";
					}

					if (ImGui::BeginMenu(theCatName.c_str()))
					{
						CNodeInstance* node = nullptr;


						if (theCatName == "New Node Type")
						{
							for (const auto& instantiableVariable : myInstantiableVariables)
							{
								if (ImGui::MenuItem(instantiableVariable.c_str()))
								{
									myNewVariableType = instantiableVariable;
									myEnteringNodeName = true;
								}
							}
						}
						else if (theCatName == "Delete Node Type")
						{
							for (int i = 0; i < myCustomDataNodes.size(); ++i)
							{
								if (ImGui::MenuItem(myCustomDataNodes[i].c_str()))
								{
									for (auto& graph : myGraphs)
									{

										auto it = graph.myNodeInstances.begin();
										while (it != graph.myNodeInstances.end())
										{
											if ((*it)->myNodeType->NodeName() == "Get: " + myCustomDataNodes[i])
											{

												(*it)->myNodeType->ClearNodeInstanceFromMap((*it));
												std::vector<SNodeInstanceLink> links = (*it)->GetLinks();
												for (auto& link : links)
												{
													CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myFromPinID));
													CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myToPinID));

													firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myFromPinID));
													secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myToPinID));
												}
												it = graph.myNodeInstances.erase(it);
											}
											else if ((*it)->myNodeType->NodeName() == "Set: " + myCustomDataNodes[i])
											{

												(*it)->myNodeType->ClearNodeInstanceFromMap((*it));
												std::vector<SNodeInstanceLink> links = (*it)->GetLinks();
												for (auto& link : links)
												{
													CNodeInstance* firstNode = GetNodeFromPinID(static_cast<unsigned int>(link.myFromPinID));
													CNodeInstance* secondNode = GetNodeFromPinID(static_cast<unsigned int>(link.myToPinID));

													firstNode->RemoveLinkToVia(secondNode, static_cast<unsigned int>(link.myFromPinID));
													secondNode->RemoveLinkToVia(firstNode, static_cast<unsigned int>(link.myToPinID));
												}
												it = graph.myNodeInstances.erase(it);
											}
											else
											{
												++it;
											}
										}
									}
									CNodeTypeCollector::DegisterCustomDataType(myCustomDataNodes[i]);
									std::swap(myCustomDataNodes[i], myCustomDataNodes.back());
									myCustomDataNodes.pop_back();
									CNodeDataManager::Get()->SaveDataTypesToJson();
								}
							}
						}
						else
						{

							for (int i = 0; i < category.second.size(); i++)
							{
								if (category.second[i] == nullptr)
									break;

								CNodeType* type = category.second[i];
								if (ImGui::MenuItem(type->NodeName().c_str()))
								{
									node = new CNodeInstance(this, true);

									//int nodeType = i;
									node->myNodeType = type;
									node->CheckIfInputNode();
									node->ConstructUniquePins();
									ed::SetNodePosition(node->myUID.AsInt(), newNodePostion);
									node->myHasSetEditorPosition = true;

									myCurrentGraph->myNodeInstances.push_back(node);

									if (myPushCommand)
									{
										mySave = true;
										myUndoCommands.push({ ECommandAction::ECreate, node, nullptr, {0,0,0}, node->myUID.AsInt() });
									}
								}
							}
						}

						ImGui::EndMenu();
					}
				}

			}
			ImGui::EndPopup();
		}
		else
		{
			mySearchFokus = true;
			memset(&myMenuSearchField[0], 0, sizeof(myMenuSearchField));
		}

		ImGui::PopStyleVar();
		ed::Resume();

		myPushCommand = true;

		if (ed::BeginShortcut())
		{
			if (ed::AcceptCopy())
			{
				SaveNodesToClipboard();
			}

			if (ed::AcceptPaste())
			{
				LoadNodesFromClipboard();
			}

			if (ed::AcceptUndo())
			{
				if (!myUndoCommands.empty())
				{
					myPushCommand = false;
					ed::ResetShortCutAction();
					auto& command = myUndoCommands.top();
					EditorCommand inverseCommand = command;
					SPin* firstPin;
					SPin* secondPin;

					switch (command.myAction)
					{
					case CGraphManager::ECommandAction::ECreate:
						inverseCommand.myAction = ECommandAction::EDelete;
						ed::DeleteNode(command.myResourceUID);
						break;
					case CGraphManager::ECommandAction::EDelete:
						inverseCommand.myAction = ECommandAction::ECreate;
						myCurrentGraph->myNodeInstances.push_back(command.myNodeInstance);
						break;
					case CGraphManager::ECommandAction::EAddLink:
						inverseCommand.myAction = ECommandAction::ERemoveLink;
						ed::DeleteLink(command.myEditorLinkInfo.myID);
						break;
					case CGraphManager::ECommandAction::ERemoveLink:
						inverseCommand.myAction = ECommandAction::EAddLink;
						command.myNodeInstance->AddLinkToVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), command.myResourceUID);
						command.mySecondNodeInstance->AddLinkToVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), command.myResourceUID);

						firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()));
						secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()));

						if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
							myCurrentGraph->myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myInputID, command.myEditorLinkInfo.myOutputID });
						else
							myCurrentGraph->myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myOutputID, command.myEditorLinkInfo.myInputID });
						break;
					default:
						break;
					}
					myUndoCommands.pop();
					mySave = true;
					myRedoCommands.push(inverseCommand);
				}
			}

			if (ed::AcceptRedo())
			{
				if (!myRedoCommands.empty())
				{
					myPushCommand = false;
					ed::ResetShortCutAction();
					auto& command = myRedoCommands.top();
					EditorCommand inverseCommand = command;
					SPin* firstPin;
					SPin* secondPin;

					switch (command.myAction)
					{
					case CGraphManager::ECommandAction::ECreate:
						inverseCommand.myAction = ECommandAction::EDelete;
						ed::DeleteNode(command.myResourceUID);
						break;
					case CGraphManager::ECommandAction::EDelete:
						inverseCommand.myAction = ECommandAction::ECreate;
						myCurrentGraph->myNodeInstances.push_back(command.myNodeInstance);
						break;
					case CGraphManager::ECommandAction::EAddLink:
						inverseCommand.myAction = ECommandAction::ERemoveLink;
						ed::DeleteLink(command.myEditorLinkInfo.myID);
						break;
					case CGraphManager::ECommandAction::ERemoveLink:
						inverseCommand.myAction = ECommandAction::EAddLink;
						command.myNodeInstance->AddLinkToVia(command.mySecondNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), command.myResourceUID);
						command.mySecondNodeInstance->AddLinkToVia(command.myNodeInstance, static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()), static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()), command.myResourceUID);

						firstPin = command.myNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myInputID.Get()));
						secondPin = command.mySecondNodeInstance->GetPinFromID(static_cast<unsigned int>(command.myEditorLinkInfo.myOutputID.Get()));

						if (firstPin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
							myCurrentGraph->myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myInputID, command.myEditorLinkInfo.myOutputID });
						else
							myCurrentGraph->myLinks.push_back({ command.myEditorLinkInfo.myID, command.myEditorLinkInfo.myOutputID, command.myEditorLinkInfo.myInputID });
						break;
					default:
						break;
					}
					myRedoCommands.pop();
					mySave = true;
					myUndoCommands.push(inverseCommand);
				}
			}
		}
	}
#endif
}

void CGraphManager::PostFrame()
{
#ifdef _DEBUG
	if (mySave)
	{
		mySave = false;
		SaveTreeToFile();
	}
	if (myShowFlow)
	{
		for (int i = 0; i < myCurrentGraph->myLinks.size(); i++)
		{
			ed::Flow(myCurrentGraph->myLinks[i].myID);
		}
	}
	for (auto i : myFlowsToBeShown)
	{
		ed::Flow(i);
	}
	myFlowsToBeShown.clear();
	ed::End();
	ed::SetCurrentEditor(nullptr);
#endif
}