
#include "vhcl.h"
#include "TreeItemInfoWidget.h"

#include <FL/Fl_Float_Input.H>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include <sb/SBEvent.h>
#include <sb/SBAnimationStateManager.h>

#include "ResourceWindow.h"
#include "channelbufferviewer/GlChartViewArchive.hpp"
#include "RootWindow.h"


const int Pad = 10;


/************************************************************************/
/* Tree Item Info Widget                                                */
/************************************************************************/

TreeItemInfoWidget::TreeItemInfoWidget( int x, int y, int w, int h, const char* name) : Fl_Group(x,y,w,h)
{	
	this->begin();
// 	Fl_Input* info = new Fl_Input(Pad*5+x,Pad+y,120,20,"Type");
// 	info->value(name);
// 	info->type(FL_NORMAL_OUTPUT);
// 	Fl_Input* itemInfo = new Fl_Input(Pad*5+x,Pad*4+y,120,20,"Name");
// 	itemInfo->value(inputItem->label());
// 	itemInfo->type(FL_NORMAL_OUTPUT);
	this->end();
}

/************************************************************************/
/* Joint Info Object                                                    */
/************************************************************************/

JointInfoObject::JointInfoObject()
{
	itemSkeleton = NULL;
	jointName = "";
}

JointInfoObject::~JointInfoObject()
{

}

void JointInfoObject::notify( SBSubject* subject )
{
	if (!itemSkeleton)
		return;	

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBSkeleton* templateSkel = scene->getSkeleton(itemSkeleton->getName());

	SmartBody::SBJoint* templateSelectJoint = templateSkel->getJointByName(jointName);
	SmartBody::SBJoint* selectJoint = itemSkeleton->getJointByName(jointName);
	if (!selectJoint || !templateSelectJoint)
		return;

	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		SrVec offset = selectJoint->offset();
		SmartBody::DoubleAttribute* offsetAttr = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);		
		if (!offsetAttr)
			return;

		if (attribute->getName() == "offset X")
		{
			offset.x = offsetAttr->getValue();
		}
		else if (attribute->getName() == "offset Y")
		{
			offset.y = offsetAttr->getValue();
		}
		else if (attribute->getName() == "offset Z")
		{
			offset.z = offsetAttr->getValue();
		}	
		selectJoint->setOffset(offset);
		templateSelectJoint->setOffset(offset);
		itemSkeleton->update();
	}
}

void JointInfoObject::setSkeleton( SmartBody::SBSkeleton* skel )
{
	itemSkeleton = skel;
	jointName = "";
}

void JointInfoObject::setJointName( const std::string& jname )
{
	jointName = jname;
}
/************************************************************************/
/* Skeleton Info                                                        */
/************************************************************************/

SkeletonItemInfoWidget::SkeletonItemInfoWidget( const std::string& characterName, int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow) : TreeItemInfoWidget(x,y,w,h,name)
{
	jointInfoObject = new JointInfoObject();
	this->begin();
	skeletonTree = new Fl_TreeHorizontal(Pad*2+x,Pad*2+y,w-30,100);//new Fl_Tree(10,10,w - 300, h - 30);			
	skeletonTree->callback(treeCallBack,this);	
	attrWindow = new AttributeWindow(jointInfoObject,Pad*2+x,Pad*2+y+120,w - 30,h -(Pad*2+130),"Joint Attributes");
	attrWindow->setOffset(150);
	attrWindow->begin();
	attrWindow->end();
	this->end();	
	this->resizable(skeletonTree);
	itemSkeleton = NULL;	
	skeletonName = name;
	charName = characterName;
	updateWidget();
}

void SkeletonItemInfoWidget::updateWidget()
{
	skeletonTree->clear_children(skeletonTree->root());

	SmartBody::SBCharacter* sbmChar = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (sbmChar)
	{
		itemSkeleton = sbmChar->getSkeleton();
	}
	else
	{
		SmartBody::SBSkeleton * skeleton = SmartBody::SBScene::getScene()->getSkeleton(skeletonName);
		if (!skeleton)
			return; // skeleton is lost, no update

		//LOG("skelName = %s\n",skelName.c_str());
		itemSkeleton = skeleton;
	}
	jointInfoObject->setSkeleton(itemSkeleton);
	updateSkeletonTree(skeletonTree->root(), itemSkeleton);
}

