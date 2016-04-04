#include "vhcl.h"
#include "ErrorVisualization.h"
#include <sb/SBAnimationState.h>
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sr/sr_camera.h>
#include <sb/PABlend.h>

ErrorVisualization::ErrorVisualization( int x, int y, int w, int h, char* name) : VisualizationBase(x,y,w,h,name)
{
	curBlend = NULL;
	drawType = "error";
}

ErrorVisualization::~ErrorVisualization()
{

}

void ErrorVisualization::setDrawType( const std::string& type )
{
	if (type != "error" && type != "smooth") return;
	drawType = type;
}


void ErrorVisualization::setAnimationState( SmartBody::SBAnimationBlend* animBlend )
{
	curBlend = animBlend;
}

void ErrorVisualization::draw()
{
	if (!visible()) 
		return;
	if (!valid()) 
	{
		init_opengl();
		valid(1);
	}

	//----- Clear Background --------------------------------------------
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//----- Set Projection ----------------------------------------------
	SrMat mat;
	cam.setAspectRatio((float)w()/(float)h());
	glMatrixMode(GL_PROJECTION);
	glLoadMatrix(cam.get_perspective_mat(mat));

	//----- Set Visualisation -------------------------------------------
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrix(cam.get_view_mat(mat));
	glScalef(cam.getScale(), cam.getScale(), cam.getScale());
	SrLight light;
	//	light.directional = false;
	light.directional = true;
	light.diffuse = SrColor( 1.0f, 0.95f, 0.8f );
	light.position = SrVec( 100.0, 250.0, 400.0 );
	//	light.constant_attenuation = 1.0f/cam.scale;
	light.constant_attenuation = 1.0f;

	SrLight light2 = light;
	light2.directional = false;
	light2.diffuse = SrColor( 0.8f, 0.85f, 1.0f );
	light2.position = SrVec( 100.0, 500.0, -200.0 );
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
	glEnable(GL_LIGHTING);
	glLight( 0, light );
	glLight( 1, light2 );

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );
	glDisable( GL_COLOR_MATERIAL );

	//----- Render user scene -------------------------------------------	
	// draw axis
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex(SrVec(gridSize, 0, 0));
	glVertex(SrVec(-gridSize, 0, 0));
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex(SrVec(0, gridSize, 0));
	glVertex(SrVec(0, -gridSize, 0));
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex(SrVec(0, 0, gridSize));
	glVertex(SrVec(0, 0, -gridSize));
	glEnd();
	drawGrid();

	if (!curBlend) return;

	std::vector<SrSnColorSurf*>& drawSurfs = (drawType == "error" ) ? curBlend->getErrorSurfaces() : curBlend->getSmoothSurfaces();
	
	for (unsigned int i=0;i<drawSurfs.size();i++)
	{
		SrSnColorSurf* drawSurf = drawSurfs[i];
		drawSurf->gl_render_node();
	}	
}
