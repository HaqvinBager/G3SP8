#pragma once

struct SVertexPaintColorData {
	std::vector<Vector3> myColors;
	std::vector<Vector3> myVertexPositions;
	int myVertexMeshID;
};

struct SVertexPaintCollection {
	std::vector<SVertexPaintColorData> myData;
};

namespace Binary {
	struct SInstanceID {
		int instanceID;
		std::string name;
		std::string tag;
	};
	struct STransform {
		int instanceID;
		Vector3 pos;
		Vector3 rot;
		Vector3 scale;
	};

	struct SInstancedTransform {
		Vector3 pos;
		Vector3 rot;
		Vector3 scale;
		//Matrix matrix;
	};
	struct SModel {
		int instanceID;
		int assetID;
		int vertexColorID;
		std::vector<int> materialIDs;
	};
	struct SDirectionalLightData
	{
		int instanceID;
		Vector3 direction;
		float r;
		float g;
		float b;
		float intensity;
		int assetID;
		//std::string cubemapName;
	};
	struct SPointLight
	{
		int instanceID;
		float range;
		Vector3 color;
		float intensity;
	};
	struct SPlayer
	{
		int instanceID;
		std::vector<int> childrenIDs;
	};
	struct SCollider
	{
		int instanceID;
		Vector3 positionOffest;
		int colliderType;
		Vector3 boxSize;
		float sphereRadius;
		float capsuleHeight;
		float capsuleRadius;
		int isStatic;
		int isKinematic;
		int isTrigger;
		float mass;
		Vector3 localMassPosition;
		Vector3 inertiaTensor;
		float dynamicFriction;
		float staticFriction;
		float bounciness;
		int layer;
	};
	struct SEnemy
	{
		int instanceID;
		float speed;
		float radius;
		float health;
		float attackDistance;
	};
	struct SParentData
	{
		int parent;
		std::vector<int> children;
	};
	struct SEventData
	{
		int instanceID;
		int eventFilter;
		std::string gameEvent;

		//size_t operator+=()

	};
	struct SInstancedModel
	{
		int assetID;
		std::string tag;
		std::vector<int> materialIDs;
		std::vector<SInstancedTransform> transforms;
	};
	struct SLevelData {
		std::vector<SInstanceID> myInstanceIDs;
		std::vector<STransform> myTransforms;
		std::vector<SModel> myModels;
		std::vector<SPointLight> myPointLights;
		std::vector<SCollider> myColliders;
		std::vector<SInstancedModel> myInstancedModels;
		//std::vector<SInstanceName> myInstanceNames;
	};

	struct SResources {
		std::vector<std::string> myPaths;
	};

	template<typename T>
	struct CopyBin {

		template<typename T>
		size_t Copy(T& aData, char* aStreamPtr, const unsigned int aCount = 1)
		{
			memcpy(&aData, aStreamPtr, sizeof(T) * aCount);
			return sizeof(T) * aCount;
		}

		//template<typename T>
		size_t operator()(T& aData, char* aPtr, const unsigned int aCount = 1) noexcept
		{
			return Copy(aData, aPtr, aCount);
		}

		//template<typename T>
		size_t operator()(std::vector<T>& someData, char* aPtr)
		{
			char* ptr = aPtr;
			int count = 0;
			ptr += Copy(count, ptr);
			someData.resize(count);
			ptr += Copy(someData.data()[0], ptr, count);
			return ptr - aPtr;
		}
	};

	template<>
	struct CopyBin<SModel> {
		template<typename T>
		size_t Copy(T& aData, char* aStreamPtr, const unsigned int aCount = 1)
		{
			memcpy(&aData, aStreamPtr, sizeof(T) * aCount);
			return sizeof(T) * aCount;
		}
		size_t operator()(std::vector<SModel>& someData, char* aPtr)
		{
			char* ptr = aPtr;
			int count = 0;
			ptr += Copy(count, ptr);
			someData.resize(count);

			for (int i = 0; i < count; ++i)
			{
				ptr += Copy(someData[i].instanceID, ptr);
				ptr += Copy(someData[i].assetID, ptr);
				ptr += Copy(someData[i].vertexColorID, ptr);

				int materialCount = 0;
				ptr += Copy(materialCount, ptr);
				if (materialCount > 0)
				{
					someData[i].materialIDs.resize(materialCount);
					ptr += Copy(someData[i].materialIDs.data()[0], ptr, materialCount);
				}
			}
			return ptr - aPtr;
		}
	};

	template<>
	struct CopyBin<SInstancedModel> {

		template<typename SInstancedModel>
		size_t Copy(SInstancedModel& aData, char* aStreamPtr, const unsigned int aCount = 1)
		{
			memcpy(&aData, aStreamPtr, sizeof(SInstancedModel) * aCount);
			return sizeof(SInstancedModel) * aCount;
		}

