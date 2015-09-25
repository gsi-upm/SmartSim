#include <sb/SBTypes.h>
#include <string>
#include <vector>


namespace SmartBody { class SBScene; }
namespace SmartBody { class SBJoint; }


class SBDebuggerServer
{
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
   SmartBody::SBScene * m_scene;

public:
	vhcl::Vector3 m_cameraPos;
	vhcl::Vector3 m_cameraLookAt;
   vhcl::Vector4 m_cameraRot;
   double m_cameraFovY;
   double m_cameraAspect;
   double m_cameraZNear;
   double m_cameraZFar;
   bool m_rendererIsRightHanded;


public:
   SBDebuggerServer();
   virtual ~SBDebuggerServer();

   void Init();
   void Close();

   void SetSBScene(SmartBody::SBScene * scene) { m_scene = scene; }
   SBAPI void SetID(const std::string & id);
   SBAPI const std::string& GetID();

   void Update();

   void GenerateInitHierarchyMsg(SmartBody::SBJoint * root, std::string & msg, int tab);

   SBAPI void ProcessVHMsgs(const char * op, const char * args);
};
