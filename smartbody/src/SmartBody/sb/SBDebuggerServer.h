#include <sb/SBTypes.h>
#include <string>
#include <vector>
#include <sb/SBService.h>

namespace SmartBody {

class SBJoint;
class SBScene; 

class SBDebuggerServer : public SBService
{
	public:
		SBAPI SBDebuggerServer();
		SBAPI virtual ~SBDebuggerServer();

		SBAPI virtual void setEnable(bool val);

		SBAPI virtual void afterUpdate(double time);

		SBAPI virtual void notify(SBSubject* subject);

	   void Update();

	   void GenerateInitHierarchyMsg(SmartBody::SBJoint * root, std::string & msg, int tab);

	   SBAPI void ProcessVHMsgs(const char * op, const char * args);

		vhcl::Vector3 m_cameraPos;
		vhcl::Vector3 m_cameraLookAt;
		vhcl::Vector4 m_cameraRot;
		double m_cameraFovY;
		double m_cameraAspect;
		double m_cameraZNear;
		double m_cameraZFar;
		bool m_rendererIsRightHanded;

	protected:
		void Init();
		void Close();


	private:
		std::vector<std::string> m_processIdList;
		std::string m_sbmFriendlyName;
		std::string m_hostname;
		int m_port;
		std::string m_fullId;
		bool m_connectResult;
		double m_updateFrequencyS;
		double m_lastUpdate;
		vhcl::Timer m_timer;

public:


};

}

