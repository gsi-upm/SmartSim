#include "GenericViewer.h"

GenericViewer::GenericViewer()
{
}

GenericViewer::GenericViewer(int x, int y, int w, int h)
{
}

GenericViewer::~GenericViewer()
{
}

void GenericViewer::label_viewer(std::string name)
{
}

void GenericViewer::show_viewer()
{
}

void GenericViewer::hide_viewer()
{
}

void GenericViewer::update_viewer()
{
}

GenericViewerFactory::GenericViewerFactory()
{
}

GenericViewer* GenericViewerFactory::create(int x, int y, int w, int h)
{
	return new GenericViewer(x, y, w, h);
}


void GenericViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
	viewer = NULL;
}

