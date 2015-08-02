///////////////////////////////////////////
//#define USE_PBUFFER
///////////////////////////////////////////
#define _USE_MATH_DEFINES
typedef unsigned int uint;
///////////////////////////////////////////
#include <iostream> 
#include <assert.h>
#include <string> 
#include <vector> 
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <math.h>
using namespace std;
///////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>     // Header File For sleeping.
#endif
///////////////////////////////////////////
#include "VecMath.h"
#include "GFX.h"
#include "PBuffer.h"
#include "glsl.h"
#include "Tools.h"
#include "MouseKeyboard.h"
///////////////////////////////////////////
const int num_characters = 33;
int rendered_triangles = 0;
int rendered_models = 0;
//float global_bend_adjust = 5;
///////////////////////////////////////////
enum RenderPass { SHADOW , NORMAL };
static RenderPass renderpass;
static matrix44 model_view44;
static matrix44 model_view_inverse44;
glShaderManager *pshader_manager;
static glShader* shader_shadow;
static glShader* shader_normal;
static glShader* shader_deform;
static glShader* shader_matrices;
static glShader* shader_matrices2;
static vector3  light_pos_in;
static matrix44 light_view44;
static matrix44 light_2_shadow;
static int light_tex;
static int depth_tex;
static int render_index=0;
///////////////////////////////////////////
#include "Spline.h"
static   SplineTex spline_tex;
///////////////////////////////////////////

//static Bmp weight_map("../media/bindleg2.bmp");
//static Bmp weight_map("../media/female-weights.bmp");
static Bmp weight_map("../media/female-bind2.bmp");

//static Bmp deform_map("../media/style1/textures/deform1.bmp");
static Bmp displacement_map_front("../media/style1/displacement_front.bmp");
static Bmp displacement_map_left_right("../media/style1/displacement_left_right.bmp");
static Bmp displacement_map_back("../media/style1/displacement_back.bmp");

Spline spline_elbow,spline_arm,spline_body;

