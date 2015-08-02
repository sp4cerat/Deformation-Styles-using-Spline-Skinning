//////////////////////////////////////////////////////////////////////////////////////////
//	PBUFFER.h
//	class to setup pBuffer
//	You may use this code however you wish, but if you do, please credit me and
//	provide a link to my website in a readme file or similar
//	Downloaded from: www.paulsprojects.net
//	Created:	9th September 2002
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef PBUFFER_H
#define PBUFFER_H

class PBUFFER
{
public:
	HGLRC hRC;													//rendering context
	HDC   hDC;													//device context
	HPBUFFERARB hBuffer;										//buffer handle

	HDC	   tmp_hDC; // Tmp - context
	HGLRC  tmp_hRC;

	int tmp_viewport[4];

	unsigned int pbufferTexture;

	bool useMipmapFilter;
	int  currentAnisotropy,maxAnisotropy;

	int width, height;											//window size
	int colorBits, depthBits, stencilBits;						//window bpp
	
	bool Init(	int newWidth, int newHeight );
	bool begin(void);
	bool end(void);

	void Shutdown(void);
};

#endif	//PBUFFER_H