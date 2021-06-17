#include "stdafx.h"
#include "DialogueSystem.h"

#include <sstream>
#include <cstdio>

#include "TextFactory.h"
#include "TextInstance.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"
#include "Scene.h"
#include "WindowHandler.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include "Input.h"
#include "JsonReader.h"
#include "MainSingleton.h"
#include <EngineException.h>

namespace SM = DirectX::SimpleMath;

CDialogueSystem::CDialogueSystem()
{
	myAnimatedDialogue = nullptr;
	myDialogueBox = nullptr;
	myText1600x900 = nullptr;
	myText1920x1080 = nullptr;
	myText2560x1440 = nullptr;
}

CDialogueSystem::~CDialogueSystem() 
{
	delete myAnimatedDialogue;
	myAnimatedDialogue = nullptr;
	delete myDialogueBox;
	myDialogueBox = nullptr;

	// For some reason we always seem to destroy the postmaster before the dialogue system
	//CMainSingleton::PostMaster().Unsubscribe(EMessageType::LoadDialogue, this);
}

bool CDialogueSystem::Init()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::LoadDialogue, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::IntroStarted, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::StopDialogue, this);
	CMainSingleton::PostMaster().Subscribe("DELevel1", this);
	CMainSingleton::PostMaster().Subscribe("DELevel2", this);
	CMainSingleton::PostMaster().Subscribe("DELevel3", this);

	rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/UI/DialogueSystemInit.json");
	ENGINE_BOOL_POPUP(!document.HasParseError(), "Could not load 'Json/UI/DialogueSystemInit.json'!");

	CSpriteFactory* factory = CSpriteFactory::GetInstance();
	CTextFactory* textFactory = CTextFactory::GetInstance();

	std::string dialogueBoxPath = document["Dialogue Box Path"].GetString();
	if(!dialogueBoxPath.empty())
	{
		myDialogueBox = new CSpriteInstance();
		myDialogueBox->Init(factory->GetSprite(dialogueBoxPath));
		myDialogueBox->SetPosition({0.0f, 0.4f});
	}

	myText1600x900 = nullptr;
	myText1920x1080 = nullptr;
	myText2560x1440 = nullptr;

	myText1600x900 = new CTextInstance();
	myText1600x900->Init(textFactory->GetText(document["Dialogue Text Font and Size 1600x900"].GetString()));
	myText1600x900->SetPivot({0.5f, 0.5f});
	myText1600x900->SetPosition({document["Dialogue Text Position X"].GetFloat(), document["Dialogue Text Position Y"].GetFloat()});
	myText1600x900->SetColor({document["Dialogue Text Color R"].GetFloat(), document["Dialogue Text Color G"].GetFloat(), document["Dialogue Text Color B"].GetFloat(), 1.0f});

	myText1920x1080 = new CTextInstance();
	myText1920x1080->Init(textFactory->GetText(document["Dialogue Text Font and Size 1920x1080"].GetString()));
	myText1920x1080->SetPivot({ 0.5f, 0.5f });
	myText1920x1080->SetPosition({ document["Dialogue Text Position X"].GetFloat(), document["Dialogue Text Position Y"].GetFloat() });
	myText1920x1080->SetColor({ document["Dialogue Text Color R"].GetFloat(), document["Dialogue Text Color G"].GetFloat(), document["Dialogue Text Color B"].GetFloat(), 1.0f });

	myText2560x1440 = new CTextInstance();
	myText2560x1440->Init(textFactory->GetText(document["Dialogue Text Font and Size 2560x1440"].GetString()));
	myText2560x1440->SetPivot({ 0.5f, 0.5f });
	myText2560x1440->SetPosition({ document["Dialogue Text Position X"].GetFloat(), document["Dialogue Text Position Y"].GetFloat() });
	myText2560x1440->SetColor({ document["Dialogue Text Color R"].GetFloat(), document["Dialogue Text Color G"].GetFloat(), document["Dialogue Text Color B"].GetFloat(), 1.0f });

	float resolutionY = CEngine::GetInstance()->GetWindowHandler()->GetResolution().y;

	if (resolutionY == 900.0f)
		myAnimatedDialogue = myText1600x900;
	if (resolutionY == 1080.0f)
		myAnimatedDialogue = myText1920x1080;
	if (resolutionY == 1440.0f)
		myAnimatedDialogue = myText2560x1440;

	myLineBreakDialogue = document["Dialogue Line Break After Characters"].GetInt();

	return true;
}

