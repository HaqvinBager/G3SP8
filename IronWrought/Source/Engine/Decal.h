#pragma once

class CDecal
{
public:
	struct SDecalData {
		std::array<ID3D11ShaderResourceView*, 3> myMaterial {};
		std::string myMaterialName = "";
		float myAlphaClipThreshold = 0.0f;
		bool myShouldRenderAlbedo = true;
		bool myShouldRenderMaterial = true;
		bool myShouldRenderNormals = true;
	};

public:
	CDecal();
	~CDecal();

	void Init(SDecalData data);
	SDecalData& GetDecalData();

	void RenderAlbedo(const bool aShouldRender) { myDecalData.myShouldRenderAlbedo = aShouldRender; }
	void RenderMaterial(const bool aShouldRender) { myDecalData.myShouldRenderMaterial = aShouldRender; }
	void RenderNormal(const bool aShouldRender) { myDecalData.myShouldRenderNormals = aShouldRender; }

private:
	SDecalData myDecalData;
};

