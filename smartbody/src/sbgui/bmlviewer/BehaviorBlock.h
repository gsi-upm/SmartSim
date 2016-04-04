#ifndef _BEHAVIORBLOCK_
#define _BEHAVIORBLOCK_

#include <bml/bml.hpp>
#include "nle/NonLinearEditor.h"

class BehaviorMark : public nle::Mark
{
	public:
		BehaviorMark();		
};

class BehaviorTimingMark : public nle::Mark
{
	public:
		BehaviorTimingMark();		
	
	protected:
};

class BehaviorBlock : public nle::Block
{
	public:
		BehaviorBlock();
	
	
	protected:
		BML::BmlRequest* m_behavior;;
		
};

class BehaviorTrack : public nle::Track
{
	public:
		BehaviorTrack();
};

class RequestMark : public nle::Mark
{
	public:
		RequestMark();		
};

class RequestTimingMark : public nle::Mark
{
	public:
		RequestTimingMark();		
	
};

class RequestBlock : public nle::Block
{
	public:
		RequestBlock();
	
		
};

class EventBlock : public nle::Block
{
	public:
		EventBlock();
	
		
};

class MotionBlock : public nle::Block
{
	public:
		MotionBlock();
	
		
};

class NodBlock : public nle::Block
{
	public:
		NodBlock();
	
		
};


class RequestTrack : public nle::Track
{
	public:
		RequestTrack();
};


#endif