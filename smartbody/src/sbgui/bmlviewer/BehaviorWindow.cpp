
#include "vhcl.h"

#include "BehaviorWindow.h"

#include <iostream>
#include <vector>
#include <math.h>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>

#include <controllers/me_ct_blend.hpp>
#include <controllers/me_ct_time_shift_warp.hpp>
#include <controllers/me_ct_gaze.h>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include "BehaviorBlock.h"
#include <bml/bml_types.hpp>
#include <sbm/text_speech.h>
#include <sbm/remote_speech.h>
#include <sbm/sbm_speech_audiofile.hpp>
#include <bml/behavior_scheduler_fixed.hpp>
#include "sbm/xercesc_utils.hpp"
#include <bml/bml_processor.hpp>



BehaviorWindow::BehaviorWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(w, h, name), GenericViewer(x, y, w, h)
{
	this->begin();

	Fl_Group* topGroup = new Fl_Group(10, 0, w - 20, 30, "Behaviors");
	topGroup->begin();

		choiceContexts = new Fl_Choice(100 + topGroup->x(), 0 + topGroup->y(), 200, 20, "Contexts");
		choiceContexts->callback(ContextCB, this);
		buttonClear = new Fl_Button(350 + topGroup->x(), 0 + topGroup->y(), 100, 20, "Clear");
		buttonClear->callback(ClearCB, this);
		buttonReplay = new Fl_Button(470 + topGroup->x(), 0 + topGroup->y(), 100, 20, "Replay BML");
		buttonReplay->callback(ReplayCB, this);
		Fl_Group* topSizer = new Fl_Group(590 + topGroup->x(), 0 + topGroup->y(), 10, 0);
		
	topGroup->end();
	topGroup->resizable(topSizer);
 
	Fl_Group* bottomGroup = new Fl_Group(0, 30, w - 20, h - 40);
	bottomGroup->begin();

		Fl_Scroll* leftGroup = new Fl_Scroll(0 + bottomGroup->x(), 0 + bottomGroup->y() , bottomGroup->w() - 250, bottomGroup->h());
		leftGroup->type(Fl_Scroll::VERTICAL_ALWAYS);
		leftGroup->begin();

			nleWidget = new BehaviorEditorWidget(0 + leftGroup->x(), 20 + leftGroup->y(), leftGroup->w(), leftGroup->h() - 20, (char*)"");		
			nleWidget->box(FL_BORDER_BOX);	
			

		leftGroup->end();
		leftGroup->resizable(nleWidget);

		Fl_Scroll* rightGroup = new Fl_Scroll(bottomGroup->x() + bottomGroup->w() - 250,  20 + bottomGroup->y(), 250, bottomGroup->h() - 20, "Behavior Info");
		rightGroup->type(Fl_Scroll::VERTICAL);
		rightGroup->box(FL_BORDER_BOX);
		rightGroup->begin();
			textXML = new Fl_Text_Editor(rightGroup->x(), rightGroup->y(), rightGroup->w(), rightGroup->h());
			textXML->color(FL_WHITE);
			textXML->textcolor(FL_BLACK);
			bufferXML = new Fl_Text_Buffer();
			textXML->buffer(bufferXML);
			textXML->wrap_mode(true, 0);

            //Group* rightSizer = new Group(80, 110, 90, 20);
      
		rightGroup->end();
		rightGroup->resizable(textXML);

	bottomGroup->end();
	bottomGroup->resizable(leftGroup);

	this->end();

	this->resizable(bottomGroup);

	contextCounter = 0;
	selectedContext = "";


	nleModel = new nle::NonLinearEditorModel();
	nle::EditorWidget* behaviorEditorWidget = this->getEditorWidget();
	behaviorEditorWidget->setModel(nleModel);
	nleModel->addModelListener(behaviorEditorWidget);

	this->size_range(800, 480);
	updateGUI();
	redraw();
}


BehaviorWindow::~BehaviorWindow()
{
	delete nleModel;
}

void BehaviorWindow::label_viewer(std::string name)
{
	this->label(strdup(name.c_str()));
}

void BehaviorWindow::show_viewer()
{
	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->registerRequestCallback(OnRequest, this);

	this->show();
	this->updateGUI();
	this->redraw();
}

void BehaviorWindow::hide_viewer()
{
	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->registerRequestCallback(NULL, NULL);
	this->hide();
}

void BehaviorWindow::update_viewer()
{
}

int BehaviorWindow::handle(int event)
{
	return  Fl_Double_Window::handle(event);
}

void BehaviorWindow::show()
{    
	nleWidget->setup();
    Fl_Double_Window::show();   
}

void BehaviorWindow::resize(int x, int y, int w, int h)
{    
    Fl_Double_Window::resize(x, y, w, h);   
	nleWidget->setup();
}
      
void BehaviorWindow::draw()
{
    nle::NonLinearEditorModel* model = nleWidget->getModel();
    if (model)
    {
        if (model->isModelChanged())
        {
            updateGUI();
            model->setModelChanged(false);
        }
    }
    
    Fl_Double_Window::draw();   
}

nle::EditorWidget* BehaviorWindow::getEditorWidget()
{
	return nleWidget;
}

void BehaviorWindow::updateGUI()
{
	nle::NonLinearEditorModel* model = nleWidget->getModel();
	if (!model)
		return;

	model->setContext(selectedContext);
	choiceContexts->clear();

	int count = 1;
	choiceContexts->add("-------");
	std::vector<std::pair<std::string, std::vector<nle::Track*> > >& contexts = model->getContexts();
	for (std::vector<std::pair<std::string, std::vector<nle::Track*> > >::iterator iter = contexts.begin();
		iter != contexts.end(); 
		iter++)
	{
		std::string contextName = (*iter).first;
		choiceContexts->add(contextName.c_str());
		if (contextName == selectedContext)
			choiceContexts->value(count);
		count++;

	}
	if (selectedContext == "" || selectedContext == "-------")
	{
		choiceContexts->value(0);
		textXML->buffer()->remove(0, textXML->buffer()->length());
		textXML->buffer()->insert(0, "");
		textXML->redraw();
	}

	std::string selectedTrackName = "";
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		if (track->isSelected())
			selectedTrackName = track->getName();
	}

 
	bool found = false;
	// activate the inputs based on the selected block
    nle::Track* track = this->getSelectedTrack();
    if (track)
    {
        std::string trackName = track->getName();
    }
    else
    {
    }
        
	nle::Block* block = this->getSelectedBlock();
	if (nleWidget->getBlockSelectionChanged())
	{
		
		if (block && block->isSelected())
		{   
			std::string name = block->getName();
			double startTime = block->getStartTime();
			double endTime = block->getEndTime();	
			textXML->buffer()->remove(0, textXML->buffer()->length());
			textXML->buffer()->insert(0, block->getInfo().c_str());
			textXML->buffer()->unselect();

			nleWidget->setBlockSelectionChanged(false);
			if (textXML->parent()) 
			{
				textXML->parent()->redraw();
			}
		
			//textXML->redisplay_range()
			textXML->redraw();
			nleWidget->redraw();			
		}
		else
		{
			textXML->buffer()->remove(0, textXML->buffer()->length());
			textXML->redraw();
			nleWidget->redraw();
		}
	}	
}