///////////////////////////////////////////
#include "Geometry.h"
#include "Deformer.h"
#include "ObjControl.h"
#include "ObjManager.h"
///////////////////////////////////////////
PBUFFER pbuffer;
GLuint  pbufferTexture;	//texture object which is used when pbuffer texture is displayed
///////////////////////////////////////////
ObjManager manager;
///////////////////////////////////////////
void setup_bind_pose_female_legs(float x=0,float y=0)
{
	//y=y-0.5;

	vec3f a1( -0.019, 2.694, -1.179 );
	vec3f a2;
	vec3f a3( -0.019, -0.299, -1.179);
	
	vec3f e1( -0.019, -0.299, -1.179);
	vec3f e2;
	vec3f e3( -0.019, -5.028, -1.340);

	vec3f b1( -0.019, 5.336, 0.042);
	vec3f b2;
	vec3f b3( -0.019, 2.009, 0.042);

	a2 = (a1+a3)*0.5;
	b2 = (b1+b3)*0.5;
	e2 = (e1+e3)*0.5;

	x+=0.01;
	y+=0.01;
	//x=y=0.01;

	if(x<0)		a3.z+=x;
	a3.y+=-0.01;
	a3.x+=-0.01;
	e3.y+=-0.01;

	a3.x+=-(y);
	if(y<0)a3.y+=abs((y)/2);

	e3.x+=-abs(y);
	e3.y+=abs(y);

	b3.x+=-y;
	b3.z+=-x;
	b3.x+=0.01;

	if(y<0)
	{
	//	e3.y+=-y*4;
		//e3.z+=abs(y*4);
	}
	//b1.x+=0.01;//-y*3;
	//b1.x+=y;
/*
	b3.z+=y;
	b3.x+=x*2;

	e3.x+=x/2;
	e3.z-=y;
*/
	a3 = a2 + vec3f::normalize(a3-a2) * (a2-a1).length();
	b3 = b2 + vec3f::normalize(b3-b2) * (b2-b1).length();
	e3 = e2 + vec3f::normalize(e3-e2) * (e2-e1).length();

	spline_body.set ( b1,b2,b3,0,-x/2);
	spline_arm.set  ( a1,a2,a3,0,0,spline_body.spline_xform);
	spline_elbow.set( e1,e2,e3,0,0,spline_body.spline_xform*spline_arm.spline_xform);
}
void setup_bind_pose_female(float x=0,float y=0)
{
	vec3f a1( -0.25, 6.625, -1.671-0.3 );
	vec3f a2;
	vec3f a3( -0.25, 6.625, -2.864-0.6);
	
	vec3f e1( -0.25, 6.625, -2.864-0.6);
	vec3f e2;
	vec3f e3( -0.25, 6.625, -8.562);

	vec3f b1( -0.25, 5.625, 0.09 );
	vec3f b2;
	vec3f b3( -0.25, 1.398, 0.09);

	a2 = (a1+a3)*0.5;
	b2 = (b1+b3)*0.5;
	e2 = (e1+e3)*0.5;

	//x=y=0;

	a3.y+=y*2;
	a3.x+=-0.01;
	e3.x+=-0.01;
	if(x<0)a3.x+=-x;
	if(x<0)e3.x+=-x;
	if(y<0)
	{
		e3.y+=-y*4;
		e3.z+=abs(y*4);
	}
	b1.x+=0.01;//-y*3;
	//b1.x+=y;
/*
	b3.z+=y;
	b3.x+=x*2;

	e3.x+=x/2;
	e3.z-=y;
*/
	a3 = a2 + vec3f::normalize(a3-a2) * (a2-a1).length();
	b1 = b2 + vec3f::normalize(b1-b2) * (b2-b3).length();
	e3 = e2 + vec3f::normalize(e3-e2) * (e2-e1).length();

	spline_body.set ( b3,b2,b1,0,x);
	spline_arm.set  ( a1,a2,a3,0,0,spline_body.spline_xform);
	spline_elbow.set( e1,e2,e3,0,0,spline_body.spline_xform*spline_arm.spline_xform);
}
void setup_bind_pose_legs(float x=0,float y=0)
{
	vec3f a1( -1.023, 6.671, -0.051 );
	vec3f a2;
	vec3f a3(-0.787, 5.088, -0.081 );
	
	vec3f e1( 0.778, 6.701, 0.073);
	vec3f e2;
	vec3f e3( 0.855, 5.043, 0.272);

	vec3f b1( 0.049, 8.069+0.5, -0.172 );
	vec3f b2;
	vec3f b3( 0.137, 6.245, -0.393 );

	a2 = (a1+a3)*0.5;
	b2 = (b1+b3)*0.5;
	e2 = (e1+e3)*0.5;

	a3.x+=x/2;
	a3.z+=y;

	b3.z+=y;
	b3.x+=x*2;

	e3.x+=x/2;
	e3.z-=y;

	a3 = a2 + vec3f::normalize(a3-a2) * (a2-a1).length();
	b3 = b2 + vec3f::normalize(b3-b2) * (b2-b1).length();
	e3 = e2 + vec3f::normalize(e3-e2) * (e2-e1).length();

	spline_body.set ( b1,b2,b3,0,x*2);
	spline_arm.set  ( a1,a2,a3,0,x,spline_body.spline_xform);
	spline_elbow.set( e1,e2,e3,0,x,spline_body.spline_xform);
}

void setup_bind_pose(float x=0,float y=0)
{
// setup_bind_pose_legs(x,y);
// setup_bind_pose_female(x,y);
 setup_bind_pose_female_legs(x,y);
 

}


