#include "vhcl.h"
#include "ObjectManipulationHandle.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sr/sr_gl.h>

#include <FL/gl.h>
#include <FL/glu.h>

bool ObjectManipulationHandle::renderSelectedBoundingBox = true;

ObjectManipulationHandle::ObjectManipulationHandle(void)
{
	active_control = NULL;
	bHasPicking = false;
	pickType = CONTROL_SELECTION;
	defaultColor.set(1, 0, 0);
}

ObjectManipulationHandle::~ObjectManipulationHandle(void)
{
}

void ObjectManipulationHandle::get_pawn_list(std::vector<SbmPawn*>& pawn_list)
{
	const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
	for (size_t i = 0; i < pawnNames.size(); i++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(pawnNames[i]);
		pawn_list.push_back(pawn);
	}
}

SbmPawn* ObjectManipulationHandle::get_selected_pawn()
{
	if (active_control)
	{
		PawnPosControl* active_pawn_control = (PawnPosControl*) get_active_control();
		return active_pawn_control->get_attach_pawn();
	}
	return NULL;
}

void ObjectManipulationHandle::draw(SrCamera& cam)
{
	if (active_control)
	{
		active_control->renderControl(cam);
		if (renderSelectedBoundingBox)
		{
			SmartBody::SBPawn* sbPawn = dynamic_cast<SmartBody::SBPawn*>(active_control->get_attach_pawn());
			if (sbPawn)
			{
				if (sbPawn->getBoolAttribute("showSelected"))
				{
					SrBox box = sbPawn->getBoundingBox();
					if (box.max_size() < 1e-9) // no bounding box
					{
						SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
						float boxSize = 0.05f / scene->getScale();
						box.a = sbPawn->getPosition() - SrVec(boxSize,boxSize,boxSize);
						box.b = sbPawn->getPosition() + SrVec(boxSize,boxSize,boxSize);
					}
					PositionControl::drawBox(box,true, defaultColor);
				}
			}
		}
	}
	//pawn_control.draw();
	
}


void ObjectManipulationHandle::drag(SrCamera& cam,  float fx, float fy, float tx, float ty)
{
	if (active_control)
		active_control->updateControl(cam,fx,fy,tx,ty);
}

void ObjectManipulationHandle::set_selected_pawn( SbmPawn* pawn )
{
	if (active_control)
	{
		active_control->detach_pawn();
		active_control->attach_pawn(pawn);
	}
}

SbmPawn* ObjectManipulationHandle::getPickingPawn( float x, float y, SrCamera* cam, std::vector<int>& hitNames)
{
	GLint viewport[4];
	const int BUFSIZE = 512;
	GLuint selectBuf[BUFSIZE];
	float ratio;	
	SrMat mat;

	glSelectBuffer(BUFSIZE,selectBuf);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glRenderMode(GL_SELECT);
	glInitNames();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int cursorX,cursorY;
	cursorX = int((x+1.0)*viewport[2]*0.5);
	cursorY = int((y+1.0)*viewport[3]*0.5);

	//gluPickMatrix(cursorX,viewport[3]-cursorY,100,100,viewport);
	//printf("cursorX = %d, cursorY = %d\n",cursorX,cursorY);
	gluPickMatrix(cursorX,cursorY,20,20,viewport);
	ratio = (viewport[2]+0.0f) / viewport[3];	
	glMultMatrixf ( (const float*)cam->get_perspective_mat(mat) );	
	glMatrixMode(GL_MODELVIEW);

	// draw buffer object for each joints ?	
	// test for each joints
	//pawn_control.hitTest();
	std::vector<SbmPawn*> pawn_list;
	this->get_pawn_list(pawn_list);

	// determine the size of the pawns relative to the size of the characters
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	float pawnSize = 0.05f / scene->getScale();
// 	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
// 	for (size_t i = 0; i < charNames.size(); i++)
// 	{
// 		SmartBody::SBCharacter * character = SmartBody::SBScene::getScene()->getCharacter(charNames[i]);
// 		pawnSize = character->getHeight() / 30.0f;
// 		break;
// 	}

	for (unsigned int i=0;i<pawn_list.size();i++)
	{
		SbmPawn* pawn = pawn_list[i];
		SmartBody::SBPawn* sbpawn = dynamic_cast<SmartBody::SBPawn*>(pawn);
		// ignore the active camera from the picking selection
		SmartBody::SBPawn* activeCamera = SmartBody::SBScene::getScene()->getActiveCamera();
		if (activeCamera && sbpawn == activeCamera)
			continue;
		SrVec pawn_pos = PawnPosControl::get_pawn_pos(pawn);
		glPushName(0xffffffff);
		glLoadName(i);
		SmartBody::SBCharacter* curChar = dynamic_cast<SmartBody::SBCharacter*>(pawn);
		//PositionControl::drawSphere(pawn_pos, pawnSize);				
		if (active_control && active_control->get_attach_pawn() == pawn)
		{			
			SrBox bbox = sbpawn->getBoundingBox();
			PositionControl::drawBox(bbox, false, defaultColor);
			//pawnPosControl.hitOPS(cam);
			active_control->hitTest(*cam);			
		}
#if 1
		else if (curChar) // the selected pawn is actually a character
		{			
			SrBox bbox = curChar->getBoundingBox();
			//LOG("curChar = %s, bounding box : max = %f %f %f, min = %f %f %f",curChar->getName().c_str(),bbox.b[0],bbox.b[1],bbox.b[2],bbox.a[0],bbox.a[1],bbox.a[2]);
			PositionControl::drawBox(bbox, false, defaultColor);						
		}
		else if (sbpawn->getPhysicsObject())
		{
			SrMat gmat = sbpawn->getPhysicsObject()->getGlobalTransform().gmat();
			//FltkViewer::drawColObject(pawn->getGeomObject(), gmat);
		}
#endif
		else
		{		
			PositionControl::drawSphere(pawn_pos, pawnSize, defaultColor);				
		}		
		glPopName();			
	}
	// process picking
	GLint hits;
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	//LOG("num of hits = %d",hits);
	SbmPawn* selectPawn = NULL;
	if (hits != 0){
		//processHits2(hits,selectBuf,0);
		hitNames = this->process_hit(selectBuf,hits);		
		int pawnIdx = hitNames[0];
		selectPawn = pawn_list[pawnIdx];		
	}	
	return selectPawn;
}

