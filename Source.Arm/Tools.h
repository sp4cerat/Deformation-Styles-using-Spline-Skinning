std::string int_to_str(const int& x)
{
	std::ostringstream o;
	if (!(o << x)) return "ERROR";
	return o.str();
}
/*
void Line ( vec3f p1 , vec3f p2 , vec3f color=vec3f(1,1,0) ){
	glBegin(GL_LINES);
	glColor3f(color.x ,color.y ,color.z );
	glVertex3f( p1.x,p1.y,p1.z );
	glVertex3f( p2.x,p2.y,p2.z );
	glEnd();
}
*/
void Box ( vec3f pos , vec3f size  ){

	float xyz[3][4]={{-1,1,1,-1},{1,1,-1,-1},{1,1,1,1}};
	int  xchg[6][3]={{1,2,0},{0,1,2},{2,0,1},{0,1,2},{1,2,0},{2,0,1}};
	int  mult[6][3]={{0,0,0},{0,0,0},{0,0,0},{1,0,1},{1,1,0},{1,0,1}};
	float tex_x[4]={0,1,1,0};
	float tex_y[4]={1,1,0,0};

	// draw a triangle
	glBegin(GL_QUADS);				// start drawing a polygon
	for (int j=0;j<6;j++)
	{
		for (int i=0;i<4;i++)
		{
			glColor3f(1,1,1);
			float x = xyz[xchg[j][0]][i]; if(mult[j][0]) x=-x; 
			float y = xyz[xchg[j][1]][i]; if(mult[j][1]) y=-y; 
			float z = xyz[xchg[j][2]][i]; if(mult[j][2]) z=-z; 
				
			glTexCoord2f( tex_x[i], tex_y[i] );

			glNormal3f( x,y,z );
			glVertex3f( 
				pos.x+x*size.x,
				pos.y+y*size.y,
				pos.z+z*size.z );
		}
	}
	glEnd();					// we're done with the polygon
}