///////////////////////////////////////////
void DrawScene()
{
	mouse_keyboard();

	#ifdef USE_PBUFFER
	pbuffer.begin();
	#endif

	
	vec3f p ( 0,4,0 );
	vec3f d1 ( 0,0,6 );
	vec3f d2 ( GFX::mouseX*4-2 , -GFX::mouseY*8+4 , 1);
	d2.normalize();
	d2 = d2 * 6;
	vec3f normal; 
	normal.cross(d1,d2);
	normal.normalize();
	float angle = normal.angle2( vec3f (0,1,0) , d1 );
	float twist = 0;//GFX::mouseX*10-5;
	//manager.controls[0]->set_spline(p , p+d1, p+d1+d2, angle,twist);

	//static int texture = GFX::LoadTex("data/textures/flare.bmp");
	static int white   = GFX::LoadTex("../media/textures/white.bmp");

	glLoadIdentity();			// Reset The View
	glRotatef(rot.x,1,0,0);		// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(rot.y,0,1,0);		// Move Left 1.5 Units And Into The Screen 6.0
	glTranslatef(pos.x,pos.y,pos.z);		// Move Left 1.5 Units And Into The Screen 6.0
	glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*)model_view44.m); 
	model_view_inverse44 = model_view44;
	model_view_inverse44.invert();

	matrix44 correct44 = matrix44
	(	0.5,  0,0  ,0,
		0  ,0.5,0  ,0,
		0  ,  0,0.5,0,
		0.5,0.5,0.5,1);

	bool light_change = false;
	if ( GFX::mousePressed[2] )
	{
		light_view44.m[3][0] -= (GFX::mouseDX)*50;
		light_view44.m[3][1] += (GFX::mouseDY)*50;
		light_change = true;
	}
	if ( GFX::mousePressed[1] )
	{
		light_view44.m[3][2] += (GFX::mouseDY)*50;
		light_change = true;
	}
	//if ( GFX::keyPressed[13] )//enter
	{
		glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*)light_view44.m); 
		light_change = true;
	}
	if ( light_change )
	{
		matrix44 light_inverse44 = light_view44; 
		light_inverse44.invert();		
		light_pos_in = light_inverse44 * vector3(0,0,0);
	}

	matrix44 projection44;
	glGetFloatv (GL_PROJECTION_MATRIX, (GLfloat*)projection44.m);
	light_2_shadow =  light_view44 * projection44 * correct44;

	//static FBO fbo; 
	static bool init = true;
	static glShaderManager shader_manager;

	static int bridge_start;

	if (init)
	{
		pshader_manager = &shader_manager;

		if(0)
		for (int a=0;a<10;a++)
		{
			int i = manager.new_control_from (5);
			if( a==0) bridge_start = i;
		}
		int max_draw_buffers;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &max_draw_buffers);
		printf("MaxDrawBuffers:%d\n",max_draw_buffers);

	//	fbo.init( 512,512 );
		
		shader_deform = shader_manager.loadfromFile(
			"shaders/deform.vert",
			"shaders/deform.frag"
			);
		
		shader_normal = shader_manager.loadfromFile(
			"shaders/normal.vert",
			"shaders/normal.frag"
			);
		
		shader_matrices = shader_manager.loadfromFile(
			"shaders/matrices.vert",
			"shaders/matrices.frag"
			);
		
		shader_matrices2 = shader_manager.loadfromFile(
			"shaders/matrices2.vert",
			"shaders/matrices2.frag"
			);
		/*
		shader_shadow = shader_manager.loadfromFile(
			"shaders/shadow.vert",
			"shaders/shadow.frag"
			);
		*/	
		const int sb_w=32,sb_h=3*num_characters,pose_h=64+1;
		
		// nbtp = normal binormal tangent pos
		static float *mem_spline_nbtpx= new float[sb_w*(sb_h+pose_h*2)*4];
		static float *mem_spline_nbtpy= new float[sb_w*(sb_h+pose_h*2)*4];
		static float *mem_spline_nbtpz= new float[sb_w*(sb_h+pose_h*2)*4];

		const int   spline_num_params = 6;
		static float mem_spline_params[sb_h*spline_num_params*4];

		for (int y=0;y<pose_h*2;y++)
		for (int x=0;x<sb_w;x++)
		{
			float pos_x = float(x) / float (sb_w);
			float pos_y = float(y) / float (pose_h-1);

			vec3f f_dx_dz_1 = displacement_map_front.get_f_fdx_fdy(pos_x,pos_y);
			vec3f f_dx_dz_2 = displacement_map_left_right.get_f_fdx_fdy(pos_x,pos_y);
			vec3f f_dx_dz_3 = displacement_map_back.get_f_fdx_fdy(pos_x,pos_y);

			if (y==pose_h-1)
			{
				f_dx_dz_1 = vec3f(1,0,0);
				f_dx_dz_2 = vec3f(1,0,0);
				f_dx_dz_3 = vec3f(1,0,0);

				if(pos_x>0.5)
				{
					f_dx_dz_1.x =  0.1+0.9*sqrt(sin((pos_x-0.4) * M_PI));
					f_dx_dz_1.y =  0.9*0.25*(cos((pos_x-0.4) * M_PI));
				}else
				{
					f_dx_dz_1.x =  0.1+0.9*sqrt(-sin((pos_x-0.6) * M_PI));
					f_dx_dz_1.y =  -0.9*0.25*(cos((pos_x-0.6) * M_PI));
				}
				//f_dx_dz_1 = vec3f(1,0,0);
				float limit = 0.4;
				/*
				if(pos_x<limit)
				{
					f_dx_dz_1.x += -cos(pos_x * 2*M_PI/limit)*0.25+0.25;
					f_dx_dz_1.y += sin(pos_x * 2*M_PI/limit)*0.25;
				}*/
				f_dx_dz_1.z =  0;

				limit = 0.4;
				/*
				if(pos_x<limit)
				{
					f_dx_dz_2.x = -cos(pos_x * 2*M_PI/limit)*0.15+1.15;
					f_dx_dz_2.z = -sin(pos_x * 2*M_PI/limit)*0.15;
					f_dx_dz_2.y =  0;
				}
				*/
				if(pos_x>0.25)
				if(pos_x<0.75)
				{
					//f_dx_dz_2.x +=-cos(((pos_x-0.25)*2) * 2*M_PI)*0.15+0.15;
					//f_dx_dz_2.z +=-sin(((pos_x-0.25)*2) * 2*M_PI)*0.15;
				}
				if(pos_x>0.25)
				if(pos_x<0.75)
				{
					//f_dx_dz_3.x +=-cos(((pos_x-0.25)*2) * 2*M_PI)*0.15+0.15;
					//f_dx_dz_3.y +=-sin(((pos_x-0.25)*2) * 2*M_PI)*0.15;
				}
				//f_dx_dz_3.x = cos(pos_x * 2*M_PI)*0.25+0.75;
				//f_dx_dz_3.y = sin(pos_x * 2*M_PI)*0.25;
				//f_dx_dz_3.z = 0;
			}
			else
			if ((x<5)||(x>sb_w-5)||(y<5)||(y>pose_h-5))
			{
				f_dx_dz_1 = vec3f(1,0,0);
				f_dx_dz_2 = vec3f(1,0,0);
				f_dx_dz_3 = vec3f(1,0,0);
			}

			if (y>=pose_h)
			{
				f_dx_dz_1 = vec3f(1,0,0);
				f_dx_dz_2 = vec3f(1,0,0);
				f_dx_dz_3 = vec3f(1,0,0);
			}
			//f_dx_dz_1 = vec3f(1,0,0);
			//f_dx_dz_2 = vec3f(1,0,0);
			//f_dx_dz_3 = vec3f(1,0,0);

			vec3f dx1 ( 1 ,-f_dx_dz_1.z ,0 );
			vec3f dz1 ( 0, -f_dx_dz_1.y ,1 );
			vec3f normal_1;normal_1.cross(dz1,dx1);

			vec3f dx2 ( 1 ,-f_dx_dz_2.z ,0 );
			vec3f dz2 ( 0, -f_dx_dz_2.y ,1 );
			vec3f normal_2;normal_2.cross(dz2,dx2);

			vec3f dx3 ( 1 ,-f_dx_dz_3.z ,0 );
			vec3f dz3 ( 0, -f_dx_dz_3.y ,1 );
			vec3f normal_3;normal_3.cross(dz3,dx3);

			int ofs = (x+(sb_h+y)*sb_w)*4;
			
			mem_spline_nbtpx[ofs+0] = normal_1.x;//normal.x;//deform.m[0][0];//*scale;
			mem_spline_nbtpx[ofs+1] = normal_1.y;//normal.y;//deform.m[1][0];//*scale;
			mem_spline_nbtpx[ofs+2] = normal_1.z;//deform.m[2][0];//normal.z;//deform.m[2][0];//*scale;
			mem_spline_nbtpx[ofs+3] = f_dx_dz_1.x;//deform.m[3][0];//f_dx_dz.x;////1;//deform.m[3][0];//*scale;

			mem_spline_nbtpy[ofs+0] = normal_2.x;//deform.m[0][1];//*scale;
			mem_spline_nbtpy[ofs+1] = normal_2.y;//deform.m[1][1];//*scale;
			mem_spline_nbtpy[ofs+2] = normal_2.z;//deform.m[2][1];//*scale;
			mem_spline_nbtpy[ofs+3] = f_dx_dz_2.x;//deform.m[3][1];//*scale;

			mem_spline_nbtpz[ofs+0] = normal_3.x;//deform.m[0][2];//*scale;
			mem_spline_nbtpz[ofs+1] = normal_3.y;//deform.m[1][2];//*scale;
			mem_spline_nbtpz[ofs+2] = normal_3.z;//deform.m[2][2];//*scale;
			mem_spline_nbtpz[ofs+3] = f_dx_dz_3.x;//deform.m[3][2];//*scale;
		}

		SplineTex tex;
		tex.num_params= spline_num_params;
		tex.width	= sb_w;
		tex.height	= sb_h;
		tex.data_nbtpx = (float*) mem_spline_nbtpx;
		tex.data_nbtpy = (float*) mem_spline_nbtpy;
		tex.data_nbtpz = (float*) mem_spline_nbtpz;
		tex.data_params= (float*) mem_spline_params;

		matrix44 mats_body	[sb_w];
		matrix44 mats_arm	[sb_w];
		matrix44 mats_elbow	[sb_w];

		setup_bind_pose( 0 , 0 );
		spline_body.bind_normal	= spline_body.bind_normal_cmp ;
		spline_arm.bind_normal	= spline_arm.bind_normal_cmp ;
		spline_elbow.bind_normal	= spline_elbow.bind_normal_cmp ;

		for (int c=0;c<num_characters;c++)
		{
			spline_body	.sample(tex,0+c*3);
			spline_arm		.sample(tex,1+c*3);
			spline_elbow	.sample(tex,2+c*3);
		}
		tex.handle_nbtpx=  GFX::NewFloat16Tex(sb_w,sb_h+pose_h*2,(char*)mem_spline_nbtpx,true);
		tex.handle_nbtpy=  GFX::NewFloat16Tex(sb_w,sb_h+pose_h*2,(char*)mem_spline_nbtpy,true);
		tex.handle_nbtpz=  GFX::NewFloat16Tex(sb_w,sb_h+pose_h*2,(char*)mem_spline_nbtpz,true);
		tex.handle_params= -1;
		spline_tex = tex;

		init = false;
	}

	static int t1  = timeGetTime();
	       int t2  = timeGetTime();
	static int fps = 0;
	static int tps = 0;

	tps += rendered_triangles;

	if( t2-t1 >= 1000 )
	{
		float ms = (t2-t1)/float(fps+1);
		printf("Fps:%d, %3.3f ms %3.2f MTps %3.2f MVps Scene:%3.2f MVerts %d Characters\r",
			fps,ms,float(tps)/1000000.0,float(tps)*3.0/1000000.0,float(rendered_triangles)*3.0/1000000.0,rendered_models); 
		//printf("Fps:%d, %3.3f ms Light: %3.3f %3.3f %3.3f r %3.3f %3.3f %3.3f\r",fps,ms,pos.x,pos.y,pos.z,rot.x,rot.y,rot.z); 
		fps = 0;
		t1 = t2;
		tps = 0;
	}
	fps++;

	renderpass = SHADOW;

	/*
	fbo.enable();
	glDrawBuffer( GL_NONE );

	glEnable (GL_POLYGON_OFFSET_FILL);
	glPolygonOffset (12, 12);

	glPushMatrix();
	glLoadMatrixf((GLfloat*)light_view44.m);
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);		// This Will Clear The Background Color To Black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	//manager.draw();
	glPopMatrix();

	glDisable (GL_POLYGON_OFFSET_FILL);

	fbo.disable();
	*/
	//glDrawBuffer( GL_BACK );

	renderpass = NORMAL;

	light_tex = white;//texture;
	depth_tex = white;//fbo.depth_tex;

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int tex_width=viewport[2];
	int tex_height=viewport[3];
	glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity(); 
	glMatrixMode(GL_PROJECTION);glPushMatrix();	glLoadIdentity();
	gluOrtho2D(0.0,tex_width,0.0,tex_height);	
	glMatrixMode(GL_MODELVIEW); 
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBegin(GL_QUADS); 
	float r1=0.5;//0.2;
	float g1=0.5;//0.2;
	float b1=0.5;//0.6;
	float r2=1.0;
	float g2=1.0;
	float b2=1.0;
	glColor3f(r1,g1,b1);
	glVertex2f(0, 0);
	glColor3f(r1,g1,b1);
	glVertex2f(tex_width,0);
	glColor3f(r2,g2,b2);
	glVertex2f(tex_width, tex_height);
	glColor3f(r2,g2,b2);
	glVertex2f(0, tex_height);
	glEnd();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glMatrixMode(GL_PROJECTION);glPopMatrix();
	glMatrixMode(GL_MODELVIEW); glPopMatrix();
	glDrawBuffer(GL_BACK);

	static float rnd[8] ={
		vec3f::random_float(),vec3f::random_float(),
		vec3f::random_float(),vec3f::random_float(),
		vec3f::random_float(),vec3f::random_float(),
		vec3f::random_float(),vec3f::random_float()
	};

	rendered_triangles = 0;

	static int style = 0;
	if ( GFX::keyPressed['1'] ) style=0; 
	if ( GFX::keyPressed['2'] ) style=1; 

	spline_body  	.set_style(1);
	spline_arm		.set_style(1);
	spline_elbow	.set_style(style);

	spline_body  	.set_hardness(1.5);
	spline_arm		.set_hardness(1);
	spline_elbow	.set_hardness(2);

	spline_body  	.set_twist_adjust(1);
	spline_arm		.set_twist_adjust(1);
	spline_elbow	.set_twist_adjust(1);

	static float mx=0;
	static float my=0;

	mx = 0.1*mx + GFX::mouseX*0.9;
	my = 0.1*my + GFX::mouseY*0.9;

	for (int c=0;c<num_characters;c++)
	{
		float a = 4*0.5*sin(mx*4+rnd[c&7]*(float(c)*20/float(num_characters)));
		float b = 4*0.5*sin(my*4+rnd[c&7]*(float(c)*20/float(num_characters)));
		//if(c==0)
		//	setup_bind_pose( (GFX::mouseX-0.5)*3,(GFX::mouseY-0.5)*6 );
		//else
			setup_bind_pose( a , b );

		//setup_bind_pose( (GFX::mouseX-0.5)*3 , (GFX::mouseY-0.5)*3 );

		spline_body  	.set_params_to_tex(spline_tex,0+c*3);
		spline_arm		.set_params_to_tex(spline_tex,1+c*3);
		spline_elbow	.set_params_to_tex(spline_tex,2+c*3);
	}
	spline_tex.update_params();
	render_index=0;
	setup_bind_pose( 0,0 );
	manager.draw();

	vec3f lp ( light_pos_in.x,light_pos_in.y,light_pos_in.z );

	glDisable(GL_DEPTH_TEST);
	
