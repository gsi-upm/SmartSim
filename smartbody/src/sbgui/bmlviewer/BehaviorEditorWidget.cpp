
#include "vhcl.h"

#include "BehaviorEditorWidget.h"
#include "BehaviorBlock.h"


BehaviorEditorWidget::BehaviorEditorWidget(int x, int y, int w, int h, char* name) : EditorWidget(x, y, w, h, name)
{
	blockSelectionChanged = false;
	trackSelectionChanged = false;
}
	
void BehaviorEditorWidget::drawBlock(nle::Block* block, int trackNum, int blockNum)
{
	EditorWidget::drawBlock(block, trackNum, blockNum);

}

void BehaviorEditorWidget::drawMark(nle::Block* block, nle::Mark* mark, int trackNum, int blockNum, int markNum)
{
	EditorWidget::drawMark(block, mark, trackNum, blockNum, markNum);

	BehaviorTimingMark* timingMark = dynamic_cast<BehaviorTimingMark*>(mark);
	if (timingMark)
	{
		int trackTop = padding + timeWindowHeight + trackNum * trackHeight;
		int bounds[4];
		timingMark->getBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
	}
}

void BehaviorEditorWidget::changeBlockSelectionEvent(nle::Block* block)
{
	blockSelectionChanged = true;
}

void BehaviorEditorWidget::changeTrackSelectionEvent(nle::Track* track)
{
	trackSelectionChanged = true;
}

void BehaviorEditorWidget::setBlockSelectionChanged(bool val)
{
	blockSelectionChanged = val;
}

bool BehaviorEditorWidget::getBlockSelectionChanged()
{
	return blockSelectionChanged;
}


void BehaviorEditorWidget::setTrackSelectionChanged(bool val)
{
	trackSelectionChanged = val;
}

bool BehaviorEditorWidget::getTrackSelectionChanged()
{
	return trackSelectionChanged;
}
