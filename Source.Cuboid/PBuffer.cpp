//////////////////////////////////////////////////////////////////////////////////////////
//	PBUFFER.cpp
//	functions to set up a pbuffer
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	9th September 2002
//////////////////////////////////////////////////////////////////////////////////////////	

#include <stdio.h>
#include <windows.h>
#include <GLee.h>
#include "PBUFFER.h"

void print_error(char* err)
{
	printf("Error:%s\n",err);
	while(1)Sleep(100);
}

bool PBUFFER::Init(	int newWidth, int newHeight)
{
	tmp_hRC = wglGetCurrentContext();
	tmp_hDC = wglGetCurrentDC();

	int newColorBits=32, newDepthBits=24, newStencilBits=8, numExtraIAttribs=1; 
	int extraIAttribList[]={WGL_BIND_TO_TEXTURE_RGBA_ARB, true,0};
	int flags[]={WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGBA_ARB,
						WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
						//request mipmap space if mipmaps are to be used
						GLEE_SGIS_generate_mipmap ? WGL_MIPMAP_TEXTURE_ARB : 0,
						GLEE_SGIS_generate_mipmap ? true : 0,
						0};

	//Check for pbuffer support
	if(GLEE_WGL_EXT_pbuffer)
	{
		print_error("Extension required for pbuffer unsupported");
		return false;
	}

	//set class's member variables
	width=newWidth;
	height=newHeight;
	colorBits=newColorBits;
	depthBits=newDepthBits;
	stencilBits=newStencilBits;

	//Get the current device context
	HDC hCurrentDC=wglGetCurrentDC();
	if(!hCurrentDC)
	{
		print_error("Unable to get current Device Context");
		return false;
	}
	

	//choose pixel format
	GLint pixelFormat;

	const int standardIAttribList[]={	WGL_DRAW_TO_PBUFFER_ARB, 1,
										WGL_COLOR_BITS_ARB, colorBits,
										WGL_ALPHA_BITS_ARB, colorBits==32 ? 8 : 0,
										WGL_DEPTH_BITS_ARB, depthBits,
										WGL_STENCIL_BITS_ARB, stencilBits};
	const float fAttribList[]={	
										0};

	//add the extraIAttribList to the standardIAttribList
	int * iAttribList=new int[sizeof(standardIAttribList)/sizeof(int)+numExtraIAttribs*2+1];
	if(!iAttribList)
	{
		print_error("Unable to allocate space for iAttribList");
		return false;
	}

	memcpy(	iAttribList, standardIAttribList, sizeof(standardIAttribList));
	memcpy( iAttribList+sizeof(standardIAttribList)/sizeof(int),
			extraIAttribList, numExtraIAttribs*2*sizeof(int)+sizeof(int));

	//Choose pixel format
	unsigned int numFormats;
	if(!wglChoosePixelFormatARB(hCurrentDC, iAttribList, fAttribList, 1,
								&pixelFormat, &numFormats))
	{
		print_error("Unable to find a pixel format for the pbuffer");
		return false;
	}

	//Create the pbuffer
	hBuffer=wglCreatePbufferARB(hCurrentDC, pixelFormat, width, height, flags);
	if(!hBuffer)
	{
		print_error("Unable to create pbuffer");
		return false;
	}

	//Get the pbuffer's device context
	hDC=wglGetPbufferDCARB(hBuffer);
	if(!hDC)
	{
		print_error("Unable to get pbuffer's device context");
		return false;
	}

	//Create a rendering context for the pbuffer
	hRC=wglCreateContext(hDC);
	if(!hRC)
	{
		print_error("Unable to create pbuffer's rendering context");
		return false;
	}

	//Set and output the actual pBuffer dimensions
	wglQueryPbufferARB(hBuffer, WGL_PBUFFER_WIDTH_ARB, &width);
	wglQueryPbufferARB(hBuffer, WGL_PBUFFER_HEIGHT_ARB, &height);
	printf("Pbuffer Created: (%d x %d)\n", width, height);

	//Create the texture object to relate to the pbuffer
	glGenTextures(1, &pbufferTexture);
	glBindTexture(GL_TEXTURE_2D, pbufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Use generated mipmaps if supported
	useMipmapFilter = false;
	if(GLEE_SGIS_generate_mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
		useMipmapFilter=true;
	}

	//Use maximum anisotropy if supported
	if(GLEE_EXT_texture_filter_anisotropic)
	{
		currentAnisotropy=1;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		currentAnisotropy=maxAnisotropy;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, currentAnisotropy);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	return TRUE;										//success!
}

void PBUFFER::Shutdown(void)
{
	if(hRC)													//have a rendering context?
	{
		if(!wglDeleteContext(hRC))							//try to delete RC
		{
			print_error("Release of Pbuffer Rendering Context Failed.");
		}
		
		hRC=NULL;											//set RC to NULL
	}

	if(hDC && !wglReleasePbufferDCARB(hBuffer, hDC))		//Are we able to release DC?
	{
		print_error("Release of Pbuffer Device Context Failed.");
		hDC=NULL;
	}
	
	if(!wglDestroyPbufferARB(hBuffer))
	{
		print_error("Unable to destroy pbuffer");
	}
}

bool PBUFFER::begin()
{
	glGetIntegerv(GL_VIEWPORT, tmp_viewport);
	glViewport(0, 0, width, height);		// Reset The Current Viewport And Perspective Transformation

	//wglShareLists(hRC,tmp_hRC);

	if(!wglMakeCurrent(hDC, hRC))
	{
		print_error("Unable to change current context");
		return false;
	}

	return true;
}
bool PBUFFER::end()
{
	if(!wglMakeCurrent(tmp_hDC, tmp_hRC))
	{
		print_error("Unable to change current context");
		return false;
	}
	glViewport(
		tmp_viewport[0],
		tmp_viewport[1],
		tmp_viewport[2],
		tmp_viewport[3]);

	return true;
}