//	setup_bind_pose( (GFX::mouseX-0.5)*3 , (GFX::mouseY-0.5)*3 );
		float a = 4*0.5*sin(mx*4+rnd[1&7]*(float(1)*20/float(num_characters)));
		float b = 4*0.5*sin(my*4+rnd[1&7]*(float(1)*20/float(num_characters)));

	setup_bind_pose( a,b );
	spline_arm.draw();
	spline_elbow.draw();
	spline_body.draw();

	//GFX::Line( spline_body.p3, spline_arm.p1, vec3f(0,1,0) );
	//GFX::Line( spline_arm.p3, spline_elbow.p1, vec3f(0,1,0) );

	glBindTexture  (GL_TEXTURE_2D, 	white );
	glColor3f(1,0,0);
	//Box( lp , vec3f(0.1,0.1,0.1) );
	glEnable(GL_DEPTH_TEST);

	vec3f p1 (0,-10+0,0);
	vec3f p2 (0,-10+5,0);
	vec3f p3 (5,-10+5,0);
	vec3f p4 (5,-10+0,0);
	vec3f p12=(p1+p2)*0.5;
	vec3f p23=(p2+p3)*0.5;
	vec3f p34=(p3+p4)*0.5;
	vec3f p41=(p4+p1)*0.5;


	#ifdef USE_PBUFFER
	pbuffer.end();
	
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);		// This Will Clear The Background Color To Black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glLoadIdentity();				// Reset The View
	glTranslatef(0,0,-30);
	//glRotatef( (GFX::mouseX-0.5)*100 , 1,0,0);
	glBindTexture (GL_TEXTURE_2D, 	pbuffer.pbufferTexture );
	//use the pbuffer as the texture
	wglBindTexImageARB(pbuffer.hBuffer, WGL_FRONT_LEFT_ARB);
	Box( vec3f (0,0,0) , vec3f(10,10,5) );
	wglReleaseTexImageARB(pbuffer.hBuffer, WGL_FRONT_LEFT_ARB);
	#endif

	GFX::Flip();
}