		size_t ReadCharBuffer(char* aPtr, std::string& outString)
		{
			char buffer[128];// = new char[aPtr[0]];
			memcpy(&buffer[0], aPtr + 1, aPtr[0]);
			buffer[aPtr[0]] = '\0';
			outString.resize(aPtr[0]);
			memcpy(&outString.data()[0], &aPtr[1], aPtr[0]);
			return sizeof(char) * aPtr[0] + 1;
		}

		size_t operator()(std::vector<SInstancedModel>& someData, char* aPtr)
		{
			char* ptr = aPtr;
			int count = 0;
			ptr += Copy(count, ptr);
			someData.resize(count);

			for (int i = 0; i < count; ++i)
			{
				ptr += Copy(someData[i].assetID, ptr);

				ptr += ReadCharBuffer(ptr, someData[i].tag);

				int materialCount = 0;
				ptr += Copy(materialCount, ptr);
				someData[i].materialIDs.resize(materialCount);
				ptr += Copy(someData[i].materialIDs.data()[0], ptr, materialCount);

				int transformCount = 0;
				ptr += Copy(transformCount, ptr);

				someData[i].transforms.resize(transformCount);
				ptr += Copy(someData[i].transforms.data()[0], ptr, transformCount);
			}
			return ptr - aPtr;
		}
	};

	template<>
	struct CopyBin<SInstanceID> {

		template<typename T>
		size_t Copy(T& aData, char* aStreamPtr, const unsigned int aCount = 1)
		{
			memcpy(&aData, aStreamPtr, sizeof(T) * aCount);
			return sizeof(T) * aCount;
		}

		size_t operator()(SInstanceID& aData, char* aPtr, const unsigned int /*aCount= 1*/) noexcept
		{
			char* ptr = aPtr;
			ptr += Copy(aData.instanceID, ptr);		
			char buffer[128];// = new char[aPtr[0]];
			memcpy(&buffer[0], aPtr + 1, aPtr[0]);
			buffer[aPtr[0]] = '\0';
			aData.name.resize(aPtr[0]);
			memcpy(&aData.name.data()[0], &aPtr[1], aPtr[0]);
			return ptr - aPtr;
		}

		size_t ReadCharBuffer(char* aPtr, std::string& outString)
		{
			char buffer[128];// = new char[aPtr[0]];
			memcpy(&buffer[0], aPtr + 1, aPtr[0]);
			buffer[aPtr[0]] = '\0';
			outString.resize(aPtr[0]);
			memcpy(&outString.data()[0], &aPtr[1], aPtr[0]);
			return sizeof(char) * aPtr[0] + 1;
		}

		size_t operator()(std::vector<SInstanceID>& someData, char* aPtr)
		{
			char* ptr = aPtr;
			int count = 0;
			ptr += Copy(count, ptr);
			someData.resize(count);
			for (int i = 0; i < count; ++i)
			{
				ptr += Copy(someData[i].instanceID, ptr);
				ptr += ReadCharBuffer(ptr, someData[i].name);
				ptr += ReadCharBuffer(ptr, someData[i].tag);
			}
			return ptr - aPtr;
		}
	};
}

class CBinReader
{
	friend class CSceneFactory;
public:
	CBinReader();
	~CBinReader();

	static SVertexPaintCollection LoadVertexPaintCollection(const std::string& aSceneName);
	//static SVertexPaintColorData LoadVertexColorData(const std::string& aBinFilePath);
	//static void Test(const std::string& aBinFile);

	static Binary::SLevelData Load(const std::string& aPath);


	static void Write(const std::string& aFileNameAndPath, const std::vector<Vector3>& someData);
	static bool Read(const std::string& aFileNameAndPath, std::vector<Vector3>& outData);

private:



private:
	template<typename T>
	static size_t Read(T& aData, char* aStreamPtr, const unsigned int aCount = 1)
	{
		memcpy(&aData, aStreamPtr, sizeof(T) * aCount);
		return sizeof(T) * aCount;
	}

	//static std::string ReadStringAuto(char* aStreamPtr)
	//{
	//	int length = 0;
	//	aStreamPtr += Read(length, aStreamPtr);

	//	std::string text = "";
	//	aStreamPtr += ReadString(text, aStreamPtr, length);

	//	return text;
	//}

	size_t ReadCharBuffer(char* aPtr, std::string& outString)
	{
		char buffer[128];// = new char[aPtr[0]];
		memcpy(&buffer[0], aPtr + 1, aPtr[0]);
		buffer[aPtr[0]] = '\0';
		outString.resize(aPtr[0]);
		memcpy(&outString.data()[0], &aPtr[1], aPtr[0]);
		return sizeof(char) * aPtr[0] + 1;
	}

	//static size_t ReadString(std::string& data, char* aStreamPtr, size_t aLength)
	//{
	//	data.reserve(aLength + 1);
	//	memcpy(&data.data()[0], aStreamPtr, sizeof(char) * aLength);
	//	data.data()[aLength] = '\0';
	//	return sizeof(char) * aLength;
	//}
};