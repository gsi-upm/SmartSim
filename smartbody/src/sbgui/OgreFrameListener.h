#ifndef _OGRELISTENER_H
#define _OGRELISTENER_H

#include "ExampleFrameListener.h"

class EmbeddedOgre;
class DeformableMeshInstance;

// namespace Ogre
// {
// 	class Entity;
// }
// Event handler to animate
class OgreFrameListener : public ExampleFrameListener
{
	public:
		OgreFrameListener(Ogre::RenderWindow * win, Ogre::Camera * cam, const std::string & debugText, Ogre::SceneManager * mgr, EmbeddedOgre* ogreInterface);
		void windowFocusChange( Ogre::RenderWindow * rw );
		bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);
		virtual bool processUnbufferedMouseInput( const Ogre::FrameEvent & evt );
		bool frameStarted( const Ogre::FrameEvent & evt );
		void scheduleQuit(void);
		void SetOgreMouse( const bool enabled );	

		std::vector<std::string>	m_characterList;
		std::vector<std::string>    m_pawnList;
		std::map<std::string, std::map<std::string, Ogre::Vector3> > m_initialBonePositions;

		std::set<std::string> m_validJointNames;
	protected:
		void ogreBlendShape(Ogre::Entity* sbEntity, DeformableMeshInstance* meshInstance);
	protected:
		Ogre::SceneManager * mSceneMgr;			
		std::vector<std::string> m_initialCommands;
		bool m_ogreMouseEnabled;	
		bool mQuit;
		EmbeddedOgre* embeddedOgre;
};

	

#endif
