#pragma once
#include <map>

class CDirectXFramework;
class CDecal;

struct ID3D11Device;

class CDecalFactory
{
	friend class CEngine;
public:
	static CDecalFactory* GetInstance();
	bool Init(CDirectXFramework* aFramework);
	void ClearFactory();

	CDecal* GetDecal(std::string aFilePath);
	void ClearDecal(std::string aFilePath);

private:
	CDecal* LoadDecal(std::string aFilePath);

private:
	CDecalFactory();
	~CDecalFactory();

private:
	std::map<std::string, CDecal*> myDecalMap;
	std::map<std::string, int> myDecalMapReferences;
	static CDecalFactory* ourInstance;
};