void CDialogueSystem::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::LoadDialogue:
		PostMaster::SPlayDynamicAudioData data = *static_cast<PostMaster::SPlayDynamicAudioData*>(aMessage.data);
		LoadDialogue(data.mySoundIndex, data.myChannel);
		break;
	case EMessageType::StopDialogue:
		ExitDialogue();
	default:
		break;
	}
}

void CDialogueSystem::Receive(const SStringMessage& aMessage)
{
	std::array<std::string, 3> dialogueScenes = { "DELevel1", "DELevel2", "DELevel3" };
	for (size_t i = 0; i < dialogueScenes.size(); ++i)
	{
		if (dialogueScenes[i] == std::string(aMessage.myMessageType))
		{
			LoadDialogue(static_cast<int>(i), nullptr);
			break;
		}
	}
}

void CDialogueSystem::LoadDialogue(int aSceneIndex, CAudioChannel* a3DChannel) {
	ExitDialogue();
	myDialogueBuffer.clear();
	myCurrentVoiceLine = -2;

	FILE* fp;
	fopen_s(&fp, "Json/Dialogue/DialogueTextLines.json", "rb");
	char read_buffer[200];
	rapidjson::FileReadStream is(fp, read_buffer, sizeof(read_buffer));

	rapidjson::Document doc;
	doc.ParseStream(is);

	fclose(fp);

	if (doc.HasMember("Scenes")) 
	{
		const rapidjson::Value& object = doc["Scenes"][aSceneIndex]["Lines"].GetArray();
		assert(object.IsArray());
		for (unsigned int i = 0; i < object.Size(); ++i) 
		{
			myDialogueBuffer.emplace_back(object[i]["Text"].GetString(), object[i]["Duration"].GetFloat(), object[i]["Voiceline Index"].GetInt());
			if (object[i]["3DChannel"].GetBool() && a3DChannel)
			{
				myDialogueBuffer.back().myChannel = a3DChannel;
			}
		}
	}

	float resolutionY = CEngine::GetInstance()->GetWindowHandler()->GetResolution().y;

	if (resolutionY == 900.0f)
		myAnimatedDialogue = myText1600x900;
	if (resolutionY == 1080.0f)
		myAnimatedDialogue = myText1920x1080;
	if (resolutionY == 1440.0f)
		myAnimatedDialogue = myText2560x1440;

	myIsActive = !myDialogueBuffer.empty();
}

#include "AudioChannel.h"
void CDialogueSystem::ExitDialogue() 
{
	if (!myDialogueBuffer.empty())
	{
		if (myDialogueBuffer[myCurrentDialogueIndex].myChannel)
			myDialogueBuffer[myCurrentDialogueIndex].myChannel->Stop();
	}

	myIsActive = false;
	myCurrentDialogueIndex = 0;
	myLastDialogueIndex = -1;
	myHeldButtonTimer = 0.0f;
	myDialogueTimer = 0.0f;
	myLineBreakCounter = 0;

	myAnimatedDialogue->SetText("");

	myEnabled = true;
}

void CDialogueSystem::Update() 
{
	if (!myIsActive) {
		return;
	}

	//HandleInput();
	
	myDialogueTimer += CTimer::Dt();
	if (myDialogueTimer >= myDialogueBuffer[myCurrentDialogueIndex].myDuration)
	{
		myCurrentDialogueIndex = static_cast<size_t>(myCurrentDialogueIndex + 1) % myDialogueBuffer.size();
		myDialogueTimer = 0.0f;
		myLineBreakCounter = 0;
		myCurrentLine = "";

		if (myCurrentDialogueIndex == 0) {
			CMainSingleton::PostMaster().SendLate({ EMessageType::StopDialogue, NULL });
			ExitDialogue();
		}
	}

	if (myIsActive && (myCurrentDialogueIndex != myLastDialogueIndex))
	{
		if (myDialogueBuffer[myCurrentDialogueIndex].myChannel)
		{
			PostMaster::SPlayDynamicAudioData playData = 
			{
				myDialogueBuffer[myCurrentDialogueIndex].myVoiceLine,
				myDialogueBuffer[myCurrentDialogueIndex].myChannel
			};
			CMainSingleton::PostMaster().Send({ EMessageType::Play3DVoiceLine, &playData });
		}
		else
		{
			CMainSingleton::PostMaster().SendLate({ EMessageType::Play2DVoiceLine, &myDialogueBuffer[myCurrentDialogueIndex].myVoiceLine });
		}
		ProcessLineBreaks();
		myDialogueTimer = 0.0f;
	}

	if (myIsActive && (myCurrentDialogueIndex < myDialogueBuffer.size())) 
	{
		//myDialogueTimer += CTimer::Dt();
		//int length = static_cast<int>(myDialogueBuffer[myCurrentDialogueIndex].myText.length());
		//float percentage = myDialogueTimer / (myDialogueSpeed * length);
		//int numberOfLetters = static_cast<int>(percentage * length);

		myCurrentLine.assign(myDialogueBuffer[myCurrentDialogueIndex].myText, 0, myDialogueBuffer[myCurrentDialogueIndex].myText.size());

		myAnimatedDialogue->SetText(myCurrentLine);

		myLastDialogueIndex = myCurrentDialogueIndex;
	}
}

