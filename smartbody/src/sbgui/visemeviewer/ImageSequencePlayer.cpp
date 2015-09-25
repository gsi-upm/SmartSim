#include "vhcl.h"
#include "ImageSequencePlayer.h"
#include "external/glew/glew.h"
#include <sr/sr_gl.h>
#include <sbm/GPU/SbmTexture.h>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sbm/lin_win.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

ImageSequenceViewer::ImageSequenceViewer(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name), SBWindowListener()
{
	this->label(name);
	this->begin();
		_inputImageSequenceFolder = new Fl_Input(70, 10, 300, 20, "Input Folder:");
		_inputImageSequenceFolder->callback(OnInputFolder, this);
		_sliderSequenceIndex = new Fl_Value_Slider(180, 30, 300, 20);
		_sliderSequenceIndex->callback(OnSlideSequence, this);
		_sliderSequenceIndex->type(FL_HORIZONTAL);
		_sliderSequenceIndex->align(FL_ALIGN_LEFT);
		_buttonPlay = new Fl_Button(140, 30, 40, 20, "Play");	// this button play the whole sequence and stop when it's done
		_buttonPlay->callback(OnButtonOnePlay, this);
		_inputPlayTimeDelay = new Fl_Input(40, 30, 30, 20, "Delay");
		_inputPlayTimeDelay->value("1");
		_inputAudioLength = new Fl_Input(110, 30, 30, 20, "Length");
		_inputAudioLength->value("-1");

		_imageSequencePlayer = new ImageSequencePlayer(10, 60, 480, 430, "");
	this->end();

	_startTime = -1.0f;
	_playbackLength = -1.0f;
}

ImageSequenceViewer::~ImageSequenceViewer()
{

}

void ImageSequenceViewer::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}

void ImageSequenceViewer::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}

void ImageSequenceViewer::draw()
{
	_imageSequencePlayer->draw();
	Fl_Double_Window::draw();
}

void ImageSequenceViewer::OnSimulationUpdate()
{
	float curTime = (float)SmartBody::SBScene::getScene()->getSimulationManager()->getTime();
	if (curTime >= _startTime && _startTime >= 0.0 && _playbackLength > 0.0)
	{
		int numImages = _imageSequencePlayer->_imageSequences.size();
		float localPlayTime = curTime - _startTime;
		int id = int(localPlayTime * numImages / _playbackLength);
		//LOG("ImageSequenceViewer::OnSimulationUpdate local playback time %f(%d), total lenght %f", localPlayTime, id, _playbackLength);
		if (id >= (int)_imageSequencePlayer->_imageSequences.size())
		{
			_startTime = -1.0f;
			_playbackLength = -1.0f;
			_buttonPlay->activate();
			_sliderSequenceIndex->activate();
		}
		else
		{
			_imageSequencePlayer->renderTexture(id);
		}
	}

}

void ImageSequenceViewer::OnInputFolder(Fl_Widget* widget, void* data)
{
	ImageSequenceViewer* viewer = (ImageSequenceViewer*) data;
	std::string inputFolderPath = viewer->_inputImageSequenceFolder->value();
	boost::filesystem::path p(inputFolderPath);
	std::vector<std::string> imageFiles;

#if (BOOST_VERSION > 104400)
	boost::filesystem::path abs_p = boost::filesystem::absolute( p );	
	if( !boost::filesystem::exists( abs_p ))
#else
	boost::filesystem::path abs_p = boost::filesystem::complete( p );	
	if( !boost::filesystem2::exists( abs_p ))
#endif
	{
		return;
	}

	boost::filesystem::directory_iterator end;
	for( boost::filesystem::directory_iterator i(abs_p); i!=end; ++i ) 
	{
		const boost::filesystem::path& cur = *i;
		if (boost::filesystem::is_directory(cur)) 
		{
			;	
		} 
		else 
		{
			std::string ext = boost::filesystem::extension(cur);
			if (_stricmp(ext.c_str(), ".jpg" ) == 0
				|| _stricmp(ext.c_str(), ".png" ) == 0)
			{
#if (BOOST_VERSION > 104400)
				imageFiles.push_back(cur.string());
#else
				imageFiles.push_back(cur.string());
#endif
			}
		}
	}

	viewer->_imageSequencePlayer->updateImageSequences(imageFiles);
	viewer->_sliderSequenceIndex->maximum(imageFiles.size());
}

void ImageSequenceViewer::OnSlideSequence(Fl_Widget* widget, void* data)
{
	ImageSequenceViewer* viewer = (ImageSequenceViewer*) data;
	int index = (int)viewer->_sliderSequenceIndex->value();
	viewer->_sliderSequenceIndex->value(index);
	viewer->_imageSequencePlayer->renderTexture(index);
}

