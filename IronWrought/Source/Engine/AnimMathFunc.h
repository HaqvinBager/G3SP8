#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "WinInclude.h"

typedef unsigned int uint;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


void set_float4(float f[4], float a, float b, float c, float d);
void color4_to_float4(const aiColor4D* c, float f[4]);
void Color4f(const aiColor4D* color);

uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);

void InitM4FromM3(aiMatrix4x4& out, const aiMatrix3x3& in);
void InitIdentityM4(aiMatrix4x4& m);
void MulM4(aiMatrix4x4& out, aiMatrix4x4& in, float m);
void ShortMulM4(aiVector3D& out, const aiMatrix4x4& m, const aiVector3D& in);

long long GetCurrentTimeMillis();
std::string getBasePath(const std::string& path);

void createAILogger();
void destroyAILogger();
void logDebug(const char* logString);
void logInfo(const std::string& logString);