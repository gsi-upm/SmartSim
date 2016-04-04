#ifndef _BEHAVIOREDITORWIDGET_
#define _BEHAVIOREDITORWIDGET_

#include "nle/NonLinearEditorWidget.h"

class BehaviorEditorWidget : public nle::EditorWidget
{
	public:
		BehaviorEditorWidget(int x, int y, int w, int h, char* name);

		virtual void changeBlockSelectionEvent(nle::Block* block);
		virtual void changeTrackSelectionEvent(nle::Track* track);

		void setBlockSelectionChanged(bool val);
		bool getBlockSelectionChanged();
		void setTrackSelectionChanged(bool val);
		bool getTrackSelectionChanged();

protected:
		virtual void drawBlock(nle::Block* block, int trackNum, int blockNum);
		virtual void drawMark(nle::Block* block, nle::Mark* mark, int trackNum, int blockNum, int markNum);

		bool blockSelectionChanged;
		bool trackSelectionChanged;

};

#endif
