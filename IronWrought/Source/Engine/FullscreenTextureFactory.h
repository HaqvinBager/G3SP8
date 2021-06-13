#pragma once

struct ID3D11Texture2D;
class CDirectXFramework;
class CFullscreenTexture;

namespace SM = DirectX::SimpleMath;

class CFullscreenTextureFactory {
public:
	CFullscreenTextureFactory();
	~CFullscreenTextureFactory();
	bool Init(CDirectXFramework* aFramework);
	CFullscreenTexture CreateTexture(SM::Vector2 aSize, DXGI_FORMAT aFormat);
	CFullscreenTexture CreateTexture(ID3D11Texture2D* aTexture);
	CFullscreenTexture CreateTexture(SM::Vector2 aSize, DXGI_FORMAT aFormat, const std::string& aFilePath);
	CFullscreenTexture CreateDepth(SM::Vector2 aSize, DXGI_FORMAT aFormat);
	class CGBuffer CreateGBuffer(DirectX::SimpleMath::Vector2 aSize);

private:
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath);

private:
	CDirectXFramework* myFramework;
};