void SkeletonItemInfoWidget::updateJointAttributes(std::string jointName)
{
	if (!itemSkeleton)
		return;
	SkJoint* curJoint = itemSkeleton->search_joint(jointName.c_str());		
	if (!curJoint)
		return;
	std::string posName[3] = { "pos X", "pos Y", "pos Z" };
	std::string offsetName[3] = { "offset X", "offset Y", "offset Z" };
	std::string quatName[4] = { "quat X", "quat Y", "quat Z", "quat W"};
	jointInfoObject->clearAttributes();	
	attrWindow->cleanUpWidgets();

	SrVec offset = curJoint->offset();
	//jointInfoObject->createVec3Attribute("offset",offset.x,offset.y,offset.z,true, "Basic", 20, false, false, false, "?");
	//jointInfoObject->setVec3Attribute("offset",offset.x,offset.y,offset.z);
	for (int i=0;i<3;i++)
	{
		jointInfoObject->createDoubleAttribute(offsetName[i],offset[i], true, "Basic",10+i , false, false, false, "?");		
		//attrWindow->notify(jointInfoObject->getAttribute(offsetName[i]));
	}
	//attrWindow->notify(jointInfoObject);

	for (int i=0;i<3;i++)
	{
		if (!curJoint->pos()->frozen(i))
		{			
			jointInfoObject->createDoubleAttribute(posName[i],curJoint->pos()->value(i), true, "Basic", 20+i, true, false, false, "?");
			//attrWindow->notify(jointInfoObject->getAttribute(posName[i]));
		}
	}

	SrQuat q = curJoint->quat()->value();	
	jointInfoObject->createDoubleAttribute(quatName[0],q.x,true, "Basic", 31, true, false, false, "?");
	jointInfoObject->createDoubleAttribute(quatName[1],q.y,true, "Basic", 32, true, false, false, "?");
	jointInfoObject->createDoubleAttribute(quatName[2],q.z,true, "Basic", 33, true, false, false, "?");
	jointInfoObject->createDoubleAttribute(quatName[3],q.w,true, "Basic", 34, true, false, false, "?");
	//for (int i=0;i<4;i++)
	//	attrWindow->notify(jointInfoObject->getAttribute(quatName[i]));
	jointInfoObject->setJointName(jointName);

	//attrWindow->reorderAttributes();	
	this->attrWindow->setDirty(true);	
	this->attrWindow->redraw();
	this->redraw();
}

void SkeletonItemInfoWidget::updateSkeletonTree( Fl_Tree_Item* root, SmartBody::SBSkeleton* skel )
{
	SkJoint* skelRoot = skel->root();	
	if (!root)
		return;
	if (!skelRoot)
		return;
	root->label(skelRoot->jointName().c_str());	
	for (int i=0;i<skelRoot->num_children();i++)
	{
		updateJointTree(root, skelRoot->child(i));
	}
}

void SkeletonItemInfoWidget::updateJointTree( Fl_Tree_Item* root, SkJoint* node )
{	
	//skeletonTree->sortorder(FL_TREE_SORT_ASCENDING);	

	Fl_Tree_Item* treeItem = skeletonTree->add(root,node->jointName().c_str());

	std::string posName[3] = { "pos X", "pos Y", "pos Z" };
	skeletonTree->sortorder(FL_TREE_SORT_NONE);
// 	for (int i=0;i<3;i++)
// 	{
// 		if (!node->pos()->frozen(i))
// 		{			
// 			skeletonTree->add(treeItem,posName[i].c_str());
// 		}
// 	}	
// 	if (node->rot_type() == SkJoint::TypeQuat)
// 		skeletonTree->add(treeItem,"quat");
	
	for (int i=0;i<node->num_children();i++)
	{
		SkJoint* child = node->child(i);
		updateJointTree(treeItem,child);
	}
}

