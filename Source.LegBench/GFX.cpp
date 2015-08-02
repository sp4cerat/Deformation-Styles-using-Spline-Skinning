#include "GFX.h"

bool  GFX::keyPressed[256] = {0};
bool  GFX::keyPressed2[256] = {0};
bool  GFX::mousePressed[256] = {0};
bool  GFX::mousePressed2[256] = {0};
float GFX::mouseX = 0;
float GFX::mouseY = 0;
float GFX::mouseDX = 0;
float GFX::mouseDY = 0;
int   GFX::width  = 0;
int   GFX::height = 0;
GFX*  GFX::This=0;

void GFX::keyDown1Static(int key, int x, int y)           { GFX::This->KeyPressed(key, x, y,true); }
void GFX::keyDown2Static(unsigned char key, int x, int y) { GFX::This->KeyPressed(key, x, y,true); }
void GFX::keyUp1Static(int key, int x, int y)             { GFX::This->KeyPressed(key, x, y,false); }
void GFX::keyUp2Static(unsigned char key, int x, int y)   { GFX::This->KeyPressed(key, x, y,false); }
//void GFX::DrawSceneStatic()                               { GFX::This->DrawScene();}
void GFX::ResizeSceneStatic(int Width, int Height)        { GFX::This->ResizeScene(Width, Height);};
void GFX::InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,1.0f,1000.0f);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_CULL_FACE);
	glCullFace (GL_FRONT);
	glEnable(GL_TEXTURE_2D);
}
void GFX::MouseMotionStatic (int x,int y)
{
	mouseX = float(x) / float(GFX::width);
	mouseY = float(y) / float(GFX::height);
}
void GFX::MouseButtonStatic(int button, int state, int x, int y)
{
	mousePressed[button] =  ( state == GLUT_DOWN ) ? true : false;
}

void GFX::KeyPressed(int key, int x, int y,bool pressed) 
{
    if(key==ESCAPE)
    {
            glutDestroyWindow(window); 
	        exit(0);
    }
    Sleep(10);

    keyPressed[ key&255 ] = pressed;

    switch (key) {
        case GLUT_KEY_UP:
        default: ;
    }
}
int GFX::Init(int width,int height,bool fullscreen, 
    void (GLUTCALLBACK *drawFunc)(void)) 
{  
  This = this;
  int c=1;char* s="GLUT";
  glutInit(&c, &s);  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
  glutInitWindowSize(width, height);  
  glutInitWindowPosition(0, 0);  
  window = glutCreateWindow("Deformation Styles (c) Sven Forstmann 2007");
  glutDisplayFunc(drawFunc);
  glutIdleFunc(drawFunc);
  glutReshapeFunc(&ResizeSceneStatic);

  glutSpecialFunc(&keyDown1Static);
  glutSpecialUpFunc(&keyUp1Static);
  glutKeyboardFunc(&keyDown2Static);
  glutKeyboardUpFunc(&keyUp2Static);

  glutMotionFunc(&MouseMotionStatic);
  glutPassiveMotionFunc(&MouseMotionStatic);
  glutMouseFunc (&MouseButtonStatic);

  this->width  = width;
  this->height = height;

  InitGL(width, height);
  GLeeInit();
  get_error();

  CheckExtensions();

  if (fullscreen) glutFullScreen();

  return 1;
}
int GFX::Run() 
{  
  get_error();
  glutMainLoop();  
  return 1;
}
void GFX::ResizeScene(int Width, int Height)
{
  if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

  this->width = Width;
  this->height = Height;
}
int GFX::LoadTex(const char *name,int flags)
{
	Bmp bmp;
	if(!bmp.load(name))
		return -1;

	return LoadTexBmp(bmp,flags);
}

int GFX::LoadTexBmp(Bmp &bmp,int flags)
{
	int gl_handle;
	glGenTextures(1,(GLuint*)(&gl_handle));

	glBindTexture  (GL_TEXTURE_2D, gl_handle);
	glPixelStorei  (GL_UNPACK_ALIGNMENT, 4);

	//if(flags==0)						bmp.addalpha(255,0,255);
	if((flags & TEX_NORMALIZE)!=0)		bmp.normalize();
	if((flags & TEX_NORMALMAP)!=0)		bmp.normalMap();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,   GL_MODULATE);

	if(bmp.bpp==32)	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA, bmp.width, bmp.height,/*GL_RGBA*/GL_BGRA_EXT, GL_UNSIGNED_BYTE, bmp.data );
	else			gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB, bmp.width, bmp.height,/*GL_RGB*/ GL_BGR_EXT , GL_UNSIGNED_BYTE, bmp.data );

  get_error();

  return gl_handle;
}
void GFX::SetTex(int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
	get_error();
}

void GFX::Line ( vec3f p1 , vec3f p2 , vec3f color){
	glBegin(GL_LINES);
	glColor3f(color.x ,color.y ,color.z );
	glVertex3f( p1.x,p1.y,p1.z );
	glVertex3f( p2.x,p2.y,p2.z );
	glEnd();
}

