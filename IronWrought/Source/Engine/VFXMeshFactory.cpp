#include "stdafx.h"
#include "VFXMeshFactory.h"
#include "VFXBase.h"
#include "Engine.h"
#include "DDSTextureLoader.h"
#include "FBXLoaderCustom.h"
#include "GraphicsHelpers.h"

#include "JsonReader.h"
//
//#include "rapidjson\document.h"
//#include "rapidjson\istreamwrapper.h"

#ifdef _DEBUG
#pragma comment (lib, "ModelLoader_Debug.lib")
#endif
#ifdef NDEBUG
#pragma comment (lib, "ModelLoader_Release.lib")
#endif

CVFXMeshFactory* CVFXMeshFactory::ourInstance = nullptr;


bool CVFXMeshFactory::Init(CDirectXFramework* aFramework) {
    myFramework = aFramework;
    return true;
}

CVFXBase* CVFXMeshFactory::LoadVFXBase(std::string aFilePath)
{
    CVFXBase::SVFXBaseData vfxBaseData;
    ReadJsonValues(aFilePath, vfxBaseData);

    std::string meshPath = vfxBaseData.meshPath;
    const size_t lastSlashIndex = meshPath.find_last_of("\\/");
    std::string modelDirectory = meshPath.substr(0, lastSlashIndex + 1);
    std::string modelName = meshPath.substr(lastSlashIndex + 1, meshPath.size() - lastSlashIndex - 5);

    CFBXLoaderCustom modelLoader;
    CLoaderModel* loaderModel = modelLoader.LoadModel(meshPath.c_str());
    CLoaderMesh* mesh = loaderModel->myMeshes[0];

    //Start Vertex Buffer
    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.ByteWidth = mesh->myVertexBufferSize * mesh->myVertexCount;
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subresourceData = { 0 };
    subresourceData.pSysMem = mesh->myVerticies;

    ID3D11Buffer* vertexBuffer;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer), "Vertex Buffer could not be created.");
    //End Vertex Buffer

    //Start Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<UINT>(mesh->myIndexes.size());
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
    indexSubresourceData.pSysMem = mesh->myIndexes.data();

    ID3D11Buffer* indexBuffer;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer), "Index Buffer could not be created.");
    //End Index Buffer

    //Start Shader
    std::ifstream vsFile;
    vsFile.open("Shaders/VFXTextureBlendVertexShader.cso", std::ios::binary);
    std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
    ID3D11VertexShader* vertexShader;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader), "Vertex Shader could not be created.");

    vsFile.close();

    std::ifstream psFile;
    psFile.open("Shaders/VFXTextureBlendPixelShader.cso", std::ios::binary);
    std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
    ID3D11PixelShader* pixelShader;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader), "Pixel Shader could not be created.");

    psFile.close();
    //End Shader

    //Start Sampler
    ID3D11SamplerState* sampler;
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateSamplerState(&samplerDesc, &sampler), "Sampler could not be created.");
    //End Sampler

    //Start Layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ID3D11InputLayout* inputLayout;
    ENGINE_HR_MESSAGE(myFramework->GetDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout), "Input Layout could not be created.");
    //End Layout

    ID3D11ShaderResourceView* textureOneShaderResourceView = Graphics::GetShaderResourceView(myFramework->GetDevice(), vfxBaseData.texturePathOne);
    ID3D11ShaderResourceView* textureTwoShaderResourceView = Graphics::GetShaderResourceView(myFramework->GetDevice(), vfxBaseData.texturePathTwo);
    ID3D11ShaderResourceView* textureThreeShaderResourceView = Graphics::GetShaderResourceView(myFramework->GetDevice(), vfxBaseData.texturePathThree);
    ID3D11ShaderResourceView* textureFourShaderResourceView = Graphics::GetShaderResourceView(myFramework->GetDevice(), vfxBaseData.texturePathMask);

    CVFXBase* vfxBase = new CVFXBase();
    if (!vfxBase) {
        return nullptr;
    }

    vfxBaseData.myNumberOfVertices = mesh->myVertexCount;
    vfxBaseData.myNumberOfIndices = static_cast<UINT>(mesh->myIndexes.size());
    vfxBaseData.myStride = mesh->myVertexBufferSize;
    vfxBaseData.myOffset = 0;
    vfxBaseData.myVertexBuffer = vertexBuffer;
    vfxBaseData.myIndexBuffer = indexBuffer;
    vfxBaseData.myVertexShader = vertexShader;
    vfxBaseData.myPixelShader = pixelShader;
    vfxBaseData.mySampler = sampler;
    vfxBaseData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    vfxBaseData.myInputLayout = inputLayout;
    vfxBaseData.myTexture[0] = textureOneShaderResourceView;
    vfxBaseData.myTexture[1] = textureTwoShaderResourceView;
    vfxBaseData.myTexture[2] = textureThreeShaderResourceView;
    vfxBaseData.myTexture[3] = textureFourShaderResourceView;

    vfxBase->Init(vfxBaseData);

    if (myVFXBaseMap.find(aFilePath) == myVFXBaseMap.end())
        myVFXBaseMap.emplace(aFilePath, vfxBase);

    delete loaderModel;
    mesh = nullptr;

    return vfxBase;
}

