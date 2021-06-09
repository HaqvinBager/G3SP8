#include "stdafx.h"
#include "CameraComponent.h"
#include "RandomNumberGenerator.h"
#include "TransformComponent.h"
#include "SpriteInstance.h"
#include "SpriteFactory.h"

#include "Engine.h"
#include "Scene.h"
#include "PostMaster.h"
#include "MainSingleton.h"
#include "JsonReader.h"

CCameraComponent::CCameraComponent(CGameObject& aParent, const float aFoV/*, float aNearPlane, float aFarPlane, DirectX::SimpleMath::Vector2 aResolution*/)
	: CComponent(aParent), myFoV(aFoV)
{
	myProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(myFoV), (16.0f / 9.0f), 0.1f, 50.0f);
	myView = DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), Vector3::Forward, Vector3::Up);

	DirectX::BoundingFrustum::CreateFromMatrix(myViewFrustum, myProjection);

	myTrauma = 0.0f;
	myTestTrauma = 0.4f;
	myIdleTrauma = 0.4f;
	myShake = 0.0f;
	myDecayInSeconds = 0.5f;
	myShakeSpeed = 0.65f;
	myMaxShakeRotation = { 2.05f, 0.5f, 0.5f };

	myIdleNoise = FastNoise::New<FastNoise::Simplex>();
	myShakeNoise = FastNoise::New<FastNoise::Simplex>();
	//myNoise = PerlinNoise(214125213);

	myShakeTimer = 0.0f;

	myFadingPlane = nullptr;
	myFadeParameter = 1.0f;
	myFadeSpeed = 1.0f;
	myFadingPlaneActive = false;
	myFadePermanent = false;

	myState = ECameraState::Default;
	myShakeState = ECameraShakeState::IdleSway;
}

CCameraComponent::~CCameraComponent()
{
	delete myFadingPlane;
	myFadingPlane = nullptr;
}

void CCameraComponent::Awake()
{
	rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/Settings/CameraInit.json");

	myFadingPlane = new CSpriteInstance();
	myFadingPlane->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Fade Screen Path"].GetString())));
	myFadingPlane->SetSize({ 15.1f, 8.5f });
	myFadingPlane->SetRenderOrder(ERenderOrder::Layer3);
	myFadingPlane->SetShouldRender(false);
}

void CCameraComponent::Start()
{
}

float LogEaseOut(float x) {
	return 1.f - (log(x) / log(0.01f));
}

float LogEaseIn(float x) {
	return (log(x) / log(0.01f));
}

void CCameraComponent::Update()
{
#ifdef _DEBUG
	if (Input::GetInstance()->IsKeyDown('J'))
	{
		/*this->Fade(false, 5.0f);*/this->SetTrauma(myTestTrauma);
	}
#endif

	if(myShakeState == ECameraShakeState::IdleSway)
		this->SetTrauma(myIdleTrauma, ECameraShakeState::IdleSway);

	if (myTrauma > 0.0f) {
		//myShakeTimer += CTimer::Dt();
		myTrauma -= (1 / myDecayInSeconds) * CTimer::Dt();
		Shake();
	}

	if (myTrauma < 0.0f) {
		myTrauma = 0.0f;
		if (myShakeState != ECameraShakeState::IdleSway)
			myShakeState = ECameraShakeState::IdleSway;
	}

	if (myState != ECameraState::Default)
	{
		if (myState == ECameraState::FadeIn || myState == ECameraState::FadeOut)
		{
			if (!myFadingPlane->GetShouldRender())
				myFadingPlane->SetShouldRender(true);

			myFadeParameter -= myFadeSpeed * CTimer::Dt();

			DirectX::SimpleMath::Vector4 color = myFadingPlane->GetColor();
			float alpha = color.w;

			if (myState == ECameraState::FadeIn)
			{
				alpha = LogEaseOut(myFadeParameter);
				if (alpha <= 0.01f)
				{
					alpha = 0.0f;
					myState = ECameraState::Default;
					CMainSingleton::PostMaster().SendLate({ EMessageType::FadeInComplete, 0 });
				}
			}
			else if (myState == ECameraState::FadeOut)
			{
				alpha = LogEaseIn(myFadeParameter);
				if (alpha >= 0.99f)
				{
					alpha = 1.0f;
					myState = ECameraState::Default;
					CMainSingleton::PostMaster().SendLate({ EMessageType::FadeOutComplete, 0 });
				}
			}

			myFadingPlane->SetColor({ color.x, color.y, color.z, alpha });

			if (myState == ECameraState::Default)
			{
				myFadeParameter = 1.0f;
				myFadeSpeed = 1.0f;
				myFadingPlaneActive = myFadePermanent;
				myFadingPlane->SetShouldRender(myFadePermanent);
			}
		}
	}
}

void CCameraComponent::SetTrauma(float aValue, const ECameraShakeState aShakeState)
{
	myTrauma = aValue;
	myShakeState = aShakeState;
}

void CCameraComponent::SetStartingRotation(DirectX::SimpleMath::Vector3 aRotation)
{
	myStartingRotation = aRotation;
}

