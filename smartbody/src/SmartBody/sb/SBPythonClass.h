#ifndef _SBMPYTHONCLASS_
#define _SBMPYTHONCLASS_




#include <map>
#include <sb/SBJoint.h>
#include <sb/SBSkeleton.h>
#include <sb/SBCharacter.h>
#include <sb/SBController.h>
#include <sr/sr_viewer.h>

//#if !defined (__ANDROID__)
#ifndef USE_PYTHON
#define USE_PYTHON
#endif
//#endif

#ifndef SB_NO_PYTHON
#include <boost/python.hpp>
#endif
namespace SmartBody 
{

SrViewer* getViewer();


#ifndef SB_NO_PYTHON
class PyLogger
{
protected:
	static std::string strBuffer;
public:
	static void pa();
	static void pb();	
	static void pc();
	static void pd();	
	static void pe();
	static void pf();	
	static void pg();
	static void ph();	
	static void pi();
	static void pj();	
	static void pk();
	static void pl();	
	static void pm();
	static void pn();	
	static void po();
	static void pp();	
	static void pq();
	static void pr();	
	static void ps();
	static void pt();	
	static void pu();
	static void pv();	
	static void pw();
	static void px();	
	static void py();
	static void pz();	
	static void p1();
	static void p2();
	static void p3();
	static void p4();
	static void p5();
	static void p6();
	static void p7();
	static void p8();
	static void p9();
	static void p0();

	static void openparen() { strBuffer += "("; }
	static void closeparen() { strBuffer += ")"; }
	static void openbracket() { strBuffer += "["; }
	static void closebracket() { strBuffer += "]"; }
	static void openbrace() { strBuffer += "{"; }
	static void closebrace() { strBuffer += "}"; }
	static void plus() { strBuffer += "+"; }
	static void minus() { strBuffer += "-"; }
	static void aster() { strBuffer += "*"; }
	static void slash() { strBuffer += "\\"; }
	static void backslash() { strBuffer += "/"; }
	static void comma() { strBuffer += ","; }
	static void colon() { strBuffer += ":"; }
	static void semicolon() { strBuffer += ";"; }
	static void equal() { strBuffer += "="; }
	static void less() { strBuffer += "<"; }
	static void more() { strBuffer += ">"; }

	static void pspace();
	static void pnon();
	static void outlog();
};

class Camera
{
	public:
		Camera();
		~Camera();
		
		void printInfo();

		void reset();
		void setDefault(int preset = 1);

		void setEye(float x = 0.0, float y = 0.0, float z = 0.0);
		SrVec getEye();
		void setCenter(float x = 0.0, float y = 0.0, float z = 0.0);
		SrVec getCenter();
		void setScale(float s = 1.0);
		float getScale();

		void setTrack(std::string cName, std::string jName);
		void removeTrack();
		void loadCamera(std::string camFileName);
		void saveCamera(std::string camFileName);
};

void pythonExit();
void quitSbm();
void reset();
void printLog(const std::string& message);

SBController* createController(std::string controllerType, std::string controllerName);

SrCamera* getCamera();

class PythonController :  public SBController
{
public:
	std::string controllerType;
	PythonController() : SBController() { controllerType = "python";}
	virtual void start() {};
	virtual void init() {};
	virtual void evaluate() {};
	virtual void stop() {};

	virtual SkChannelArray& controller_channels () { return channels;}
	virtual double controller_duration () { return  1000000.0; }
	const std::string& controller_type() const { return controllerType; }
	bool controller_evaluate(double t, MeFrameData& frame )
	{
		evaluate();
		return true;
	}

protected:
	SkChannelArray channels;
};
#endif

}


#endif