void SkeletonItemInfoWidget::treeCallBack( Fl_Widget* widget, void* data )
{
	Fl_Tree      *tree = (Fl_Tree*)widget;
	Fl_Tree_Item *item = (Fl_Tree_Item*)tree->callback_item();	// get selected item	
	SkeletonItemInfoWidget* itemInfoWidget = (SkeletonItemInfoWidget*)data;

	if (tree->callback_reason() == FL_TREE_REASON_SELECTED && item && itemInfoWidget)
	{
		itemInfoWidget->updateJointAttributes(item->label());
	}
}

/************************************************************************/
/*  Motion Item Info Widget                                             */
/************************************************************************/

MotionItemInfoWidget::MotionItemInfoWidget( int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow) : TreeItemInfoWidget(x,y,w,h,name)
{
	int channelBrowserHeight = (int)(h*0.8);
	channelInfoObject = new TreeInfoObject();
	this->begin();
	channelBrowser = new Fl_Hold_Browser(Pad*2+x,Pad*2+y,w-30,h-channelBrowserHeight,"Channels");//new Fl_Tree(10,10,w - 300, h - 30);			
	channelBrowser->callback(browserCallBack,this);	
	frameSlider = new Fl_Value_Slider(Pad*2+x,Pad*2+y+h-channelBrowserHeight+25,w-30,20,"Frames");
	frameSlider->type(FL_HORIZONTAL);
	frameSlider->callback(sliderCallBack,this);
	attrWindow = new AttributeWindow(channelInfoObject,Pad*2+x,Pad*2+y+h-channelBrowserHeight + 65 ,w-30, channelBrowserHeight - 100,"");
	attrWindow->setOffset(150);
	attrWindow->begin();
	attrWindow->end();
	this->end();	
	this->resizable(channelBrowser);	
	motionName = name;
	updateWidget();
}

void MotionItemInfoWidget::updateWidget()
{
	channelBrowser->clear();

	SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName);
	if (!motion)
		return;
	frameSlider->bounds(1, motion->frames());
	frameSlider->step(1.0);
	motionFrame = 0;
	channelIndex = -1;
	SkChannelArray& channels = motion->channels();
	for (int i = 0; i < channels.size(); i++)
	{
		const std::string& chanName = channels.name(i).c_str();
		std::string typeName = channels[i].type_name();
		channelBrowser->add((chanName+"."+typeName).c_str());
	}
}

void MotionItemInfoWidget::browserCallBack( Fl_Widget* widget, void* data )
{
	Fl_Hold_Browser* browser = dynamic_cast<Fl_Hold_Browser*>(widget);
	MotionItemInfoWidget* itemInfoWidget = (MotionItemInfoWidget*)data;
	if (browser && itemInfoWidget)
	{
		itemInfoWidget->setChannelIndex(browser->value()-1);		
		itemInfoWidget->updateChannelAttributes();
	}
}

void MotionItemInfoWidget::sliderCallBack( Fl_Widget* widget, void* data )
{
	Fl_Value_Slider* slider = dynamic_cast<Fl_Value_Slider*>(widget);
	MotionItemInfoWidget* itemInfoWidget = (MotionItemInfoWidget*)data;
	if (slider && itemInfoWidget)
	{
		itemInfoWidget->setMotionFrame((int)slider->value()-1);	
		itemInfoWidget->updateChannelAttributes();
	}
}