nle::Block* BehaviorWindow::getSelectedBlock()
{
	nle::NonLinearEditorModel* model = nleWidget->getModel();
	if (!model)
		return NULL;

	// activate the inputs based on the selected block
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* curBlock = track->getBlock(b);
			if (curBlock->isSelected())
			{
				return curBlock;
			}
		}
	}
	return NULL;
}

nle::Track* BehaviorWindow::getSelectedTrack()
{
    nle::NonLinearEditorModel* model = nleWidget->getModel();
    if (!model)
        return NULL;

    // activate the inputs based on the selected block
    for (int t = 0; t < model->getNumTracks(); t++)
    {
        nle::Track* track = model->getTrack(t);
        if (track->isSelected())
            return track;
    }
    return NULL;
}

void BehaviorWindow::ClearCB(Fl_Widget* widget, void* data)
{
	BehaviorWindow* window = (BehaviorWindow*) data;
	window->nleWidget->getModel()->clearContexts();

	window->updateGUI();
	window->redraw();
}

void BehaviorWindow::ReplayCB(Fl_Widget* widget, void* data)
{
	BehaviorWindow* window = (BehaviorWindow*) data;

	std::vector< std::pair<std::string, std::vector<nle::Track*> > >& contexts = window->nleWidget->getModel()->getContexts();
	std::string contextName = window->nleWidget->getModel()->getContextName();

	std::vector<nle::Track*> tracks;
	bool found = window->nleWidget->getModel()->getContext(contextName, tracks);
	if (found)
	{
		if (tracks.size() > 0)
		{
			nle::Track* bmlTrack = tracks[0];
			if (bmlTrack->getNumBlocks() > 0)
			{
				std::stringstream strstr;
				strstr << "test bml ";
				// get the participant id - this is a little bit of a hack, but the character
				// name is not preserved, so we need to example the XML looking for participant id=
				std::string bml = tracks[0]->getBlock(0)->getInfo();
				std::string searchStr = "participant id=\"";
				int pos = bml.find(searchStr);
				if (pos != std::string::npos)
				{
					// find the next quote
					std::string substr = bml.substr(pos + searchStr.size());
					int quotePos = substr.find("\"");
					if (quotePos != std::string::npos)
					{
						std::string characterId = substr.substr(0, quotePos);
						strstr << " char " << characterId << " ";
					}
				}
				strstr << bml;

				// run that bml
				SmartBody::SBScene::getScene()->command((char*) strstr.str().c_str());
				window->updateGUI();
				window->redraw();
			}
		}
	}

}

void BehaviorWindow::ContextCB(Fl_Widget* widget, void* data)
{
	BehaviorWindow* window = (BehaviorWindow*) data;
	Fl_Choice* choice = (Fl_Choice*) widget;

	int val = choice->value();
	if (val < 0)
		return;

	if (val == 0)
	{
		window->nleWidget->getModel()->clear(true);
		window->selectedContext = "";
		window->updateGUI();
		window->redraw();
		return;
	}

	const Fl_Menu_Item* choiceWidget = choice->mvalue();

	std::string contextName = choiceWidget->label();
	window->nleWidget->getModel()->setContext(contextName);
	window->selectedContext = contextName;

	window->nleWidget->setBlockSelectionChanged(true);
	window->nleWidget->setTrackSelectionChanged(true);

	// set the viewable time to the minimum and maximum block times
	double minTime = 9999999;
	double maxTime = -999999;
	for (int t = 0; t < window->nleWidget->getModel()->getNumTracks(); t++)
	{
		nle::Track* track = window->nleWidget->getModel()->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			if (block->getStartTime() < minTime)
				minTime = block->getStartTime();
			if (block->getEndTime() > maxTime)
				maxTime = block->getEndTime();
		}
	}
	window->nleWidget->setViewableTimeStart(minTime);
	window->nleWidget->setViewableTimeEnd(maxTime);
	window->nleWidget->getModel()->setEndTime(maxTime);
	window->selectedContext = contextName;
	window->updateGUI();
	window->redraw();
}