void ImageSequenceViewer::OnButtonOnePlay(Fl_Widget* widget, void* data)
{
	ImageSequenceViewer* viewer = (ImageSequenceViewer*) data;
	float length = viewer->_imageSequencePlayer->_imageSequences.size() / 30.0f;	// assuming fps = 30.0f
	double timeDelay = atof(viewer->_inputPlayTimeDelay->value());
	double inputLength = atof(viewer->_inputAudioLength->value());
	if (inputLength > 0)
		length = (float)inputLength;
	viewer->playbackSequence((float)SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), length, (float)timeDelay);
	viewer->_buttonPlay->deactivate();
	viewer->_sliderSequenceIndex->deactivate();
} 

void ImageSequenceViewer::playbackSequence(float startTime, float length, float delay)
{
	_startTime = startTime + delay;
	_playbackLength = length;
}


ImageSequencePlayer::ImageSequencePlayer(int x, int y, int w, int h, char* name) : Fl_Gl_Window(x, y, w, h, name)
{
	_activeTexture = new SbmTexture("active");
}

ImageSequencePlayer::~ImageSequencePlayer()
{
	if (_activeTexture)
		delete _activeTexture;
	_activeTexture = NULL;
}

void ImageSequencePlayer::draw()
{
	if (!visible())
		return;

	if (!valid())
	{
		initOpenGL();
		valid(1);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);

	if (_activeTexture->hasBuild())
	{
		glBindTexture(GL_TEXTURE_2D, _activeTexture->getID());

		int w = _activeTexture->getWidth();
		int h = _activeTexture->getHeight();
		int nChannels = _activeTexture->getNumChannels();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, _activeTexture->getBuffer());
		//gluBuild2DMipmaps(GL_TEXTURE_2D, nChannels, w, h, GL_RGB, GL_UNSIGNED_BYTE, _activeTexture->getBuffer());

		glBegin(GL_QUADS);
		float whratio = float(w) / float(h);
		if (whratio > 1)
		{
			float hwratio = 1.0f / whratio;
			glTexCoord2d(0.0,0.0); glVertex2d(0.0, (1.0 - hwratio) * 0.5);
			glTexCoord2d(1.0,0.0); glVertex2d(1.0, (1.0 - hwratio) * 0.5);
			glTexCoord2d(1.0,1.0); glVertex2d(1.0, 1.0 - (1.0 - hwratio) * 0.5);
			glTexCoord2d(0.0,1.0); glVertex2d(0.0, 1.0 - (1.0 - hwratio) * 0.5);
		}
		else
		{
			glTexCoord2d(0.0,0.0); glVertex2d((1.0 - whratio) * 0.5, 0.0);
			glTexCoord2d(1.0,0.0); glVertex2d(1.0 - (1.0 - whratio) * 0.5, 0.0);
			glTexCoord2d(1.0,1.0); glVertex2d((1.0 - whratio) * 0.5, 1.0);
			glTexCoord2d(0.0,1.0); glVertex2d(1.0 - (1.0 - whratio) * 0.5, 1.0);
		}
		glEnd();
	}
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void ImageSequencePlayer::resize(int x, int y, int w, int h)
{
	Fl_Gl_Window::resize(x, y, w, h);
	redraw();
}

void ImageSequencePlayer::initOpenGL()
{
	glViewport(0, 0, w(), h());
	glEnable( GL_DEPTH_TEST );
	glEnable ( GL_LIGHT0 ); 
	glEnable ( GL_LIGHTING );
	glCullFace ( GL_BACK );
	glDepthFunc ( GL_LEQUAL );
	glFrontFace ( GL_CCW );
	glEnable ( GL_POLYGON_SMOOTH );
	glEnable ( GL_POINT_SMOOTH );
	glShadeModel ( GL_SMOOTH );
}

// update _imageSequences data
void ImageSequencePlayer::updateImageSequences(std::vector<std::string>& imageFullPathes)
{
	if (imageFullPathes.size() == 0)
		return;

	// clear out image sequences
	for (size_t i = 0; i < _imageSequences.size(); ++i)
	{
		if (_imageSequences[i])
			delete _imageSequences[i];
		_imageSequences[i] = NULL;
	}
	_imageSequences.clear();

	for (size_t i = 0; i < imageFullPathes.size(); ++i)
	{
		std::string filebase = boost::filesystem::basename(imageFullPathes[i].c_str());
		SbmTexture* texture = new SbmTexture(filebase.c_str());
		if (!texture->loadImage(imageFullPathes[i].c_str()))
			continue;
		_imageSequences.push_back(texture);
	}

	renderTexture(0);
}

void ImageSequencePlayer::renderTexture(int index)
{
	if (index < 0 || index >=  (int)_imageSequences.size())
		return;

	_activeTexture->setBuffer(_imageSequences[index]->getBuffer(), _imageSequences[index]->getBufferSize());
	_activeTexture->setTextureSize(_imageSequences[index]->getWidth(), _imageSequences[index]->getHeight(), _imageSequences[index]->getNumChannels());
	if (!_activeTexture->hasBuild())
		_activeTexture->buildTexture(false);
	redraw();
}
