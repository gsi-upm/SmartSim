#ifndef _SBMPYTHONINTERNAL_
#define _SBMPYTHONINTERNAL_

#include <string>
#include <sb/SBScript.h>
#include <sb/SBSceneListener.h>
#include <sb/SBParser.h>
#include <sr/sr_vec.h>

#ifndef SB_NO_PYTHON

namespace SmartBody 
{
struct NvbgWrap :  Nvbg, boost::python::wrapper<Nvbg>
{
	virtual void objectEvent(std::string character, std::string name, bool isAnimate, SrVec charPosition, SrVec charVelocity, SrVec objPosition, SrVec objVelocity, SrVec relativePosition, SrVec relativeVelocity)
	{
		if (boost::python::override o = this->get_override("objectEvent"))
		{
			try {
				o(character, name, isAnimate, charPosition, charVelocity, objPosition, objVelocity, relativePosition, relativeVelocity);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_objectEvent(std::string character, std::string name, bool isAnimate, SrVec charPosition, SrVec charVelocity, SrVec objPosition, SrVec objVelocity, SrVec relativePosition, SrVec relativeVelocity)
	{
		return Nvbg::objectEvent(character, name, isAnimate, charPosition, charVelocity, objPosition, objVelocity, relativePosition, relativeVelocity);
	}

	virtual bool execute(std::string character, std::string to, std::string messageId, std::string xml)
	{
		if (boost::python::override o = this->get_override("execute"))
		{
			try {
				return o(character, to, messageId, xml);
			} catch (...) {
				PyErr_Print();
			}
		}

		return Nvbg::execute(character, to, messageId, xml);
	}

	bool default_execute(std::string character, std::string to, std::string messageId, std::string xml)
	{
		return Nvbg::execute(character, to, messageId, xml);
	}

	virtual bool executeEvent(std::string character, std::string messageId, std::string state)
	{
		if (boost::python::override o = this->get_override("executeEvent"))
		{
			try {
				return o(character, messageId, state);
			} catch (...) {
				PyErr_Print();
			}
		}

		return Nvbg::executeEvent(character, messageId, state);
	}

	bool default_executeEvent(std::string character, std::string messageId, std::string state)
	{
		return Nvbg::executeEvent(character, messageId, state);
	}

	virtual bool executeSpeech(std::string character, std::string speechStatus, std::string speechId, std::string speaker)
	{
		if (boost::python::override o = this->get_override("executeSpeech"))
		{
			try {
				return o(character, speechStatus, speechId, speaker);
			} catch (...) {
				PyErr_Print();
			}
		}

		return Nvbg::executeSpeech(character, speechStatus, speechId, speaker);
	}

	virtual bool executeSpeechRequest(std::vector<std::string> behaviors, std::vector<std::string> types, std::vector<float> times, std::vector<std::string> targets, std::vector<std::string> info)
	{
		if (boost::python::override o = this->get_override("executeSpeechRequest"))
		{
			try {
				return o(behaviors, types, times, targets, info);
			} catch (...) {
				PyErr_Print();
			}
		}

		return Nvbg::executeSpeechRequest(behaviors, types, times, targets, info);
	}

	bool default_executeSpeech(std::string character, std::string speechStatus, std::string speechId, std::string speaker)
	{
		return Nvbg::executeSpeech(character, speechStatus, speechId, speaker);
	}

	bool default_executeSpeechRequest(std::vector<std::string> behaviors, std::vector<std::string> types, std::vector<float> times, std::vector<std::string> targets, std::vector<std::string> info)
	{
		return Nvbg::executeSpeechRequest(behaviors, types, times, targets, info);
	}

	virtual void notifyAction(std::string name)
	{
		if (boost::python::override o = this->get_override("notifyAction"))
		{
			try {
				o(name);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyAction(std::string name)
	{
		notifyLocal(name);
	}

	virtual void notifyBool(std::string name, bool val)
	{
		if (boost::python::override o = this->get_override("notifyBool"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyBool(std::string name, bool val)
	{
		notifyLocal(name);
	}

	virtual void notifyInt(std::string name, int val)
	{
		if (boost::python::override o = this->get_override("notifyInt"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyInt(std::string name, int val)
	{
		notifyLocal(name);
	}

	virtual void notifyDouble(std::string name, double val)
	{
		if (boost::python::override o = this->get_override("notifyDouble"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyDouble(std::string name, double val)
	{
		notifyLocal(name);
	}

	virtual void notifyString(std::string name, std::string val)
	{
		if (boost::python::override o = this->get_override("notifyString"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyString(std::string name, std::string val)
	{
		notifyLocal(name);
	}

	virtual void notifyVec3(std::string name, SrVec val)
	{
		if (boost::python::override o = this->get_override("notifyVec3"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyVec3(std::string name, SrVec val)
	{
		notifyLocal(name);
	}

	virtual void notifyMatrix(std::string name, SrMat val)
	{
		if (boost::python::override o = this->get_override("notifyMatrix"))
		{
			try {
				o(name, val);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_notifyMatrix(std::string name, SrMat val)
	{
		notifyLocal(name);
	}


};

struct SBParserListenerWrap : SBParserListener, boost::python::wrapper<SBParserListener>
{
	virtual void onWord(std::string timeMarker, std::string word)
	{
		if (boost::python::override o = this->get_override("onWord"))
		{
			try {
				o(timeMarker, word);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_onWord(std::string timeMarker, std::string word)
	{
		return SBParserListener::onWord(timeMarker, word);
	}

	virtual void onPartOfSpeech(std::string timeMarker, std::string partOfSpeech)
	{
		if (boost::python::override o = this->get_override("onPartOfSpeech"))
		{
			try {
				o(timeMarker, partOfSpeech);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_onPartOfSpeech(std::string timeMarker, std::string partOfSpeech)
	{
		return SBParserListener::onPartOfSpeech(timeMarker, partOfSpeech);
	}
};


struct SBScriptWrap :  SBScript, boost::python::wrapper<SBScript>
{
	virtual void start()
	{
		if (boost::python::override o = this->get_override("start"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_start()
	{
		return SBScript::start();
	}

	virtual void stop()
	{
		if (boost::python::override o = this->get_override("stop"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_stop()
	{
		return SBScript::stop();
	}

	virtual void beforeUpdate(double time)
	{
		if (boost::python::override o = this->get_override("beforeUpdate"))
		{
			try {
				o(time);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_beforeUpdate(double time)
	{
		return SBScript::beforeUpdate(time);
	}

	virtual void update(double time)
	{
		if (boost::python::override o = this->get_override("update"))
		{
			try {
				o(time);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_update(double time)
	{
		return SBScript::update(time);
	}

	virtual void afterUpdate(double time)
	{
		if (boost::python::override o = this->get_override("afterUpdate"))
		{
			try {
				o(time);
			} catch (...) {
				PyErr_Print();
			}
		}
	}

	void default_afterUpdate(double time)
	{
		return SBScript::afterUpdate(time);
	}
};


struct SBEventHandlerWrap :  SBEventHandler, boost::python::wrapper<SBEventHandler>
{
	virtual void executeAction(SBEvent* event)
	{
		if (boost::python::override o = this->get_override("executeAction"))
		{
			try {
				o(event);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBEventHandler::executeAction(event);
	}

	void default_executeAction(SBEvent* event)
	{
		SBEventHandler::executeAction(event);
	}
};


struct PythonControllerWrap : SmartBody::PythonController, boost::python::wrapper<SmartBody::PythonController>
{
	virtual void start()
	{
		if (boost::python::override o = this->get_override("start"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		return PythonController::start();
	};

	void default_start()
	{
		SmartBody::PythonController::start();
	}

	virtual void init()
	{
		if (boost::python::override o = this->get_override("init"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		return PythonController::init();
	};

	void default_init()
	{
		SmartBody::PythonController::init();
	}

	virtual void evaluate()
	{
		if (boost::python::override o = this->get_override("evaluate"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		return PythonController::evaluate();
	};

	void default_evaluate()
	{
		SmartBody::PythonController::evaluate();
	}

	virtual void stop()
	{
		if (boost::python::override o = this->get_override("stop"))
		{
			try {
				o();
			} catch (...) {
				PyErr_Print();
			}
		}

		return PythonController::stop();
	};

	void default_stop()
	{
		SmartBody::PythonController::stop();
	}
};

struct TransitionRuleWrap : SmartBody::SBAnimationTransitionRule, boost::python::wrapper<SmartBody::SBAnimationTransitionRule>
{
	virtual bool check(SmartBody::SBCharacter* character, SmartBody::SBAnimationBlend* blend)
	{
		if (boost::python::override o = this->get_override("check"))
		{
			try {
				return o(character, blend);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBAnimationTransitionRule::check(character, blend);
	}

	bool default_check(SmartBody::SBCharacter* character, SmartBody::SBAnimationBlend* blend)
	{
		return SBAnimationTransitionRule::check(character, blend);
	}
};

struct CharacterListenerWrap : SmartBody::SBSceneListener, boost::python::wrapper<SmartBody::SBSceneListener>
{
	virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass )
	{
		if (boost::python::override o = this->get_override("OnCharacterCreate"))
		{
			try {
				o(name, objectClass);
			} catch (...) {
				PyErr_Print();
			}
		}

		return SBSceneListener::OnCharacterCreate(name, objectClass);
	}

	void default_OnCharacterCreate( const std::string & name, const std::string & objectClass ) \
	{
		SBSceneListener::OnCharacterCreate(name, objectClass);
	}

	virtual void OnCharacterDelete( const std::string & name )
	{
		if (boost::python::override o = this->get_override("OnCharacterDelete"))
		{
			try {
				o(name);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnCharacterDelete(name);
	
	}
	void default_OnCharacterDelete( const std::string & name )
	{
		SBSceneListener::OnCharacterDelete(name);
	}

	virtual void OnCharacterUpdate( const std::string & name)
	{
		if (boost::python::override o = this->get_override("OnCharacterUpdate"))
		{
			try {
				o(name);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnCharacterUpdate(name);
	}
	void default_OnCharacterUpdate( const std::string & name )
	{
		SBSceneListener::OnCharacterUpdate(name);
	}

	virtual void OnPawnCreate( const std::string & name )
	{
		if (boost::python::override o = this->get_override("OnPawnCreate"))
		{
			try {
				o(name);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnPawnCreate(name);
	}
	
	void default_OnPawnCreate( const std::string & name )
	{
		SBSceneListener::OnPawnCreate(name);
	}

	virtual void OnPawnDelete( const std::string & name )
	{
		if (boost::python::override o = this->get_override("OnPawnDelete"))
		{
			try {
				o(name);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnPawnDelete(name);
	}

		
	void default_OnPawnDelete( const std::string & name )
	{
		SBSceneListener::OnPawnDelete(name);
	}

	virtual void OnEvent( const std::string & type, const std::string & params )
	{
		if (boost::python::override o = this->get_override("OnEvent"))
		{
			try {
				o(type, params);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnEvent(type, params);
	}
	
	void default_OnEvent( const std::string & type, const std::string & params )
	{
		SBSceneListener::OnEvent(type, params);
	}

	virtual void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime )
	{
		if (boost::python::override o = this->get_override("OnViseme"))
		{
			try {
				o(name, visemeName, weight, blendTime);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnViseme(name, visemeName, weight, blendTime);
	}
	
	void default_OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime )
	{
		SBSceneListener::OnViseme(name, visemeName, weight, blendTime);
	}

	virtual void OnChannel( const std::string & name, const std::string & channelName, const float value)
	{
		if (boost::python::override o = this->get_override("OnChannel"))
		{
			try {
				o(name, channelName, value);
			} catch (...) {
				PyErr_Print();
			}
		}

		SBSceneListener::OnChannel(name, channelName, value);
	}
	
	void default_OnChannel( const std::string & name, const std::string & channelName, const float value)
	{
		SBSceneListener::OnChannel(name, channelName, value);
	}

	virtual void OnLogMessage( const std::string & message)
	{
		if (boost::python::override o = this->get_override("OnLogMessage"))
		{
			try {
				o(message);
			} catch (...) {
				PyErr_Print();
			}
		}
		SBSceneListener::OnLogMessage(message);
	}
	
	void default_OnLogMessage( const std::string & message)
	{
		SBSceneListener::OnLogMessage(message);
	}
};

}



// wrapper for std::map
template<class T>
struct map_item
{
	typedef typename T::key_type K;
	typedef typename T::mapped_type V;
	static V get(T const& x, K const& i)
	{
		V temp;
		if( x.find(i) != x.end() ) 
			return x.find(i)->second;
		PyErr_SetString(PyExc_KeyError, "Key not found");
		return temp;		
	}
	static void set(T & x, K const& i, V const& v)
	{
		x[i]=v; // use map autocreation feature
	}
	static void del(T & x, K const& i)
	{
		if( x.find(i) != x.end() ) x.erase(i);
		else PyErr_SetString(PyExc_KeyError, "Key not found");
	}
};

#endif

#endif