void BehaviorWindow::updateBehaviors(BML::BmlRequest* request)
{
	nle::NonLinearEditorModel* model = nleWidget->getModel();
	if (!model)
		return;

	model->clear(true);

	
	double curTime = SmartBody::SBScene::getScene()->getSimulationManager()->getTime();

	// first, display the bml request in an intuitive way
	RequestTrack* requestTrack = new RequestTrack();
	requestTrack->setName(request->actorId);
	model->addTrack(requestTrack);
	RequestBlock* requestBlock = new RequestBlock();
	requestBlock->setName(request->msgId);
	requestBlock->setShowName(false);
	requestTrack->addBlock(requestBlock);
	requestBlock->setStartTime(curTime);
	requestBlock->setEndTime(curTime + 1);
	requestBlock->setColor(FL_GREEN);

	// dump the xml
	XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
//    DOMLSSerializer* theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
/*    DOMLSSerializer* theSerializer = impl->createLSSerializer();
	XMLCh* xmlOutput = theSerializer->writeToString(request->doc);
	theSerializer->release();
	std::string xmlStr = xml_utils::xml_translate_string(xmlOutput);
*/
	std::string xmlStr = "";
	//XMLString::release(&tempStr);
	requestBlock->setInfo(xmlStr);

	std::vector<std::pair<RequestMark*, std::string> > untimedMarks; // marks that have no associated time
	std::map<std::string, double> syncMap; // keeps track of sync points and their times

	int counter = 0;
	for (BML::MapOfSyncPoint::iterator spIter = request->idToSync.begin();
		spIter != request->idToSync.end();
		spIter++)
	{
		RequestMark* requestMark = new RequestMark();
		std::string syncPointName((*spIter).first.begin(), (*spIter).first.end()); 
		requestMark->setName(syncPointName);
		BML::SyncPointPtr syncPointPtr = (*spIter).second;
		if (syncPointPtr.get()->is_set())
		{
			double syncPointTime = syncPointPtr.get()->time;
			requestMark->setStartTime(syncPointTime);
			requestMark->setEndTime(syncPointTime);
		}
		else if (syncPointName == "bml:start") // why doesn't this sync point get set?
		{
			syncPointPtr.get()->time = request->bml_start->time; // careful! we are setting the time of this sync point
			double syncPointTime = syncPointPtr.get()->time;
			if (syncPointTime != syncPointTime) 
				syncPointTime = curTime; // why is this sync point time unset?
			requestMark->setStartTime(syncPointTime);
			requestMark->setEndTime(syncPointTime);
			requestMark->setColor(FL_RED);
		}
		else
		{
			// save this mark for later since it contains sync point information
			// that we haven't obtained the proper time for yet
			untimedMarks.push_back(std::pair<RequestMark*, std::string>(requestMark, syncPointName));
		}
		//if (requestMark)
			requestBlock->addMark(requestMark);
		
		counter++;
	}

	double triggerTime = 0;
	for (BML::VecOfTriggerEvent::iterator triggerIter = request->triggers.begin();
		 triggerIter != request->triggers.end();
		 triggerIter++)
	{
		BML::TriggerEvent* trigger = (*triggerIter).get();
		std::wstring name = trigger->name;
		BML::MapOfSyncPoint& syncMap = request->idToSync;
		BML::MapOfSyncPoint::iterator syncIter = syncMap.find(name);
		if (syncIter != syncMap.end())
		{
			BML::SyncPoint* syncPoint = (*syncIter).second.get();
			triggerTime = syncPoint->time;
			if (triggerTime != triggerTime)
				triggerTime = curTime; // why is this sync point time unset?
		}
	}

	BML::VecOfBehaviorRequest b = request->behaviors;
	for (BML::VecOfBehaviorRequest::iterator iter = b.begin();
		iter != b.end();
		iter++)
	{
		BML::BehaviorRequestPtr requestPtr = (*iter);
		BML::BehaviorRequest* behavior = requestPtr.get();

		BML::BehaviorSyncPoints::iterator startIter = behavior->behav_syncs.sync_start();
		if (startIter != behavior->behav_syncs.end())
		{
			triggerTime =(*startIter).sync()->time;
			if (triggerTime != triggerTime)
				triggerTime = curTime; // why is this sync point time unset?
		}

		BML::BehaviorScheduler* scheduler = behavior->scheduler.get();
		double readyTime = 0;
		double relaxTime = 0;
		double startTime = 0;
		double strokeTime = 0;
		double endTime = 0;
		double speed = 1;
		BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler = dynamic_cast<BML::BehaviorSchedulerConstantSpeed*>(scheduler);
		if (constantSpeedScheduler)
		{
			readyTime = constantSpeedScheduler->readyTime;
			relaxTime = constantSpeedScheduler->relaxTime;
			startTime = constantSpeedScheduler->startTime;
			strokeTime = constantSpeedScheduler->strokeTime;
			endTime = constantSpeedScheduler->endTime;
			speed = constantSpeedScheduler->speed;
		}
		bool processed = false;

		for (BML::BehaviorSyncPoints::iterator bhIter = behavior->behav_syncs.begin(); 
				bhIter !=  behavior->behav_syncs.end();
				bhIter++)
		{
			BML::SyncPointPtr syncPtr = (*bhIter).sync();
			BML::SyncPoint* sp = syncPtr.get();
			double time = (*bhIter).time();
			if (time != time)
			{
				 BML::SyncPointPtr parentPtr = sp->parent;
				 if (parentPtr)
				 {
					 BML::SyncPoint* parentSp = parentPtr.get();
					 double parentTime = parentSp->time;
					 if (parentTime == parentTime)
					 {
						 float offset = sp->offset;
					 }
				 }
			}

		}

		//BML::BehaviorSpan span = behavior->behav_syncs.getBehaviorSpan();
		//std::cout << behavior->unique_id << std::endl;
		//behavior->behav_syncs.printSyncIds();
		//behavior->behav_syncs.printSyncTimes();

		BML::MotionRequest* motionRequest = dynamic_cast<BML::MotionRequest*>(behavior);
		if (motionRequest)
		{
			processMotionRequest(motionRequest, model, behavior, triggerTime, constantSpeedScheduler, syncMap, untimedMarks);
			processed = true;
		}

		BML::MeControllerRequest* controllerRequest = dynamic_cast<BML::MeControllerRequest*>(behavior);
		if (controllerRequest)
		{
			processControllerRequest(controllerRequest, model, behavior, triggerTime, constantSpeedScheduler, syncMap, untimedMarks);
			processed = true;
		}

		BML::SpeechRequest* speechRequest = dynamic_cast<BML::SpeechRequest*>(behavior);
		if (speechRequest)
		{
			processSpeechRequest(speechRequest, model, behavior, triggerTime, constantSpeedScheduler, syncMap, untimedMarks);
			processed = true;
		}

		BML::EventRequest* eventRequest = dynamic_cast<BML::EventRequest*>(behavior);
		if (eventRequest)
		{
			processEventRequest(eventRequest, model, behavior, triggerTime, constantSpeedScheduler, syncMap, untimedMarks);
			processed = true;
		}

		BML::VisemeRequest* visemeRequest = dynamic_cast<BML::VisemeRequest*>(behavior);
		if (visemeRequest)
		{
			processVisemeRequest(visemeRequest, model, behavior, triggerTime, constantSpeedScheduler, syncMap, untimedMarks);
			processed = true;
		}

		if (!processed)
		{
			// need to handle additional request types here
			processed = true;
		}
		
	}

	std::vector<std::pair<BML::BehaviorRequest*, nle::Block*> > behaviorBlocks;
	// now match any marks against their proper times
	for (std::vector<std::pair<RequestMark*, std::string> >::iterator iter = untimedMarks.begin();
		iter != untimedMarks.end();
		iter++)
	{
		std::string spName = (*iter).second;
		std::map<std::string, double>::iterator mapIter = syncMap.find(spName);
		if (mapIter != syncMap.end())
		{
			bool processed = false;
			// make sure that event blocks only last for the time of the event
			EventBlock* eventBlock = dynamic_cast<EventBlock*>((*iter).first->getBlock());
			if (eventBlock)
			{
				eventBlock->setStartTime((*mapIter).second);
				eventBlock->setEndTime((*mapIter).second + .5);
				(*iter).first->setStartTime((*mapIter).second);
				(*iter).first->setEndTime((*mapIter).second);
				processed = true;
			}
			// adjust motion blocks
			MotionBlock* motionBlock = dynamic_cast<MotionBlock*>((*iter).first->getBlock());
			if (motionBlock)
			{
				bool found = false;
				for (BML::VecOfBehaviorRequest::iterator behavIter = request->behaviors.begin();
					behavIter != request->behaviors.end();
					behavIter++)
				{
					if (motionBlock->getName() == (*behavIter)->unique_id)
					{
						for (unsigned int b = 0; b < behaviorBlocks.size(); b++)
						{
							if (behaviorBlocks[b].first == (*behavIter).get())
							{
								found = true;
								break;
							}
						}
						if (!found)
							behaviorBlocks.push_back(std::pair<BML::BehaviorRequest*, nle::Block*>((*behavIter).get(), motionBlock));
					}
					
				}
				
				processed = true;
			}
			// adjust nod blocks
			NodBlock* nodBlock = dynamic_cast<NodBlock*>((*iter).first->getBlock());
			if (nodBlock)
			{
				bool found = false;
				for (BML::VecOfBehaviorRequest::iterator behavIter = request->behaviors.begin();
					behavIter != request->behaviors.end();
					behavIter++)
				{
					if (nodBlock->getName() == (*behavIter).get()->unique_id)
					{
						
						for (unsigned int b = 0; b < behaviorBlocks.size(); b++)
						{
							if (behaviorBlocks[b].first == (*behavIter).get())
							{
								found = true;
								break;
							}
						}
						if (!found)
							behaviorBlocks.push_back(std::pair<BML::BehaviorRequest*, nle::Block*>((*behavIter).get(), nodBlock));
					}
				}
				
				processed = true;
				
			}
			if (!processed)
			{
				(*iter).first->setStartTime((*mapIter).second);
				(*iter).first->setEndTime((*mapIter).second);
			}
		}
		else
		{
			// remove this mark
			nle::Block* b = (*iter).first->getBlock();
			b->removeMark((*iter).first);
		}
	}

	//for (int b = 0; b < behaviorBlocks.size(); b++)
	//{
//		adjustSyncPoints(behaviorBlocks[b].first, behaviorBlocks[b].second, syncMap);
//	}
	
	double startTime = 99999;
	double endTime = -99999;
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			if (block->getEndTime() > endTime)
				endTime = block->getEndTime();
			if (block->getStartTime() < startTime)
				startTime = block->getStartTime();
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->getEndTime() > endTime)
					endTime = mark->getEndTime();
				if (mark->getStartTime() > startTime)
					startTime = mark->getStartTime();
			}
		}
	}
	// set the main block on the first track to span the max times
	requestBlock->setEndTime(endTime);

	/*
	// set the time for the widget
	getEditorWidget()->setViewableTimeStart(startTime);
	getEditorWidget()->setViewableTimeEnd(endTime);
	*/
	
	
	//model->setEndTime(endTime);
	if (model->getNumTracks() > 0)
		contextCounter++;
	std::stringstream strstr;
	strstr << contextCounter;
	model->saveContext(strstr.str());

	model->setModelChanged(true);
	updateGUI();
	redraw();
}



