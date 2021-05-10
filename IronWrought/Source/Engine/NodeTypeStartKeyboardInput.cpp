#include "stdafx.h"
#include "NodeTypeStartKeyboardInput.h"
#include "NodeType.h"
#include "NodeInstance.h"

CNodeTypeStartKeyboardInput::CNodeTypeStartKeyboardInput()
{
	myPins.push_back(SPin("Key", SPin::EPinTypeInOut::EPinTypeInOut_IN, SPin::EPinType::EString));
	myPins.push_back(SPin("", SPin::EPinTypeInOut::EPinTypeInOut_OUT));
}

int CNodeTypeStartKeyboardInput::OnEnter(class CNodeInstance* aTriggeringNodeInstance)
{
	SPin::EPinType outType;
	NodeDataPtr someData = nullptr;
	size_t outSize = 0;

	GetDataOnPin(aTriggeringNodeInstance, 0, outType, someData, outSize);
	std::string nodeInput = static_cast<char*>(someData);
	//char input = reinterpret_cast<char>(keyboardInput.c_str());
	CGameObject* gameObject = aTriggeringNodeInstance->GetCurrentGameObject();
	gameObject;
	int keyboardInput = 0;
	if (nodeInput.size() <= 1)
	{
		if (std::islower(nodeInput[0]))
			keyboardInput = std::toupper(nodeInput[0]);
		else
			keyboardInput = nodeInput[0];
	}
	else
	{
		if (nodeInput == "return" || nodeInput == "Return" || nodeInput == "RETURN" 
			|| nodeInput == "enter" || nodeInput == "Enter" || nodeInput == "ENTER")
		{
			keyboardInput = VK_RETURN;
		}
		else if (nodeInput == "escape" || nodeInput == "Escape" || nodeInput == "ESCAPE")
		{
			keyboardInput = VK_ESCAPE;
		}
		else if (nodeInput == "shift" || nodeInput == "Shift" || nodeInput == "SHIFT")
		{
			keyboardInput = VK_SHIFT;
		}
		else if (nodeInput == "control" || nodeInput == "Control" || nodeInput == "CONTROL")
		{
			keyboardInput = VK_CONTROL;
		}
	}

	if (Input::GetInstance()->IsKeyPressed(keyboardInput))
	{
		return 1;
	}

	return -1;

}
