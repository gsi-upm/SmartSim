#include "AttributeEditor.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBPawn.h>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <sb/SBService.h>
#include <sb/SBController.h>
#include <sb/SBJointMap.h>
#include <sb/SBGestureMap.h>
#include <sb/SBFaceDefinition.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sb/SBEvent.h>

AttributeEditor::AttributeEditor(int x, int y, int w, int h, char* name) : Fl_Group(x, y, w, h, name), SmartBody::SBObserver(), SBWindowListener(), SelectionListener()
{
	_currentSelection = "";
	_currentWidget = NULL;
	_dirty = true;
}

AttributeEditor::~AttributeEditor()
{
}

void AttributeEditor::OnSelect(const std::string& value)
{
	if (_currentSelection != value)
	{
		removeCurrentWidget();
	}
	_currentSelection = value;
	_dirty = true;
	this->redraw();
}

void AttributeEditor::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	
}

void AttributeEditor::OnCharacterDelete( const std::string & name )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBCharacter* character = scene->getCharacter(name);
	std::string id = scene->getStringFromObject(character);
	if (id == _currentSelection)
	{
		OnSelect("");
	}
}

void AttributeEditor::OnCharacterUpdate( const std::string & name )
{
}

void AttributeEditor::OnPawnCreate( const std::string & name )
{

}

void AttributeEditor::OnPawnDelete( const std::string & name )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBPawn* pawn = scene->getPawn(name);
	std::string id = scene->getStringFromObject(pawn);
	if (id == _currentSelection)
	{
		OnSelect("");
	}
}

void AttributeEditor::OnObjectCreate( SmartBody::SBObject* object )
{
}

void AttributeEditor::OnObjectDelete( SmartBody::SBObject* object )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::string id = scene->getStringFromObject(object);
	if (id == _currentSelection)
	{
		OnSelect("");
	}
}

void AttributeEditor::OnSimulationStart()
{
	OnSelect("");
	_dirty = true;
}

void AttributeEditor::draw()
{
	if (_dirty)
	{
		updateGUI();
	}
	// make sure the object still exists
	if (_currentSelection != "")
	{
		SmartBody::SBObject* object = SmartBody::SBScene::getScene()->getObjectFromString(_currentSelection);
		if (!object)
		{
			OnSelect("");
		}
	}
	Fl_Group::draw();
}


void AttributeEditor::updateGUI()
{
	if (_currentSelection != "")
	{
		removeCurrentWidget();
		_currentWidget = createInfoWidget(this->x(),this->y(),this->w(),this->h(), _currentSelection);
		if (_currentWidget)
		{
			this->add(_currentWidget);
			this->show();
			_currentWidget->show();
		}
	}
	else
	{
		removeCurrentWidget();
	}
	_dirty = false;
}