void BehaviorWindow::processMotionRequest(BML::MotionRequest* motionRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
										  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
										  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks)
{
	RequestTrack* track = new RequestTrack();
	track->setName("motion");
	model->addTrack(track);
	MotionBlock* block = new MotionBlock();
	track->addBlock(block);
	block->setStartTime(triggerTime);
	block->setEndTime(triggerTime + 1);
	block->setName(behavior->unique_id);
	block->setShowName(false);
	int counter = 0;

	BML::BehaviorSyncPoints syncPoints = motionRequest->behav_syncs;

	double syncTimes[7] = {0, 0, 0, 0, 0, 0, 0};
	double phaseTimes[6] = {0, 0, 0, 0, 0, 0};

	const std::map<std::wstring, std::wstring>& behaviorToNameMap = syncPoints.getBehaviorToSyncNames();
	// start
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_start();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[0] = syncPointIter->time();
/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
		block->setStartTime(spMark->getStartTime());
	}
	// ready
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_ready();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
				spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[1] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
			
	}
	// stroke start
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_stroke_start();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[2] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
	}
	// stroke 
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_stroke();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[3] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
	}
	// stroke end
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_stroke_end();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[4] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
	}
	// relax
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_relax();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[5] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/
	}
	// end
	{
		BML::BehaviorSyncPoints::iterator syncPointIter = syncPoints.sync_end();
		std::wstring spName = syncPointIter->name();
		std::string spMarkName(spName.begin(), spName.end());
		RequestMark* spMark = new RequestMark();
		spMark->setName(spMarkName);
		if (constantSpeedScheduler)
			spMark->setStartTime(syncPointIter->time());
		spMark->setEndTime(spMark->getStartTime());
		block->addMark(spMark);
		syncTimes[6] = syncPointIter->time();

		/*
		const std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.find(spName);
		if (iter != behaviorToNameMap.end())		
		{
			std::string name((*iter).first.begin(), (*iter).first.end());
			untimedMarks.push_back( std::pair<RequestMark*, std::string>(spMark, name));
		}
		*/

		block->setEndTime(spMark->getEndTime());
	}


	for (int p = 0; p < 6; p++)
	{
		phaseTimes[p] = syncTimes[p + 1] - syncTimes[p];
	}

	/*
	MeCtScheduler2* schedulerController = motionRequest->schedule_ct;
	if (schedulerController)
	{
		std::vector<MeCtScheduler2::TrackPtr> tracks = schedulerController->tracks();
		if (tracks.size() > 0)
		{
			MeCtUnary* unaryTimingCt = tracks[0]->timing_ct();
			MeCtTimeShiftWarp* timeWarpCt = dynamic_cast<MeCtTimeShiftWarp*>(unaryTimingCt);
			if (timeWarpCt)
			{
				double t = 0.0;
				double warpTime = timeWarpCt->time_func()(t);
			}
		}

	}
	*/


	MeController* animController = motionRequest->anim_ct;
	MeCtMotion* motion = dynamic_cast<MeCtMotion*>(animController);
	if (motion)
	{ 
		/*
		RequestTrack* track = new RequestTrack();
		track->setName("Motion");
		model->addTrack(track);
		MotionBlock* motionBlock = new MotionBlock();
		motionBlock->setName(motion->name());
		motionBlock->setShowName(false);
		motionBlock->setInfo(motion->name());
		track->addBlock(motionBlock);
		double duration = motion->controller_duration();
		double speed = constantSpeedScheduler->speed;
		duration /= speed;
		// TODO
		// speed needs to change the timing of the motion
		// set the duration on the original block on the preceding track as well
		
		motionBlock->setStartTime(triggerTime);
		motionBlock->setEndTime(triggerTime + duration);
		RequestMark* inMark = new RequestMark();
		inMark->setName("in");
		inMark->setStartTime(block->getStartTime() + motion->indt() / speed);
		inMark->setEndTime(block->getStartTime() + motion->indt() / speed);
		motionBlock->addMark(inMark);
		RequestMark* outMark = new RequestMark();
		outMark->setName("out");
		motionBlock->addMark(outMark);
		if (motion->outdt() >= motion->indt())
		{
			outMark->setStartTime(inMark->getStartTime() + duration - motion->outdt() / speed);
			outMark->setEndTime(inMark->getStartTime() + duration - motion->outdt() / speed);
		}
		RequestMark* emphasisMark = new RequestMark();
		emphasisMark->setName("emphasis");
		motionBlock->addMark(emphasisMark);
		emphasisMark->setStartTime(motion->emphasist() / speed + triggerTime);
		emphasisMark->setEndTime(motion->emphasist() / speed + triggerTime);
		*/

		// show the current and original timings in the info box
		SkMotion* skmotion = motion->motion();
		std::stringstream strstr;
		strstr << motion->getName() << "\n";
		strstr << "File: " << skmotion->filename() << "\n\n";
		strstr << "Original timings:\n";
		strstr << "Ready       : " << skmotion->time_ready() << "\n";
		strstr << "StrokeStart : " << skmotion->time_stroke_start() << "\n";
		strstr << "Stroke      : " << skmotion->time_stroke_emphasis() << "\n";
		strstr << "StrokeEnd   : " << skmotion->time_stroke_end() << "\n";
		strstr << "Relax       : " << skmotion->time_relax() << "\n";
		strstr << "\n";
		strstr << "indt        : " << motion->indt() << "\n";
		strstr << "emphasis    : " << motion->emphasist() << "\n";
		strstr << "outdt       : " << motion->outdt() << "\n";
		strstr << "\n";

		strstr << "Current timings:\n";
		strstr << "Ready       : " << syncTimes[1] - triggerTime << "\n";
		strstr << "StrokeStart : " <<  syncTimes[2] - triggerTime << "\n";
		strstr << "Stroke      : " <<  syncTimes[3] - triggerTime << "\n";
		strstr << "StrokeEnd   : " <<  syncTimes[4] - triggerTime << "\n";
		strstr << "Relax       : " <<  syncTimes[5] - triggerTime << "\n";
		strstr << "\n";
		
		strstr << "Speedups:\n";
		if (skmotion->time_ready() == 0 || phaseTimes[0] == 0)
			strstr << "Start-Ready        : 1\n";
		else
			strstr << "Start-Ready        : " << skmotion->time_ready() / phaseTimes[0] << "\n";
	
		if ( skmotion->time_stroke_start() - skmotion->time_ready() == 0 || phaseTimes[1] == 0)
			strstr << "Ready-StrokeStart  : 1\n";
		else
			strstr << "Ready-StrokeStart  : " <<  ( skmotion->time_stroke_start() - skmotion->time_ready()) / phaseTimes[1] << "\n";

		if (skmotion->time_stroke_emphasis() - skmotion->time_stroke_start() == 0 || phaseTimes[2] == 0)
			strstr << "StrokeStart-Stroke : 1\n";
		else
			strstr << "StrokeStart-Stroke : " << ( skmotion->time_stroke_emphasis() - skmotion->time_stroke_start()) / phaseTimes[2]  << "\n";

		if (skmotion->time_stroke_end() - skmotion->time_stroke_emphasis() == 0 || phaseTimes[3] == 0)
			strstr << "Stroke-StrokeEnd   : 1\n";
		else
			strstr << "Stroke-StrokeEnd   : " << ( skmotion->time_stroke_end() - skmotion->time_stroke_emphasis()) / phaseTimes[3]  << "\n";

		if (skmotion->time_relax() - skmotion->time_stroke_end() == 0 || phaseTimes[4] == 0)
			strstr << "StrokeEnd-Relax    : 1\n";
		else
			strstr << "StrokeEnd-Relax    : " << ( skmotion->time_relax() - skmotion->time_stroke_end()) / phaseTimes[4] << "\n";

		if (skmotion->duration() - skmotion->time_relax() == 0 || phaseTimes[5] == 0)
			strstr << "Relax-End          : 1\n";
		else
			strstr << "Relax-End          : " << ( skmotion->duration() - skmotion->time_relax()) / phaseTimes[5] << "\n";
	

		//motionBlock->setInfo(strstr.str());
		block->setInfo(strstr.str());
	}

}


