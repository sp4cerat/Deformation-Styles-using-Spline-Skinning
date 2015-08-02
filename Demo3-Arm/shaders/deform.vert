//uniform sampler2D tex_points;

varying vec3 normal;
varying vec3 vertex;
varying vec2 textureCoord;
varying vec2 textureCoord1;
varying vec2 textureCoord2;
varying vec2 textureCoord3;
varying vec2 textureCoord4;

//uniform mat4 light2shadow;
//uniform vec3  spline1_p1;

void main(void)
{
	//normal			= gl_Color.xyz * vec3( 2.0,2.0,2.0 ) - vec3 (1.0,1.0,1.0);
    //vertex			= gl_Vertex.xyz;
    
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
	textureCoord  = gl_MultiTexCoord0.xy;
	textureCoord1 = gl_MultiTexCoord0.xy;
	textureCoord2 = gl_MultiTexCoord0.xy;
	textureCoord3 = gl_MultiTexCoord0.xy;
	textureCoord4 = gl_MultiTexCoord0.xy;
	
	textureCoord1.x = gl_MultiTexCoord0.x*4;
	textureCoord2.x = gl_MultiTexCoord0.x*4+1;
	textureCoord3.x = gl_MultiTexCoord0.x*4+2;
	textureCoord4.x = gl_MultiTexCoord0.x*4+3;
}