void MotionItemInfoWidget::updateChannelAttributes()
{
	SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName);
	if (!motion)
		return;
	SkChannelArray& channels = motion->channels();
	if (motionFrame < 0 || motionFrame >= motion->frames() || channelIndex < 0 || channelIndex >= channels.size())
		return;

	channelInfoObject->clearAttributes();	
	attrWindow->cleanUpWidgets();
	SkChannel& chan = channels[channelIndex];
	const std::string& chanName = channels.name(channelIndex);
	std::string typeName = channels[channelIndex].type_name();	
	int floatIdx = channels.float_position(channelIndex);
	float* buffer = motion->posture(motionFrame);
	std::string tag[4] = {"W", "X", "Y", "Z"};
	for (int i=0;i<chan.size();i++)
	{
		std::string attrName = (chanName+"."+typeName);
		if (chan.size() > 1)
			attrName += tag[i];			
		SmartBody::DoubleAttribute* attr = channelInfoObject->createDoubleAttribute(attrName.c_str(),buffer[floatIdx+i],true,"Basic",10+i,true,false,false,"?");
		//attrWindow->notify(attr);
	}
	if (chan.type == SkChannel::Quat)
	{
		SrQuat quat = SrQuat(&buffer[floatIdx]);
		
		//SrVec euler = GlChartViewSeries::GetEulerFromQuaternion(quat);
		//gwiz::euler_t(p,h,r);	
		gwiz::euler_t eu = gwiz::euler_t(gwiz::quat_t(quat.w, quat.x,quat.y,quat.z));
		std::string attrName = (chanName+"."+"euler");
		SrVec euler = SrVec((float)eu.p(),(float)eu.h(),(float)eu.r());
		for (int k=0;k<3;k++)
		{			
			SmartBody::DoubleAttribute* attr = channelInfoObject->createDoubleAttribute((attrName+tag[k+1]).c_str(),euler[k],true,"Basic",20+k,true,false,false,"?");
		}
	}

	attrWindow->reorderAttributes();	
	this->attrWindow->setDirty(true);	
	this->attrWindow->redraw();
	this->redraw();
}
/************************************************************************/
/* Path Info Widget                                                     */
/************************************************************************/
PathItemInfoWidget::PathItemInfoWidget( int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow) : TreeItemInfoWidget(x,y,w,h,name)
{
	observer = observerWindow;
	this->begin();
	pathName = name;
	
	Fl_Button* dirButton = new Fl_Button( x + 20 , y + 10 , 100, 20, "Add Path");
	dirButton->callback(addDirectoryCallback, this);
	this->end();	
}

void PathItemInfoWidget::addDirectoryCallback( Fl_Widget* widget, void* data )
{
	PathItemInfoWidget* pathInfoWidget = (PathItemInfoWidget*)data;
	pathInfoWidget->addDirectory(NULL);	
}

void PathItemInfoWidget::addDirectory( const char* dirName )
{
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	
	std::string str = "Add path for " + pathName;
	std::string directory = BaseWindow::chooseDirectory(str, mediaPath);
	if (directory == "")
		return;
	
	char relativePath[256];
	fl_filename_relative(relativePath, sizeof(relativePath), directory.c_str());

	std::string paraType = "";//getTypeParameter(itemType);

	SmartBody::SBScene::getScene()->addAssetPath(paraType, relativePath);
	updateWidget();
	observer->notify(NULL);



}

std::string PathItemInfoWidget::getTypeParameter( int type )
{
	std::string paraType = "script";
/*
	switch (type)
	{
	case ResourceWindow::ITEM_SEQ_PATH :
		paraType = "script";
		break;
	case ResourceWindow::ITEM_AUDIO_PATH :
		paraType = "audio";
		break;
	case ResourceWindow::ITEM_ME_PATH :
		paraType = "motion";
		break;
	case ResourceWindow::ITEM_MESH_PATH :
		paraType = "mesh";
		break;
	}
*/
	return paraType;
}

