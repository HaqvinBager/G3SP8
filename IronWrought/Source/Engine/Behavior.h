#pragma once
#include "Component.h"

class CBehavior : public CComponent
{
public:

	CBehavior(CGameObject& aParent) : CComponent(aParent), myEnabled(true) {}

	virtual ~CBehavior() {}

	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;

	void Enabled(bool aEnabled) override { 
		if (myEnabled != aEnabled)
		{
			if (aEnabled == true)
				OnEnable();
			else
				OnDisable();
		}
		myEnabled = aEnabled;
	}
	const bool Enabled() const override { return myEnabled; }

protected:
	bool myEnabled;

private:
};

