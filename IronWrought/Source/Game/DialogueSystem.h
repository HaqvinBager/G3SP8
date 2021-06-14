#pragma once
#include <vector>
#include <memory>
#include <string>
#include "SimpleMath.h"
#include "PostMaster.h"

class CSpriteInstance;
class CTextInstance;
class CScene;
class CAudioChannel;

struct SSpeakerLine {
	SSpeakerLine(std::string aString, float aDuration, int aLine)
		: myText(aString)
		, myDuration(aDuration)
		, myChannel(nullptr)
		, myVoiceLine(aLine) {}

	CAudioChannel* myChannel;
	std::string myText;
	float myDuration;
	int myVoiceLine;
};

class CDialogueSystem : public IObserver, public IStringObserver {
public:
	CDialogueSystem();
	~CDialogueSystem();

	bool Init();
	void Receive(const SMessage& aMessage) override;
	void Receive(const SStringMessage& aMessage) override;
	void Update();

public:
	void EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const;
	void EmplaceTexts(std::vector<CTextInstance*>& someText) const;
	
	bool Active() {
		return myIsActive;
	}

	void ExitDialogue();

private:
	void Active(bool isActive) {
		myIsActive = isActive;
	}

	void LoadDialogue(int aSceneIndex, CAudioChannel* a3DChannel);

	void HandleInput();
	void SetDialogueSpeed(float aSpeed, int aLineLength);
	void ProcessLineBreaks();

private:
	CSpriteInstance* myDialogueBox;

	CTextInstance* myAnimatedDialogue;
	CTextInstance* myText1600x900;
	CTextInstance* myText1920x1080;
	CTextInstance* myText2560x1440;

	std::vector<SSpeakerLine> myDialogueBuffer;
	std::string myCurrentLine = "";

	bool myIsActive = false;
	bool myEnabled = true;
	int myCurrentDialogueIndex = 0;
	int myLastDialogueIndex = -1;
	int myCurrentVoiceLine = -2;
	int myLineBreakCounter = 0;
	int myLineBreakDialogue = 0;
	float myHeldButtonTimer = 0.0f;
	float myDialogueTimer = 0.0f;
	float myDialogueSpeed = 0.05f;
	float myDialogueSlowSpeed = 0.05f;
	float myDialogueFastSpeed = 0.01f;
};

