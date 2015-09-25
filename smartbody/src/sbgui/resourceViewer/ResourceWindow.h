#ifndef _RESOURCEWINDOW_
#define _RESOURCEWINDOW_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Light_Button.H>
#include <sb/SBFaceDefinition.h>
#include <sb/SBService.h>
#include <sb/SBPhysicsSim.h>
#include <sb/SBSceneListener.h>
#include <sb/SBPhysicsManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBScript.h>
#include <sb/SBGestureMap.h>
#include <sb/SBAnimationState.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBAnimationTransition.h>
#include <sbm/GenericViewer.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sb/SBEvent.h>
#include "Fl_Tree_Horizontal.h" // a custom tree widget with horizontal scroll bar
#include "TreeItemInfoWidget.h"
#include "TreeInfoObject.h"
#include <SBSelectionManager.h>
#include <SBWindowListener.h>

class srPathList;
class BoneMap;
class EventHandler;

class ResourceWindow;

class ResourceWindow : public Fl_Group, public SmartBody::SBObserver, public SBWindowListener, public SelectionListener
{
	public:
		
//		std::vector<std::string> _itemNameList;

		ResourceWindow(int x, int y, int w, int h, char* name);
		~ResourceWindow();
		
		virtual void label_viewer(std::string name);

		virtual void notify(SmartBody::SBSubject* subject);
		int handle(int event);
        virtual void show();      
		virtual void hide(); 
        void draw();
		void resize(int x, int y, int w, int h);
		void update();
        
		void selectPawn(const std::string& name);
		void updateGUI();				
		static void refreshUI(Fl_Widget* widget, void* data);
		static void treeCallBack(Fl_Widget* widget, void* data);

		// selection callbacks
		virtual void OnSelect(const std::string& value);
		// object lifecycle callbacks
		void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		void OnCharacterDelete( const std::string & name );
		void OnCharacterUpdate( const std::string & name );
		void OnPawnCreate( const std::string & name );
		void OnPawnDelete( const std::string & name );
		void OnSimulationStart();
		void OnObjectCreate( SmartBody::SBObject* object );
		void OnObjectDelete( SmartBody::SBObject* object );

	protected:		
		Fl_Tree* resourceTree;
		bool _dirty;
		bool _firstTime;
		
		int addSpecialName(const std::string& name);
		void removeSpecialName(const std::string& name);

		void hideTree();
		void showTree();

		std::map<Fl_Tree_Item*, std::string> _treeMap;
		std::string emptyString;
		std::map<int, std::string> _specialNames;
		std::map<std::string, int> _reverseSpecialNames;
		int _specialNameIndex;

		TreeItemInfoWidget* itemInfoWidget;
		std::string lastClickedItemPath;
		std::vector<TreeItemInfoWidget*> widgetsToDelete;

		std::string getNameFromTree(Fl_Tree_Item* item);
		Fl_Tree_Item* getTreeFromName(const std::string& name);
		std::string getNameFromItem(Fl_Tree_Item* item);

		void updatePath(Fl_Tree_Item* tree, const std::vector<std::string>& pathList);
		void updateScriptFiles(Fl_Tree_Item* tree, std::string pathName);
		void updateScript(Fl_Tree_Item* tree, SmartBody::SBScript* script);
		void updateSkeleton(Fl_Tree_Item* tree, SmartBody::SBSkeleton* skel);
		void updateJointMap(Fl_Tree_Item* tree, SmartBody::SBJointMap* jointMap);
		void updateGestureMap(Fl_Tree_Item* tree, SmartBody::SBGestureMap* gestureMap);
		void updateMotion(Fl_Tree_Item* tree, SmartBody::SBMotion* motion);
		void updateAnimationBlend(Fl_Tree_Item* tree, SmartBody::SBAnimationBlend* blend);
		void updateBlendTransition(Fl_Tree_Item* tree, SmartBody::SBAnimationTransition* transition);
		void updateMesh(Fl_Tree_Item* tree, DeformableMesh* mesh);
		void updatePawn(Fl_Tree_Item* tree, SmartBody::SBPawn* pawn);
		void updateCharacter(Fl_Tree_Item* tree, SmartBody::SBCharacter* character);
		void updatePhysicsCharacter(Fl_Tree_Item* tree, SmartBody::SBPhysicsCharacter* phyChar);
		void updateService(Fl_Tree_Item* tree, SmartBody::SBService* service);
		void updatePhysicsManager(Fl_Tree_Item* tree, SmartBody::SBPhysicsManager* phyService);
		void updateFaceDefinition(Fl_Tree_Item* tree, SmartBody::SBFaceDefinition* faceDefinition);
		void updateEventHandler(Fl_Tree_Item* tree, SmartBody::SBEventHandler* handler);
		void updateBehaviorSet(Fl_Tree_Item* tree, SmartBody::SBBehaviorSet* behaviorSet);
		bool processedDragAndDrop(std::string& dndText);
};

 #endif