CVFXBase* CVFXMeshFactory::GetVFXBase(std::string aFilePath) {
	if (myVFXBaseMap.find(aFilePath) == myVFXBaseMap.end()) {
		return LoadVFXBase(aFilePath);
	}
	return myVFXBaseMap.at(aFilePath);
}

CVFXMeshFactory* CVFXMeshFactory::GetInstance() {
	return ourInstance;
}

std::vector<CVFXBase*> CVFXMeshFactory::GetVFXBaseSet(std::vector<std::string> someFilePaths)
{
    std::vector<CVFXBase*> bases;
    for (unsigned int i = 0; i < someFilePaths.size(); ++i)
    {
        bases.emplace_back(GetVFXBase(someFilePaths[i]));
    }
    return std::move(bases);
}

std::vector<CVFXBase*> CVFXMeshFactory::ReloadVFXBaseSet(std::vector<std::string> someFilePaths)
{
    std::vector<CVFXBase*> bases;
    for (unsigned int i = 0; i < someFilePaths.size(); ++i)
    {
        bases.emplace_back(LoadVFXBase(someFilePaths[i]));
    }
    return std::move(bases);
}

CVFXMeshFactory::CVFXMeshFactory() {
	ourInstance = this;
    myFramework = nullptr;
}

CVFXMeshFactory::~CVFXMeshFactory() {
	ourInstance = nullptr;
    myFramework = nullptr;

    auto vfxIt = myVFXBaseMap.begin();
    while (vfxIt != myVFXBaseMap.end())
    {
        delete vfxIt->second;
        vfxIt->second = nullptr;
        ++vfxIt;
    }
}

void CVFXMeshFactory::ReadJsonValues(std::string aFilePath, CVFXBase::SVFXBaseData& someVFXBaseData)
{
    using namespace rapidjson;

    Document document = CJsonReader::Get()->LoadDocument(aFilePath);
    ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(document), "Invalid Json document: %s", aFilePath.c_str());

    someVFXBaseData.scrollSpeed1 = { document["Scroll Speed 1 X"].GetFloat(), document["Scroll Speed 1 Y"].GetFloat() };
    someVFXBaseData.scrollSpeed2 = { document["Scroll Speed 2 X"].GetFloat(), document["Scroll Speed 2 Y"].GetFloat() };
    someVFXBaseData.scrollSpeed3 = { document["Scroll Speed 3 X"].GetFloat(), document["Scroll Speed 3 Y"].GetFloat() };
    someVFXBaseData.scrollSpeed4 = { document["Scroll Speed 4 X"].GetFloat(), document["Scroll Speed 4 Y"].GetFloat() };
    someVFXBaseData.uvScale1 = document["UV Scale 1"].GetFloat();
    someVFXBaseData.uvScale2 = document["UV Scale 2"].GetFloat();
    someVFXBaseData.uvScale3 = document["UV Scale 3"].GetFloat();
    someVFXBaseData.uvScale4 = document["UV Scale 4"].GetFloat();
    someVFXBaseData.opacityStrength = document["Opacity Strength"].GetFloat();
    someVFXBaseData.texturePathOne = ASSETPATH(document["Texture 1"].GetString());
    someVFXBaseData.texturePathTwo = ASSETPATH(document["Texture 2"].GetString());
    someVFXBaseData.texturePathThree = ASSETPATH(document["Texture 3"].GetString());
    someVFXBaseData.texturePathMask = ASSETPATH(document["Texture Mask"].GetString());
    someVFXBaseData.meshPath = ASSETPATH(document["Mesh Path"].GetString());
}