void BehaviorWindow::processControllerRequest(BML::MeControllerRequest* controllerRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
											  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
											  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks)
{
	MeController* controller = controllerRequest->anim_ct;
	
	MeCtGaze* gazeController = dynamic_cast<MeCtGaze*>(controller);
	if (gazeController)
	{
		RequestTrack* track = new RequestTrack();
		track->setName(controller->controller_type());
		model->addTrack(track);
		RequestBlock* block = new RequestBlock();
		block->setName("gazer");
		track->addBlock(block);
		block->setStartTime(triggerTime);
		block->setEndTime(triggerTime + 1);
	//	block->setColor(FL_GREEN);
		// find the gaze target
		float x, y, z;
		SkJoint* joint = gazeController->get_target_joint(x, y, z);
		std::stringstream strstr;
		if (joint)
		{
			strstr << joint->skeleton()->getName() << "(" << x << ", " << y << ", " << z << ")";
			block->setName(strstr.str());
		}
		// gaze parameters
		std::stringstream gazestr;
		//gazestr << gazeController->print

	}

	BML::NodRequest* nodRequest = dynamic_cast<BML::NodRequest*>(behavior);
	if (nodRequest)
	{
		RequestTrack* nodTrack = new RequestTrack();
		nodTrack->setName("nod");
		model->addTrack(nodTrack);
		NodBlock* nodBlock = new NodBlock();
		nodBlock->setName(behavior->unique_id);
		nodBlock->setShowName(false);
		nodTrack->addBlock(nodBlock);

		nodBlock->setStartTime(triggerTime);
		nodBlock->setEndTime(triggerTime + constantSpeedScheduler->endTime);

		BML::BehaviorSyncPoints syncPoints = behavior->behav_syncs;
		const std::map<std::wstring, std::wstring>& behaviorToNameMap = syncPoints.getBehaviorToSyncNames();
		
		{
			RequestMark* readyMark = new RequestMark();
			readyMark->setName("ready");
			readyMark->setStartTime(triggerTime + constantSpeedScheduler->readyTime);
			readyMark->setEndTime(readyMark->getStartTime());
			nodBlock->addMark(readyMark);
			const std::map<std::wstring, std::wstring>::const_iterator biter = behaviorToNameMap.find(L"ready");
			if (biter != behaviorToNameMap.end())		
			{
				std::string spName((*biter).second.begin(), (*biter).second.end());
				untimedMarks.push_back(std::pair<RequestMark*, std::string>(readyMark, spName));
			}
		}

		{
			RequestMark* strokeMark = new RequestMark();
			strokeMark->setName("stroke");
			strokeMark->setStartTime(triggerTime + constantSpeedScheduler->strokeTime);
			strokeMark->setEndTime(strokeMark->getStartTime());
			nodBlock->addMark(strokeMark);
			const std::map<std::wstring, std::wstring>::const_iterator biter = behaviorToNameMap.find(L"stroke");	
			if (biter != behaviorToNameMap.end())		
			{
				std::string spName((*biter).second.begin(), (*biter).second.end());
				untimedMarks.push_back(std::pair<RequestMark*, std::string>(strokeMark, spName));
			}
		}

		{
			RequestMark* relaxMark = new RequestMark();
			relaxMark->setName("relax");
			relaxMark->setStartTime(triggerTime + constantSpeedScheduler->relaxTime);
			relaxMark->setEndTime(relaxMark->getStartTime());
			nodBlock->addMark(relaxMark);
			const std::map<std::wstring, std::wstring>::const_iterator biter = behaviorToNameMap.find(L"relax");
			if (biter != behaviorToNameMap.end())		
			{
				std::string spName((*biter).second.begin(), (*biter).second.end());
				untimedMarks.push_back(std::pair<RequestMark*, std::string>(relaxMark, spName));
			}
		}

		{
			RequestMark* endMark = new RequestMark();
			endMark->setName("end");
			endMark->setStartTime(triggerTime + constantSpeedScheduler->endTime);
			endMark->setEndTime(endMark->getStartTime());
			nodBlock->addMark(endMark);
			const std::map<std::wstring, std::wstring>::const_iterator biter = behaviorToNameMap.find(L"end");
			if (biter != behaviorToNameMap.end())		
			{
				std::string spName((*biter).second.begin(), (*biter).second.end());
				untimedMarks.push_back(std::pair<RequestMark*, std::string>(endMark, spName));
			}
		}
	}

	BML::PostureRequest* postureRequest = dynamic_cast<BML::PostureRequest*>(behavior);
	if (postureRequest)
	{
		RequestTrack* track = new RequestTrack();
		track->setName(controller->controller_type());
		model->addTrack(track);
		MeController* animController = postureRequest->anim_ct;
		MeCtMotion* motion = dynamic_cast<MeCtMotion*>(animController);
		if (motion)
		{ 
			RequestTrack* track = new RequestTrack();
			track->setName("Posture");
			model->addTrack(track);
			MotionBlock* motionBlock = new MotionBlock();
			motionBlock->setName(behavior->unique_id);
			motionBlock->setInfo(motion->getName());
			track->addBlock(motionBlock);
			double duration = motion->controller_duration();
			if (duration < 0)
			{
				// if duration < 0, this motion is looped,
				// but I'd like to see the duration of the original motion
				// regardless
				SkMotion* skMotion = motion->motion();
				duration = skMotion->duration();
				motionBlock->setColor(FL_DARK3);

			}
			motionBlock->setStartTime(triggerTime);
			motionBlock->setEndTime(triggerTime + duration);
			RequestMark* inMark = new RequestMark();
			inMark->setName("in");
			inMark->setStartTime(motion->indt() + triggerTime);
			inMark->setEndTime(motion->indt() + triggerTime);
			motionBlock->addMark(inMark);
			RequestMark* outMark = new RequestMark();
			outMark->setName("out");
			if (motion->outdt() >= motion->indt())
			{
				outMark->setStartTime(motion->outdt() + triggerTime);
				outMark->setEndTime(motion->outdt() + triggerTime);
			}
			motionBlock->addMark(outMark);
		}
	}
}

