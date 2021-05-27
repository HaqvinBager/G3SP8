#pragma once
#include "Behavior.h"

class CCustomAnimation : public CBehavior
{
public:
	CCustomAnimation(CGameObject& aParent);
	void Awake()		override;
	void Start()		override;
	void Update()		override;
	void OnEnable()		override;
	void OnDisable()	override;


};

