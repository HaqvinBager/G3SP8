#pragma once
#include "ResponseBehavior.h"

class CToggleResponse : public IResponseBehavior
{
public:
	struct SSettings {
		bool myEnableOnStart;
		bool myEnableOnNotify;
		std::string myType;
		int myTargetInstanceID;
	};
	CToggleResponse(CGameObject& aParent, const SSettings& someSettings);
	~CToggleResponse() override;
	void Awake() override;
	void Start() override;
	void Update() override;
	void OnRespond() override;

private:
	template<typename T>
	void SetTarget(const CGameObject& aTarget);

	CBehavior* myTarget;
	SSettings mySettings;
};

template<typename T>
inline void CToggleResponse::SetTarget(const CGameObject& aTarget)
{
	T* componentType = nullptr;
	if (aTarget.TryGetComponent(&componentType))
	{
		myTarget = componentType;
		//myTarget->Enabled(mySettings.myEnableOnStart);
	}
	else
	{
		std::cout << __FUNCTION__ << "[Puzzle Error] Toggle's Target of type: " << mySettings.myType << " was not found on " << GameObject().Name() << std::endl;
	}
}
