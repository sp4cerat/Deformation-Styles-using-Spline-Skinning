//vec3f pos(-75,-29,12);
vec3f pos(-2.1,-6.1,6.8);
vec3f rot(-9.5,-161,0);

void mouse_keyboard()
{
	/*glPushMatrix();
	glLoadIdentity();				// Reset The View
	glRotatef(rot.x,1,0,0);		// Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(rot.y,0,1,0);		// Move Left 1.5 Units And Into The Screen 6.0
	float m[16];
    glGetFloatv (GL_MODELVIEW_MATRIX, m); 
	glPopMatrix();*/

	static int time = timeGetTime ();
	int timeNew = timeGetTime ();
	float deltaTime = timeNew-time; time =timeNew;
	bool* key = GFX::keyPressed;

	vec3f movX = vec3f ( 1,0,0 );
	vec3f movY = vec3f ( 0,1,0 );
	vec3f movZ = vec3f ( 0,0,1 );

	movZ.rot_x (   rot.x * 2*M_PI / 360 );
	movX.rot_x (   rot.x * 2*M_PI / 360 );

	movZ.rot_y (   rot.y * 2*M_PI / 360 );
	movX.rot_y (   rot.y * 2*M_PI / 360 );

	if ( key[GLUT_KEY_UP   ] ) pos=pos+movZ * (0.01 * deltaTime);
	if ( key[GLUT_KEY_DOWN ] ) pos=pos-movZ * (0.01 * deltaTime);
	if ( key[GLUT_KEY_LEFT ] ) pos=pos+movX * (0.01 * deltaTime);
	if ( key[GLUT_KEY_RIGHT] ) pos=pos-movX * (0.01 * deltaTime);

	//printf("p %f %f %f\n", pos.x, pos.y, pos.z );

	if ( key[GLUT_KEY_PAGE_UP   ] ) rot.y-=0.1 * deltaTime;
	if ( key[GLUT_KEY_PAGE_DOWN ] ) rot.y+=0.1 * deltaTime;

	if ( GFX::mousePressed[0] )
	{
		rot.y += GFX::mouseDX * 300;
		rot.x += GFX::mouseDY * 300;
	}
	/*
	if ( keyPressed[GLUT_KEY_LEFT ] ) rot.x-=0.1;
	if ( keyPressed[GLUT_KEY_RIGHT] ) rot.x+=0.1;
	*/
}