/************************************************************************/
/* Seq Item Info Widget                                                 */
/************************************************************************/
SeqItemInfoWidget::SeqItemInfoWidget( int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow) : TreeItemInfoWidget(x,y,w,h,name)
{
	seqFilename = name;
	this->begin();
	//runSeqButton = new Fl_Button()
	runSeqButton = new Fl_Button( Pad*2+x,Pad*2+y, 100, 20, "Run Seq");
	runSeqButton->callback(runSeqCallback,this);
	editSeqButton = new Fl_Button( Pad*2+x + 130,Pad*2+y, 100, 20, "Edit Seq");
	editSeqButton->callback(editSeqCallback,this);
	textDisplay = new Fl_Text_Display(Pad*2+x,Pad*2+y + 30,w-40,h-70);		
	textDisplay->color(FL_GRAY);
	textDisplay->textcolor(FL_BLACK);
	textBuffer = new Fl_Text_Buffer();
	textDisplay->buffer(textBuffer);
	this->end();
	updateWidget();
}

void SeqItemInfoWidget::updateWidget()
{
	// clean up text buffer
	textBuffer->select(0, textBuffer->length());
	textBuffer->remove_selection();
	
	std::string fullSeqPath;
	/* 
	FILE* fp = mcu.open_sequence_file(seqFilename.c_str(),fullSeqPath);
	//LOG("seq file name = %s, full path = %s\n",seqFilename.c_str(),fullSeqPath.c_str());
	if (fp)
	{
		textBuffer->loadfile(fullSeqPath.c_str());
		seqFullPathName = fullSeqPath;
		fclose(fp);
	}
	*/
	textDisplay->redraw();
	//textBuffer->loadfile()
}

void SeqItemInfoWidget::runSeqCallback( Fl_Widget* widget, void* data )
{
	SeqItemInfoWidget* seqInfoWidget = (SeqItemInfoWidget*)data;
	if (seqInfoWidget)
	{
		std::string seqName = seqInfoWidget->getSeqFile();
		std::string seqCmd = "seq ";
		seqCmd += seqName;
		LOG("seq cmd = %s\n", seqCmd.c_str());
		SmartBody::SBScene::getScene()->command(seqCmd);
	}
}

void SeqItemInfoWidget::editSeqCallback( Fl_Widget* widget, void* data )
{
	SeqItemInfoWidget* seqInfoWidget = (SeqItemInfoWidget*)data;
	if (seqInfoWidget)
	{
		std::string seqFullPath = seqInfoWidget->getSeqFullPathName();
#ifdef WIN32 // forking a process is system specific		
		STARTUPINFOA siStartupInfo; 
		PROCESS_INFORMATION piProcessInfo; 
		memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
		memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
		siStartupInfo.cb = sizeof(siStartupInfo); 		
		std::string cmd = "notepad.exe ";		
		BOOL success = CreateProcess(NULL,const_cast<LPSTR>((cmd+seqFullPath).c_str()),0, 0, false, 
									 CREATE_DEFAULT_ERROR_MODE, 0, 0,&siStartupInfo, &piProcessInfo);		
#else
		pid_t child_pid;
		child_pid = fork();
		if (child_pid == 0)
		{
		  char* pathChar = const_cast<char*>(seqFullPath.c_str());
		  
		  char* guiEditorName = getenv ("VISUAL");		  
		  char* argList[3];
		  if (guiEditorName)
			  argList[0] = guiEditorName;
		  else
			  argList[0] = "gedit";
		  argList[1] = pathChar;
		  argList[2] = NULL;
		  execvp("gedit",argList);
		}		
#endif
	}
}
/************************************************************************/
/* Event Item Info Widget                                                 */
/************************************************************************/

EventItemInfoWidget::EventItemInfoWidget( int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow)
: TreeItemInfoWidget(x,y,w,h,name)
{
	eventInfoObject = new TreeInfoObject();
	eventName = name;
	eventInfoObject->setName(eventName);
	eventInfoObject->createStringAttribute("EventType","",true,"Basic",10,false,false,false,"?");
	eventInfoObject->createStringAttribute("Action","",true,"Basic",20,false,false,false,"?");
	updateWidget();

	SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();
	std::map<std::string, SmartBody::SBEventHandler*>& eventMap = manager->getEventHandlers();		
	eventInfoObject->registerObserver(this);
	this->begin();
		attrWindow = new AttributeWindow(eventInfoObject,x,y,w,h,name);
		attrWindow->setOffset(150);
		attrWindow->begin();
		attrWindow->end();			
		Fl_Button* eventButton = new Fl_Button( x + 20 , y + 80 , 100, 20, "Remove Event");
		if (eventMap.find(eventName) == eventMap.end())		
			eventButton->label("Add New Event");		
		eventButton->callback(addEventCallback, this);		
	this->end();

}

