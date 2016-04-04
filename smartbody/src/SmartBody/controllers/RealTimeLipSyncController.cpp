#include "RealTimeLipSyncController.h"
#include <sb/SBScene.h>
#include <sb/SBMotion.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBPhoneme.h>
#include <bml/bml_speech.hpp>
#include <vector>

RealTimeLipSyncController::RealTimeLipSyncController(SmartBody::SBCharacter* c) : SmartBody::SBController()
{
	_pawn = c;
	setup();
}

RealTimeLipSyncController::RealTimeLipSyncController() : SmartBody::SBController()
{
	setup();
}

void RealTimeLipSyncController::setup()
{
	// breathing settings
	setDefaultAttributeGroupPriority("Lip Sync", 450);
	addDefaultAttributeBool("lipsync.useRealTimeLipSync", false, "Lip Sync");
	addDefaultAttributeString("lipsync.realTimeLipSyncName", "", "Lip Sync");
	addDefaultAttributeDouble("lipsync.realTimeLipSyncDelay", .25, "Lip Sync");
	addDefaultAttributeDouble("lipsync.realTimeLipSyncMaxCoarticulationTime", 1.0, "Lip Sync");
	addDefaultAttributeBool("lipsync.useRealTimeCurveCleanup", true, "Lip Sync");
	addDefaultAttributeBool("lipsync.useRealTimeDebug", false, "Lip Sync");
}

void RealTimeLipSyncController::updateLipSyncChannels()
{
	if (!_pawn)
		return;

	SmartBody::StringAttribute* attribute = dynamic_cast<SmartBody::StringAttribute*>(_pawn->getAttribute("lipsyncSetName"));
	if (attribute)
	{
		const std::string& value = attribute->getValue();
		std::vector<SmartBody::SBDiphone*>& allDiphones = SmartBody::SBScene::getScene()->getDiphoneManager()->getDiphones(value);
		std::set<std::string> allVisemes;
		for (size_t d = 0; d < allDiphones.size(); d++)
		{
			std::vector<std::string> visemes = allDiphones[d]->getVisemeNames();
			for (size_t v = 0; v < visemes.size(); v++)
			{
				allVisemes.insert(visemes[v]);
			}
		}
	}
}

RealTimeLipSyncController::~RealTimeLipSyncController()
{
}

void RealTimeLipSyncController::init(SmartBody::SBPawn* pawn)
{
	MeController::init(pawn);

	_lastPhonemeTime = 0.0;
	_lastPhoneme = "";

	SmartBody::SBAttribute* attribute = _pawn->getAttribute("lipsyncSetName");
	if (!attribute)
		return;
	// make sure that the channel list is updated if the set is updated
	attribute->registerObserver(this);

	// determine the channels to be used for lip syncing
	const std::string& lipSyncSetName = _pawn->getStringAttribute("lipsyncSetName");

}

