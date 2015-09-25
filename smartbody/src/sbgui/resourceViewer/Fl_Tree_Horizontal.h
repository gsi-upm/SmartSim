#pragma once
#include <FL/Fl_Tree.H>

class Fl_TreeHorizontal : public Fl_Tree
{
protected:
	Fl_Scrollbar *_hscroll;
public:
	Fl_TreeHorizontal(int X, int Y, int W, int H, const char *L=0);
	virtual void draw();
protected:
	void getHorizontalWidth(const Fl_Tree_Item* root, int& maxWidth);
};
