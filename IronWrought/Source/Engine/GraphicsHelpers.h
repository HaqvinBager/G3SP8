#pragma once
#include <fstream>

namespace Graphics {
	static bool CreateVertexShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11VertexShader** outVertexShader, std::string& outShaderData)
	{
		std::ifstream vsFile;
		vsFile.open(aFilepath, std::ios::binary);
		std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		outShaderData = vsData;
		ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, outVertexShader), "Vertex Shader could not be created.");
		vsFile.close();
		return true;
	}

	static bool CreatePixelShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11PixelShader** outPixelShader)
	{
		std::ifstream psFile;
		psFile.open(aFilepath, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, outPixelShader), "Pixel Shader could not be created.");
		psFile.close();
		return true;
	}

	static bool CreateGeometryShader(std::string aFilepath, CDirectXFramework* aFramework, ID3D11GeometryShader** outGeometryShader)
	{
		std::ifstream gsFile;
		gsFile.open(aFilepath, std::ios::binary);
		std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
		ENGINE_HR_BOOL_MESSAGE(aFramework->GetDevice()->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, outGeometryShader), "Geometry Shader could not be created.");
		gsFile.close();
		return true;
	}

	static ID3D11ShaderResourceView* TryGetShaderResourceView(ID3D11Device* aDevice, const std::string& aTexturePath)
	{
		ID3D11ShaderResourceView* shaderResourceView;

		wchar_t* widePath = new wchar_t[aTexturePath.length() + 1];
		std::copy(aTexturePath.begin(), aTexturePath.end(), widePath);
		widePath[aTexturePath.length()] = 0;

		HRESULT result;
		result = DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView);
		if (FAILED(result))
		{
			shaderResourceView = nullptr;
		}

		delete[] widePath;
		return shaderResourceView;
	}

	static ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath)
	{
		ID3D11ShaderResourceView* shaderResourceView;

		wchar_t* widePath = new wchar_t[aTexturePath.length() + 1];
		std::copy(aTexturePath.begin(), aTexturePath.end(), widePath);
		widePath[aTexturePath.length()] = 0;

		////==ENABLE FOR TEXTURE CHECKING==
		//ENGINE_HR_MESSAGE(DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView), aTexturePath.append(" could not be found.").c_str());
		////===============================

		//==DISABLE FOR TEXTURE CHECKING==
		HRESULT result;
		result = DirectX::CreateDDSTextureFromFile(aDevice, widePath, nullptr, &shaderResourceView);
		if (FAILED(result))
		{
			std::string errorTexturePath = aTexturePath.substr(aTexturePath.length() - 6);
			errorTexturePath = "Assets/ErrorTextures/Checkboard_128x128" + errorTexturePath;

			wchar_t* wideErrorPath = new wchar_t[errorTexturePath.length() + 1];
			std::copy(errorTexturePath.begin(), errorTexturePath.end(), wideErrorPath);
			wideErrorPath[errorTexturePath.length()] = 0;

			DirectX::CreateDDSTextureFromFile(aDevice, wideErrorPath, nullptr, &shaderResourceView);
			delete[] wideErrorPath;
		}

		delete[] widePath;
		return shaderResourceView;
	}
}
