#ifndef _SBM_DEBUGGER_UTILITY_
#define _SBM_DEBUGGER_UTILITY_

#include <string>

class SBDebuggerUtility
{
public:
	SBDebuggerUtility();
	~SBDebuggerUtility();

	void initScene();
	void queryResources();

	void initCharacter(const std::string& name, const std::string& skelName);
	void initCharacterFaceDefinition(const std::string& characterName, const std::string& faceDefName, const std::string& message);
	void initPawn(const std::string& name);
	void initSkeleton(const std::string& skFileName, const std::string& info);

	void runPythonCommand(const std::string& info);

	void updateCharacter(const std::string& cName, const std::string& jName, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ, float& rotW);
	void updatePawn(const std::string& pName, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ, float& rotW);
	void updateCamera(float& eyePosX, float& eyePosY, float& eyePosZ, float& lookAtPosX, float& lookAtPosY, float& lookAtPosZ, float& fovY, float& aspect, float& zNear, float zFar);
};

#endif