#ifndef	_ME_CT_NOISE_CONTROLLER_H_
#define _ME_CT_NOISE_CONTROLLER_H_

#include <sb/SBController.h>
#include <sb/sbm_character.hpp>
#include <external/perlin/perlin.h>
#include <map>

class MeCtNoiseController : public SmartBody::SBController
{
	public:
		static std::string CONTROLLER_TYPE;

	private:
		// fade parameters
		double			_fadeStartTime;
		double			_fadeInInterval;
		double			_fadeOutInterval;
		int				_fadeType;

		double			_prevTime;
		double			_dt;
		float 			_duration;
		SkChannelArray	_channels;
		SbmCharacter* 	_character;
		SmartBody::SBSkeleton*     _skeletonCopy;
		bool			_valid;	
		std::map<std::string, Perlin> perlinMap;
		float perlinScale, perlinFrequency, perlinDuration;
	public:
		MeCtNoiseController(SbmCharacter* character);
		~MeCtNoiseController();

		bool getValid()						{return _valid;}
		void setValid(bool v);
		void setFadeInInterval(double v)		{_fadeInInterval = v;}
		void setFadeOutInterval(double v)		{_fadeOutInterval = v;}
		void setJointNoise(std::vector<std::string>& jointNames, float scale, float frequency);
	protected:
		float getNormalizeTime(float t, float offset);
	private:
		virtual bool controller_evaluate(double t, MeFrameData& frame);
		virtual SkChannelArray& controller_channels()	{ return(_channels); }
		virtual double controller_duration()			{ return((double)_duration); }
		virtual const std::string& controller_type() const		{ return(CONTROLLER_TYPE); }			
};

#endif