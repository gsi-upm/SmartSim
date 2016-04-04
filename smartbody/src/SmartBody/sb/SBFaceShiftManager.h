#ifndef _SBFACESHIFTMANAGER_H_
#define _SBFACESHIFTMANAGER_H_

#include <vhcl.h>
#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <sr/sr_quat.h>
#include <string>
#include <sb/SBSubject.h>

#ifdef WIN32
#include <WinSock2.h>
namespace SmartBody {


class SBFaceShiftManager : public SBService
{
	public:
		SBAPI SBFaceShiftManager();
		SBAPI ~SBFaceShiftManager();

		SBAPI virtual void setEnable(bool val);
		SBAPI virtual bool isEnable();
		SBAPI virtual void start();
		SBAPI virtual void stop();

		SBAPI void notify(SBSubject* subject);

		SBAPI void initConnection();
		SBAPI void stopConnection();
		SBAPI virtual void update(double time);
		SBAPI double getCoeffValue(const std::string& blendName);
		SBAPI SrQuat getHeadRotation();
		SBAPI std::vector<std::string>& getShapeNames();


	protected:
		SOCKET connectSocket;	
		std::vector<std::string> blendShapeNames;
		std::map<std::string, double> coeffTable;
		SrQuat headRotation;
		std::vector<std::string> shapeNames;
};

}

#else // no support for FaceShift in non-Windows build
namespace SmartBody {


	class SBFaceShiftManager : public SBService
	{
	public:
		SBAPI SBFaceShiftManager() {}
		SBAPI ~SBFaceShiftManager() {}

		SBAPI virtual void setEnable(bool val) {}
		SBAPI virtual bool isEnable() { return false; }
		SBAPI virtual void start() {}
		SBAPI virtual void stop() {}

		SBAPI void notify(SBSubject* subject) {}

		SBAPI void initConnection() {}
		SBAPI void stopConnection() {}
		SBAPI virtual void update(double time) {}
		SBAPI double getCoeffValue(const std::string& blendName) { return 0.0; }
		SBAPI SrQuat getHeadRotation() { return SrQuat(); }

	};

}
#endif

#endif
