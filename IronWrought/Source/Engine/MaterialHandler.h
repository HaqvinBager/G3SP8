#pragma once
#include <array>
#include <map>

class CDirectXFramework;
struct SVertexPaintColorData;
typedef std::array<ID3D11ShaderResourceView*, 3> material;

struct SVertexPaintData
{
	std::vector<std::string> myRGBMaterialNames;
	int myVertexColorID;
};

struct SMaterialInstance {

	SMaterialInstance(const UINT aStartSlot, const UINT aNumViews, const std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>& someShaderResourceViews) 
		: myStartSlot(aStartSlot)
		, myNumViews(aNumViews)
		, myShaderResourceView(someShaderResourceViews)
	{
	}

	const UINT myStartSlot;
	const UINT myNumViews;
	const std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>& myShaderResourceView;
};

class CMaterialHandler
{
	friend class CMainSingleton;
	friend class CEngine;

public:


	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const int aMaterialID, const std::string& aModelPath);
	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const int aMaterialID);
	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const std::string& aMaterialName);
	std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const std::string& aMaterialName, int& anOutMaterialID);
	std::array<ID3D11ShaderResourceView*, 3> RequestDefualtMaterial(int& anOutMaterialID);

	//std::array<ID3D11ShaderResourceView*, 3> RequestMaterial(const std::string& aMaterialName);
	std::array<ID3D11ShaderResourceView*, 3> RequestDecal(const std::string& aDecalName);
	std::array<ID3D11ShaderResourceView*, 9> GetVertexPaintMaterials(const std::vector<std::string>& someMaterialNames);
	void ReleaseMaterial(const std::string& aMaterialName);

	SVertexPaintData RequestVertexColorID(std::vector<SVertexPaintColorData>::const_iterator& it, const std::string& aFbxModelPath, const std::vector<std::string>& someMatrials);
	std::vector<DirectX::SimpleMath::Vector3>& GetVertexColors(unsigned int aVertexColorID);
	ID3D11Buffer* GetVertexColorBuffer(unsigned int aVertexColorID);
	void ReleaseVertexColors(unsigned int aVertexColorID);

	const std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>& GetMaterial(const int aMaterialID) const;

	const SMaterialInstance GetMaterialInstance(const int aMaterialID) const;
	const bool IsMaterialAlpha(const int aMaterialID) const;


protected:
	bool Init(CDirectXFramework* aFramwork);

private:
	inline bool MaterialIsAlpha(const std::array<std::string, 3>& someTexturePaths);

	std::map<std::string, std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 3>> myMaterials;
	std::map<std::string, std::array<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, 9>> myVertexPaintMaterials;

	std::map<int, bool> myMaterialIsAlphaMap;

	std::map<unsigned int, std::vector<DirectX::SimpleMath::Vector3>> myVertexColors;
	std::map<unsigned int, ID3D11Buffer*> myVertexColorBuffers;
	std::map<std::string, int> myMaterialReferences;
	std::map<unsigned int, int> myVertexColorReferences;

	ID3D11Device* myDevice;
	const std::string myMaterialPath;
	const std::string myDecalPath;
	const std::string myVertexLinksPath;

private:
	CMaterialHandler();
	~CMaterialHandler();

private:
	struct VertexPositionComparer {
		const float epsilon = 0.0001f;
		bool operator()(const Vector3& a, const Vector3& b) const {	
			bool xIsEqual = fabs(a.x - b.x) <= ((fabs(a.x) < fabs(b.x) ? fabs(b.x) : fabs(a.x)) * epsilon);
			bool yIsEqual = fabs(a.y - b.y) <= ((fabs(a.y) < fabs(b.y) ? fabs(b.y) : fabs(a.y)) * epsilon);
			bool zIsEqual = fabs(a.z - b.z) <= ((fabs(a.z) < fabs(b.z) ? fabs(b.z) : fabs(a.z)) * epsilon);
			return xIsEqual && yIsEqual && zIsEqual;
		}

		int friend operator<(const Vector3& a, const Vector3& b) {
			const float epsi = 0.0001f;
			float a0 = a.x;
			float b0 = b.x;

			float a1 = a.y;
			float b1 = b.y;

			float a2 = a.z;
			float b2 = b.z;

			if ((b0 - a0) > ((fabs(a0) < fabs(b0) ? fabs(b0) : fabs(a0)) * epsi) &&
				(b1 - a1) > ((fabs(a1) < fabs(b1) ? fabs(b1) : fabs(a1)) * epsi) &&
				(b2 - a2) > ((fabs(a2) < fabs(b2) ? fabs(b2) : fabs(a2)) * epsi)) {
				return -1;
			}
			else if ((a0 - b0) > ((fabs(b0) < fabs(a0) ? fabs(a0) : fabs(b0)) * epsi) &&
					(a1 - b1) > ((fabs(b1) < fabs(a1) ? fabs(a1) : fabs(b1)) * epsi) &&
					(a2 - b2) > ((fabs(b2) < fabs(a2) ? fabs(a2) : fabs(b2)) * epsi)) {
				return 1;
			}
			else {
				return 0;
			}
		}
	};

	class VectorHasher {
	public:
		std::size_t operator()(const Vector3& aVector) const {
			size_t xHash = std::hash<float>()(aVector.x);
			size_t yHash = std::hash<float>()(aVector.y);
			size_t zHash = std::hash<float>()(aVector.z);
			return xHash + yHash + zHash;
		}
	};

};

