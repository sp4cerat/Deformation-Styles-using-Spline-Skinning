#ifndef _Gfx_class
#define _Gfx_class

#include "Bmp.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>     // Header File For sleeping.
#endif
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
/*
#include <GL/glut.h>// Header File For The GLUT Library 
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <GL/glext.h>
*/
#include <GLee.h>
#include <GL/glut.h>// Header File For The GLUT Library 

class GFX {

    public :

		enum TexLoadFlags {
		  TEX_ADDALPHA=1,
		  TEX_NORMALMAP=2,
		  TEX_HORIZONMAP=4,
		  TEX_HORIZONLOOKUP=8,
		  TEX_NORMALIZE=16,
		  TEX_16BIT=32,
		};

		static void  BindTexture( int gl_texture_handle , int tmu=0 );
		static void  ClearTexture();
		static bool  keyPressed[256];
		static bool  mousePressed[256];
		static bool  keyPressed2[256];
		static bool  mousePressed2[256];
		static float mouseX,mouseY;
		static float mouseDX,mouseDY;
		static int   width,height;

       static GFX* This;

       int Init(int width,int height,bool fullscreen, 
            void (GLUTCALLBACK *drawFunc)(void)) ;        
		int Run() ;
		static void Flip();
		static int LoadTex(const char *name,int flags=0);
		static int LoadTexBmp(Bmp &bmp,int flags=0);
		static void SetTex(int id);
		static void Line ( vec3f p1 , vec3f p2 , vec3f color=vec3f(1,1,0) );
		static void CheckExtensions(void);

		static GLuint NewFloatTex(int x,int y,char* buffer,bool alpha=true);
		static GLuint NewFloat16Tex(int x,int y,char* buffer,bool alpha=true);
		static GLuint NewHFloat16Tex(int x,int y,char* buffer,bool alpha=true);
		static GLuint NewFloatRectTex(int x,int y,char* buffer,bool alpha=true);
		static GLuint NewByteTex(int x,int y,char* buffer,bool alpha=true);
		static GLuint NewByteRectTex(int x,int y,char* buffer,bool alpha=true);

		static bool KeyDn(char key);//key down
		static bool KeyPr(char key);//pressed
		static bool KeyRl(char key);//released

		void InitGL(int Width, int Height);	        // We call this right after our OpenGL window is created.

    private :

        /* The number of our GLUT window */
        int window; 

        /* ASCII code for the escape key. */
        #define ESCAPE 27

        /* A general OpenGL initialization function.  Sets all of the initial parameters. */
        void ResizeScene(int Width, int Height);
        static void DrawSceneStatic  ();
        static void keyDown1Static   (int key, int x, int y);
        static void keyDown2Static   (unsigned char key, int x, int y);
        static void keyUp1Static     (int key, int x, int y);
        static void keyUp2Static     (unsigned char key, int x, int y);
        static void ResizeSceneStatic(int Width, int Height);
        void KeyPressed(int key, int x, int y,bool pressed) ;

		static void MouseMotionStatic (int x,int y);
		static void MouseButtonStatic (int button, int state, int x, int y);
public:
		static void get_error();
};

class FBO {

	public:

	enum Type { COLOR=1 , DEPTH=2 }; // Bits

	int color_tex;
	int color_bpp;
	int depth_tex;
	int depth_bpp;
	Type type;

	int width;
	int height;

	int tmp_viewport[4];

	FBO ()
	{
		color_tex = -1;
		depth_tex = -1;
		fbo = -1;
		dbo = -1;
	}

	void clear ()
	{		
		if(color_tex!=-1)
		{
			// destroy objects
			glDeleteRenderbuffersEXT(1, &dbo);
			glDeleteTextures(1, (GLuint*)&color_tex);
			glDeleteTextures(1, (GLuint*)&depth_tex);
			glDeleteFramebuffersEXT(1, &fbo);
		}
	}

	void enable()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, dbo);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
			GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
			GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_tex, 0);

		glGetIntegerv(GL_VIEWPORT, tmp_viewport);
		glViewport(0, 0, width, height);		// Reset The Current Viewport And Perspective Transformation
		//glMatrixMode(GL_PROJECTION);
		//glPushMatrix();
		//glLoadIdentity();
		//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
		//glMatrixMode(GL_MODELVIEW);

	}

	void disable()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glViewport(
			tmp_viewport[0],
			tmp_viewport[1],
			tmp_viewport[2],
			tmp_viewport[3]);
		//glMatrixMode(GL_PROJECTION);
		//glPopMatrix();
		//glMatrixMode(GL_MODELVIEW);
	}

	void init (int texWidth,int texHeight)//,Type type = Type(COLOR | DEPTH),int color_bpp=32,int depth_bpp=24)
	{
	//	clear ();
		this->width = texWidth;
		this->height = texHeight;

		glGenFramebuffersEXT(1, &fbo); 
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);    
		get_error();

			// init texture
			glGenTextures(1, (GLuint*)&color_tex);
			glBindTexture(GL_TEXTURE_2D, color_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
				texWidth, texHeight, 0, 
				GL_RGBA, GL_FLOAT, NULL);

			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0,
			//	    GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			//GL_TEXTURE_2D,GL_RGBA, bmp.width, bmp.height,
			//	/*GL_RGBA*/GL_BGRA_EXT, GL_UNSIGNED_BYTE, bmp.data );

			get_error();
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2DEXT(
				GL_FRAMEBUFFER_EXT, 
				GL_COLOR_ATTACHMENT0_EXT, 
				GL_TEXTURE_2D, color_tex, 0);
			get_error();

		glGenRenderbuffersEXT(1, &dbo);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, dbo);

			glGenTextures(1, (GLuint*)&depth_tex);
			glBindTexture(GL_TEXTURE_2D, depth_tex);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 
			//	texWidth, texHeight, 0, 
			//	GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			get_error();
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			/*
			//Use generated mipmaps if supported
			if(GLEE_SGIS_generate_mipmap)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
			}

			//Use maximum anisotropy if supported
			if(GLEE_EXT_texture_filter_anisotropic)
			{
				GLint maxAnisotropy=1;
				glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
			}
			*/

			glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 
				texWidth, texHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT/*GL_UNSIGNED_INT*/, NULL);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, 
								GL_TEXTURE_2D, depth_tex, 0);

			get_error();
			glBindTexture(GL_TEXTURE_2D, 0);// don't leave this texture bound or fbo (zero) will use it as src, want to use it just as dest GL_DEPTH_ATTACHMENT_EXT

		get_error();

		check_framebuffer_status();
	    
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	}

	private:

	GLuint fbo; // frame buffer object ref
	GLuint dbo; // depth buffer object ref

	void get_error()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) 
		{
			printf("GL FBO Error: %s\n",gluErrorString(err));
			printf("Programm Stopped!\n");
			while(1);;
		}
	}

	void check_framebuffer_status()
	{
		GLenum status;
		status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch(status) {
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				return;
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				printf("Unsupported framebuffer format\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				printf("Framebuffer incomplete, missing attachment\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
				printf("Framebuffer incomplete, duplicate attachment\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				printf("Framebuffer incomplete, attached images must have same dimensions\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				printf("Framebuffer incomplete, attached images must have same format\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				printf("Framebuffer incomplete, missing draw buffer\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				printf("Framebuffer incomplete, missing read buffer\n");
				break;
			case 0:
				printf("Not ok but trying...\n");
				return;
				break;
			default:;
				printf("Framebuffer error code %d\n",status);
				break;
		};
		printf("Programm Stopped!\n");
		while(1)Sleep(100);;
	}
};

#endif