bool RealTimeLipSyncController::controller_evaluate ( double t, MeFrameData& frame )
{
	if (!_pawn)
		return false;

	if (!_pawn->getBoolAttribute("lipsync.useRealTimeLipSync"))
		return false;

	// _currentPhonemes and _currentPhonemeTimings contain a list phonemes that are
	//    gathered from the real time phoneme manager
	// _currentCurves contain the animation data that should be played
	// the algorithm converts the phonemes+timings into animation curves

	// get the offset between the actual phoneme timings and when it will be played on the character
	double timeDelay = _pawn->getDoubleAttribute("lipsync.realTimeLipSyncDelay");

	// remove any curves that are no longer valid
	if (_currentCurves.size() > 0)
	{
		if (_pawn->getBoolAttribute("lipsync.useRealTimeCurveCleanup"))
		{
			bool removeCurves = true;
			while (removeCurves)
			{
				removeCurves = false;
				for (std::vector<std::pair<std::string, srLinearCurve*> >::iterator iter = _currentCurves.begin();
					 iter != _currentCurves.end();
					 iter++)
				{
					srLinearCurve* curve = (*iter).second;
					double lastKeyTime = curve->get_tail_param();
					if (lastKeyTime + timeDelay < t)
					{
						delete curve;
						_currentCurves.erase(iter);
						removeCurves = true;
						break;
					}
				}
			}
		}
	}



	if (_currentCurves.size() > 0)
	{
		
		std::vector<std::pair<std::string, double> > curveData;
		for (std::vector<std::pair<std::string, srLinearCurve*> >::iterator iter = _currentCurves.begin();
			 iter != _currentCurves.end();
			 iter++)
		{
			const std::string& visemeName = (*iter).first;
			srLinearCurve* curve = (*iter).second;

			double value = curve->evaluate(t - timeDelay);
			if (value > 0.0)
			{
				curveData.push_back(std::pair<std::string, double>(visemeName, value));
			}
		}
		
		// at this point, there might be several curves of the same name with different timings.
		// need to derive a single value based on multiple curve data.
		// loop through all the evaluated data and take the max value of any duplicated curve
		std::set<std::string> finishedVisemes;
		std::vector<std::pair<std::string, double> >::iterator startIter = curveData.begin();
		while (startIter != curveData.end())
		{

			std::string curCurveName = "";
			double val = 0.0;
			for (std::vector<std::pair<std::string, double> >::iterator curIter = startIter;
				 curIter != curveData.end();
				 curIter++)
			{
				if (curCurveName == "")
				{
					std::set<std::string>::iterator existIter = finishedVisemes.find((*curIter).first);
					if (existIter != finishedVisemes.end())
						continue;
					curCurveName = (*curIter).first;
					val = (*curIter).second;
					
				}
				else
				{
					if (curCurveName == (*curIter).first)
					{
						// use the max value
						if (val < (*curIter).second)
							val = (*curIter).second;
					}
				}
			}
			if (curCurveName != "")
			{
				this->setChannelValue(curCurveName, val);
			}
			if (_pawn->getBoolAttribute("lipsync.useRealTimeDebug"))
				LOG("%s %f", curCurveName.c_str(), val);
			finishedVisemes.insert(curCurveName);
			startIter++;
		}
	}

	
	SmartBody::SBPhonemeManager* phonemeManager = SmartBody::SBScene::getScene()->getDiphoneManager();

	const std::string& realTimeLipSyncName = _pawn->getStringAttribute("lipsync.realTimeLipSyncName");

	const std::string& lipSyncSetName = _pawn->getStringAttribute("lipSyncSetName");
	bool hasRealtimePhonemes = true;
	double maxCoarticulationTime = _pawn->getDoubleAttribute("lipsync.realTimeLipSyncMaxCoarticulationTime");
	// eliminate any phonemes that exceed the maximum coarticulation time
	phonemeManager->removePhonemesRealtimeByTime(realTimeLipSyncName, t - maxCoarticulationTime);

	while (hasRealtimePhonemes)
	{
		hasRealtimePhonemes = false;
		std::vector<std::string> phonemeList = phonemeManager->getPhonemesRealtime(realTimeLipSyncName, 3);
		std::vector<double> phonemeTimingsList = phonemeManager->getPhonemesRealtimeTimings(realTimeLipSyncName, 3);
		if (phonemeTimingsList.size() == 3)
		{
			phonemeManager->removePhonemesRealtime(realTimeLipSyncName, 1);
			hasRealtimePhonemes = true;
		}
		else
		{
			// less than three phonemes, wait until at least three are present
			break;
		}


		for (size_t p = 0; p < phonemeTimingsList.size(); p++)
		{
			_currentPhonemes.push_back(phonemeList[p]);
			_currentPhonemeTimings.push_back(phonemeTimingsList[p]);
		}

		double fromTime = _currentPhonemeTimings[_currentPhonemeTimings.size() - 3];
		double toTime = _currentPhonemeTimings[_currentPhonemeTimings.size() - 1];

		double diphoneInterval = toTime - fromTime;
		if (diphoneInterval <= 0.0)
		{
			// if the timings are improper (end time > start time) 
			// then assume this data is bad, and remove the phonemes from the list
			_currentPhonemes.clear();
			_currentPhonemeTimings.clear();
			return true;
		}

		std::string fromPhoneme = _currentPhonemes[_currentPhonemes.size() - 3];
		std::string toPhoneme = _currentPhonemes[_currentPhonemes.size() - 2];
		
		SmartBody::VisemeData* visemeStart = new SmartBody::VisemeData(fromPhoneme, 0);
		SmartBody::VisemeData* visemeEnd = new SmartBody::VisemeData(toPhoneme, (float) diphoneInterval);
		std::vector<SmartBody::VisemeData*> visemes;
		visemes.push_back(visemeStart);
		visemes.push_back(visemeEnd);

		_lastPhonemeTime = t;
		// obtain a set of curves from the phoneme manager based on the two phonemes
		std::map<std::string, std::vector<float> > lipSyncCurves = BML::SpeechRequest::generateCurvesGivenDiphoneSet(&visemes, lipSyncSetName, _pawn->getName());

		// smooth the curves
		for (std::map<std::string, std::vector<float> >::iterator iter = lipSyncCurves.begin();
				iter != lipSyncCurves.end();
				iter++)
		{
			smoothCurve((*iter).second,  _pawn->getDoubleAttribute("lipSyncSmoothWindow"));
		}

		for (std::map<std::string, std::vector<float> >::iterator iter = lipSyncCurves.begin();
				iter != lipSyncCurves.end();
				iter++)
		{
			srLinearCurve* curve = new srLinearCurve();
			for (std::vector<float>::iterator fiter = (*iter).second.begin();
					fiter != (*iter).second.end();
					fiter++)
			{
				float time = (*fiter);
				fiter++;
				float value = (*fiter);
				curve->insert(time + _lastPhonemeTime, value);
			}
			_currentCurves.push_back(std::pair<std::string, srLinearCurve*>((*iter).first, curve));
			
		}

		// since we have converted the phonemes into animation curves, remove the phonemes from the list
		_currentPhonemes.clear();
		_currentPhonemeTimings.clear();
	}



	return true;
}


