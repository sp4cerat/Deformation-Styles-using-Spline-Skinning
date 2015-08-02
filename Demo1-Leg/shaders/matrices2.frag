uniform sampler2DRect texBindRowX;
uniform sampler2DRect texBindRowY;
uniform sampler2DRect texBindRowZ;

uniform sampler2DRect texSplineRowX;
uniform sampler2DRect texSplineRowY;
uniform sampler2DRect texSplineRowZ;

varying vec2 texCoordMatrix;

void main(void)
{
	mat4 bind;
	mat4 spline;
	bind[0]= texture2DRect(texBindRowX,texCoordMatrix);
	bind[1]= texture2DRect(texBindRowY,texCoordMatrix);
	bind[2]= texture2DRect(texBindRowZ,texCoordMatrix);
	bind[3]= vec4 (0.0,0.0,0.0,1.0);
	
	spline[0]= texture2DRect(texSplineRowX,texCoordMatrix);
	spline[1]= texture2DRect(texSplineRowY,texCoordMatrix);
	spline[2]= texture2DRect(texSplineRowZ,texCoordMatrix);
	spline[3]= vec4 (0.0,0.0,0.0,1.0);

	mat4 final_mat = bind * spline;
	
	gl_FragData[0] = final_mat[0] ;
	gl_FragData[1] = final_mat[1] ;
	gl_FragData[2] = final_mat[2] ;
}

