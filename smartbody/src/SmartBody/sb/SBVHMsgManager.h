#ifndef _SBVHMSGMANAGER_H_
#define _SBVHMSGMANAGER_H_

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <string>

namespace vhcl {
	namespace Log {
		class Listener;
	}
}

namespace SmartBody {

class SBVHMsgManager : public SBService
{
	public:
		SBAPI SBVHMsgManager();
		SBAPI ~SBVHMsgManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();

		SBAPI bool isConnected();
		SBAPI bool connect();
		SBAPI void disconnect();

		SBAPI int sendOpMessage(const std::string& op, const std::string& message);
		SBAPI int sendMessage(const std::string& message);
		SBAPI int send2(const char *op, const char* message);
		SBAPI int send(const char* message);

		SBAPI int poll();

		SBAPI void setPort(const std::string& port);
		SBAPI const std::string& getPort();
		SBAPI void setServer(const std::string& server);
		SBAPI const std::string& getServer();
		SBAPI void setScope(const std::string& scope);
		SBAPI const std::string& getScope();

		SBAPI virtual void setEnableLogging(bool val);
		SBAPI virtual bool isEnableLogging();


	protected:
		static void vhmsgCallback( const char *op, const char *args, void * user_data );

		std::string _port;
		std::string _server;
		std::string _scope;
		vhcl::Log::Listener* _logListener;
};

}

#endif