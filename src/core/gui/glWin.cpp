#include "gl/glew.h"
#include "glWin.h"

GLWin::GLWin(wxFrame* parent) :
wxGLCanvas(parent, wxID_ANY,  wxDefaultPosition, wxSize(320, 480), 0, wxT(""))
{
	m_init = false;
}

BEGIN_EVENT_TABLE(GLWin, wxGLCanvas)
EVT_SIZE(GLWin::onResized)
EVT_PAINT(GLWin::onRender)
END_EVENT_TABLE()


void GLWin::onResized(wxSizeEvent& evt)
{
    wxGLCanvas::OnSize(evt);
	
    Refresh();
}

int GLWin::getWidth()
{
    return GetSize().x;
}
 
int GLWin::getHeight()
{
    return GetSize().y;
}

void GLWin::InitGL()
{
	if (m_init)
		return;
	//make sure this doesn't happen after execution

    wxGLCanvas::SetCurrent();

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	m_init = true;
}
 
void GLWin::onRender( wxPaintEvent& evt )
{
    if(!IsShown()) return;

	InitGL();
	
    //wxGLCanvas::SetCurrent();
    wxPaintDC(this); 

    SwapBuffers();
}