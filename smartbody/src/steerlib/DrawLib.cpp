//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file DrawLib.cpp
/// @brief Implements Util::DrawLib functionality.
///

#ifdef ENABLE_GUI

#include "util/DrawLib.h"
#include "util/Geometry.h"
#include <iostream>

using namespace Util;
using namespace std;


std::vector<GLuint> DrawLib::_displayLists;
int DrawLib::_currentDisplayListBeingWritten = -1;
int DrawLib::_agentDisplayList = -1;
int DrawLib::_flagDisplayList = -1;
GLUquadricObj * DrawLib::_quadric = NULL;
bool DrawLib::_initialized = false;

void DrawLib::init() {

	if (_initialized) return;

	_initialized = true;

	_displayLists.clear();

	// setup quadric
	_quadric = gluNewQuadric();
	gluQuadricNormals(_quadric, GLU_SMOOTH);
	gluQuadricTexture(_quadric, GL_TRUE);

	// create display lists
	_agentDisplayList = _buildAgentDisplayList();
	_flagDisplayList = _buildFlagDisplayList();

}

//
// setBackgroundColor()
//
void DrawLib::setBackgroundColor(const Color& color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
}


//
// enableLights()
//
void DrawLib::enableLights()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


//
// disableLights()
//
void DrawLib::disableLights()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}


//
// positionLights()
//
void DrawLib::positionLights()
{
	GLfloat position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}



//
// drawLine()
//
void DrawLib::drawLine(const Point & startPoint, const Point & endPoint)
{
	// draw line
	glBegin(GL_LINES);
	{
		glNormal(Vector(0.0f, 1.0f, 0.0f));
		glVertex(startPoint);
		glVertex(endPoint);
	}
	glEnd();
}


//
// drawQuad()
//
void DrawLib::drawQuad(const Point & a, const Point & b, const Point & c, const Point & d)
{
	// Assuming the quad is convex and planar.  that way, we only have to compute the normal once.
	Vector norm = normalize(cross((b-a),(d-a)));
	glBegin(GL_QUADS);
    {
		glNormal(norm);
		glVertex(a);
		glNormal(norm);
		glVertex(b);
		glNormal(norm);
		glVertex(c);
		glNormal(norm);
		glVertex(d);
    }
    glEnd();
}


//
// drawStar()
//
void DrawLib::drawStar(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	Vector forward = radius * normalize(dir);
	Vector side;
	side = cross(forward, Vector(0.0f, 1.0f, 0.0f));

	float theta = 3.1415926535f/4;

	DrawLib::glColor(color);

	// draw star
	drawQuad(pos-side, pos-forward, pos+side, pos+forward);
	drawQuad(pos - rotateInXZPlane(side,theta), pos - rotateInXZPlane(forward,theta), pos + rotateInXZPlane(side,theta), pos + rotateInXZPlane(forward,theta));
}



//
// drawFlag() - use only after the flag display lists are built.
//
void DrawLib::drawFlag(const Point & loc, const Color& color)
{
	glPushMatrix();
	{
		glTranslatef(loc.x, loc.y, loc.z);
		glColor3f(color.r, color.g, color.b);
		_drawDisplayList(_flagDisplayList);
	}
	glPopMatrix();
}


//
// drawAgentDisc() - use only after the agent display lists are built.
//
void DrawLib::drawAgentDisc(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	glPushMatrix();
	{
		float rad = atan2(dir.z, dir.x)*(-180.0f/3.14159265358979323846f);
		glColor(color);
		glTranslate(pos);
		glRotatef(rad,0.0f,1.0f,0.0f);
		glScalef(radius, radius, radius);

		_drawDisplayList(_agentDisplayList);
	}
	glPopMatrix();
}

//
// drawBox() - draws a box with wireframe 
//
void DrawLib::drawBox(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
	Point topLeft(xmin, ymin-0.01f, zmin);
	Point topRight(xmax, ymin-0.01f, zmin);
	Point botLeft(xmin, ymin-0.01f, zmax);
	Point botRight(xmax, ymin-0.01f, zmax);

	Point topLefth(xmin, ymax, zmin);
	Point topRighth(xmax, ymax, zmin);
	Point botLefth(xmin, ymax, zmax);
	Point botRighth(xmax, ymax, zmax);

	// upper plane
	DrawLib::drawQuad(botLefth, botRighth, topRighth, topLefth);

	// top/bot sides
	DrawLib::drawQuad(topLeft, topLefth, topRighth, topRight);
	DrawLib::drawQuad(botRight, botRighth, botLefth, botLeft);

	// left/right sides
	DrawLib::drawQuad(botLeft, botLefth, topLefth, topLeft);
	DrawLib::drawQuad(topRight, topRighth, botRighth, botRight);

}

