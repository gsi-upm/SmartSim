#ifndef _SBSELECTIONMANAGER_H
#define _SBSELECTIONMANAGER_H

#include <string>
#include <vector>
#include <sb/SBObject.h>

class SelectionListener
{
	public:
		SelectionListener();
		~SelectionListener();

		virtual void OnSelect(const std::string& value);
};


class SBSelectionManager
{
	public:
		SBSelectionManager();
		~SBSelectionManager();

		void select(const std::string& value);

		const std::string& getCurrentSelection();

		void addSelectionListener(SelectionListener* listener);
		void removeSelectionListener(SelectionListener* listener);

		static SBSelectionManager* getSelectionManager();

	protected:
		static SBSelectionManager* _selectionManager;
		std::vector<SelectionListener*> _selectionListeners;
		std::string _currentSelection;
};


#endif