#ifndef _SBVERSION_H_
#define _SBVERSION_H_

#include <string>
#include <sb/SBTypes.h>

namespace SmartBody {

std::string getVersion(void)
{
	std::stringstream strstr;
	strstr << "SmartBody ";
#ifdef _DEBUG
	strstr << " debug ";
#else
	strstr << " release ";
#endif

#ifdef WIN32
	strstr << " win32 ";
#else
#ifdef SB_MAC
#else
#ifdef SB_MAC
	strstr << " osx ";
#else
#ifdef SB_IPHONE
	strstr << " ios ";
#else
#ifdef __ANDROID__
	strstr << " android ";
#else
	strstr << " other ";
#endif
#endif
#endif
#endif
#endif
	strstr << "version $WCREV$ $WCDATE$";

	return strstr.str();
}

}


#endif