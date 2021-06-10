#pragma once

struct ID3D11ShaderResourceView;
class CDirectXFramework;
class CFullscreenTexture;

class CEnvironmentLight
{
public:
	CEnvironmentLight();
	~CEnvironmentLight();

	bool Init(CDirectXFramework* aFramework, std::string aFilePath);

	ID3D11ShaderResourceView* const* GetCubeMap();

	DirectX::SimpleMath::Matrix GetShadowView() const;
	DirectX::SimpleMath::Matrix GetShadowProjection() const;
	DirectX::SimpleMath::Vector4 GetDirection() { return myDirection; }
	DirectX::SimpleMath::Vector4 GetColor() { return myColor; }
	DirectX::SimpleMath::Vector4 GetShadowPosition() const;

	const Vector2& GetShadowmapResolution() const;

	const float& GetNumberOfSamples() const;
	const float& GetLightPower() const;
	const float& GetScatteringProbability() const;
	const float& GetHenyeyGreensteinGValue() const;

	const bool& GetIsVolumetric() const;
	const bool& GetIsFog() const;

	const std::string& GetFirstCubemapReference() const;
	void AddCubemap(const std::string& aCubemapReference, CDirectXFramework* aFramework);
	void SetCubemapIndex(unsigned int anIndex);

	void SetDirection(DirectX::SimpleMath::Vector3 aDirection);
	void SetColor(DirectX::SimpleMath::Vector3 aColor);
	void SetIntensity(float anIntensity);
	void SetPosition(DirectX::SimpleMath::Vector3 aPosition); // Used for shadow calculation

	void SetNumberOfSamples(float aNumberOfSamples);
	void SetLightPower(float aPower);
	void SetScatteringProbability(float aScatteringProbability);
	void SetHenyeyGreensteinGValue(float aGValue);

	void SetIsVolumetric(bool aShouldBeVolumetric);
	void SetIsFog(bool aShouldBeFog);

private:
	DirectX::SimpleMath::Matrix GetShadowTransform() const; // Used for shadow position calculation
	bool LoadCubemap(const std::string& aFilePath, CDirectXFramework* aFramework, ID3D11ShaderResourceView** aCubemapSRV);

private:
	Matrix myShadowmapViewMatrix;
	Matrix myShadowmapProjectionMatrix;
	DirectX::SimpleMath::Vector2 myShadowcastSize;
	DirectX::SimpleMath::Vector2 myShadowmapResolution;

	ID3D11ShaderResourceView* myCubeShaderResourceView;
	std::vector<ID3D11ShaderResourceView*> myCubemaps;
	DirectX::SimpleMath::Vector4 myPosition;
	DirectX::SimpleMath::Vector4 myDirection; //Points towards the Light
	DirectX::SimpleMath::Vector4 myColor; //Color (XYZ) and Intensity (W)

	std::vector<std::string> myCubemapReferences;

	float myNumberOfSamples;
	float myLightPower;
	float myScatteringProbability;
	float myHenyeyGreensteinGValue;

	bool myIsVolumetric;
	bool myIsFog;

	int myCurrentCubemapIndex;
};