void BehaviorWindow::processSpeechRequest(BML::SpeechRequest* speechRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
										  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
										  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks)
{
	RequestTrack* track = new RequestTrack();
	track->setName("speech");
	model->addTrack(track);
	RequestBlock* block = new RequestBlock();
	block->setName(speechRequest->unique_id);
	block->setShowName(false);
	block->setInfo(speechRequest->unique_id);
	track->addBlock(block);
	block->setStartTime(triggerTime);
	block->setEndTime(triggerTime + 1); // this should be based on sound duration
	block->setColor(FL_GREEN);

	SmartBody::SpeechInterface* speechInterface = speechRequest->get_speech_interface();
	char* audioFilename = speechInterface->getSpeechAudioFilename(speechRequest->get_speech_request_id());
	SmartBody::AudioFileSpeech* audioSpeechInterface = dynamic_cast<SmartBody::AudioFileSpeech*>(speechInterface);
	char* playCommand = speechInterface->getSpeechPlayCommand(speechRequest->get_speech_request_id());
	if (audioSpeechInterface)
	{
		RequestTrack* visemeTrack = new RequestTrack();
		visemeTrack->setName("viseme");
		model->addTrack(visemeTrack);
		RequestBlock* visemeBlock = new RequestBlock();
		visemeBlock->setName(speechRequest->unique_id);
		visemeBlock->setStartTime(triggerTime);
		visemeBlock->setShowName(false);
		visemeTrack->addBlock(visemeBlock);
		std::stringstream strstr;
		strstr << "Visemes:\n";
		float lastTime = 0;
		const std::vector<SmartBody::VisemeData *>* visemes = audioSpeechInterface->getVisemes(speechRequest->get_speech_request_id(), NULL);
		for (unsigned int v = 0; v < visemes->size(); v++)
		{
			SmartBody::VisemeData* viseme = (*visemes)[v];
			const char* id =  viseme->id();
			float visemeTime = viseme->time();
			float weight = viseme->weight();
			float blendDuration = viseme->duration();
			strstr << id << " " << visemeTime << " " << weight << " " << blendDuration << "\n";
			RequestMark* visemeMark = new RequestMark();
			visemeMark->setName(id);
			visemeMark->setStartTime(triggerTime + visemeTime);
			visemeMark->setEndTime(visemeMark->getStartTime());
			std::stringstream strstr;
			strstr << "Id = " << id << std::endl << "Time = " << visemeTime << std::endl << "Weight = " << weight << std::endl << "Duration = " << blendDuration;
			visemeMark->setInfo(strstr.str());
			if (weight == 0.0)
				visemeMark->setColor(FL_RED);
			else if (weight == 1.0)
				visemeMark->setColor(FL_GREEN);

			visemeBlock->addMark(visemeMark);
			if (visemeTime > lastTime)
				lastTime = visemeTime;
		}
		visemeBlock->setEndTime(triggerTime + lastTime);
		block->setEndTime(triggerTime + lastTime);
		visemeBlock->setInfo(strstr.str());


		std::map< SmartBody::RequestId, SmartBody::AudioFileSpeech::SpeechRequestInfo >& speechRequestInfo = audioSpeechInterface->getSpeechRequestInfo();
		for (std::map< SmartBody::RequestId, SmartBody::AudioFileSpeech::SpeechRequestInfo >::iterator iter = speechRequestInfo.begin();
			iter != speechRequestInfo.end();
			iter++)
		{
			RequestTrack* timeMarkerTrack = new RequestTrack();
			timeMarkerTrack->setName("speechtimemarkers");
			model->addTrack(timeMarkerTrack);
			RequestBlock* timeMarkerBlock = new RequestBlock();
			timeMarkerBlock->setName(speechRequest->unique_id);
			timeMarkerTrack->addBlock(timeMarkerBlock);
			timeMarkerBlock->setStartTime(triggerTime);
			timeMarkerBlock->setEndTime(triggerTime + lastTime);
			timeMarkerBlock->setShowName(false);

			SmartBody::RequestId reqId = (*iter).first;
			SmartBody::AudioFileSpeech::SpeechRequestInfo& info = (*iter).second;
			std::stringstream speechTimeStr;
			speechTimeStr << "Sync Points\n";
			for(std::map< std::string, float >::iterator markerIter = info.timeMarkers.begin();
				markerIter != info.timeMarkers.end();
				markerIter++)
			{
				std::string markerName = (*markerIter).first;
				float markerTime = (*markerIter).second;
				RequestMark* timeMark = new RequestMark();
				timeMark->setName(markerName);
				timeMark->setStartTime(triggerTime + markerTime);
				timeMark->setEndTime(timeMark->getStartTime());
				timeMarkerBlock->addMark(timeMark);
				speechTimeStr << markerName << " " << markerTime << "\n";
				// add these times to the syncMap so that we can use them 
				// to calculate other timings
				std::string syncMapName = speechRequest->local_id;
				syncMapName.append(":");
				syncMapName.append(markerName);
				syncMap.insert(std::pair<std::string, double>(syncMapName, triggerTime + markerTime));
			}
			timeMarkerBlock->setInfo(speechTimeStr.str());
		}
	}
	else
	{
		text_speech* textSpeech = dynamic_cast<text_speech*>(speechInterface);
		if (textSpeech)
		{
				int x = 2;
		}
		else
		{
			remote_speech* remoteSpeech = dynamic_cast<remote_speech*>(speechInterface);
			if (remoteSpeech)
			{
				std::stringstream strstr;
				strstr << "Visemes:\n";
				RequestTrack* visemeTrack = new RequestTrack();
				visemeTrack->setName("viseme");
				model->addTrack(visemeTrack);
				RequestBlock* visemeBlock = new RequestBlock();
				visemeBlock->setName(speechRequest->unique_id);
				visemeBlock->setStartTime(triggerTime);
				visemeBlock->setShowName(false);
				visemeTrack->addBlock(visemeBlock);
				float lastTime = 0;
				std::vector<SmartBody::VisemeData*>& visemes = speechRequest->getVisemes();
				for (unsigned int v = 0; v < visemes.size(); v++)
				{
					SmartBody::VisemeData* viseme = visemes[v];
					const char* id =  viseme->id();
					float visemeTime = viseme->time();
					float weight = viseme->weight();
					float blendDuration = viseme->duration();
					strstr << id << " " << visemeTime << " " << weight << " " << blendDuration << "\n";
					RequestMark* visemeMark = new RequestMark();
					visemeMark->setName(id);
					visemeMark->setStartTime(triggerTime + visemeTime);
					visemeMark->setEndTime(visemeMark->getStartTime());
					std::stringstream strstr;
					strstr << "Id = " << id << std::endl << "Time = " << visemeTime << std::endl << "Weight = " << weight << std::endl << "Duration = " << blendDuration;
					visemeMark->setInfo(strstr.str());
					if (weight == 0.0)
						visemeMark->setColor(FL_RED);
					else if (weight == 1.0)
						visemeMark->setColor(FL_GREEN);

					visemeBlock->addMark(visemeMark);
					if (visemeTime > lastTime)
						lastTime = visemeTime;
				}
				visemeBlock->setEndTime(triggerTime + lastTime);
				block->setEndTime(triggerTime + lastTime);
				visemeBlock->setInfo(strstr.str());

				RequestTrack* timeMarkerTrack = new RequestTrack();
				timeMarkerTrack->setName("speechtimemarkers");
				model->addTrack(timeMarkerTrack);
				RequestBlock* timeMarkerBlock = new RequestBlock();
				timeMarkerBlock->setName(speechRequest->unique_id);
				timeMarkerTrack->addBlock(timeMarkerBlock);
				timeMarkerBlock->setStartTime(triggerTime);
				timeMarkerBlock->setEndTime(triggerTime + lastTime);
				timeMarkerBlock->setShowName(false);
				std::stringstream speechTimeStr;
				speechTimeStr << "Sync Points\n";
				for (BML::BehaviorSyncPoints::iterator bhIter = speechRequest->behav_syncs.begin(); 
					bhIter !=  speechRequest->behav_syncs.end();
					bhIter++)
				{
					BML::SyncPointPtr syncPtr = (*bhIter).sync();
					BML::SyncPoint* sp = syncPtr.get();
					double time = (*bhIter).time();
					std::wstring markerNameW = (*bhIter).name();
					std::string markerName(markerNameW.begin(), markerNameW.end());
					RequestMark* timeMark = new RequestMark();
					timeMark->setName(markerName);
					timeMarkerBlock->addMark(timeMark);
					if (time == time)
					{
						double markerTime = time;
						timeMark->setStartTime(markerTime);
						timeMark->setEndTime(timeMark->getStartTime());
						speechTimeStr << markerName << " " << markerTime << "\n";
						std::string prefixMarker = speechRequest->local_id;
						if (prefixMarker.size() > 0)
							prefixMarker.append(":");
						prefixMarker.append(markerName);
						XMLCh tempStr[100];
						XMLString::transcode(markerName.c_str(), tempStr, 99);
						double markTimeFromInterface = speechInterface->getMarkTime(speechRequest->get_speech_request_id(), tempStr);
						//XMLString::release(&tempStr);
						if (markTimeFromInterface == -1.0 || fabs(markTimeFromInterface + triggerTime - markerTime) > .001)
						{
							LOG("Interface time %f does not match sync point time %f", (markTimeFromInterface + triggerTime), markerTime);
						}
						
						// add these times to the syncMap so that we can use them 
						// to calculate other timings
						std::string syncMapName = speechRequest->local_id;
						syncMapName.append(":");
						syncMapName.append(markerName);
						syncMap.insert(std::pair<std::string, double>(syncMapName, markerTime));
					}
					else
					{
						std::string prefixMarker = speechRequest->local_id;
						if (prefixMarker.size() > 0)
							prefixMarker.append(":");
						prefixMarker.append(markerName);
						XMLCh tempStr[100];
						XMLString::transcode(prefixMarker.c_str(), tempStr, 99);
						float markTime = speechInterface->getMarkTime(speechRequest->get_speech_request_id(), tempStr);
						speechTimeStr << markerName << " " << markTime << "\n";
						if (markTime != -1)
						{
							timeMark->setStartTime(triggerTime + markTime);
							timeMark->setEndTime(timeMark->getStartTime());
							
							// add these times to the syncMap so that we can use them 
							// to calculate other timings
							std::string syncMapName = speechRequest->local_id;
							syncMapName.append(":");
							syncMapName.append(markerName);
							syncMap.insert(std::pair<std::string, double>(syncMapName, triggerTime + markTime));
						}
						else
						{
							untimedMarks.push_back(std::pair<RequestMark*, std::string>(timeMark, timeMark->getName()));
						}
					}

				}
				timeMarkerBlock->setInfo(speechTimeStr.str());

			}

		}
	}

	std::stringstream speechStr;
	speechStr << block->getInfo() << std::endl << audioFilename << std::endl;
	if (playCommand)
		speechStr << playCommand;
	block->setInfo(speechStr.str());

	BML::MapOfSyncPoint& wordBreaks = speechRequest->getWorkBreakSync();
	int counter = 0;
	for (BML::MapOfSyncPoint::iterator wordBreakIter = wordBreaks.begin();
		 wordBreakIter != wordBreaks.end();
		 wordBreakIter++)
	{
		BML::SyncPoint* syncPoint = (*wordBreakIter).second.get();
		RequestMark* mark = new RequestMark();
		std::string name((*wordBreakIter).first.begin(), (*wordBreakIter).first.end());
		mark->setName(name);
		double time = syncPoint->time;
		double offset = syncPoint->offset;
		if (time != time)
		{
			time = 0; // word breaks should have times set
			std::string prefixMarker = speechRequest->local_id;
			if (prefixMarker.size() > 0)
				prefixMarker.append(":");
			prefixMarker.append(name);
			untimedMarks.push_back(std::pair<RequestMark*, std::string>(mark, prefixMarker));
		}
		//mark->setStartTime(triggerTime + time);
		//mark->setEndTime(triggerTime + time);
		// word breaks use absolute time?
		mark->setStartTime(time);
		mark->setEndTime(time);
		
		block->addMark(mark);
		counter++;
	}
}

