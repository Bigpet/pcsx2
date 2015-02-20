/*
 *	Copyright (C) 2007-2012 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "stdafx.h"
#include "GSWndSWWin.h"
#include "GSDeviceSWWin.h"
#include "GSTextureSW.h"

#if defined(_WINDOWS)

GSWndSWWin::GSWndSWWin(GSDeviceSWWin *device)
	: m_NativeWindow(NULL), m_NativeDisplay(NULL), m_Device(device)
{
}

bool GSWndSWWin::Attach(void* handle, bool managed)
{
	m_NativeWindow = (HWND)handle;
	m_managed = managed;

	if (!OpenWinDisplay()) return false;

	UpdateWindow(m_NativeWindow);

	return true;
}

void GSWndSWWin::Detach()
{
	// Actually the destructor is not called when there is only a GSclose/GSshutdown
	// The window still need to be closed

	CloseWinDisplay();
}

bool GSWndSWWin::OpenWinDisplay()
{
	GLuint	  PixelFormat;			// Holds The Results After Searching For A Match
	PIXELFORMATDESCRIPTOR pfd =			 // pfd Tells Windows How We Want Things To Be

	{
		sizeof(PIXELFORMATDESCRIPTOR),			  // Size Of This Pixel Format Descriptor
		1,										  // Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		//PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_SUPPORT_GDI,						   // Must Support Double Buffering
		PFD_TYPE_RGBA,							  // Request An RGBA Format
		32,										 // Select Our Color Depth
		0, 0, 0, 0, 0, 0,						   // Color Bits Ignored
		0,										  // 8bit Alpha Buffer
		0,										  // Shift Bit Ignored
		0,										  // No Accumulation Buffer
		0, 0, 0, 0,								 // Accumulation Bits Ignored
		0,										 // 24Bit Z-Buffer (Depth Buffer)
		8,										  // 8bit Stencil Buffer
		0,										  // No Auxiliary Buffer
		PFD_MAIN_PLANE,							 // Main Drawing Layer
		0,										  // Reserved
		0, 0, 0									 // Layer Masks Ignored
	};

	m_NativeDisplay = GetDC(m_NativeWindow);
	if (!m_NativeDisplay)
	{
		MessageBox(NULL, "(1) Can't Create A Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	PixelFormat = ChoosePixelFormat(m_NativeDisplay, &pfd);
	if (!PixelFormat)
	{
		MessageBox(NULL, "(2) Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!SetPixelFormat(m_NativeDisplay, PixelFormat, &pfd))
	{
		MessageBox(NULL, "(3) Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void GSWndSWWin::CloseWinDisplay()
{
	if (m_NativeDisplay && !ReleaseDC(m_NativeWindow, m_NativeDisplay))				 // Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
	}
	m_NativeDisplay = NULL;									 // Set DC To NULL
}

//TODO: GSopen 1 => Drop?
bool GSWndSWWin::Create(const string& title, int w, int h)
{
#if 0
	if(m_NativeWindow) return false;

	if(w <= 0 || h <= 0) {
		w = theApp.GetConfig("ModeWidth", 640);
		h = theApp.GetConfig("ModeHeight", 480);
	}

	m_managed = true;

	// note this part must be only executed when replaying .gs debug file
	m_NativeDisplay = XOpenDisplay(NULL);

	int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		None
	};
	XVisualInfo* vi = glXChooseVisual(m_NativeDisplay, DefaultScreen(m_NativeDisplay), attrListDbl);

	/* create a color map */
	XSetWindowAttributes attr;
	attr.colormap = XCreateColormap(m_NativeDisplay, RootWindow(m_NativeDisplay, vi->screen),
			vi->visual, AllocNone);
	attr.border_pixel = 0;
	attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask |
		StructureNotifyMask | SubstructureRedirectMask | SubstructureNotifyMask |
		EnterWindowMask | LeaveWindowMask | FocusChangeMask ;

	// Create a window at the last position/size
	m_NativeWindow = XCreateWindow(m_NativeDisplay, RootWindow(m_NativeDisplay, vi->screen),
			0 , 0 , w, h, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap | CWEventMask, &attr);

	XMapWindow (m_NativeDisplay, m_NativeWindow);
	XFree(vi);

	if (!CreateContext(3, 3)) return false;

	AttachContext();

	return (m_NativeWindow != 0);
#else
	return false;
#endif
}

//Same as DX
GSVector4i GSWndSWWin::GetClientRect()
{
	GSVector4i r;

	::GetClientRect(m_NativeWindow, r);

	return r;
}

void GSWndSWWin::SetVSync(bool enable)
{

}

void GSWndSWWin::Flip()
{
	GSVector4i rect = GetClientRect();
	GSTexture *tex = m_Device->GetLastMerged();
	GSTextureSW *framebuffer = static_cast<GSTextureSW *>(tex);
	GSTexture::GSMap pixel_data;
	framebuffer->Map(pixel_data, NULL);

	BITMAPINFO info{
		{
			sizeof(BITMAPINFOHEADER), //DWORD biSize;
			tex->GetWidth(), //LONG  biWidth;
			-1 * tex->GetHeight(),//LONG  biHeight;
			1, //WORD  biPlanes;
			32, //WORD  biBitCount;
			BI_RGB, //DWORD biCompression;
			pixel_data.pitch * tex->GetHeight(), //DWORD biSizeImage;
			0,//LONG  biXPelsPerMeter;
			0,//LONG  biYPelsPerMeter;
			0, //DWORD biClrUsed;
			0 //DWORD biClrImportant;
		}, {} };
	
	//int ret = SetDIBitsToDevice(m_NativeDisplay, 0, 0, tex->GetWidth(), tex->GetHeight(), 0, 0, 0, tex->GetHeight(), pixel_data.bits, &info, DIB_RGB_COLORS);
	int ret = StretchDIBits(m_NativeDisplay, 0, 0, rect.z, rect.w, 0, 0, tex->GetWidth(), tex->GetHeight(), pixel_data.bits, &info, DIB_RGB_COLORS, SRCCOPY);
	framebuffer->Unmap();
	SwapBuffers(m_NativeDisplay);
}

void GSWndSWWin::Show()
{
}

void GSWndSWWin::Hide()
{
}

void GSWndSWWin::HideFrame()
{
}

// Returns FALSE if the window has no title, or if th window title is under the strict
// management of the emulator.

bool GSWndSWWin::SetWindowText(const char* title)
{
	return false;
}


#endif