void EventItemInfoWidget::updateWidget()
{	
	SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();
	std::map<std::string, SmartBody::SBEventHandler*>& eventMap = manager->getEventHandlers();	
	std::map<std::string, SmartBody::SBEventHandler*>::iterator mi = eventMap.find(eventName);
	if (mi != eventMap.end())
	{
		SmartBody::SBEventHandler* handler = mi->second;
		eventInfoObject->setStringAttribute("EventType", mi->first);
		SmartBody::SBBasicHandler* basicHandler = dynamic_cast<SmartBody::SBBasicHandler*>(handler);
		if (basicHandler)
		{
			eventInfoObject->setStringAttribute("Action",basicHandler->getAction());	
		}
	}		
}

void EventItemInfoWidget::notify( SmartBody::SBSubject* subject )
{
	SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();
	std::map<std::string, SmartBody::SBEventHandler*>& eventMap = manager->getEventHandlers();
	std::map<std::string, SmartBody::SBEventHandler*>::iterator mi = eventMap.find(eventName);
	if (mi != eventMap.end())
	{
		SmartBody::SBEventHandler* handler = mi->second;
		SmartBody::SBBasicHandler* basicHandler = dynamic_cast<SmartBody::SBBasicHandler*>(handler);
		if (basicHandler)
			basicHandler->setAction(eventInfoObject->getStringAttribute("Action"));		
	}		
}

void EventItemInfoWidget::addEventCallback( Fl_Widget* widget, void* data )
{
	EventItemInfoWidget* pathInfoWidget = (EventItemInfoWidget*)data;
	std::string buttonLabel = widget->label();
	if (buttonLabel == "Add New Event")
		pathInfoWidget->addNewEvent();	
	else if (buttonLabel == "Remove Event")
		pathInfoWidget->removeEvent();
}

void EventItemInfoWidget::addNewEvent()
{
	std::string eventType = eventInfoObject->getStringAttribute("EventType");
	std::string eventAction = eventInfoObject->getStringAttribute("Action");	
	if (eventType != "" && eventAction != "")
	{
		std::string eventCmd = "registerevent " + eventType + " " + "\"" + eventAction + "\"";	
		SmartBody::SBScene::getScene()->command(eventCmd);
	}
}

void EventItemInfoWidget::removeEvent()
{
	std::string eventType = eventInfoObject->getStringAttribute("EventType");
	std::string eventAction = eventInfoObject->getStringAttribute("Action");	
	if (eventType != "" && eventAction != "")
	{
		std::string eventCmd = "unregisterevent " + eventType;
		SmartBody::SBScene::getScene()->command(eventCmd);
		//LOG("Remove Event %s",eventType.c_str());
	}
}
/************************************************************************/
/* Pawn Item Info Widget                                                */
/************************************************************************/
PawnItemInfoWidget::PawnItemInfoWidget( int x, int y, int w, int h, const char* name, SBObserver* observerWindow ) 
: TreeItemInfoWidget(x,y,w,h,name)
{
	pawnInfoObject = NULL;
	pawnName = name;
	updateWidget();
	//if (observerWindow)
	//	pawnInfoObject->registerObserver(observerWindow);
	
	this->begin();
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
		if (pawn)
			pawn->registerObserver(this);
		attrWindow = new AttributeWindow(pawn,x,y,w,h,name);
		attrWindow->setOffset(150);
		attrWindow->begin();
		attrWindow->end();
	this->end();
}

PawnItemInfoWidget::~PawnItemInfoWidget()
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
	if (pawn)
		pawn->unregisterObserver(this);
}

