#ifndef _SBBONEBUSMANAGER_
#define _SBBONEBUSMANAGER_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#ifndef SB_NO_BONEBUS
#include "bonebus.h"
#endif
#include <sbm/general_param_setting.h>

namespace SmartBody {

	class SBSkeleton;

class SBBoneBusManager : public SmartBody::SBService
{
	public:
		SBAPI SBBoneBusManager();
		SBAPI ~SBBoneBusManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI void setHost(const std::string& host);
		SBAPI const std::string& getHost();

		SBAPI virtual void start();
		SBAPI virtual void beforeUpdate(double time);
		SBAPI virtual void update(double time);
		SBAPI virtual void afterUpdate(double time);
		SBAPI virtual void stop();
#ifndef SB_NO_BONEBUS
		SBAPI bonebus::BoneBusClient& getBoneBus();
		SBAPI void NetworkSendSkeleton( bonebus::BoneBusCharacter * character, SmartBody::SBSkeleton* skeleton, GeneralParamMap * param_map );
#endif
		SBAPI virtual void notify(SBSubject* subject);

	private:
#ifndef SB_NO_BONEBUS
		bonebus::BoneBusClient _boneBus;
#endif
		std::string _host;

};

}

#endif
