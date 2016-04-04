#ifndef _SBREALTIMEMANAGER_H_
#define _SBREALTIMEMANAGER_H_

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <sr/sr_quat.h>
#include <string>
#include <sb/SBSubject.h>
namespace SmartBody {


class SBRealtimeManager : public SBService
{
	public:
		SBAPI SBRealtimeManager();
		SBAPI ~SBRealtimeManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();
		SBAPI virtual void start();
		SBAPI virtual void stop();

		SBAPI virtual void setChannelNames(const std::string& names);
		SBAPI std::vector<std::string>& getChannelNames();
		
		SBAPI void notify(SBSubject* subject);

		SBAPI void initConnection();
		SBAPI void stopConnection();
		SBAPI virtual void update(double time);
		SBAPI void setData(const std::string& channel, const std::string& data);
		SBAPI std::string getData(const std::string& channel);
		SBAPI double getDataDouble(const std::string& channel);
		SBAPI int getDataInt(const std::string& channel);
		SBAPI SrVec getDataVec(const std::string& channel);
		SBAPI SrQuat getDataQuat(const std::string& channel);
		SBAPI SrMat getDataMat(const std::string& channel);

	protected:
		std::vector<std::string> blendShapeNames;
		std::map<std::string, std::string> channelTable;
		std::vector<std::string> channelNames;
};

}

#endif
