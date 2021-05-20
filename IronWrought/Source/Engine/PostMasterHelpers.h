#pragma once

namespace PostMaster
{
	static const char* SMSG_DISABLE_GLOVE  = "DisableGlove";
	static const char* SMSG_ENABLE_GLOVE   = "EnableGlove";
	static const char* SMSG_DISABLE_CANVAS = "DisableCanvas";
	static const char* SMSG_ENABLE_CANVAS  = "EnableCanvas";

	static const char* SMSG_AUDIO = "AudioEvent";
	static const char* SMSG_TO_MAIN_MENU = "ToMainMenu";

	static const char* SMSG_UIMOVE = "UIMove";
	static const char* SMSG_UIINTERACT = "UIInteract";
	static const char* SMSG_UIPULL = "UIPull";
	static const char* SMSG_UIPUSH = "UIPush";
	static const char* SMSG_UICROUCH = "UICrouch";
	static const char* SMSG_UIJUMP = "UIJump";

	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);
	const bool DisableGravityGlove(const char* aStringMsg);
	const bool EnableGravityGlove(const char* aStringMsg);
	const bool DisableCanvas(const char* aStringMsg);
	const bool EnableCanvas(const char* aStringMsg);
	
	const bool CompareStringMessage(const char* aCompareTo, const char* aReceived);

}