//
// drawBoxWireFrame() - draws a wireframe of a box
//
void DrawLib::drawBoxWireFrame(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
	Point topLeft(xmin, ymin-0.01f, zmin);
	Point topRight(xmax, ymin-0.01f, zmin);
	Point botLeft(xmin, ymin-0.01f, zmax);
	Point botRight(xmax, ymin-0.01f, zmax);

	Point topLefth(xmin, ymax, zmin);
	Point topRighth(xmax, ymax, zmin);
	Point botLefth(xmin, ymax, zmax);
	Point botRighth(xmax, ymax, zmax);

	DrawLib::drawLine(topRight,topRighth);
	DrawLib::drawLine(topLeft,topLefth);
	DrawLib::drawLine(botLeft,botLefth);
	DrawLib::drawLine(botRight,botRighth);

	DrawLib::drawLine(topRighth, topLefth);
	DrawLib::drawLine(topLefth, botLefth);
	DrawLib::drawLine(botLefth, botRighth);
	DrawLib::drawLine(botRighth, topRighth);

	DrawLib::drawLine(topRight, topLeft);
	DrawLib::drawLine(topLeft, botLeft);
	DrawLib::drawLine(botLeft, botRight);
	DrawLib::drawLine(botRight, topRight);

}

//
// drawCylinder() - draws a cylinder 
//
void DrawLib::drawCylinder(const Point & pos, float radius, float ymin, float ymax )
{
	float h = fabsf(ymax-ymin);
	glPushMatrix();
	{
		glTranslate(pos);
		//glScalef(radius, fabsf(ymax-ymin), radius);

		// draw cylinder
		glPushAttrib(GL_ENABLE_BIT);
		{
			glDisable(GL_CULL_FACE);

			// align cylinder to y axis
			glRotatef(-90, 1, 0, 0);

			// draw cylinder aligned w/ x axis
			gluCylinder(_quadric, radius, radius, h, 32, 16);
			glTranslatef(0, 0, h);
			gluDisk(_quadric, 0, radius, 32, 16);
		}
		glPopAttrib();

	}
	glPopMatrix();
}

//
// drawHighlight() - draws a highlight that can be used to identify various objects for annotation
//
void DrawLib::drawHighlight(const Point & pos, const Vector & dir, float radius, const Color& color)
{
	drawStar(pos+Vector(0.f,0.249f,0.f), dir, radius*1.75f, color);
}


//
// _startDefiningDisplayList()
//
int DrawLib::_startDefiningDisplayList()
{
	// we better not be already creating another list
	if (_currentDisplayListBeingWritten >= 0)
		return -1;

	// create a new list
	GLuint newDisplayList = glGenLists(1);

	// see if a new one was actually created
	if(newDisplayList == 0)
		return -1;

	// save the new list
	_displayLists.push_back(newDisplayList);

	// start the list
	glNewList(newDisplayList, GL_COMPILE);

	_currentDisplayListBeingWritten = (int)_displayLists.size() - 1;

	// return the id for the display list
	return _currentDisplayListBeingWritten;
}


//
// _endDefiningDisplayList()
//
int DrawLib::_endDefiningDisplayList(int displayList)
{
	// check to make sure we're actually ending the current list
	if(_currentDisplayListBeingWritten != displayList)
		return -1;

	// end the display list
	glEndList();

	// end the list
	_currentDisplayListBeingWritten = -1;

	return displayList;
}


//
// drawDisplayList()
//
int DrawLib::_drawDisplayList(int displayList)
{
	if(displayList < 0 || displayList >= (int)_displayLists.size())
		return -1;
	glCallList(_displayLists[displayList]);
	return displayList;
}


//
// buildAgentDisplayList
//
int DrawLib::_buildAgentDisplayList()
{
	static const float h = .5;

	int dl = _startDefiningDisplayList();

	glPushMatrix();
	{
		glPushAttrib(GL_ENABLE_BIT);
		{
			glDisable(GL_CULL_FACE);

			// align cylinder to y axis
			glRotatef(-90, 1, 0, 0);

			// draw cylinder aligned w/ x axis
			gluCylinder(_quadric, 1, 1, h, 32, 16);
			glTranslatef(0, 0, h);
			gluDisk(_quadric, 0, 1, 32, 16);
		}
		glPopAttrib();
	}
	glPopMatrix();

	// draw arrow
	glColor3f(0.2f, 0.5f, 0.2f);
	drawQuad(Point(1.0f, h+.01f, 0.0f),
		Point(-0.6f, h+0.01f, -0.5f),
		Point(-0.62f, h+0.01f, 0.0f),
		Point(-0.6f, h+0.01f, 0.5f));

	_endDefiningDisplayList(dl);

	return dl;
}


int DrawLib::_buildFlagDisplayList()
{
	int dl = _startDefiningDisplayList();

	glPushMatrix();
	{
		// raise the flag
		glRotatef(-90,1,0,0);

		// flag
		drawQuad(Point(0.0f, 0.0f, 1.05f),
			Point(0.5,  0.0f, 0.9f),
			Point(0.0f, 0.0f, 0.75f),
			Point(-0.02f, 0.0f, 0.9f));

		drawQuad(Point(-0.02f,  0.0f,  0.9f),
			Point(0.0f,  0.0f,  0.75f),
			Point(0.5f,  0.0f, 0.9f),
			Point(0.0f, 0.0f, 1.05f));

		// flag pole

		glColor(Color(0.6f, 0.6f, 0.6f));
		gluCylinder(_quadric, 0.05, 0.05, 1.1, 32, 1);
		glTranslatef(0.0f,  0.0f, 1.1f);
		gluSphere(_quadric, .07, 16, 16);
	}
	glPopMatrix();

	_endDefiningDisplayList(dl);

	return dl;
}



#endif // ifdef ENABLE_GUI