void GFX::Flip(void)
{
	static float tmpX = 0;
	static float tmpY = 0;
	mouseDX = GFX::mouseX-tmpX; tmpX = mouseX;
	mouseDY = GFX::mouseY-tmpY; tmpY = mouseY;

	glViewport(0, 0, width, height);		// Reset The Current Viewport And Perspective Transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glutSwapBuffers();

	memcpy(keyPressed2 , keyPressed, 128*sizeof(bool));
	memcpy(mousePressed2, mousePressed, 128*sizeof(bool));
}

bool GFX::KeyDn(char key)//key down
{
	return keyPressed[key];
}
bool GFX::KeyPr(char key)//pressed
{
	return ((!keyPressed2[key]) && keyPressed[key] );
}
bool GFX::KeyRl(char key)//released
{
	return ((!keyPressed[key]) && keyPressed2[key] );
}

void GFX::CheckExtensions(void)
{
	bool useGLSL = true;
	
    if (GLEE_VERSION_2_0)
    {
       printf("OpenGL 2.0 is available!\n");
    }
    else if (GLEE_VERSION_1_5)
    {
       printf("OpenGL 1.5 core functions are available\n");
    }
    else if (GLEE_VERSION_1_4)
    {
       printf("OpenGL 1.4 core functions are available\n");
    }
    else if (GLEE_VERSION_1_3)
    {
       printf("OpenGL 1.3 core functions are available\n");
    }
    else if (GLEE_VERSION_1_2)
    {
       printf("OpenGL 1.2 core functions are available\n");
    }

    if (!GLEE_ARB_shader_objects)
    {
        printf("GLEE_ARB_shader_objects extension is not available!\n");
        useGLSL = false;
    }

    if (useGLSL)
    {
        printf("[OK] OpenGL Shading Language is available!\n");
    }
    else
    {
        printf("[FAILED] OpenGL Shading Language is not available...\n");
		while(1);;
    }   
	int tmus;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&tmus);
	printf("Number of TMUs : %d\n",tmus);
}

void GFX::BindTexture( int gl_texture_handle , int tmu )
{
	glActiveTextureARB( GL_TEXTURE0_ARB + tmu );
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gl_texture_handle);
	get_error();
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	return;
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ) ;//GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ) ;//GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf      (GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,   GL_MODULATE);
	*/
	
}

void GFX::ClearTexture()
{
	for ( int i = 3 ; i>=1 ; i-- )
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glBindTexture(GL_TEXTURE_2D, 0);
		//glDisable(GL_TEXTURE_2D);
	}
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GFX::NewFloatTex(int x,int y,char* buffer,bool alpha)
{
//		GLuint type = GL_TEXTURE_RECTANGLE_ARB;
	GLuint type = GL_TEXTURE_2D; // GL_TEXTURE_RECTANGLE_ARB;
	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format

	//alpha=true;

	GLuint format1 = GL_RGB_FLOAT32_ATI;
	GLuint format0 = GL_RGB32F_ARB;
	GLuint format2 = GL_FLOAT_RGB32_NV;
	GLuint format3 = GL_RGB;

	if (alpha)
	{
		format1 = GL_RGBA_FLOAT32_ATI;
		format0 = GL_RGBA32F_ARB;
		format2 = GL_FLOAT_RGBA32_NV;
		format3 = GL_RGBA;
	}
	glTexImage2D(type,0,format0,x,y,0,format3,GL_FLOAT,buffer);		
	
	if (glGetError() != GL_NO_ERROR) 
	{
		glTexImage2D(type,0,format1,x,y,0,format3,GL_FLOAT,buffer);
		if (glGetError() != GL_NO_ERROR) 
			glTexImage2D(type,0,format2,x,y,0,format3,GL_FLOAT,buffer);
	}
	
	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}


GLuint GFX::NewFloatRectTex(int x,int y,char* buffer,bool alpha)
{
	GLuint type = GL_TEXTURE_RECTANGLE_ARB;
//	GLuint type = GL_TEXTURE_2D; // GL_TEXTURE_RECTANGLE_ARB;
	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format

	//alpha=true;

	GLuint format2 = GL_RGB32F_ARB;
	GLuint format1 = GL_RGB_FLOAT32_ATI;
	GLuint format0 = GL_FLOAT_RGB32_NV;
	GLuint format3 = GL_BGR;

	if (alpha)
	{
		format2 = GL_RGBA32F_ARB;
		format1 = GL_RGBA_FLOAT32_ATI;
		format0 = GL_FLOAT_RGBA32_NV;
		format3 = GL_BGRA;
	}
	glTexImage2D(type,0,format0,x,y,0,format3,GL_FLOAT,buffer);		
	
	if (glGetError() != GL_NO_ERROR) 
	{
		glTexImage2D(type,0,format1,x,y,0,format3,GL_FLOAT,buffer);
		if (glGetError() != GL_NO_ERROR) 
			glTexImage2D(type,0,format2,x,y,0,format3,GL_FLOAT,buffer);
	}
	
	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}

