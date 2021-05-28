#pragma once
#include "Behavior.h"

enum class EKeyInteractionTypes
{
	Destroy,
	Animate
};

class CKeyBehavior : public CBehavior
{
public:
	struct SSettings
	{
		std::string myOnCreateNotify;
		std::string myInteractNotify;

		void* myData;
	};

	CKeyBehavior(CGameObject& aParent, const SSettings& someSettings);
	virtual ~CKeyBehavior() override;

public:
	void Awake() override;
	void Start() override;
	virtual void Update() override;

	void OnEnable() override;
	void OnDisable() override;
public:
	virtual void OnInteract() = 0;

	SSettings mySettings;
};