void RealTimeLipSyncController::notify(SmartBody::SBSubject* subject)
{
	SBController::notify(subject);

	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "lipsyncSetName")
		{
			updateLipSyncChannels();
		}
	}
}


void RealTimeLipSyncController::smoothCurve(std::vector<float>& c, float windowSize)
{
	if (windowSize <= 0)
		return;

	bool keepSmoothing = true;
	int numIter = 0;

	while (keepSmoothing)
	{
		numIter++;
		std::vector<float> x;
		std::vector<float> y;
		std::vector<bool> markDelete;
		for (size_t i = 0; i < c.size(); i++)
		{
			if ((i % 2) == 0)
			{
				x.push_back(c[i]);
				markDelete.push_back(false);
			}
			else
				y.push_back(c[i]);
		}


		// global smoothing by window size
		std::vector<int> localMaxId;
		for (size_t i = 1; i < x.size() - 1; ++i)
		{
			if ((y[i] - y[i - 1]) >= 0 &&
				(y[i] - y[i + 1]) >= 0)
			{
				localMaxId.push_back(i);
			}
		}

		if (localMaxId.size() == 0)
			return;

		for (size_t i = 0; i < localMaxId.size() - 1; ++i)
		{
			if (x[localMaxId[i + 1]] - x[localMaxId[i]] <= windowSize)
			{
				for (int markeId = (localMaxId[i] + 1); markeId < localMaxId[i + 1]; ++markeId)
				{
					float toDeleteY = y[markeId];
					float ratio = (x[markeId] - x[localMaxId[i]]) / (x[localMaxId[i +1]] - x[localMaxId[i]]);
					float actualY = ratio * (y[localMaxId[i + 1]] - y[localMaxId[i]]) + y[localMaxId[i]];
					if (actualY >= toDeleteY)
						markDelete[markeId] = true;
				}
			}
		}

		c.clear();
		for (size_t i = 0; i < markDelete.size(); i++)
		{
			if (!markDelete[i])
			{
				c.push_back(x[i]);
				c.push_back(y[i]);
			}
		}

		if (c.size() == x.size() * 2)
			keepSmoothing = false;
	}
//	LOG("Number of smoothing iteration %d", numIter);
}



