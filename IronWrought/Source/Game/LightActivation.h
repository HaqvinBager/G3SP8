#pragma once
#include "ActivationBehavior.h"
class CLightActivation : public IActivationBehavior
{
public:
	struct SData {
		std::string myTargetType;
		Vector3 myColor;
		float myIntensity;
		int myTargetInstanceID;
	};

	CLightActivation(CGameObject& aParent, const SData& aData);
	~CLightActivation() override;

	void Awake() override;
	void OnActivation() override;

private:
	template<typename T>
	void SetTarget(const CGameObject& aTarget);


private:
	SData myData;
	CBehavior* myTarget;
};

template<typename T>
inline void CLightActivation::SetTarget(const CGameObject& aTarget)
{
	T* componentType = nullptr;
	if (!aTarget.TryGetComponent(&componentType))
		std::cout << __FUNCTION__ << "[Puzzle Error] Light Activation's Target of type: " << myData.myTargetType << " was not found on " << GameObject().Name() << std::endl;
	myTarget = componentType;
}
