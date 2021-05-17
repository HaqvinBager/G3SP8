#pragma once
#include "Behaviour.h"

class CCustomAnimation : public CBehaviour
{
public:
	CCustomAnimation(CGameObject& aParent);
	void Awake()		override;
	void Start()		override;
	void Update()		override;
	void OnEnable()		override;
	void OnDisable()	override;


};