void BehaviorWindow::processEventRequest(BML::EventRequest* eventRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
										  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
										  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks)
{
	RequestTrack* eventTrack = new RequestTrack();
	eventTrack->setName("event");
	EventBlock* eventBlock = new EventBlock();
	eventBlock->setName(eventRequest->getMessage());
	eventBlock->setShowName(false);
	eventBlock->setInfo(eventRequest->getMessage());
	eventBlock->setStartTime(triggerTime);
	eventBlock->setEndTime(triggerTime + 1);
	eventBlock->setColor(FL_WHITE);
	eventTrack->addBlock(eventBlock);
	model->addTrack(eventTrack);

	std::string syncPointName = eventRequest->getSyncPointName();
	RequestMark* syncPointMark = new RequestMark();
	syncPointMark->setName(syncPointName);
	eventBlock->addMark(syncPointMark);
	untimedMarks.push_back(std::pair<RequestMark*, std::string> (syncPointMark, syncPointName));
}

void BehaviorWindow::processVisemeRequest(BML::VisemeRequest* visemeRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
										  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
										  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks)
{
	RequestTrack* eventTrack = new RequestTrack();
	eventTrack->setName("Viseme");
	EventBlock* eventBlock = new EventBlock();
	eventBlock->setName(visemeRequest->getVisemeName());
	eventBlock->setStartTime(triggerTime + constantSpeedScheduler->startTime);
	eventBlock->setEndTime(triggerTime + constantSpeedScheduler->endTime);
	eventBlock->setColor(FL_WHITE);
	eventTrack->addBlock(eventBlock);
	model->addTrack(eventTrack);

	std::stringstream strstr;
	strstr << "Weight    = " << visemeRequest->getWeight() << "\n";
	strstr << "Duration  = " << visemeRequest->getDuration() << "\n";
	strstr << "Ramp up   = " << visemeRequest->getRampUp() << "\n";
	strstr << "Ramp down = " << visemeRequest->getRampDown() << "\n";
	eventBlock->setInfo(strstr.str());

	RequestMark* rampUpMark = new RequestMark();
	rampUpMark->setName("rampup");
	rampUpMark->setStartTime(eventBlock->getStartTime() +  visemeRequest->getRampUp());
	rampUpMark->setEndTime(rampUpMark->getStartTime());
	eventBlock->addMark(rampUpMark);

	RequestMark* rampDownMark = new RequestMark();
	rampDownMark->setName("rampdown");
	rampDownMark->setStartTime(eventBlock->getEndTime() - visemeRequest->getRampDown());
	rampDownMark->setEndTime(rampDownMark->getStartTime());
	eventBlock->addMark(rampDownMark);
	
}

