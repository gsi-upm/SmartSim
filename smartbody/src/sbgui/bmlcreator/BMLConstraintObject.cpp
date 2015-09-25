#include "BMLConstraintObject.h"
#include <sb/SBAttribute.h>

BMLConstraintObject::BMLConstraintObject() : BMLObject()
{
	setName("constraint");

	createStringAttribute("target", "", true, "Basic", 10, false, false, false, "target pawn/joint whose the positional or rotational values will be used as constraints");
	createStringAttribute("sbm:handle", "", true, "Basic", 20, false, false, false, "Handle of this constraint instance, can be reused during later constraint commands.");
	createStringAttribute("sbm:root", "", true, "Basic", 30, false, false, false, "the root joint for the current character. by default it is the based joint.");
	createStringAttribute("effector", "", true, "Basic", 40, false, false, false, "the end effector where the constraint will be enforced through optimization");
	createStringAttribute("sbm:effector-root", "", true, "Basic", 50, false, false, false, "the influence root of this end effector. anything higher than this root will not be affected by optimization");

	createDoubleAttribute("sbm:fade-in",-1.0,true,"Basic", 60, false, false, false,"the time for the constraint to blend in. this option is ignored if set to negative.");
	createDoubleAttribute("sbm:fade-out",-1.0,true,"Basic", 70, false, false, false,"the time for the constraint to blend out. this option is ignored if set to negative.");

	SmartBody::StringAttribute* constraintTypeAttr = createStringAttribute("sbm:constraint-type","pos",true,"Basic", 80, false, false, false,"the constraint type to be enforced. it can be positional or rotational constraint.");
	std::vector<std::string> constraintTypes;
	constraintTypes.push_back("pos");
	constraintTypes.push_back("rot");	
	constraintTypeAttr->setValidValues(constraintTypes);

	createDoubleAttribute("pos-x",0.0,true,"Basic", 90, false, false, false,"the x positional offset added on top of positional constraint");
	createDoubleAttribute("pos-y",0.0,true,"Basic", 100, false, false, false,"the y positional offset added on top of positional constraint");
	createDoubleAttribute("pos-z",0.0,true,"Basic", 110, false, false, false,"the z positional offset added on top of positional constraint");

	createDoubleAttribute("rot-x",0.0,true,"Basic", 120, false, false, false,"the x rotational offset added on top of positional constraint");
	createDoubleAttribute("rot-y",0.0,true,"Basic", 130, false, false, false,"the y rotational offset added on top of positional constraint");
	createDoubleAttribute("rot-z",0.0,true,"Basic", 140, false, false, false,"the z rotational offset added on top of positional constraint");

	createStringAttribute("start", "", true, "Basic", 150, false, false, false);
	createStringAttribute("ready", "", true, "Basic", 160, false, false, false);
}

BMLConstraintObject::~BMLConstraintObject()
{
}

void BMLConstraintObject::notify(SmartBody::SBSubject* subject)
{
	BMLObject::notify(subject);
}