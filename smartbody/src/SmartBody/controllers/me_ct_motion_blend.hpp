
#ifndef _ME_MOTION_BLEND_CONTROLLER_
#define _ME_MOTION_BLEND_CONTROLLER_

#include "me_ct_data_interpolation.h"
#include "me_ct_barycentric_interpolation.h"
#include "me_ct_motion_parameter.h"
#include "me_ct_jacobian_IK.hpp"
#include "me_ct_ccd_IK.hpp"
#include "me_ct_constraint.hpp"

#include <controllers/MeCtBlendEngine.h>
#include <sb/SBController.h>

class ReachStateData;
class ReachStateInterface;
class ReachHandAction;

using namespace std;
// used when we want exact control for an end effector

typedef std::map<int,MeCtBlendEngine*> BlendEngineMap; 

class MeCtMotionBlend :
	public SmartBody::SBController, public FadingControl
{
private:
	static std::string CONTROLLER_TYPE;
public:	
	//enum HandActionState { PICK_UP_OBJECT = 0, TOUCH_OBJECT, PUT_DOWN_OBJECT };

protected:	
	int                   defaultBlendType;
	BlendEngineMap        blendEngineMap;
	MeCtBlendEngine*      currentBlendEngine;
	std::string           characterName;						
	vector<SkJoint*>      affectedJoints;		
	BodyMotionFrame       inputMotionFrame;		
	float 			      _duration;
	SkChannelArray	      _channels;

public:	
	MeCtMotionBlend(std::map<int,MeCtBlendEngine*>& blendMap);
	virtual ~MeCtMotionBlend(void);		
	virtual void controller_map_updated();
	virtual void controller_start();	
	virtual bool controller_evaluate( double t, MeFrameData& frame );

	virtual SkChannelArray& controller_channels()	{ return( _channels ); }
	virtual double controller_duration()			{ return( (double)_duration ); }	
	virtual const std::string& controller_type() const		{ return( CONTROLLER_TYPE ); }

	MeCtBlendEngine* getBlendEngine() const { return currentBlendEngine; }	
	void set_duration(float duration) { _duration = duration; }

	void setParameterTargetPawn(SbmPawn* targetPawn);
	void setParameterTargetJoint(SkJoint* targetJoint);
	void setParameterTargetPos(SrVec& targetPos);
	void setParameterVector(dVector& para);
	void init(SbmPawn* pawn);		
protected:			
	void updateChannelBuffer(MeFrameData& frame, BodyMotionFrame& motionFrame, bool bRead = false);		
};

#endif





