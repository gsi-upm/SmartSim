#include "SBController.h"
#include <sb/SBPawn.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>

namespace SmartBody {

SBController::SBController() : MeController()
{
	controllerType = "SBController";
	this->setName("controller");
}

SBController::SBController(const SBController& controller)
{
	// ?
}

SBController::~SBController()
{
}

void SBController::addChannel(std::string channelName, std::string type)
{
	SkChannel::Type channelType = SkChannel::XPos;
	if (type == "XPos")
	{
		channelType = SkChannel::XPos;
	}
	else if (type == "YPos")
	{
		channelType = SkChannel::YPos;
	}
	else if (type == "ZPos")
	{
		channelType = SkChannel::ZPos;
	}
	else if (type == "Quat")
	{
		channelType = SkChannel::Quat;
	}
	else 
	{
		LOG("Channel type %s not valid, must be one of: XPos, YPos, ZPos, Quat. Channel %s not added.", type.c_str(), channelName.c_str());
		return;
	}

	int index = _context->channels().search(channelName, channelType);
	if (index != -1)
	{
		LOG("Channel %s of type %s already present in motion, duplicate channel not added.", channelName.c_str(), type.c_str());
		return;
	}

	 _context->channels().add(channelName, channelType);
}

const std::string& SBController::getType()
{
	return controller_type();
}

const std::string& SBController::getCharacterName()
{
	return controllerType; // need to fix this
}

void SBController::setIgnore(bool val)
{
	setEnable(!val);
	BoolAttribute* enableAttribute = dynamic_cast<BoolAttribute*>(getAttribute("enable"));

	if (enableAttribute)
	{
		enableAttribute->setValueFast(!val);
	}
}

bool SBController::isIgnore()
{
	return !isEnabled();
}

void SBController::setDebug(bool val)
{
	record_buffer_changes(val);
}

bool SBController::isDebug()
{
	return is_record_buffer_changes();
}

double SBController::getDuration()
{
	return controller_duration();
}

void SBController::setJointChannelPos( const std::string& jointName, MeFrameData& frame, SrVec& outPos )
{
	bool hasTranslation = true;

	for (int i=0;i<3;i++)
	{
		int positionChannelID = _context->channels().search(jointName, (SkChannel::Type)(SkChannel::XPos+i));
		if (positionChannelID < 0) hasTranslation = false;
		int posBufferID = frame.toBufferIndex(positionChannelID);
		if (posBufferID < 0) hasTranslation = false;
		//LOG("SBController : posChannelID = %d, posBufferID = %d",positionChannelID, posBufferID);
		if (hasTranslation)
		{		
			frame.buffer()[posBufferID] = outPos[i];				
		}
	}	
}

void SBController::setJointChannelQuat( const std::string& jointName, MeFrameData& frame, SrQuat& inQuat )
{
	bool hasRotation = true;
	int channelId = _context->channels().search(jointName, SkChannel::Quat);
	if (channelId < 0)	hasRotation = false;
	int bufferId = frame.toBufferIndex(channelId);
	if (bufferId < 0)	hasRotation = false;	

	if (hasRotation)
	{
		frame.buffer()[bufferId + 0] = inQuat.w;
		frame.buffer()[bufferId + 1] = inQuat.x;;
		frame.buffer()[bufferId + 2] = inQuat.y;;
		frame.buffer()[bufferId + 3] = inQuat.z;;
	}
}


SBAPI void SBController::getJointChannelValues( const std::string& jointName, MeFrameData& frame, SrMat& outMat )
{
	SrVec outPos;
	SrQuat outQuat;
	getJointChannelValues(jointName,frame,outQuat, outPos);
	outQuat.get_mat(outMat);
	outMat.set_translation(outPos);
}

void SBController::getJointChannelValues( const std::string& jointName, MeFrameData& frame, SrQuat& outQuat, SrVec& outPos )
{
	bool hasRotation = true;
	int channelId = _context->channels().search(jointName, SkChannel::Quat);
	if (channelId < 0)	hasRotation = false;
	int bufferId = frame.toBufferIndex(channelId);
	if (bufferId < 0)	hasRotation = false;	

	bool hasTranslation = true;
	int positionChannelID = _context->channels().search(jointName, SkChannel::XPos);
	if (positionChannelID < 0) hasTranslation = false;
	int posBufferID = frame.toBufferIndex(positionChannelID);
	if (posBufferID < 0) hasTranslation = false;		
	// input reference pose
	if (hasRotation)
	{
		outQuat.w = frame.buffer()[bufferId + 0];
		outQuat.x = frame.buffer()[bufferId + 1];
		outQuat.y = frame.buffer()[bufferId + 2];
		outQuat.z = frame.buffer()[bufferId + 3];
	}
	if (hasTranslation)
	{
		outPos.x = frame.buffer()[posBufferID + 0];
		outPos.y = frame.buffer()[posBufferID + 1];
		outPos.z = frame.buffer()[posBufferID + 2];				
	}
}

double SBController::getChannelValue(const std::string& channelName)
{
	if (!_pawn)
		return 0.0;

	if (!_curFrame)
		return 0.0;
	
	SrVec data;
	SrQuat quat;
	getJointChannelValues(channelName, *_curFrame, quat, data);
	return data[0];
}

SrVec SBController::getChannelPos(const std::string& channelName)
{
	if (!_pawn)
		return SrVec();

	if (!_curFrame)
		return SrVec();

	SrVec data;
	SrQuat quat;
	getJointChannelValues(channelName, *_curFrame, quat, data);
	return data;
}

SrQuat SBController::getChannelQuat(const std::string& channelName)
{
	if (!_pawn)
		return SrQuat();

	if (!_curFrame)
		return SrQuat();

	SrVec data;
	SrQuat quat;
	getJointChannelValues(channelName, *_curFrame, quat, data);
	return quat;
}

void SBController::setChannelValue(const std::string& channelName, double val)
{
	if (!_pawn)
		return;

	if (!_curFrame)
		return;

	bool hasTranslation = true;

	int positionChannelID = _context->channels().search(channelName, (SkChannel::Type)(SkChannel::XPos));
	if (positionChannelID < 0) hasTranslation = false;
	int posBufferID = (*_curFrame).toBufferIndex(positionChannelID);
	if (posBufferID < 0) hasTranslation = false;
	//LOG("SBController : posChannelID = %d, posBufferID = %d",positionChannelID, posBufferID);
	if (hasTranslation)
	{		
		(*_curFrame).buffer()[posBufferID] = val;				
	}
}

void SBController::setChannelPos(const std::string& channelName, SrVec pos)
{
	if (!_pawn)
		return;	
	if (!_curFrame)
		return;

	SrVec data;
	data = pos;
	setJointChannelPos(channelName, *_curFrame, data);
}

void SBController::setChannelQuat(const std::string& channelName, SrQuat quat)
{
	if (!_pawn)
		return;
	if (!_curFrame)
		return;

	SrQuat data;
	data = quat;
	setJointChannelQuat(channelName, *_curFrame, data);
}

SBAPI void SBController::setChannelQuatGlobal( const std::string& channelName, SrQuat quat )
{
	if (!_pawn)
		return;
	if (!_curFrame)
		return;

	SmartBody::SBJoint* joint = _pawn->getSkeleton()->getJointByMappedName(channelName);
	if (!joint)
		return;

	SrQuat preRot = SrQuat(joint->gmatZero());
	SrQuat inversePreRot = preRot.inverse();
	SrQuat temp = inversePreRot * quat * preRot;
	setJointChannelQuat(channelName, *_curFrame, temp);
}

void SBController::addControllerModifier(SBControllerModifier* modifier)
{
}

void SBController::removeControllerModifier(SBControllerModifier* modifier)
{
}

void SBController::removeAllControllerModifiers()
{
}

SmartBody::SBControllerModifier* SBController::getControllerModifier(const std::string& name)
{
	return NULL;
}

std::vector<SmartBody::SBControllerModifier*>& SBController::getControllerModifiers()
{
	return _modifiers;
}





};

