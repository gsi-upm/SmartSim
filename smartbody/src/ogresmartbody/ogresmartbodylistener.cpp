#include "ogresmartbodylistener.h"
#include "ogresmartbody.h"


OgreSmartBodyListener::OgreSmartBodyListener(OgreSmartBody* osb) : SBSceneListener()
{
	ogreSB = osb;
}

OgreSmartBodyListener::~OgreSmartBodyListener()
{
}

void OgreSmartBodyListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	if (ogreSB->getSceneManager()->hasEntity(name))
	{
		std::cout << "An entity named '" << name << "' already exists, ignoring..." << std::endl;
		return;
	}

	Ogre::Entity* entity = ogreSB->getSceneManager()->createEntity(name, objectClass + ".mesh");
	Ogre::SceneNode* node = ogreSB->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	node->attachObject(entity);

	Ogre::Skeleton* meshSkel = entity->getSkeleton();
	Ogre::Skeleton::BoneIterator it = meshSkel->getBoneIterator(); 
	while ( it.hasMoreElements() ) 
	{ 
		Ogre::Bone* bone = it.getNext();
		bone->setManuallyControlled(true);
	}
}
	
void OgreSmartBodyListener::OnCharacterDelete( const std::string & name )
{
	if (!ogreSB->getSceneManager()->hasEntity(name))
	{
		std::cout << "An entity named '" << name << "' does not exist, ignoring delete..." << std::endl;
		return;
	}

	Ogre::SceneNode * node = (Ogre::SceneNode *)ogreSB->getSceneManager()->getRootSceneNode()->getChild(name);
	node->detachAllObjects();
	ogreSB->getSceneManager()->destroyEntity(name);
	ogreSB->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name);
}
	
void OgreSmartBodyListener::OnCharacterChanged( const std::string& name )
{
	if (!ogreSB->getSceneManager()->hasEntity(name))
	{
		std::cout << "An entity named '" << name << "' does not exist, ignoring update..." << std::endl;
		return;
	}

	Ogre::Entity* entity = ogreSB->getSceneManager()->getEntity(name);
	Ogre::Skeleton* meshSkel = entity->getSkeleton();
	Ogre::Skeleton::BoneIterator it = meshSkel->getBoneIterator(); 
	while ( it.hasMoreElements() ) 
	{ 
		Ogre::Bone* bone = it.getNext();
		bone->setManuallyControlled(true);
	}
}
		 
void OgreSmartBodyListener::OnLogMessage( const std::string & message )
{
#ifdef WIN32
	LOG(message.c_str());
#endif
}


