uniform float angle;
uniform float twist;
uniform vec3 spline_p;
uniform vec3 spline_d1;
uniform vec3 spline_d2;
uniform vec3 spline_normal;

uniform float arm_angle;
uniform float arm_twist;
uniform vec3  arm_spline_p;
uniform vec3  arm_spline_d1;
uniform vec3  arm_spline_d2;
uniform vec3  arm_spline_normal;

uniform float elbow_angle;
uniform float elbow_twist;
uniform vec3  elbow_spline_p;
uniform vec3  elbow_spline_d1;
uniform vec3  elbow_spline_d2;
uniform vec3  elbow_spline_normal;

uniform float body_angle;
uniform float body_twist;
uniform vec3  body_spline_p;
uniform vec3  body_spline_d1;
uniform vec3  body_spline_d2;
uniform vec3  body_spline_normal;
/*
void eval_spline ( 
	in float a, 
	in twist,
	in angle,
	in vec3 spline_p,
	in vec3 spline_d1,
	in vec3 spline_d2,
	in vec3 spline_normal 
	in p
	out)
{

}
*/

void main(void)
{
	vec3 pos;
	vec3 n1; // x
	vec3 n2; // y
	vec3 n3; // z
	
	vec3 p = spline_p;
	vec3 d1= spline_d1;
	vec3 d2= spline_d2;
	
	float a = gl_MultiTexCoord1.x;
	float b = gl_MultiTexCoord1.y;
	float c = gl_MultiTexCoord1.z;
	float c01 = clamp ( c , 0.0 , 1.0 );
	float alpha = 0.5 * 3.14159265358979323846 * c01;
	float cos_c = cos ( alpha );
	float sin_c = sin ( alpha );
	float min0 = min(c,0.0);
	float max1 = max(c,1.0);
	pos  = p + d1 * sin_c + d2 * (1.0-cos_c) + d1 * min0 + d2 * max1;
	n3 = normalize(d1 * cos_c + d2 * sin_c);
	n1 = spline_normal;
	n2 = cross( n1,n3 );
	
	float r = angle + twist*c01;
	float sin_r = sin ( r );
	float cos_r = cos ( r );
	float aa = cos_r * b + sin_r * a;
	float bb = cos_r * a - sin_r * b;
	
	vec3 result_pos = n1 * aa + n2 * bb + pos;
	
    gl_Position     = gl_ModelViewProjectionMatrix * vec4 ( result_pos.xyz , 1 ) ;
}