void PawnItemInfoWidget::updateWidget()
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
	if (!pawn) return;
	float x, y, z, h, p, r;
	pawn->get_world_offset(x,y,z,h,p,r);
	pawn->setDoubleAttribute("posX",x);
	pawn->setDoubleAttribute("posY",y);
	pawn->setDoubleAttribute("posZ",z);

	pawn->setDoubleAttribute("rotX",h);
	pawn->setDoubleAttribute("rotY",p);
	pawn->setDoubleAttribute("rotZ",r);
	//pawnInfoObject->getAttribute("pos X")->

}

void PawnItemInfoWidget::notify( SmartBody::SBSubject* subject )
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnName);
	if (!pawn) return;

	TreeInfoObject* infoObject = dynamic_cast<TreeInfoObject*>(subject);
	if (infoObject && infoObject == pawnInfoObject)
	{
		float x,y,z,h,p,r;
		x = (float)infoObject->getDoubleAttribute("pos X");
		y = (float)infoObject->getDoubleAttribute("pos Y");
		z = (float)infoObject->getDoubleAttribute("pos Z");

		h = (float)infoObject->getDoubleAttribute("rot X");
		p = (float)infoObject->getDoubleAttribute("rot Y");
		r = (float)infoObject->getDoubleAttribute("rot Z");
		pawn->set_world_offset(x,y,z,h,p,r);
		//updateWidget();
	}
}

AttributeItemWidget::AttributeItemWidget( SmartBody::SBObject* object, int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow /*= NULL*/ )
: TreeItemInfoWidget(x,y,w,h,name)
{
	attrWindow = NULL;
	infoObject = object;
	if (!object)
		return;
	this->begin();
	attrWindow = new AttributeWindow(infoObject,x,y,w,h,name);
	attrWindow->setOffset(150);
	attrWindow->begin();
	attrWindow->end();
	this->end();	
}

void AttributeItemWidget::updateWidget()
{
	redraw();
}

AttributeItemWidget::~AttributeItemWidget()
{	
	delete attrWindow;
}

DoubleAttributeItemWidget::DoubleAttributeItemWidget( SmartBody::SBObject* object1, SmartBody::SBObject* object2, int x, int y, int w, int h, int ySep, const char* name1, const char* name2, SmartBody::SBObserver* observerWindow /*= NULL*/ )
: TreeItemInfoWidget(x,y,w,h,name1)
{
	attrWindow1 = NULL;
	attrWindow2 = NULL;
	if (!object1 && !object2)
		return;

	int speLen = 15;
	infoObject1 = object1;	
	infoObject2 = object2;
	this->begin();
	attrWindow1 = new AttributeWindow(infoObject1,x,y,w,(ySep-y)- speLen,name1);
	attrWindow1->setOffset(150);
	attrWindow1->begin();
	attrWindow1->end();
	attrWindow2 = new AttributeWindow(infoObject2,x,ySep+speLen,w,h-(ySep-y)-speLen,name2);
	attrWindow2->setOffset(150);
	attrWindow2->begin();
	attrWindow2->end();
	this->end();	
}

DoubleAttributeItemWidget::~DoubleAttributeItemWidget()
{
	if (attrWindow1)
		delete attrWindow1;
	if (attrWindow2)
		delete attrWindow2;
}

void DoubleAttributeItemWidget::updateWidget()
{
	redraw();
}

MultiAttributeItemWidget::MultiAttributeItemWidget( std::vector<SmartBody::SBObject*>& objectList, int x, int y, int w, int h, int yStep, const char* name, std::vector<std::string>& objectNameList, SmartBody::SBObserver* observerWindow /*= NULL*/ )
:TreeItemInfoWidget(x,y,w,h,name) 
{
	infoObjectList = objectList;
	attrNameList = objectNameList;
	int speLen = 15;
	int yCur = y;
	AttributeWindow* attrWin = NULL;
	this->begin();
	mainGroup = new Fl_Tabs( x,  y, w, h, name);
	mainGroup->begin();
	for (size_t i = 0; i < infoObjectList.size(); i++)
	{
		const char* attrWinName = (i<attrNameList.size())?attrNameList[i].c_str():NULL;
		attrWin = new AttributeWindow(infoObjectList[i],x+speLen,y+speLen,w-speLen,h-speLen,attrWinName,false);
		attrWin->setOffset(150);
		attrWin->begin();
		attrWin->end();
	}
	mainGroup->end();
	this->end();
}

