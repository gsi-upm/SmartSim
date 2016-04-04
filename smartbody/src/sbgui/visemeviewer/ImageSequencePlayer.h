#ifndef _IMAGESEQUENCEPLAYER_H_
#define _IMAGESEQUENCEPLAYER_H_

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <vector>
#include <SBWindowListener.h>

class SbmTexture;
class ImageSequencePlayer : public Fl_Gl_Window
{
public:
	ImageSequencePlayer(int x, int y, int w, int h, char* name);
	~ImageSequencePlayer();

	virtual void draw();
	virtual void resize(int x, int y, int w, int h);

	void initOpenGL();
	void updateImageSequences(std::vector<std::string>& imageFullPathes);
	void renderTexture(int index);

public:
	SbmTexture*				_activeTexture;
	std::vector<SbmTexture*> _imageSequences;
};

class ImageSequenceViewer : public Fl_Double_Window, SBWindowListener
{
public:
	ImageSequenceViewer(int x, int y, int w, int h, char* name);
	~ImageSequenceViewer();

	void show();
	void hide();
	void draw();
	virtual void OnSimulationUpdate();

	void playbackSequence(float startTime, float length, float delay = 0.0f);

public:
	Fl_Input* _inputImageSequenceFolder;
	Fl_Value_Slider* _sliderSequenceIndex;
	Fl_Button* _buttonPlay;
	Fl_Input* _inputPlayTimeDelay;
	Fl_Input* _inputAudioLength;
	ImageSequencePlayer* _imageSequencePlayer;

	float _startTime;
	float _playbackLength;

public:
	static void OnInputFolder(Fl_Widget* widget, void* data);
	static void OnSlideSequence(Fl_Widget* widget, void* data);
	static void OnButtonOnePlay(Fl_Widget* widget, void* data);

};

#endif