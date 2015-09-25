#include "VisemeRTCurveViewer.h"
#include "VisemeRunTimeWindow.h"
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sb/SBScene.h>
VisemeRTCurveViewer::VisemeRTCurveViewer(int x, int y, int w, int h, char* name, RunTimeCurveData* data) : Fl_Gl_Window(x, y, w, h)
{
	colorTable[0] = SrVec(1.0f, 0.0f, 0.0f);
	colorTable[1] = SrVec(0.0f, 1.0f, 0.0f);
	colorTable[2] = SrVec(0.0f, 0.0f, 1.0f);
	colorTable[3] = SrVec(1.0f, 1.0f, 0.0f);
	colorTable[4] = SrVec(0.0f, 1.0f, 1.0f);
	colorTable[5] = SrVec(1.0f, 0.0f, 1.0f);
	colorTable[6] = SrVec(0.5f, 0.5f, 0.5f);

	_data = data;
	xSpan = 3.0f;
	maxGridX = -1.0f;
	resetCamera();
}

VisemeRTCurveViewer::~VisemeRTCurveViewer()
{
	_data = NULL;
}


void VisemeRTCurveViewer::draw()
{
	if (!visible())
		return;
	if (!valid())
	{
		initOpenGL();
		initFont();
		valid(1);
	}
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// specifically for drawing font because you can't scale font separated on x and y axis
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((-0.1 + xShift) * zoom, (xSpan + 0.1 + xShift) * zoom, -1.1 * xSpan / 3.0f * zoom, 1.1 * xSpan / 3.0f * zoom, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawLabel();
	drawPhonemeNames();

	// camera for drawing others
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((-0.1 + xShift) * zoom, (xSpan + 0.1 + xShift) * zoom, -1.1, 1.1, -1, 1);
	drawAxisNGrid();
	drawCurves();
	drawPhonemeLines();
}


void VisemeRTCurveViewer::drawCurves()
{
	glDisable(GL_LIGHTING);
	for (size_t i = 0; i < _data->_curves.size(); ++i)
	{
		if (_data->_curves[i].display)
		{
			if (_data->_curves[i].highlight)
			{
				glLineWidth(3.0f);
				glPointSize(8.0f);
			}
			else
			{
				glLineWidth(1.0f);
				glPointSize(4.0f);
			}
			SrVec color = colorTable[_data->_curves[i].groupId % colorTable.size()];
			glColor3f(color.x, color.y, color.z);

			// lines
			glBegin(GL_LINES);
			std::vector<float>& c = _data->_curves[i].data;
			float prevX = 0.0f;
			float prevY = 0.0f;
			for (size_t j = 0; j < c.size() / 2; ++j)
			{
				if (j != 0)
				{

					glVertex3f(prevX, prevY, 0.0f);
					glVertex3f(c[j * 2 + 0], c[j * 2 + 1], 0.0f);
				}
				prevX = c[j * 2 + 0];
				prevY = c[j * 2 + 1];
			}
			glEnd();

			// points
			glBegin(GL_POINTS);
			for (size_t j = 0; j < c.size() / 2; ++j)
			{
				glVertex3f(c[j * 2 + 0], c[j * 2 + 1], 0.0f);
			}
			glEnd();
		}
	}
	glEnable(GL_LIGHTING);
}

int VisemeRTCurveViewer::handle(int event)
{
	float prevZoom = zoom;
	float dragDy = 0.0f;
	switch (event)
	{ 
	case FL_MOUSEWHEEL:
		zoom += (float)Fl::event_dy() * zoomResolution;
		if (zoom < 0)
			zoom = prevZoom;
		redraw();
		break;

	case FL_RELEASE:
		mouseCoordInit = false;
	case FL_DRAG:
		if (Fl::event_state(FL_BUTTON1))
		{
			if (!mouseCoordInit)
				mouseCoordInit = true;
			else
			{
				float deltaX = (float)Fl::event_x() - prevMouseCoord.x;
				float deltaY = (float)Fl::event_y() - prevMouseCoord.y;
				xShift -= deltaX * 0.01f / zoom;
			}
			prevMouseCoord.x = (float)Fl::event_x();
			prevMouseCoord.y = (float)Fl::event_y();
		}
		redraw();
		break;
	default:
		break;
	}

	if (event == FL_PUSH)
		return 1;

	return Fl_Gl_Window::handle(event);
}

void VisemeRTCurveViewer::resize(int x, int y, int w, int h)
{
	Fl_Gl_Window::resize(x, y, w, h);
}

void VisemeRTCurveViewer::initOpenGL()
{
	glViewport(0, 0, w(), h());
	glEnable ( GL_DEPTH_TEST );
	glEnable ( GL_LIGHT0 ); 
	glEnable ( GL_LIGHTING );
	glCullFace ( GL_BACK );
	glDepthFunc ( GL_LEQUAL );
	glFrontFace ( GL_CCW );
	glEnable ( GL_POLYGON_SMOOTH );
	glEnable ( GL_POINT_SMOOTH );
	glPointSize ( 2.0 );
	glShadeModel ( GL_SMOOTH );
}

void VisemeRTCurveViewer::initFont()
{
	GLint src;
	glGetIntegerv(GL_BLEND_SRC, &src);
	GLint dest;
	glGetIntegerv(GL_BLEND_DST, &dest);

	glPushAttrib(GL_ENABLE_BIT);
	GLuint textureName;	
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(1, &textureName);

#ifdef WIN32
	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string fontPath = mediaPath + "/" +  "fonts/font.glf";
	if (!label.Create(fontPath.c_str(), textureName))
	{
		if(!label.Create(".font.glf", textureName))
			LOG("GlChartViewCoordinate::InitFont(): Error: Cannot load font file\n");
	}
#endif
	glPopAttrib();
	glBlendFunc(src, dest);
}

void VisemeRTCurveViewer::resetCamera()
{
	mouseCoordInit = false;
	zoom = 1.0f;
	xShift = 0.0f;
	setZoomRes(0.05f);
	redraw();
}

float VisemeRTCurveViewer::getMaxX()
{
	maxGridX = -1.0f;
	if (_data)
	{
		for (size_t i = 0; i < _data->_phonemeTimes.size(); ++i)
		{
			if (maxGridX <= (float)_data->_phonemeTimes[i])
				maxGridX = (float)_data->_phonemeTimes[i];
		}
	}
	float maxX = maxGridX > xSpan * zoom ? maxGridX : xSpan * zoom;
	return maxX;
}

void VisemeRTCurveViewer::drawAxisNGrid()
{
	float maxX = getMaxX(); 
	float stepSizeX = xSpan * zoom / 10.0f;
	int numStepsX = int(maxX / stepSizeX);

	// variable grid size (but always 10 grid under same viewport)
	glDisable(GL_LIGHTING);

	glColor3f(0.3f, 0.3f, 0.3f);
	glLineWidth(0.05f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(4, 0xAAAA);
	glBegin(GL_LINES);
	for (int i = -10; i <= 10; ++i)
	{
		glVertex3f(0.0f, i * 0.1f, 0.0f);
		glVertex3f(maxX, i * 0.1f, 0.0f);
	}
	for (int i = 0; i <= numStepsX; ++i)
	{
		glVertex3f(i * stepSizeX, -1.0f, 0.0f);
		glVertex3f(i * stepSizeX, 1.0f, 0.0f);
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	// draw axis
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glVertex(SrVec(0.0f, 0.0f, 0.0f));
	glVertex(SrVec(maxX, 0.0f, 0.0f));
	glEnd();

	glEnable(GL_LIGHTING);
}

void VisemeRTCurveViewer::drawLabel()
{
#ifdef WIN32
	float maxX = getMaxX();
	char value[10];
	SrVec2 size;
	float fontScale = 0.0015f * xSpan / 3.0f;// 0.0015f is for xSpan = 3.0f
	float labelMargin = 0.01f * xSpan / 3.0f;

	glEnable(GL_TEXTURE_2D);
	glColor4f(0.4f, 0.4f, 0.4f, 0.3f);
	label.Begin();

	float stepSizeX = xSpan * zoom / 10.0f;
	int numStepsX = int(maxX / stepSizeX);

	// draw x axis label
	for (int i = 0; i <= numStepsX; ++i)
	{
		sprintf(value, "%.3f", i * stepSizeX);
		size = getStringSize(value) * fontScale;
		label.DrawString(value, fontScale * zoom, i * stepSizeX, -labelMargin * zoom);
	}
	glDisable(GL_TEXTURE_2D);
#endif
}

void VisemeRTCurveViewer::drawPhonemeNames()
{
#ifdef WIN32
	if (_data == NULL)
		return;

	if (_data->_phonemeNames.size() == 0)
		return;

	char value[10];
	char value1[10];
	SrVec2 size;
	float fontScale = 0.0015f * xSpan / 3.0f;// 0.0015f is for xSpan = 3.0f
	float labelMargin = 0.01f * xSpan / 3.0f;

	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 0.0f, 0.0f, 0.3f);

	// draw x axis label
	int l = 2;
	float prevX = -1.0f;
	float prevSizeX = 0.0f;
	for (size_t i = 0; i < _data->_phonemeNames.size(); ++i)
	{
		if (i != 0) // check if needed to draw on a second layer
		{
			l = 2;
			if ((float)_data->_phonemeTimes[i] < prevSizeX)
				l = 3;
		}

		sprintf(value, "%s", _data->_phonemeNames[i].c_str());
		sprintf(value1, "%.3f", _data->_phonemeTimes[i]);
		size = getStringSize(value) * fontScale;
		label.DrawString(value, fontScale * zoom, (float)_data->_phonemeTimes[i], (-labelMargin - size.y * l) * zoom);
		label.DrawString(value1, fontScale * zoom, (float)_data->_phonemeTimes[i], (-labelMargin - size.y * (l + 1)) * zoom);

		prevX = (float)_data->_phonemeTimes[i];
		prevSizeX = size.x + prevX;
	}
	glDisable(GL_TEXTURE_2D);
#endif
}

void VisemeRTCurveViewer::drawPhonemeLines()
{
#ifdef WIN32
	if (_data == NULL)
		return;

	if (_data->_phonemeNames.size() == 0)
		return;

	glDisable(GL_LIGHTING);
	glColor3f(0.8f, 0.8f, 0.8f);
	glLineWidth(0.2f);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0xAAAA);
	glBegin(GL_LINES);
	for (size_t i = 0; i < _data->_phonemeNames.size(); ++i)
	{
		glVertex3f((float)_data->_phonemeTimes[i], 1.0f, 0.0f);
		glVertex3f((float)_data->_phonemeTimes[i], -1.0f, 0.0f);
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_LIGHTING);
#endif
}

SrVec2 VisemeRTCurveViewer::getStringSize(const char* str)
{
	SrVec2 size;
#ifdef WIN32
	size.y = (float)label.GetCharHeightGL(str[0]);
#endif
	int length = strlen(str);
	for(int i = 0; i < length; ++i)
	{
#ifdef WIN32
		size.x += (float)label.GetCharWidthGL(str[i]);
#endif
	}
	return size;
}

void VisemeRTCurveViewer::setZoomRes(float val)
{
	zoomResolution = val;
}

float VisemeRTCurveViewer::getZoomRes()
{
	return zoomResolution;
}