void ObjectManipulationHandle::picking(float x,float y,SrCamera* cam)
{
	// start picking
	/*
	GLint viewport[4];
	const int BUFSIZE = 512;
	GLuint selectBuf[BUFSIZE];
	float ratio;	
	SrMat mat;
	
	glSelectBuffer(BUFSIZE,selectBuf);
	glGetIntegerv(GL_VIEWPORT,viewport);
	glRenderMode(GL_SELECT);
	glInitNames();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	int cursorX,cursorY;
	cursorX = int((x+1.0)*viewport[2]*0.5);
	cursorY = int((y+1.0)*viewport[3]*0.5);

	//gluPickMatrix(cursorX,viewport[3]-cursorY,100,100,viewport);
	//printf("cursorX = %d, cursorY = %d\n",cursorX,cursorY);
	gluPickMatrix(cursorX,cursorY,5,5,viewport);
	ratio = (viewport[2]+0.0f) / viewport[3];	
	glMultMatrixf ( (const float*)cam.get_perspective_mat(mat) );	
	glMatrixMode(GL_MODELVIEW);

	// draw buffer object for each joints ?	
	// test for each joints
	//pawn_control.hitTest();
	SrArray<SbmPawn*> pawn_list;
	this->get_pawn_list(pawn_list);

	// determine the size of the pawns relative to the size of the characters
	float pawnSize = 1.0;
	
	mcu.character_map.reset();
	while (SbmCharacter* character = mcu.character_map.next())
	{
		pawnSize = character->getHeight() / 30.0f;
		break;
	}

	for (int i=0;i<pawn_list.size();i++)
	{
		SbmPawn* pawn = pawn_list[i];
		SrVec pawn_pos = PawnPosControl::get_pawn_pos(pawn);
		glPushName(0xffffffff);
	    glLoadName(i);
		//PositionControl::drawSphere(pawn_pos, pawnSize);		
		if (active_control && active_control->get_attach_pawn() == pawn)
		{			
			//pawnPosControl.hitOPS(cam);
			active_control->hitTest(cam);
		}
		else
			PositionControl::drawSphere(pawn_pos, pawnSize);				
	    glPopName();			
	}
	// process picking
	GLint hits;
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	*/
	
	//printf("Hits = %d\n",hits);
	PawnControl* tempControl = getPawnControl(pickType);

	std::vector<int> hitNames;
	SbmPawn* selectPawn = getPickingPawn(x,y,cam,hitNames);
	
	if (selectPawn){		
		if (active_control != tempControl)
			active_control = tempControl;		
		active_control->attach_pawn(selectPawn);
		if (hitNames.size() > 1)
			active_control->processHit(hitNames);		
	}
	else
	{
		if (active_control)
		{
			active_control->detach_pawn();
			//pawnPosControl.active = false;
			active_control = NULL;
		}		
	}	
	// stop picking
}

std::vector<int> ObjectManipulationHandle::process_hit(unsigned int *pickbuffer,int nhits)
{
	GLuint d1,d2,i,n,zmin,zmax,sel=0, nameSize = 0;
	std::vector<int> hitNames;
	if(0<=nhits){    
		for(i=0,zmin=zmax=4294967295U; nhits>0; i+=n+3,nhits--)
		{      
			n=pickbuffer[i];
      		d1=pickbuffer[1+i];
			d2=pickbuffer[2+i];
			
			// this test ensures that we always give control widgets (name size = 2) higher selection priority over pawn ( name size = 1).
			// if the name size is the same, then the priority is determined by the depth value.
			if ( n >= nameSize ) // only process highest level hit
			{
				bool depthTest = d1<zmin || (d1==zmin && d2<=zmax);
				bool nameSizeTest = (n > nameSize);
				if( depthTest || nameSizeTest )
				{
					zmin=d1;
					zmax=d2;
					sel=i;
					nameSize = n;
				}
			}			
		}
		//return pickbuffer[3+sel];
		int n = pickbuffer[sel];
		for (int k=0;k<n;k++)
			hitNames.push_back(pickbuffer[3+sel+k]);
	}
	return hitNames;
}

PawnControl* ObjectManipulationHandle::getPawnControl( ControlType type )
{
	if (type == CONTROL_POS)
		return &pawnPosControl;
	if (type == CONTROL_ROT)
		return &pawnRotControl;
	if (type == CONTROL_SELECTION)
		return &pawnSelectControl;

	return &pawnSelectControl;
}