MultiAttributeItemWidget::~MultiAttributeItemWidget()
{

}

void MultiAttributeItemWidget::updateWidget()
{
	redraw();
}

AnimationBlendInfoWidget::AnimationBlendInfoWidget( SmartBody::SBAnimationBlend* blend, int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow )
:TreeItemInfoWidget(x,y,w,h,name) 
{
	if (!blend) return;
	attrWindow = NULL;
	blendName = blend->stateName;
	blendInfoObject = new TreeInfoObject();
	this->begin();
	attrWindow = new AttributeWindow(blendInfoObject,x,y,w,h,blendName.c_str());
	attrWindow->setOffset(150);
	attrWindow->begin();
	attrWindow->end();
	this->end();	
	updateWidget();
}

void AnimationBlendInfoWidget::updateWidget()
{
	SmartBody::SBAnimationBlendManager* blendManager = SmartBody::SBScene::getScene()->getBlendManager();
	SmartBody::SBAnimationBlend* animBlend = blendManager->getBlend(blendName);
	if (!animBlend)
		return;
	
	blendInfoObject->clearAttributes();	
	attrWindow->cleanUpWidgets();
	std::vector<SrVec> parameters = animBlend->getParameters();
	for (unsigned int i=0;i< (size_t) animBlend->getNumMotions();i++)
	{
		std::string motionName = animBlend->getMotionName(i);
		SrVec& para = parameters[i];
		blendInfoObject->createVec3Attribute(motionName,para[0],para[1],para[2], true, "Blend Parameters", i+1,true, false, false, "");		
	}

	this->attrWindow->setDirty(true);	
	this->attrWindow->redraw();
	this->redraw();
}

BlendTransitionInfoWidget::BlendTransitionInfoWidget( SmartBody::SBAnimationTransition* transition, int x, int y, int w, int h, const char* name, SmartBody::SBObserver* observerWindow )
:TreeItemInfoWidget(x,y,w,h,name) 
{
	if (!transition) return;
	attrWindow = NULL;
	transitionName = transition->getTransitionName();
	transitionInfoObject = new TreeInfoObject();
	this->begin();
	attrWindow = new AttributeWindow(transitionInfoObject,x,y,w,h,transitionName.c_str());
	attrWindow->setOffset(150);
	attrWindow->begin();
	attrWindow->end();
	this->end();	
	updateWidget();

}

void BlendTransitionInfoWidget::updateWidget()
{
	SmartBody::SBAnimationBlendManager* blendManager = SmartBody::SBScene::getScene()->getBlendManager();
	SmartBody::SBAnimationTransition* blendTransition = blendManager->getTransitionByName(transitionName);
	if (!blendTransition)
		return;

	transitionInfoObject->clearAttributes();	
	attrWindow->cleanUpWidgets();

	transitionInfoObject->createStringAttribute("Source Blend",blendTransition->getSourceBlend()->stateName, true, "Transition Parameters", 1,true, false, false, "");
	transitionInfoObject->createStringAttribute("Source Motion",blendTransition->getSourceMotionName(), true, "Transition Parameters", 5,true, false, false, "");
	transitionInfoObject->createStringAttribute("Target Blend",blendTransition->getDestinationBlend()->stateName, true, "Transition Parameters", 10,true, false, false, "");
	transitionInfoObject->createStringAttribute("Target Motion",blendTransition->getDestinationMotionName(), true, "Transition Parameters", 15,true, false, false, "");	

	this->attrWindow->setDirty(true);	
	this->attrWindow->redraw();
	this->redraw();
}