void BehaviorWindow::adjustSyncPoints(BML::BehaviorRequest* behavior, nle::Block* block, std::map<std::string, double>& syncMap)
{
	std::string stages[] = { "start", "ready", "stroke_start", "stroke", "stroke_end", "relax", "end" };
	bool hasTiming[] =     { false,   false,   false,          false,    false,        false,   false };
	bool overrides[] =     { false,   false,   false,          false,    false,        false,   false };
	double origTiming[] =   {     0,       0,       0,              0,        0,            0,       0 };
	double newTiming[] =    {     0,       0,       0,              0,        0,            0,       0 };
	
	// determine which sync points are present and have legitimate timings
	for (int x = 0; x < 7; x ++)
	{
		nle::Mark* mark = block->getMark(stages[x]);
		if (mark)
			if (mark->getStartTime() == mark->getStartTime())
			{
				hasTiming[x] = true;
				origTiming[x] = mark->getStartTime();
			}
	}
	
	const std::map<std::wstring, std::wstring>& behaviorToNameMap = behavior->behav_syncs.getBehaviorToSyncNames();

	// first pass, get the proper timings
	for (std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.begin();
		iter != behaviorToNameMap.end();
		iter++)
	{
		std::string syncPointName((*iter).first.begin(), (*iter).first.end());
		std::string syncPointTiming((*iter).second.begin(), (*iter).second.end());

		std::map<std::string, double>::iterator mapIter = syncMap.find(syncPointTiming);
		if (mapIter != syncMap.end())
		{
			// find the mark associated with this sync point
			RequestMark* syncPointMark = dynamic_cast<RequestMark*>(block->getMark(syncPointName));
			if (syncPointMark)
			{
				syncPointMark->setStartTime((*mapIter).second);
				syncPointMark->setEndTime(syncPointMark->getStartTime());
			}
			else
			{
				// did not find mark - this is a problem!
				syncPointMark = new RequestMark();
				syncPointMark->setName(syncPointName);
				syncPointMark->setStartTime((*mapIter).second);
				syncPointMark->setEndTime(syncPointMark->getStartTime());
				block->addMark(syncPointMark);
			}
			for (int x = 0; x < 7; x ++)
			{
				if (syncPointName == stages[x])
				{
					newTiming[x] = (*mapIter).second;
				}
			}
		}
	}

	// second pass, adjust the other timings accordingly
	for (std::map<std::wstring, std::wstring>::const_iterator iter = behaviorToNameMap.begin();
		iter != behaviorToNameMap.end();
		iter++)
	{
		std::string syncPointName((*iter).first.begin(), (*iter).first.end());
		for (int x = 0; x < 7; x++)
			if (syncPointName == stages[x])
				overrides[x] = true;
	}

	for (int x = 0; x < 7; x++)
	{
		if (overrides[x])
		{
			double timingDiff = newTiming[x] - origTiming[x];
			for (int y = x + 1; y < 7; y++)
			{
				if (overrides[y])
					break;

				if (hasTiming[y])
				{
					nle::Mark* mark = block->getMark(stages[y]);
					if (mark)
					{
						if (!hasTiming[x])
						{
							mark->setStartTime(newTiming[x]);
							mark->setEndTime(newTiming[x]);
						}
						else
						{
							double oldTime = mark->getStartTime();
							double newTime = oldTime + timingDiff;
							mark->setStartTime(newTime);
							mark->setEndTime(newTime);
						}
						
					}
			
				}
			}
			
		}
	}


	// adjust the block to min/max of marks
	double minTime = 9999999;
	double maxTime = -999999;
	for (int m = 0; m < block->getNumMarks(); m++)
	{
		nle::Mark* mark = block->getMark(m);
		if (mark->getStartTime() < minTime)
			minTime = mark->getStartTime();
		if (mark->getEndTime() > maxTime)
			maxTime = mark->getEndTime();
	}
	block->setStartTime(minTime);
	block->setEndTime(maxTime);

}

BehaviorViewerFactory::BehaviorViewerFactory()
{
}

GenericViewer* BehaviorViewerFactory::create(int x, int y, int w, int h)
{
	BehaviorWindow* behaviorWindow = new BehaviorWindow(x, y, w, h, (char*)"Behavior Requests");
	return behaviorWindow;
}

void BehaviorViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
	viewer = NULL;
}


void BehaviorWindow::OnRequest(BML::BmlRequest* request, void* data)
{
	BehaviorWindow* behaviorWindow = (BehaviorWindow*) data;
	behaviorWindow->updateBehaviors(request);
}
