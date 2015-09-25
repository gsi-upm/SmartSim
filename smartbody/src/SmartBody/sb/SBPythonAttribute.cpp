#include "vhcl.h"
#include "SBPython.h"
#include "SBPythonClass.h"
#include "controllers/me_ct_scheduler2.h"
#include "controllers/me_ct_gaze.h"
#include "controllers/me_ct_eyelid.h"
#include "controllers/me_ct_curve_writer.hpp"
#include "SBFaceDefinition.h"
#include <sb/nvbg.h>
#include "SBBehavior.h"
#include <sb/SBMotion.h>
#include <sb/SBParseNode.h>

#include <sb/SBScene.h>
#include <sb/SBScript.h>
#include <sb/SBService.h>
#include <sb/SBServiceManager.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBAnimationState.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationTransitionRule.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBSteerManager.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBReach.h>
#include <sb/SBReachManager.h>
#include <sb/SBGestureMap.h>
#include <sb/SBGestureMapManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBParser.h>
#include <sb/SBBoneBusManager.h>
#include <sb/SBCollisionManager.h>
#include <sb/SBSteerAgent.h>
#include <sb/SBPhoneme.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBRetarget.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBEvent.h>
#include <sb/SBSceneListener.h>
#include <sr/sr_box.h>
#include <sr/sr_camera.h>
#include <stdlib.h>
#include <sbm/GenericViewer.h>
#include <controllers/me_ct_motion.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#ifndef SB_NO_PYTHON
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> 
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/args.hpp>
#endif

#include "SBPythonInternal.h"


typedef std::map<std::string,SrQuat> QuatMap;
typedef std::map<std::string,SrVec> VecMap;
typedef std::map<std::string, std::string> StringMap;


#ifndef SB_NO_PYTHON


namespace SmartBody
{

void pythonFuncsAttribute()
{

	boost::python::class_<SBAttributeInfo>("SBAttributeInfo")
		.def("getPriority", &SBAttributeInfo::getPriority, "Returns the priority of the attribute. Used for display purposes.")
		.def("setPriority", &SBAttributeInfo::setPriority, "Sets the priority of the attribute. Used for display purposes.")
		.def("getReadOnly", &SBAttributeInfo::getReadOnly, "Determines if the attribute is read-only and cannot be changed.")
		.def("setReadOnly", &SBAttributeInfo::setReadOnly, "Sets the read-only status of the attribute. Attributes marked read-only cannot have their value's changed.")
		.def("getHidden", &SBAttributeInfo::getHidden, "Determines if the attribute is hidden from view.")
		.def("setHidden", &SBAttributeInfo::setHidden, "Sets the hidden status of the attribute. Hidden attributes typically aren't visible to the user.")
		.def("setDescription", &SBAttributeInfo::setDescription, "Sets the description or help text associated with this attribute.")
		.def("getDescription", &SBAttributeInfo::getDescription, "Gets the description or help text associated with this attribute.")
	;

	boost::python::class_<SBAttribute, boost::python::bases<SBSubject> >("SBAttribute")
		.def("getName", &SBAttribute::getName, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns an attribute of a given name")
		.def("getAttributeInfo", &SBAttribute::getAttributeInfo, boost::python::return_value_policy<boost::python::reference_existing_object>(), "Returns the information associated with this attribute.")
	;

	boost::python::class_<ActionAttribute, boost::python::bases<SBAttribute> >("ActionAttribute")
		.def("setValue", &ActionAttribute::setValue, "Activates action attribute.")
	;

	boost::python::class_<BoolAttribute, boost::python::bases<SBAttribute> >("BoolAttribute")
		.def("getValue", &BoolAttribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the bool attribute.")
		.def("setValue", &BoolAttribute::setValue, "Sets the value of the boolean attribute.")
		.def("getDefaultValue", &BoolAttribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the bool attribute.")
		.def("setDefaultValue", &BoolAttribute::setDefaultValue, "Sets the default value of the boolean attribute.")
		.def("setValueFast", &BoolAttribute::setValueFast, "Sets the value of the boolean attribute without notifying observers.")
	;

	boost::python::class_<StringAttribute, boost::python::bases<SBAttribute> >("StringAttribute")
		.def("getValue", &StringAttribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the string attribute.")
		.def("setValue", &StringAttribute::setValue, "Sets the value of the string attribute.")
		.def("getDefaultValue", &StringAttribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the string attribute.")
		.def("setDefaultValue", &StringAttribute::setDefaultValue, "Sets the default value of the string attribute.")
		.def("setValueFast", &StringAttribute::setValueFast, "Sets the value of the string attribute without notifying observers.")
		.def("setValidValues", &StringAttribute::setValidValues, "Sets the valid values of the string attribute.")
		.def("getValidValues", &StringAttribute::getValidValues, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the valid values of the string attribute.")
	;

	boost::python::class_<IntAttribute, boost::python::bases<SBAttribute> >("IntAttribute")
		.def("getValue", &IntAttribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the int attribute.")
		.def("setValue", &IntAttribute::setValue, "Sets the value of the integer attribute.")
		.def("getDefaultValue", &IntAttribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the string attribute.")
		.def("setDefaultValue", &IntAttribute::setDefaultValue, "Sets the default value of the string attribute.")
		.def("setValueFast", &IntAttribute::setValueFast, "Sets the value of the integer attribute without notifying observers.")
	;

	boost::python::class_<DoubleAttribute, boost::python::bases<SBAttribute> >("DoubleAttribute")
		.def("getValue", &DoubleAttribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the double attribute.")
		.def("setValue", &DoubleAttribute::setValue, "Sets the value of the double attribute.")
		.def("getDefaultValue", &DoubleAttribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the double attribute.")
		.def("setDefaultValue", &DoubleAttribute::setDefaultValue, "Sets the default value of the double attribute.")
		.def("setValueFast", &DoubleAttribute::setValueFast, "Sets the value of the double attribute without notifying observers.")
	;

	boost::python::class_<Vec3Attribute, boost::python::bases<SBAttribute> >("Vec3Attribute")
		.def("getValue", &Vec3Attribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the vec3 attribute.")
		.def("setValue", &Vec3Attribute::setValue, "Sets the value of the vec3 attribute.")
		.def("getDefaultValue", &Vec3Attribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the vec3 attribute.")
		.def("setDefaultValue", &Vec3Attribute::setDefaultValue, "Sets the default value of the vec3 attribute.")
		.def("setValueFast", &Vec3Attribute::setValueFast, "Sets the value of the vec3 attribute without notifying observers.")
	;

	boost::python::class_<MatrixAttribute, boost::python::bases<SBAttribute> >("MatrixAttribute")
		.def("getValue", &MatrixAttribute::getValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the value of the matrix attribute.")
		.def("setValue", &MatrixAttribute::setValue, "Sets the value of the matrix attribute.")
		.def("getDefaultValue", &MatrixAttribute::getDefaultValue, boost::python::return_value_policy<boost::python::return_by_value>(), "Returns the default value of the matrix attribute.")
		.def("setDefaultValue", &MatrixAttribute::setDefaultValue, "Sets the default value of the matrix attribute.")
		.def("setValueFast", &MatrixAttribute::setValueFast, "Sets the value of the matrix attribute.")
	;

}
}


#endif