void CDialogueSystem::EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const
{
	if (!myIsActive) {
		return;
	}

	if (myDialogueBox)
		someSprites.emplace_back(myDialogueBox);
}

void CDialogueSystem::EmplaceTexts(std::vector<CTextInstance*>& someText) const
{
	if (!myIsActive) {
		return;
	}

	if (myAnimatedDialogue)
		someText.emplace_back(myAnimatedDialogue);
}

void CDialogueSystem::HandleInput() {
	if (!myIsActive) {
		return;
	}

	int length = static_cast<int>(myDialogueBuffer[myCurrentDialogueIndex].myText.length());
	if (Input::GetInstance()->IsKeyPressed(VK_SPACE) && myDialogueTimer >= (myDialogueSpeed * length)) {
		myCurrentDialogueIndex = static_cast<size_t>(myCurrentDialogueIndex + 1) % myDialogueBuffer.size();
		myDialogueTimer = 0.0f;
		myLineBreakCounter = 0;
		myCurrentLine = "";

		if (myCurrentDialogueIndex == 0) {
			CMainSingleton::PostMaster().SendLate({ EMessageType::StopDialogue, NULL });
			ExitDialogue();
		}
	}

	if (Input::GetInstance()->IsKeyDown(VK_SPACE)) {
		myHeldButtonTimer += CTimer::Dt();

		if (myHeldButtonTimer >= 0.3f) {
			SetDialogueSpeed(myDialogueFastSpeed, length);
		}
	}

	if (Input::GetInstance()->IsKeyReleased(VK_SPACE)) {
		myHeldButtonTimer = 0.0f;
		SetDialogueSpeed(myDialogueSlowSpeed, length);
	}

	// Needs to be controller by other states
	//if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE)) {
	//	CMainSingleton::PostMaster().Send({ EMessageType::StopDialogue, NULL });
	//	ExitDialogue();
	//}
}

void CDialogueSystem::SetDialogueSpeed(float aSpeed, int aLineLength)
{
	float percentage = myDialogueTimer / (myDialogueSpeed * aLineLength);
	myDialogueSpeed = aSpeed;
	myDialogueTimer = percentage * (myDialogueSpeed * aLineLength);
}

void CDialogueSystem::ProcessLineBreaks()
{
	int lineBreak = myLineBreakDialogue;
	myLineBreakCounter = 0;
	for (unsigned int i = 0; i < myDialogueBuffer[myCurrentDialogueIndex].myText.size(); ++i)
	{
		if (++myLineBreakCounter >= lineBreak)
		{
			char currentChar = myDialogueBuffer[myCurrentDialogueIndex].myText.at(i);
			if (currentChar == ' ') {
				myDialogueBuffer[myCurrentDialogueIndex].myText.insert((size_t)i + (size_t)1, "\n");
			} else {
				int backwardsCounter = 0;
				while (currentChar != ' ') {
					currentChar = myDialogueBuffer[myCurrentDialogueIndex].myText.at((size_t)i - (size_t)++backwardsCounter);
				}
				myDialogueBuffer[myCurrentDialogueIndex].myText.insert((size_t)i - (size_t)backwardsCounter + (size_t)1, "\n");
			}
			myLineBreakCounter = 0;
			i--;
		}
	}
}
