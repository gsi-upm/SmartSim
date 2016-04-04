#ifndef SBM_DEBUGGER_CLIENT_H_
#define SBM_DEBUGGER_CLIENT_H_

#include <sb/SBTypes.h>
#include <vhcl_socket.h>
#include <string>
#include <vector>

#include "sb/NetRequest.h"

namespace SmartBody {

class SBDebuggerClient
{
private:
   //vhmsg::Client m_vhmsg;
   std::vector<std::string> m_processIdList;
   std::string m_sbmId;
   bool m_connectResult;
   bool m_initFinish;
   vhcl::socket_t m_sockTCP_client;

   NetRequestManager m_netRequestManager;

public:
   SBAPI SBDebuggerClient();
   SBAPI virtual ~SBDebuggerClient();

   SBAPI void QuerySbmProcessIds();
   SBAPI const std::vector<std::string> & GetSbmProcessIds() const { return m_processIdList; }

   SBAPI void Connect(const std::string & id);
   SBAPI void Disconnect();
   SBAPI bool GetConnectResult() { return m_connectResult; }

   SBAPI void Init();
   SBAPI void Update();
   SBAPI void StartUpdates(double updateFrequencyS);
   SBAPI void EndUpdates();

   SBAPI void SendSBMCommand(int requestId, const std::string & command);
   SBAPI void SendSBMCommand(int requestId, const std::string & returnValue, const std::string & functionNameandParams,
      NetRequest::RequestCallback cb, void* callbackOwner = NULL);

   SBAPI void ProcessVHMsgs(const char * op, const char * args);
};

}


#endif
