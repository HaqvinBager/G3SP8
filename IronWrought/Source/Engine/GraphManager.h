#pragma once

#ifdef _DEBUG
#include <imgui_node_editor.h>
#endif // DEBUG
#include <stack>
#include <unordered_map>
//#include "NodeData.h"

class CNodeInstance;
class CGameObject;

struct BluePrintInstance {
	int rootID;
	std::vector<int> childrenIDs;
};

#ifdef _DEBUG
namespace ed = ax::NodeEditor;
#endif // DEBUG

class CGraphManager
{
public:
	~CGraphManager();
	void Load(const std::string& aSceneName);
	void Clear();
	void ReTriggerUpdatingTrees();

	void PreFrame();
	void ConstructEditorTreeAndConnectLinks();
	void PostFrame();

	//void ReTriggerTree();
	void SaveTreeToFile();
	void LoadTreeFromFile();
	void SaveNodesToClipboard();
	void LoadNodesFromClipboard();

	void ShowFlow(int aLinkID);


	void Update();

	void ToggleShouldRenderGraph();
	bool ToggleShouldRunScripts();
	bool ShouldRenderGraph() { return myRenderGraph; }

	CGameObject* GetCurrentGameObject();
	std::vector<CGameObject*> GetCurrentGameObjectChildren();
	const int GetCurrentBlueprintInstanceID() const;

private:
	struct SEditorLinkInfo
	{
#ifdef _DEBUG
		ed::LinkId myID;
		ed::PinId  myInputID;
		ed::PinId  myOutputID;
#endif
	};

	enum class ECommandAction {
		ECreate,
		EDelete,
		EAddLink,
		ERemoveLink,
		EChangeDataOnPin,
		ECount
	};

	struct EditorCommand
	{
		ECommandAction myAction;
		CNodeInstance* myNodeInstance;
		CNodeInstance* mySecondNodeInstance;
		SEditorLinkInfo myEditorLinkInfo;
		unsigned int myResourceUID;
		//unsigned int myInputPinID;
		//unsigned int myOutputPinID;

		//void Do();
		//void Undo();
	};

	struct SGraph
	{
		std::vector<CNodeInstance*> myNodeInstances;
		std::vector<BluePrintInstance> myBluePrintInstances;
#ifdef _DEBUG
		ImVector<SEditorLinkInfo> myLinks;
#endif
		std::string myFolderPath;
		std::string myChildrenKey;
		unsigned int myNextLinkIdCounter = 100;

		void Clear();
	};
private:
#ifdef _DEBUG


	ImTextureID HeaderTextureID();
#endif // _DEBUG
	void WillBeCyclic(CNodeInstance* aFirst, CNodeInstance* aSecond, bool& aIsCyclic, CNodeInstance* aBase);

	CNodeInstance* GetNodeFromPinID(unsigned int anID);
	CNodeInstance* GetNodeFromNodeID(unsigned int anID);
	void DrawTypeSpecificPin(struct SPin& aPin, CNodeInstance* aNodeInstance);

	void CreateNewDataNode();
	void LoadDataNodesFromFile();



private:


	std::vector<SGraph> myGraphs;
	SGraph* myCurrentGraph;
	BluePrintInstance myCurrentBluePrintInstance;

	//std::unordered_map<std::string, std::vector<CNodeInstance*>> myGraphs;
	//std::unordered_map<std::string, std::vector<BluePrintInstance>> myGameObjectIDsMap;
	//std::vector<std::string> myKeys;
	//std::string myCurrentPath;
	//std::string myCurrentKey;

	std::vector<int> myFlowsToBeShown;
	std::vector<std::string> myInstantiableVariables;

	std::stack<EditorCommand> myUndoCommands;
	std::stack<EditorCommand> myRedoCommands;


	//std::unordered_map<std::string, CNodeData*> myVariableNodeDataMap;
	std::string myNewVariableType;

	char* myMenuSearchField = nullptr;

	bool myEnteringNodeName = false;
	bool mySetPosition = false;
	bool mySave = false;
	bool myShowFlow = false;
	bool mySearchFokus = true;
	bool myPushCommand = true;
	bool myRenderGraph;
	bool myRunScripts;
	std::string mySceneFolder;

#ifdef _DEBUG
	ImTextureID myHeaderTextureID;
#endif // _DEBUG

	std::vector<std::string> myCustomDataNodes;
};