///////////////////////////////////////////
///////////////////////////////////////////
	void get_GL_error()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) 
		{
			printf("GL Error: %s\n",gluErrorString(err));
			printf("Programm Stopped!\n");
			while(1)Sleep(100);;
		}
	}

///////////////////////////////////////////
int main(int argc, char **argv) 
{ 
	setup_bind_pose();

	GFX gfx;
	gfx.Init(800,600,false,DrawScene);

	timeBeginPeriod(1); 
	glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);get_GL_error();

	wglSwapIntervalEXT(0);

	#ifdef USE_PBUFFER
	pbuffer.Init(512,512);
	pbuffer.begin();
	gfx.InitGL(512,512);
	#endif

//	manager.load_obj ("../media/trousers.obj");
//	manager.load_obj ("../media/box2.obj");
//	manager.load_obj ("../media/can4.obj");
//	manager.load_obj ("../media/suspension.obj");
//	manager.load_obj ("../media/leg3.obj");
//	manager.load_obj ("../media/female3.obj");
//	manager.load_obj ("../media/char3.obj");
	manager.load_obj ("../media/female-legs.obj");
	

	#ifdef USE_PBUFFER
	pbuffer.end();
	#endif

	manager.print_controls();
	manager.print_materials();
    gfx.Run();
    return 0;
}
