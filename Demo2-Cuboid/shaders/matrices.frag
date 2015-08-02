
uniform sampler2DRect texSplineRowX;
uniform sampler2DRect texSplineRowY;
uniform sampler2DRect texSplineRowZ;
uniform sampler2DRect texSplineParams;
uniform float adjust_pos;
uniform float adjust_rot;
//uniform float pose_ofs;

varying vec2 texCoordMatrix;
varying vec2 texCoordParams;

varying float a;

void main(void)
{
	float pi	= 3.14159265358979323846;
	
	vec4 param1 = texture2DRect(texSplineParams,texCoordParams);
	vec4 param2 = texture2DRect(texSplineParams,texCoordParams+vec2(1,0));
	vec4 param3 = texture2DRect(texSplineParams,texCoordParams+vec2(2,0));
	vec4 param4 = texture2DRect(texSplineParams,texCoordParams+vec2(3,0));
	
	vec3  spline_p1		= param1.xyz;
	vec3  spline_d1		= param2.xyz;
	vec3  spline_d2		= param3.xyz;
	vec3  spline_normal	= param4.xyz;
	float angle			= param1.w;
	float twist			= param2.w;
	
	float a01 = clamp(a,0,1);
	
	float a_21 = a*2-1;
	float a_rot = sign(a_21) * (1-pow(1-abs(a_21),adjust_rot)) * 0.5 +0.5;

	float p			= adjust_pos;
	float pow_a		= pow(a01  ,p);
	float pow_1a	= pow(1-a01,p);
	float f2_a		=      a *pow_a;
	float f1_a		= 1-(1-a)*pow_1a;
	float f2_da	= pow_a;
	float f1_da	= pow_1a;
	
	//vec4 pose= texture2DRect(texSplineRowZ,vec2(texCoordMatrix.x,pose_ofs));
	//vec4 pose_d = texture2DRect(texSplineRowZ,vec2(texCoordMatrix.x+5,pose_ofs))-pose;

	vec3 pos= spline_p1 + spline_d1 * f1_a + spline_d2 * f2_a;
	vec3 n1 =            spline_normal;// * pose.y;
	vec3 n3 = normalize (spline_d1 * f1_da + spline_d2 * f2_da);
	vec3 n2 = normalize (cross( n3,n1 ));
	
	//pos = pos - n2;
	
	//pos = pos + n2 * (pose.z-1)*1.5;
	//n2 = n2 * pose.x;
	
	mat4 bind_mat;
	mat4 spline_mat;
	
	spline_mat[0] = vec4(n1,0.0);
	spline_mat[1] = vec4(n2,0.0);
	spline_mat[2] = vec4(n3,0.0);
	spline_mat[3] = vec4(pos,1.0);
	
	spline_mat = transpose(spline_mat);

	vec4 row1= texture2DRect(texSplineRowX,texCoordMatrix);
	vec4 row2= texture2DRect(texSplineRowY,texCoordMatrix);
	vec4 row3= texture2DRect(texSplineRowZ,texCoordMatrix);

	float r = angle + twist * a_rot;
	float cos_a  = cos(r);
	float sin_a  = sin(r);
	
	mat4 rot_mat;
	rot_mat[0] = vec4 (cos_a,-sin_a,0.0,0.0);
	rot_mat[1] = vec4 (sin_a,cos_a,0.0,0.0);
	rot_mat[2] = vec4 (0.0,0.0,1.0,0.0);
	rot_mat[3] = vec4 (0.0,0.0,0.0,1.0);

	bind_mat[0] = row1;
	bind_mat[1] = row2;
	bind_mat[2] = row3;
	bind_mat[3] = vec4(0.0,0.0,0.0,1.0);
	
	/*
	spline_mat[0].w = spline_mat[0].w - dot( spline_mat[0].xyz, n2.x );
	spline_mat[1].w = spline_mat[1].w - dot( spline_mat[1].xyz, n2.y );
	spline_mat[2].w = spline_mat[2].w - dot( spline_mat[2].xyz, n2.z );
	*/
//	mat4 final_mat = bind_mat * rot_mat * spline_mat;
	mat4 final_mat = rot_mat * spline_mat;
	
	//vec4 n2_xf = final_mat * vec4(n2.xyz,0.0);
	
	gl_FragData[0] = final_mat[0] ;
	gl_FragData[1] = final_mat[1] ;
	gl_FragData[2] = final_mat[2] ;
}

