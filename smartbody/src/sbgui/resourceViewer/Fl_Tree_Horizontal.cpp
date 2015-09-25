#include "Fl_Tree_Horizontal.h"


// INTERNAL: scroller callback
static void hscroll_cb(Fl_Widget*,void *data) {
	Fl_Tree* tree = ((Fl_Tree*)data);
	tree->redraw();	
}

Fl_TreeHorizontal::Fl_TreeHorizontal( int X, int Y, int W, int H, const char *L/*=0*/ ) : Fl_Tree(X,Y,W,H,L)
{
	this->begin();
	_hscroll = new Fl_Scrollbar(0,0,W,20);		// will be resized by draw()
	_hscroll->visible();
	_hscroll->show();
	_hscroll->type(FL_HORIZONTAL);
	_hscroll->step(1);
	_hscroll->callback(hscroll_cb, (void*)this);	
	this->end();	
}

void Fl_TreeHorizontal::draw()
{
	Fl_Group::draw_box();
	Fl_Group::draw_label();
	// Handle tree
	if ( ! root() ) return;
	int cx = x() + Fl::box_dx(box());
	int cy = y() + Fl::box_dy(box());
	int cw = w() - Fl::box_dw(box());
	int ch = h() - Fl::box_dh(box());
	// These values are changed during drawing
	// 'Y' will be the lowest point on the tree
	// 'X' will be the right most point on the tree
	int X = cx + marginleft() - (_hscroll->visible() ? _hscroll->value() : 0 );	
	int Y = cy + margintop() - (_vscroll->visible() ? _vscroll->value() : 0);
	int W = cw - marginleft();			// - _prefs.marginright();
	int Ysave = Y;
	int Xsave = X;
	
	int XWidth = 0;
	getHorizontalWidth(root(),XWidth); // get the true right most point within the tree
	XWidth += 50;
	W = XWidth - X;

	Fl_Tree_Prefs prefs;
	prefs.marginleft(marginleft());
	prefs.margintop(margintop());	
	fl_push_clip(cx,cy,cw,ch);
	{
		fl_font(labelfont(), labelsize());
		root()->draw(X, Y, W, this,
			(Fl::focus()==this)?first_selected_item():0,	// show focus item ONLY if Fl_Tree has focus
			prefs);
	}
	fl_pop_clip();

	X = XWidth;	
	//X += 50;

	// Show vertical scrollbar?	
	int xdiff = (X+marginleft())-Xsave;	
	int xtoofar = (cx+cw) - X;

	if ( xtoofar > 0 ) xdiff += xtoofar;
	if ( Xsave<cx || xdiff > cw || int(_hscroll->value()) > 1 ) {
		_hscroll->visible();

		int scrollsize = scrollbar_size() ? scrollbar_size() : Fl::scrollbar_size();
		int sx = x()+Fl::box_dy(box());
		int sy = y()+h()-Fl::box_dy(box())-scrollsize;
		int sw = w()-Fl::box_dh(box());;
		int sh = scrollsize;
		_hscroll->show();
		_hscroll->range(0.0,xdiff-cw);
		_hscroll->resize(sx,sy,sw,sh);
		_hscroll->slider_size(float(cw)/float(xdiff));
	} else {
		_hscroll->Fl_Slider::value(0);
		_hscroll->hide();
	}	

	// Show vertical scrollbar?
	int ydiff = (Y+margintop())-Ysave;		// ydiff=size of tree
	int ytoofar = (cy+ch) - Y;				// ytoofar -- scrolled beyond bottom (e.g. stow)
	//printf("ydiff=%d ch=%d Ysave=%d ytoofar=%d value=%d\n",
	//int(ydiff),int(ch),int(Ysave),int(ytoofar), int(_vscroll->value()));
	if ( ytoofar > 0 ) ydiff += ytoofar;
	if ( Ysave<cy || ydiff > ch || int(_vscroll->value()) > 1 ) {
		_vscroll->visible();

		int scrollsize = scrollbar_size() ? scrollbar_size() : Fl::scrollbar_size();
		int sx = x()+w()-Fl::box_dx(box())-scrollsize;
		int sy = y()+Fl::box_dy(box());
		int sw = scrollsize;
		int sh = h()-Fl::box_dh(box());
		_vscroll->show();
		_vscroll->range(0.0,ydiff-ch);
		_vscroll->resize(sx,sy,sw,sh);
		_vscroll->slider_size(float(ch)/float(ydiff));
	} else {
		_vscroll->Fl_Slider::value(0);
		_vscroll->hide();
	}
	fl_push_clip(cx,cy,cw,ch);
	Fl_Group::draw_children();	// draws any FLTK children set via Fl_Tree::widget()
	fl_pop_clip();
}

void Fl_TreeHorizontal::getHorizontalWidth(const Fl_Tree_Item* root, int& maxWidth )
{
	int labelSize = root->widget() ? root->widget()->w() : (int)fl_width(root->label());	
	if (root->x()+labelSize > maxWidth)
		maxWidth = root->x()+labelSize;
	if (!root->has_children() || !root->is_open())
		return;

	for (int i=0;i<root->children();i++)
	{
		getHorizontalWidth(root->child(i),maxWidth);
	}

}