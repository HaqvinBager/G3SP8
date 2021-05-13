#pragma once
#include "Component.h"

class CBehaviour : public CComponent
{
public:

	CBehaviour(CGameObject& aParent) : CComponent(aParent), myEnabled(true) {}

	virtual ~CBehaviour() {}

	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;

	void Enabled(bool aEnabled) { 
		if (myEnabled != aEnabled)
		{
			if (aEnabled == true)
				OnEnable();
			else
				OnDisable();
		}
		myEnabled = aEnabled;
	}
	const bool Enabled() const { return myEnabled; }

protected:
	bool myEnabled;

private:
};