TreeItemInfoWidget* AttributeEditor::createInfoWidget( int x, int y, int w, int h, const std::string& name)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBAssetManager* assetManager = scene->getAssetManager();
	SmartBody::SBAnimationBlendManager* blendManager = scene->getBlendManager();

	SmartBody::SBObject* object = scene->getObjectFromString(name);
	TreeItemInfoWidget* widget = NULL;
	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(object);
	if (character)
	{
		widget = new AttributeItemWidget(character,x,y,w,h,strdup(object->getName().c_str()),NULL);
		return widget;
	}

	SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(object);
	if (pawn)
	{
		widget = new AttributeItemWidget(pawn,x,y,w,h,strdup(object->getName().c_str()),NULL);
		return widget;
	}

	SmartBody::SBSkeleton* skeleton = dynamic_cast<SmartBody::SBSkeleton*>(object);
	if (skeleton)
	{
		std::string pawnName = "";
		SmartBody::SBPawn* skelPawn = skeleton->getPawn();
		if (skelPawn)
			pawnName = skelPawn->getName();
		widget = new SkeletonItemInfoWidget(pawnName, x,y,w,h,strdup(object->getName().c_str()), NULL);
		return widget;
	}

	SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(object);
	if (motion)
	{
		widget = new MotionItemInfoWidget(x,y,w,h,strdup(object->getName().c_str()), this);
		return widget;
	}

	SmartBody::SBScene* objscene = dynamic_cast<SmartBody::SBScene*>(object);
	if (objscene)
	{
		widget = new AttributeItemWidget(objscene,x,y,w,h,strdup("scene"),NULL);
		return widget;
	}
	SmartBody::SBService* service = dynamic_cast<SmartBody::SBService*>(object);
	if (service)
	{
		widget = new AttributeItemWidget(service,x,y,w,h,strdup(object->getName().c_str()),NULL);
		return widget;
	}

	DeformableMesh* mesh = dynamic_cast<DeformableMesh*>(object);
	if (mesh)
	{
		widget = new AttributeItemWidget(service,x,y,w,h,strdup(object->getName().c_str()),NULL);
		return widget;
	}

	SmartBody::SBController* controller = dynamic_cast<SmartBody::SBController*>(object);
	if (controller)
	{
		widget = new AttributeItemWidget(controller,x,y,w,h,strdup(object->getName().c_str()),NULL);
		return widget;
	}

	SmartBody::SBJointMap* jointMap = dynamic_cast<SmartBody::SBJointMap*>(object);
	if (jointMap)
	{
		// no attributes for joint map
		return NULL;
	}

	SmartBody::SBGestureMap* gestureMap = dynamic_cast<SmartBody::SBGestureMap*>(object);
	if (gestureMap)
	{
		widget = new TreeItemInfoWidget(x,y,w,h,strdup(object->getName().c_str()));
		return widget;
	}

	SmartBody::SBEventHandler* eventHandler = dynamic_cast<SmartBody::SBEventHandler*>(object);
	if (eventHandler)
	{
		widget = new AttributeItemWidget(eventHandler, x,y,w,h,strdup(object->getName().c_str()));
		return widget;
	}

	SmartBody::SBAnimationBlend* blend = dynamic_cast<SmartBody::SBAnimationBlend*>(object);
	if (blend)
	{
		widget = new AnimationBlendInfoWidget(blend, x, y, w, h, strdup(object->getName().c_str()), this);
		return widget;
	}

	SmartBody::SBAnimationTransition* transition = dynamic_cast<SmartBody::SBAnimationTransition*>(object);
	if (transition)
	{
		widget = new BlendTransitionInfoWidget(transition, x, y, w, h, strdup(object->getName().c_str()), this);
		return widget;
	}

	SmartBody::SBScript* script = dynamic_cast<SmartBody::SBScript*>(object);
	if (script)
	{
		widget =  new AttributeItemWidget(script, x,y,w,h,strdup(object->getName().c_str()));
		return widget;
	}

	return NULL;
	/*

	
	else if (itemType == ITEM_SEQ_PATH || itemType == ITEM_ME_PATH || itemType == ITEM_AUDIO_PATH || itemType == ITEM_MESH_PATH)
	{
		widget = new PathItemInfoWidget(x,y,w,h,name,treeItem,itemType, this);
	}
	else if (itemType == ITEM_SEQ_FILES)
	{
		widget = new SeqItemInfoWidget(x,y,w,h,name,treeItem,itemType, this);
	}
	
	else if (itemType == ITEM_PHYSICS)
	{
		SmartBody::SBPhysicsSim* phySim = SmartBody::SBPhysicsSim::getPhysicsEngine();
		std::string itemName = treeItem->label();
		std::string parentName = treeItem->parent()->label();
		SmartBody::SBPhysicsCharacter* phyChar = phySim->getPhysicsCharacter(itemName);
		SmartBody::SBPhysicsCharacter* phyParent = phySim->getPhysicsCharacter(parentName);
		SmartBody::SBPhysicsObj*    phyBody = phySim->getPhysicsPawn(itemName);
		SmartBody::SBObject* phyObj = phySim;		
		SmartBody::SBObject* phyObj2 = NULL;

		static std::string name1 = "PHYSICS JOINT";
		static std::string name2 = "RIGID BODY";	
		std::vector<std::string> phyObjNameList;
		std::vector<SmartBody::SBObject*> phyObjList;
		if (phyChar)
		{
			phyObj = phyChar;
		}
		else if (phyParent)
		{
			SmartBody::SBPhysicsJoint* phyJoint = phyParent->getPhyJoint(itemName);
			phyObj = phyJoint;
			phyObj2 = phyJoint->getChildObj();
			phyObjNameList.push_back(name1);
			phyObjNameList.push_back(name2);
			phyObjList.push_back(phyObj);
			phyObjList.push_back(phyObj2);
		}
		else if (phyBody)
		{
			phyObj = phyBody;			
		}

		if (phyObjList.size() > 0)
		{
			//widget = new DoubleAttributeItemWidget(phyObj,phyObj2,x,y,w,h,h/2,name1.c_str(),name2.c_str(),treeItem,itemType,this);
			widget = new MultiAttributeItemWidget(phyObjList,x,y,w,h,h/2,name,phyObjNameList,treeItem,itemType,this);
		}
		else if (phyObj)
			widget = new AttributeItemWidget(phyObj,x,y,w,h,name,treeItem,itemType,this);
	}
	
	else if (itemType == ITEM_NVBG)
	{
		SmartBody::SBCharacter* curChar = scene->getCharacter(treeItem->parent()->label());
		widget = new AttributeItemWidget(curChar->getNvbg(),x,y,w,h,name,treeItem,itemType,this);
	}
	else
	{
		widget = new TreeItemInfoWidget(x,y,w,h,name,treeItem,itemType);
	}

	return widget;
		*/
return NULL;
}


void AttributeEditor::removeCurrentWidget()
{
	if (_currentWidget)
	{
		int numChildren = this->children();
		std::vector<Fl_Widget*> widgets;
		for (int c = 0; c < numChildren; c++)
		{
			widgets.push_back(this->child(c));
		}
		for (size_t w = 0; w < widgets.size(); w++)
		{
			this->remove(widgets[w]);
			// need to delete this widget, but doing so causes 
			// crashes in FLTK
		}

		_currentWidget = NULL;
	}
}

void AttributeEditor::show()
{
	SBWindowListener::windowShow();
	Fl_Group::show();
}

void AttributeEditor::hide()
{
	SBWindowListener::windowHide();
	Fl_Group::hide();
}