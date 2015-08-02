uniform float resolution;

varying vec2 texCoordMatrix;
varying vec2 texCoordParams;

varying float a;

void main(void)
{
	//float resolution	= 32.0;
	a					= (gl_MultiTexCoord0.x-0.5) / resolution;

	texCoordMatrix		= gl_MultiTexCoord0.xy;
	texCoordParams		= vec2(0,gl_MultiTexCoord0.y);
	
	gl_Position			= gl_ModelViewProjectionMatrix * gl_Vertex;
}
