#ifndef _ATTRIBUTEEDITOR_
#define _ATTRIBUTEEDITOR_

#include <vhcl.h>
#include <FL/Fl_Group.H>
#include "TreeItemInfoWidget.h"
#include <SBWindowListener.h>
#include <SBSelectionManager.h>

class AttributeEditor : public Fl_Group, public SmartBody::SBObserver, public SBWindowListener, public SelectionListener
{
	public:
		AttributeEditor(int x, int y, int w, int h, char* name);
		~AttributeEditor();
		
		virtual void draw();
		virtual void show();
		virtual void hide();

		// selection callbacks
		virtual void OnSelect(const std::string& value);
		// object lifecycle callbacks
		void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		void OnCharacterDelete( const std::string & name );
		void OnCharacterUpdate( const std::string & name );
		void OnPawnCreate( const std::string & name );
		void OnPawnDelete( const std::string & name );

		void OnObjectCreate( SmartBody::SBObject* object );
		void OnObjectDelete( SmartBody::SBObject* object );
		void OnSimulationStart();
		void updateGUI();

		void removeCurrentWidget();

	protected:		
		bool _dirty;
		Fl_Button    *refreshButton;
		std::string lastClickedItemPath;
		std::vector<TreeItemInfoWidget*> widgetsToDelete;
		std::string _currentSelection;
		TreeItemInfoWidget* _currentWidget;

		TreeItemInfoWidget* createInfoWidget( int x, int y, int w, int h, const std::string& name );

};

#endif
