#pragma once
#include <random>
#include <SimpleMath.h>

namespace
{
	std::random_device myRandomDevice{};
	std::mt19937 myRandomEngine{ myRandomDevice() };
}

static float RandomGaussian(float aMean, float aStandardDeviation)
{
	return std::normal_distribution<float>{aMean, aStandardDeviation}(myRandomEngine);
}

static float Random(float aLowerBound, float anUpperBound)
{
	return std::uniform_real_distribution<float>{ aLowerBound, anUpperBound }(myRandomEngine);
}

static int Random(int aLowerBound, int anUpperBound)
{
	return std::uniform_int_distribution<>{ aLowerBound, anUpperBound }(myRandomEngine);
}

static int Random(int aLowerBound, int anUpperBound, int anExcludedNumber)
{
	int num = anExcludedNumber;

	if (anUpperBound - aLowerBound > 0) {
		while (num == anExcludedNumber) {
			num = Random(aLowerBound, anUpperBound);
		}
	}
	else {
		return Random(aLowerBound, anUpperBound);
	}

	return num;
}

static int Random(int aLowerBound, int anUpperBound, std::vector<int>& someExcludedNumbers)
{
	if (someExcludedNumbers.empty())
	{
		int result = Random(aLowerBound, anUpperBound);
		someExcludedNumbers.push_back(result);
		return result;
	}

	int num = someExcludedNumbers[0];

	if (anUpperBound - aLowerBound > 0) 
	{
		while (std::find(someExcludedNumbers.begin(), someExcludedNumbers.end(), num) != someExcludedNumbers.end()) 
		{
			num = Random(aLowerBound, anUpperBound);
		}
		someExcludedNumbers.push_back(num);

		if (someExcludedNumbers.size() > anUpperBound)
		{
			someExcludedNumbers.clear();
		}
	}
	else 
	{
		return Random(aLowerBound, anUpperBound);
	}

	return num;
}

static DirectX::SimpleMath::Vector3 Random(DirectX::SimpleMath::Vector3& aLowerBound, DirectX::SimpleMath::Vector3& anUpperBound)
{
	float x = Random(aLowerBound.x, anUpperBound.x);
	float y = Random(aLowerBound.y, anUpperBound.y);
	float z = Random(aLowerBound.z, anUpperBound.z);

	return DirectX::SimpleMath::Vector3(x, y, z);
}

static DirectX::SimpleMath::Vector4 Random(DirectX::SimpleMath::Vector3& aLowerBound, DirectX::SimpleMath::Vector3& anUpperBound, float aW)
{
	DirectX::SimpleMath::Vector3 vector3 = Random(aLowerBound, anUpperBound);
	return DirectX::SimpleMath::Vector4(vector3.x, vector3.y, vector3.z, aW);
}
