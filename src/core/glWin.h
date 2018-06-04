#pragma once
#include <wx/wx.h>
#include <wx/glcanvas.h>

class GLWin : public wxGLCanvas
{
public:
	GLWin(wxFrame* parent);
    
	int getWidth();
	int getHeight();
    
	void onResized(wxSizeEvent& evt);
	void onRender(wxPaintEvent& evt);

	void InitGL();
    
	DECLARE_EVENT_TABLE()

private:
	bool m_init;
};