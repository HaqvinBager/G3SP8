#pragma once

class CEnvironmentLight;
class CEngine;
class CPointLight;
class CSpotLight;
class CBoxLight;

class CLightFactory
{
public:
	friend class CEngine;
	static CLightFactory* GetInstance();
	bool Init(CEngine& anEngine);
	CEnvironmentLight* CreateEnvironmentLight(std::string aCubeMapPath);
	CPointLight* CreatePointLight();
	CSpotLight* CreateSpotLight();
	CBoxLight* CreateBoxLight();

private:
	CEngine* myEngine;


private:
	CLightFactory();
	~CLightFactory();
	static CLightFactory* ourInstance;

};