GLuint GFX::NewHFloat16Tex(int x,int y,char* buffer,bool alpha)
{
	GLuint type = GL_TEXTURE_RECTANGLE_ARB;//GL_TEXTURE_2D; // GL_TEXTURE_RECTANGLE_ARB;
	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format

	//alpha=true;

	GLuint format0 = GL_RGB16F_ARB;//GL_FLOAT_RGB16_NV;
	GLuint format3 = GL_RGB;

	if (alpha)
	{
		format0 = GL_RGBA16F_ARB;//GL_FLOAT_RGBA16_NV;
		format3 = GL_RGBA;
	}
	glTexImage2D(type,0,format0,x,y,0,format3,GL_HALF_FLOAT_ARB,buffer);		

	
	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}

GLuint GFX::NewFloat16Tex(int x,int y,char* buffer,bool alpha)
{
	GLuint type = GL_TEXTURE_RECTANGLE_ARB;//GL_TEXTURE_2D; // GL_TEXTURE_RECTANGLE_ARB;
	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format
	//alpha=true;

	GLuint format0 = GL_RGB16F_ARB;//GL_FLOAT_RGB16_NV;
	GLuint format3 = GL_RGB;

	if (alpha)
	{
		format0 = GL_RGBA16F_ARB;//GL_FLOAT_RGBA16_NV;
		format3 = GL_RGBA;
	}
	glTexImage2D(type,0,format0,x,y,0,format3,GL_FLOAT,buffer);		
	
	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}

GLuint GFX::NewByteTex(int x,int y,char* buffer,bool alpha)
{
//		GLuint type = GL_TEXTURE_RECTANGLE_ARB;
	GLuint type = GL_TEXTURE_2D;

	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format
	GLuint format0 = (alpha) ? GL_RGBA : GL_RGB;		
	glTexImage2D(type,0,format0,x,y,0,format0,GL_UNSIGNED_BYTE,buffer);

	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}

GLuint GFX::NewByteRectTex(int x,int y,char* buffer,bool alpha)
{
	GLuint type = GL_TEXTURE_RECTANGLE_ARB;
//	GLuint type = GL_TEXTURE_2D;

	GLuint tex;
	glGenTextures (1, &tex);
	glBindTexture(type,tex);
	get_error();

	// set texture parameters
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// define texture with floating point format
	GLuint format1 = (alpha) ? GL_RGBA8 : GL_RGB8;		
	GLuint format2 = (alpha) ? GL_RGBA : GL_RGB;		
	glTexImage2D(type,0,format1,x,y,0,format2,GL_UNSIGNED_BYTE,buffer);

	get_error();
	glBindTexture(type,0);
	get_error();

	return tex;
}

void GFX::get_error()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) 
	{
		printf("GL GFX Error: %s\n",gluErrorString(err));
		printf("Programm Stopped!\n");
		while(1)Sleep(100);;
	}
}

/*
void GFX::InitFont()
{
	//create 96 display lists, starting at 32
	//Init the font
	HFONT font;											//windows font ID
	HWND  hwnd= FindWindow("GLUT", "ClimbingArchitecture");
	HDC hDC; hDC = GetDC(hwnd);
	//create 96 display lists
	GLuint base;					//display list base for font
	GLuint startTextModeList;
	base = glGenLists(96);
	font	=	CreateFont(	-18,						//font height
							0,							//default width
							0, 0,						//angles - escapement, orientation
							FW_BOLD,					//font weight, 0-1000, NORMAL, BOLD
							false,						//italic
							false,						//underline
							false,						//strikeout
							ANSI_CHARSET,				//character set
							OUT_TT_PRECIS,				//precision
							CLIP_DEFAULT_PRECIS,		//clip precision
							ANTIALIASED_QUALITY,		//output quality
							FF_DONTCARE | DEFAULT_PITCH,//family and pitch
							"Courier New");				//font name
	//select the font
	SelectObject(hDC, font);
	//create 96 display lists, starting at 32
	wglUseFontBitmaps(hDC, 32, 96, base);
}
void GFX::BeginText()
{
		//If not yet created, make display list
	if(!startTextModeList)
	{
		startTextModeList=glGenLists(1);
		glNewList(startTextModeList, GL_COMPILE);
		{
			//save states
			glPushAttrib(GL_ALL_ATTRIB_BITS);
			glListBase(base-32);							//set the list base
		
			//set modelview matrix
			glPushMatrix();
			glLoadIdentity();

			//set projection matrix
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
	
			//set states
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
	
			glBlendFunc(GL_ONE, GL_ONE);	//if blending, use additive
		}
		glEndList();
	}

	glCallList(startTextModeList);
}
void GFX::Print(int x, int y, const char * string, ...)
{
	char text[256];									//Holds our string
	va_list va;										//pointer to list of arguments
	
	if(string==NULL)								//If there's no text
		return;										//Do nothing

	va_start(va, string);							//parse string for variables
		vsprintf(text, string, va);					//convert to actual numbers
	va_end(va);										//results stored in text

	glRasterPos2i(x, y);								//go to correct raster position

	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	//call display lists
}
void GFX::EndText()
{
	//restore states
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}
*/