void CCameraComponent::SetFoV(float aFoV)
{
	myFoV = aFoV;
	myProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(myFoV), (16.0f / 9.0f), 0.1f, 500.0f);

	DirectX::BoundingFrustum::CreateFromMatrix(myViewFrustum, myProjection);
}

const DirectX::SimpleMath::Matrix& CCameraComponent::GetProjection() const
{
	return myProjection;
}

float CCameraComponent::GetFoV()
{
	return myFoV;
}

void CCameraComponent::Fade(bool aShouldFadeIn, const float& aFadeDuration, const bool& aFadeIsPermanent)
{
	myFadePermanent = aFadeIsPermanent;

	float alpha = 1.0f;
	if (aShouldFadeIn)
	{
		myState = ECameraState::FadeIn;
		alpha = 1.0f;
	}
	else
	{
		myState = ECameraState::FadeOut;
		alpha = 0.0f;
	}

	myFadeSpeed = 1.0f / aFadeDuration;
	DirectX::SimpleMath::Vector4 color = myFadingPlane->GetColor();
	myFadingPlane->SetColor({ color.x, color.y, color.z, alpha });
	myFadingPlaneActive = true;
}

const bool CCameraComponent::IsFading() const
{
	return myFadingPlaneActive;
}

void CCameraComponent::EmplaceSprites(std::vector<CSpriteInstance*>& someSprites) const
{
	if (myFadingPlaneActive)
	{
		someSprites.emplace_back(myFadingPlane);
	}
}

const Matrix& CCameraComponent::GetViewMatrix()
{
	myView = DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), GameObject().myTransform->Position() - GameObject().myTransform->Transform().Forward(), GameObject().myTransform->Transform().Up());
	myView.Translation(Vector3::Zero);
	myView *= myShakenMatrix;
	myView.Translation(GameObject().myTransform->Transform().Translation());
	return myView;
}

const Matrix& CCameraComponent::GetShakenMatrix()
{
	return myShakenMatrix;
}

const Vector3& CCameraComponent::GetShakeVector() const
{
	return myShakeVector;
}

const DirectX::BoundingFrustum CCameraComponent::GetViewFrustum()
{
	DirectX::BoundingFrustum viewFrustum;
	DirectX::XMVECTOR det;

	myViewFrustum.Transform(viewFrustum, DirectX::XMMatrixInverse(&det, DirectX::XMMatrixLookAtLH(GameObject().myTransform->Position(), GameObject().myTransform->Position() - GameObject().myTransform->Transform().Forward(), GameObject().myTransform->Transform().Up())));
	viewFrustum.Origin = GameObject().myTransform->WorldPosition();

	return viewFrustum;
}

void CCameraComponent::SetShakeDecay(float aDecayInSeconds)
{
	myDecayInSeconds = aDecayInSeconds;
}

void CCameraComponent::SetShakeSpeed(float aSpeed)
{
	myShakeSpeed = aSpeed;
}

void CCameraComponent::SetMaxShakeRotation(const Vector3& aMaxRotation)
{
	myMaxShakeRotation = aMaxRotation;
}

void CCameraComponent::SetTestTrauma(float aTrauma)
{
	myTestTrauma = aTrauma;
}

void CCameraComponent::Shake()
{
	myShake = myTrauma * myTrauma * myTrauma;

	auto& noise = myIdleNoise;
	switch (myShakeState)
	{
		case ECameraShakeState::Shake:
			noise = myShakeNoise;
		break;

		default:break;
	}
	static int seed = 1462;
	float speed = myShakeSpeed * myShakeSpeed * myShakeSpeed;
	float newRotX = myMaxShakeRotation.x * myShake * noise->GenSingle2D(CTimer::Time() * speed, CTimer::Time() * speed, seed);
	float newRotY = myMaxShakeRotation.y * myShake * noise->GenSingle2D(CTimer::Time() * speed, CTimer::Time() * speed, seed+1);
	float newRotZ = myMaxShakeRotation.z * myShake * noise->GenSingle2D(CTimer::Time() * speed, CTimer::Time() * speed, seed+2);

	DirectX::SimpleMath::Vector3 newRotation = { newRotX, newRotY, newRotZ };

	myShakeVector = newRotation;
	myShakenMatrix = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, DirectX::XMConvertToRadians(newRotZ));

	newRotation += myStartingRotation;
	//GameObject().myTransform->Rotation(newRotation);
	//myShakeTimer -= 0.0167f;
}


// FRUSTUM CULLING DEBUG

//myFrustumWireframe = new CLineInstance();
//DirectX::XMFLOAT3 corners[8];
//myViewFrustum.GetCorners(corners);
//std::vector<Vector3> cornerList;
//for (unsigned int i = 0; i < 8; ++i)
//{
//	cornerList.push_back(corners[i]);
//}
//
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[3]);
//
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[7]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[7]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[0]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[1]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[2]);
//cornerList.push_back(corners[7]);
//
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[4]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[5]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[6]);
//cornerList.push_back(corners[3]);
//cornerList.push_back(corners[7]);
//
//myFrustumWireframe->Init(CLineFactory::GetInstance()->CreatePolygon(cornerList));

//	IRONWROUGHT->GetActiveScene().AddInstance(myFrustumWireframe);