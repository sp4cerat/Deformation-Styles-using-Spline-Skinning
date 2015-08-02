// ---------------------------------------- //
struct Triangle
{
	int points[3];
	int normals[3];
	int tex_coords[3];
	int material;
};
// ---------------------------------------- //
class Texture
{
	public:
	Texture():gl_handle(0){}

	std::string filename;
	int gl_handle;
};
// ---------------------------------------- //
class Deformer ;

class ConDef // Connector Definition
{	
	public:

	enum Type {none,in,out};
	Type type;
	std::vector<Deformer*> compatible;	

	ConDef(void){type=none;}

	Type get_type ( std::string type_str )
	{
		if (type_str.compare(str_in ) == 0 ) return in;
		if (type_str.compare(str_out) == 0 ) return out;
		return none;
	}
	const std::string get_type ()
	{
		if (type == in ) return str_in;
		if (type == out ) return str_out;
		return str_none;
	}
	static std::string str_none;
	static std::string str_in;
	static std::string str_out;
};
std::string ConDef::str_none  = "unused";
std::string ConDef::str_in    = "incoming";
std::string ConDef::str_out   = "outgoing";

class Material
{
	public:

	ConDef con_def;

	Material()
	{
		ambient = vec3f ( 0.6 , 0.3 ,0 );
		diffuse = vec3f ( 0.3 , 0.3 ,0.3 );
		specular= vec3f ( 1,1,1 );
		name = "";
		con_def.type = ConDef::none;
	}
	std::string name;
	vec3f diffuse;
	vec3f specular;
	vec3f ambient;
	Texture diffuse_map;
	Texture bump_map;
};
// ---------------------------------------- //
struct Connector
{
	vec3f n1;
	vec3f n2;
	vec3f n3;
	vec3f origin;
	int material;
	int triangle;
};
// ---------------------------------------- //
struct vec4f
{
	float x,y,z,w;
};
// ---------------------------------------- //
struct vec2f{float x,y;};
// ---------------------------------------- //
class Config{ public: // Object configuration

	bool deformable;
	bool path;
	bool scale_until_collision;

	float twist_random_from;
	float twist_random_to;
	float scale_random_from;
	float scale_random_to;
	float bend_angle_max;
	
	Config()
	{
		deformable = false;
		path = false;
		scale_until_collision = false;
		twist_random_from = 0;
		twist_random_to = 0;
		scale_random_from = 1;
		scale_random_to = 1;
		bend_angle_max = 100;
	}
};
// ---------------------------------------- //
class Geometry
{
	public:

	/*	
	Bmp bind_normals_x( 1024,1024,24,0 );
	int bind_normals_x_handle;
	Bmp bind_normals_y( 1024,1024,24,0 );
	int bind_normals_y_handle;
	Bmp bind_normals_z( 1024,1024,24,0 );
	int bind_normals_z_handle;
	*/
	Bmp bind_normals;
	int bind_normals_handle;
	
	GLuint tex_width ;
	GLuint tex_height;
	GLuint tex_height_real;
	GLuint tex_points;
	GLuint tex_normals;
	GLuint tex_colors;
	GLuint tex_weights;
	GLuint tex_indices;
	GLuint tex_spline_ofs;
	GLuint tex_nwio; // normal weight index splineoffset

	Config config;

	std::string name;
	std::string filename;
	std::vector<vec3f> points;
	std::vector<vec3f> normals;
	std::vector<vec3f> tex_coords;
	std::vector<Triangle> triangles;
	std::vector<Connector> inputs;
	std::vector<Connector> outputs;
	vec3f bb_min;
	vec3f bb_max;
	bool display_list_dirty;
	std::vector<int> display_list;
	std::vector<int> display_list_materials;

	std::vector<int> vbo_points_handle_array;
	std::vector<int> vbo_normals_handle_array;

	GLuint vbo_points_handle;
	GLuint vbo_texcoords_handle;
	GLuint vbo_normals_handle;
	GLuint vbo_colors_handle;

	Geometry(){ 
		name = "New Object";
		display_list.clear();
		display_list_materials.clear();
	}
	~Geometry(){ clear(); }

	void clear_display_lists() 
	{
		if (display_list.size()>0)
		{
			for (uint i = 0 ; i < display_list.size() ; i++)
				glDeleteLists (display_list[i], 1);
		}
	}
	void clear() 
	{
		points.clear();
		normals.clear();
		tex_coords.clear();
		triangles.clear();
		inputs.clear();
		outputs.clear();

		clear_display_lists();
	}
	void init()
	{
		init_bbox();
		init_normals();
	}
	void normalize( vec3f size = vec3f(1,1,1) )
	{
		if ( points.size () == 0 ) return;

		vec3f scale , center;
		center = ( bb_min + bb_max ) / 2 ;

		scale  = bb_max - bb_min ;
		if ( scale.x > scale.y ) 
		if ( scale.y > scale.z ) scale.y = scale.z = scale.x;
		if ( scale.y > scale.x ) 
		if ( scale.x > scale.z ) scale.x = scale.z = scale.y;
		if ( scale.z > scale.y ) 
		if ( scale.y > scale.x ) scale.y = scale.x = scale.z;

		scale  = size / scale ;

		for ( std::size_t i = 0 ; i < points.size () ; i++ )
		{
			points[i] = ( points[i]-center ) * scale ;
		}
		display_list_dirty = true;
	}
	void init_bbox()
	{
		if (points.size ()==0) return;

		vec3f min,max;
		min = max = points[0];

		for ( std::size_t i = 1 ; i < points.size () ; i++ )
		{
			if ( points[i].x < min.x ) min.x = points[i].x;
			if ( points[i].y < min.y ) min.y = points[i].y;
			if ( points[i].z < min.z ) min.z = points[i].z;
			if ( points[i].x > max.x ) max.x = points[i].x;
			if ( points[i].y > max.y ) max.y = points[i].y;
			if ( points[i].z > max.z ) max.z = points[i].z;
		}
		bb_min = min;
		bb_max = max;
	}
	void init_normals()
	{
		if (points.size ()==0) return;
		if ( normals.size () ) return;

		normals.resize( points.size() );

		std::vector<int> count( points.size () );

		for ( std::size_t i = 0 ; i < count.size () ; i++ )	count[i] = 0;	

		for ( std::size_t i = 0 ; i < triangles.size () ; i++ )
		{
			int *index = &triangles[i].points[0];

			triangles[i].normals[0] = triangles[i].points[0];
			triangles[i].normals[1] = triangles[i].points[1];
			triangles[i].normals[2] = triangles[i].points[2];

			vec3f a = points[index[0]];
			vec3f b = points[index[1]];
			vec3f c = points[index[2]];

			vec3f n; 					
			n.cross ( (b-a) , (c-a) );
			n.normalize();

			normals[index[0]] = normals[index[0]] - n ; 
			normals[index[1]] = normals[index[1]] - n ; 
			normals[index[2]] = normals[index[2]] - n ; 

			count[index[0]]++;
			count[index[1]]++;
			count[index[2]]++;
		}
		for ( std::size_t i = 0 ; i < count.size () ; i++ )	
		{
			normals[ i ].normalize();
		}
		display_list_dirty = true;
	}
	void init_connectors(
		Material* materials,
		vec3f *points_xform = NULL)
	{
		if(!materials) return;

		vec3f *pts;

		if (points_xform) 
			pts = points_xform; 
		else 
			pts = &points[0];

		inputs.clear();
		outputs.clear();

		for ( std::size_t i = 0 ; i < triangles.size () ; i++ )
		{
			int mat_index = triangles[i].material;
			Material& mat = materials[mat_index];

			bool inp  =(mat.con_def.type == ConDef::in);
			bool outp =(mat.con_def.type == ConDef::out);

			if ( inp || outp )
			{
				vec3f p[3];
				p[0] = pts[ triangles[i].points[0] ];
				p[1] = pts[ triangles[i].points[1] ];
				p[2] = pts[ triangles[i].points[2] ];
				Connector con;

				float smallest = 100000;
				
				for ( int j = 0;j<3; j++ )
				{
					int id0 = (j+0)%3;
					int id1 = (j+1)%3;
					int id2 = (j+2)%3;

					vec3f n1 = p[id1] - p[id0];
					vec3f n2 = p[id2] - p[id0];
					float dot = fabs ( n2.dot( n1 ) );
					
					if ( dot < smallest )
					{
						smallest = dot;

						float lenN3=0;
						lenN3+= n1.length();
						lenN3+= n2.length();
						lenN3/= 2;

						con.n1 = n1;
						con.n2 = n2;
						con.n3.cross(n2,n1);
						con.n3.normalize();
						con.n3 = con.n3 * lenN3;
						con.origin = p[id0];
						con.material = mat_index;
						con.triangle = i;
					}
				}
				if ( inp  )
				{
					inputs .push_back( con );
				}				
				if ( outp ) 
				{
					outputs.push_back( con );
				}
			}
		}
		//printf("Object %s : %d in %d out\n",
		//	name.c_str(),inputs.size(),outputs.size());
	}
	void swap( int& a , int& b )
	{
		int c = a; a = b; b = c;
	}
	void invert_faces()
	{
		if  (points.size ()==0) return;

		for ( std::size_t i = 0 ; i < triangles.size () ; i++ )
		{
			this->swap ( 
				triangles[i].points[0] ,
				triangles[i].points[1] );
			this->swap ( 
				triangles[i].normals[0] ,
				triangles[i].normals[1] );
			this->swap ( 
				triangles[i].tex_coords[0] ,
				triangles[i].tex_coords[1] );
		}
		this->display_list_dirty = true;
	}
	void draw(Material *materials, Spline* spline=NULL)
	{
		//if ( shader_normal == NULL ) return;

		vec3f *points_ref = &points[0];
		vec3f *normals_ref = &normals[0];

		if ( triangles.size () == 0 ) return;

		/*if ( display_list.size != 0 )
		{
			display_list_dirty = false;
			glDeleteLists (display_list, 1);
			display_list = -1;
		}*/

		Spline* spline1 = &spline_body;
		Spline* spline2 = &spline_arm;
		Spline* spline3 = &spline_elbow;

		if ( display_list.size() > 0 )
		{
			glShader* shader=shader_normal;

			//if ( renderpass == NORMAL )	shader = shader_normal;
			//if ( renderpass == SHADOW )	shader = shader_shadow;

			render_to_vertexbuffer();

			glBegin(GL_QUADS); 
			glColor3f(0,1,0);
			glEnd();

			static int num_render = num_characters;
			static int num_instances = 1;

			if (GFX::KeyDn('+'))if (num_render<num_characters) num_render ++;
			if (GFX::KeyDn('-'))if (num_render>1) num_render --;

			if (GFX::KeyDn('*'))num_instances ++;
			if (GFX::KeyDn('/'))if (num_instances>1) num_instances --;

			int mld = 0;

			rendered_models = 0;

			int t1  = timeGetTime();

			for ( int instances_y = 0; instances_y < num_instances; instances_y ++)
			for ( int instances_x = 0; instances_x < num_render/*num_characters*/; instances_x ++)
			{
				rendered_triangles += triangles.size();

				mld = (mld + 1)%num_characters;

				rendered_models++;

				glPushMatrix();
				glTranslatef(-instances_x * 10,0,instances_y * 20);

				glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_points_handle_array[mld]);
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer  ( 4, GL_FLOAT,0, (char *) 0);
				GFX::ClearTexture ();

				shader->begin();
				
				if (renderpass == NORMAL)
				{
					// Light+Shadow Positions

					matrix44 obj2cam44;
					glGetFloatv (GL_MODELVIEW_MATRIX,(GLfloat*) obj2cam44.m);

					obj2cam44 = obj2cam44 * model_view_inverse44 ;

					matrix44 light_to_shadow44 = obj2cam44 * light_2_shadow;
	//				shader->setUniformMatrix4fv("light2shadow",1,false,(GLfloat*)light_to_shadow44.m);

					obj2cam44.invert_simpler();

					vector3 light_pos =  obj2cam44 * light_pos_in;
					vector3 cam_pos   =  model_view_inverse44 * obj2cam44 * vector3(0,0,0);

					shader->setUniform3f  ("lightPos", light_pos.x , light_pos.y , light_pos.z );
					shader->setUniform3f  ("camPos"  , cam_pos.x   , cam_pos.y   , cam_pos.z );

				}//if (renderpass == NORMAL)

				glActiveTextureARB(GL_TEXTURE0_ARB);
				//glBindTexture(GL_TEXTURE_2D,tex_points);
				glBindTexture(GL_TEXTURE_2D,0);

				for ( uint a = 0 ; a < display_list.size()-1; a++)
				{
					if ( renderpass == NORMAL )
					{
						Material material = materials[ display_list_materials[a]];
						int handle = material.diffuse_map.gl_handle; if(handle <= 0 )
						{
							static int white   = GFX::LoadTex("data/textures/white.bmp");
							handle = white;
						}
						//GFX::SetTex ( handle );
						
						shader->setUniform4f  ("ambient", 
							material.ambient.x, 
							material.ambient.y,
							material.ambient.z,
							1);
						shader->setUniform4f  ("diffuse", 
							material.diffuse.x, 
							material.diffuse.y,
							material.diffuse.z,
							1);
						shader->setUniform4f  ("specular", 
							material.specular.x, 
							material.specular.y,
							material.specular.z,
							1);
							
					}
					glDrawArrays( GL_TRIANGLES, display_list[a],display_list[a+1]-display_list[a] );
				}
				glPopMatrix();

				shader->end();
				//unload_test_programs();
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
			}

			GFX::ClearTexture ();
			GFX::SetTex (0);
			int t2  = timeGetTime();

			//printf("render:%3.3f\n",float(t2-t1));


			return;

			uint i;
			for ( i = 0 ; i < inputs.size() ; i++ )
				draw_connector( inputs[i] );
			for ( i = 0 ; i < outputs.size() ; i++ )
				draw_connector( outputs[i] );
				//printf("listid:%d\n",display_list);
		}
		else
		{
			init_connectors(materials);//,points );

			int indexM=-10, indexM_before=-20;
			Material material = materials[0];

			bool use_tex_coords = false;
			if ( material.diffuse_map.gl_handle >= 0 )
			if ( tex_coords.size() > 0 )
				use_tex_coords = true;
	
			std::size_t i ;
			
			vec3f bb_size    =  bb_max - bb_min;
			vec3f bb_center  = (bb_max + bb_min)*0.5;

			spline_body.get_at(0);
			spline_arm.get_at(0);
			spline_elbow.get_at(0);

			spline_body.bind_normal = spline_body.bind_normal_cmp ;
			spline_arm.bind_normal = spline_arm.bind_normal_cmp ;
			spline_elbow.bind_normal = spline_elbow.bind_normal_cmp ;

			int num_verts=0;
			int num_tris=0;

			bind_normals.set(1024,1024,24,NULL);

			std::vector<vec3f> vbo_points;
			std::vector<vec4f> vbo_normals;
			std::vector<vec2f> vbo_texcoords;
			std::vector<vec4f> data_neighbours;
			std::vector<unsigned int> vbo_colors; //normals
			std::vector<unsigned int> bone_weights;
			std::vector<unsigned int> bone_indices;
			std::vector<unsigned int> bone_spline_ofs;
			std::vector<unsigned int> data_nwio;

			vbo_points.clear();
			vbo_normals.clear();
			vbo_colors.clear();

			for ( i = 0 ; i < triangles.size() ; i++ )
			{
				int *indexP = &triangles[i].points[0];
				int *indexT = &triangles[i].tex_coords[0];
				int *indexN = &triangles[i].normals[0];
 					 indexM = triangles[i].material;

				if ( indexM != indexM_before )
				{
					display_list.push_back( vbo_points.size() );
					display_list_materials.push_back( (indexM >= 0) ? indexM : 0 );
				}
				indexM_before = indexM;

				vec3f normal;
				vec3f vertex;
				vec3f middlev(0,0,0);

				for ( std::size_t j = 0 ; j < 3 ; j++ )
				{
					int k=indexP[j];
					middlev = middlev + points_ref[k];
				}
				middlev = middlev/3;

				for ( std::size_t j = 0 ; j < 3 ; j++ )
				{
					int k=indexP[j];
					int l=indexN[j];
					int m=indexT[j];
				
					vec3f c = normals_ref[l];
					c.clamp(-1,1);
					int normal_color=
					 int(float((c.x * 0.49 + 0.5)*255))+
					 int(float((c.y * 0.49 + 0.5)*255))*256+
					 int(float((c.z * 0.49 + 0.5)*255))*256*256;

					vec3f t = tex_coords [m];
					vec3f w = weight_map.getPixel( t.x,t.y );
					w.clamp(0,1);
					float sum = w.x + w.y + w.z;
					w=w/sum;
					if( sum <= 0.001 ) 	w.x=w.y=w.z=1.0/3.0f;

					float multi = 1.0;///(w.y+w.z);

					int indices=
					 int(0)+
					 int(1)*256+
					 int(2)*256*256;

					vec4f vtx;
					vtx.x=points_ref[k].x;
					vtx.y=points_ref[k].y;
					vtx.z=points_ref[k].z;
					vtx.w=1;

					vec2f idx;
					idx.x = float(int(vbo_points.size()& 1023))/1024.0;
					idx.y = float(int(vbo_points.size()/1024)) ;

					//vec4f nbr;
					//nbr.x=

					vec4f nrm;
					nrm.x=c.x;
					nrm.y=c.y;
					nrm.z=c.z;
					nrm.w=1;

					float nrm_x = float(int(c.x * 126+127.5)) * float(1.0 / 256.0);
					float nrm_y = float(int(c.y * 126+127.5));
					float nrm_z = float(int(c.z * 126+127.5)) * 256;
					vtx.w = nrm_x+nrm_y+nrm_z;

					vector3 pos ( vtx.x, vtx.y, vtx.z );

					float pos_arm		= spline_arm	.get_offset(pos);
					float pos_arm_lin	= spline_arm	.get_lin_offset();
					float rot_arm		= spline_arm	.get_offset_result_angle;

					float pos_elbow		= spline_elbow.get_offset(pos);
					float pos_elbow_lin	= spline_elbow.get_lin_offset();
					float rot_elbow		= spline_elbow.get_offset_result_angle;

					float pos_body		= spline_body	.get_offset(pos);
					float pos_body_lin	= spline_elbow.get_lin_offset();
					float rot_body		= spline_body	.get_offset_result_angle;

					unsigned int weights=
					 unsigned int(float((w.x*255.0)))+
					 unsigned int(float((w.y*255.0)))*256+
					 unsigned int(float((w.z*255.0)))*256*256+
					 unsigned int(float((pos_body_lin*multi*255.0)))*256*256*256;

					//if (pos_arm>0.2)
					//if (pos_arm<0.9) 
					//	printf("%3.3f ",pos_arm);

					unsigned int spline_offset =
						unsigned int(float(pos_body * 255.0))				+
						unsigned int(float(pos_arm * 255.0))*256		+
						unsigned int(float(pos_elbow * 255.0)) *256*256+
						unsigned int(float(rot_body * 255.0)) *256*256*256;

					
					if ((w.y>w.x)&&(w.y>w.z))
					{
						float multi = 1.0;///(w.x+w.z);
						weights=
						 unsigned int(float((w.y)*255))+
						 unsigned int(float((multi*w.x)*255))*256+
						 unsigned int(float((multi*w.z)*255))*256*256+
						 unsigned int(float((pos_arm_lin)*255))*256*256*256;

						indices=
						 int(1)+
						 int(0)*256+
						 int(2)*256*256;

						spline_offset =
							unsigned int(float(pos_arm * 255.0))+
							unsigned int(float(pos_body * 255.0))*256+
							unsigned int(float(pos_elbow * 255.0))*256*256+
							unsigned int(float(rot_arm * 255.0)) *256*256*256;
					}

					if ((w.z>w.x)&&(w.z>w.y))
					{
						float multi = 1.0;///(w.x+w.y);
						weights=
						 unsigned int(float((w.z)*255))+
						 unsigned int(float((multi*w.x)*255))*256+
						 unsigned int(float((multi*w.y)*255))*256*256+
						 unsigned int(float((pos_elbow_lin)*255))*256*256*256;

						indices=
						 int(2)+
						 int(0)*256+
						 int(1)*256*256;

						spline_offset =
							unsigned int(float(pos_elbow * 255.0))+
							unsigned int(float(pos_body * 255.0))*256+
							unsigned int(float(pos_arm * 255.0))*256*256+
							unsigned int(float(rot_elbow * 255.0)) *256*256*256;
					}
					

					//vbo_points.push_back(points_ref[k]);
					vbo_texcoords.push_back(idx);
					vbo_points.push_back(points_ref[k]);//vtx);
				//	vbo_colors.push_back(normal_color);
					vbo_normals.push_back(nrm);
			//		bone_weights.push_back(weights);
			//		bone_indices.push_back(indices);
			//		bone_spline_ofs.push_back(spline_offset);

					data_nwio.push_back(normal_color);
					//data_nwio.push_back(normal_color);
					//data_nwio.push_back(normal_color);
					//data_nwio.push_back(normal_color);

					/*
					if((data_nwio.size()&1023)==0)
					{
						for(int t=0;t<1024;t++)
							data_nwio.push_back(normal_color);
					}
					*/

					data_nwio.push_back(weights);
					data_nwio.push_back(indices);
					data_nwio.push_back(spline_offset);

					num_verts++;
				}
			}
			GFX::SetTex ( 0 );

			display_list.push_back( vbo_points.size() );
			display_list_materials.push_back( (indexM >= 0) ? indexM : 0 );

			int size_before = vbo_points.size();

			tex_width  = 1024;
			tex_height = 1+vbo_points.size() / 1024;
			tex_height_real = tex_height;
			int h_tmp  = 1; while (tex_height>0) {tex_height/=2;h_tmp*=2;}
			tex_height = h_tmp;

			for (int t=0;t< (int)vbo_texcoords.size();t++)
			{
				vbo_texcoords[t].y /= float(tex_height);
			}

			vbo_points		 .resize(tex_width*tex_height);
			vbo_normals		 .resize(tex_width*tex_height);
			vbo_texcoords	 .resize(tex_width*tex_height);
			//vbo_colors		 .resize(tex_width*tex_height);
			//bone_weights	 .resize(tex_width*tex_height);
			//bone_indices	 .resize(tex_width*tex_height);
			//bone_spline_ofs.resize(tex_width*tex_height);
			data_nwio		 .resize(4*tex_width*tex_height);

			tex_points		= GFX::NewFloat16Tex  ( tex_width,tex_height,(char*)(&vbo_points[0]),false);
			//tex_neighbours= GFX::NewFloat16Tex  ( tex_width,tex_height,(char*)(&data_neighbours[0]),false);
			//tex_normals	= GFX::NewByteRectTex ( tex_width,tex_height,(char*)(&vbo_colors[0]),true);
			//tex_weights	= GFX::NewByteRectTex ( tex_width,tex_height,(char*)(&bone_weights[0]));
			//tex_indices	= GFX::NewByteRectTex ( tex_width,tex_height,(char*)(&bone_indices[0]),true);
			//tex_spline_ofs= GFX::NewByteRectTex ( tex_width,tex_height,(char*)(&bone_spline_ofs[0]));
			tex_nwio      = GFX::NewByteRectTex ( tex_width*4,tex_height,(char*)(&data_nwio[0]));
			//tex_normals = NewFloatTex( tex_width,tex_height,(char*)(&vbo_normals[0]));
			//tex_colors  = NewByteTex ( tex_width,tex_height,(char*)(&vbo_colors[0]));

			vbo_points_handle_array.clear();
			vbo_normals_handle_array.clear();

			for(int n=0;n<num_characters;n++)
			{
				GLuint handle;
				glGenBuffersARB(1, &handle);
				glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, handle);
				glBufferDataARB(GL_PIXEL_PACK_BUFFER_EXT, vbo_points.size()*4*sizeof(float),0, GL_DYNAMIC_DRAW_ARB );
				vbo_points_handle_array.push_back(handle);
			}

			glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);

			bind_normals_handle = GFX::LoadTexBmp( bind_normals );

			num_tris=num_verts/3;
			printf("GL VBO Triangles : %d\n",num_tris);
			printf("GL VBO Vertices  : %d\n",num_verts);
		}
	}
	void draw_connector ( Connector con ){
		GFX::Line ( con.origin , con.origin + con.n1 );
		GFX::Line ( con.origin , con.origin + con.n2 );
		GFX::Line ( con.origin , con.origin + con.n3 );
	}

	#include "render_matrices.h"
	#include "render_matrices_2.h"
	#include "render_to_vertexbuffer.h"

	void get_GL_error()
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) 
		{			
			printf("GL Error: %s\n",gluErrorString(err));
			check_framebuffer_status();
			printf("Programm Stopped!\n");
			while(1)Sleep(100);;
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
		//printf("Programm Stopped!\n");
		//while(1)Sleep(100);;
	}

	// load a vertex or fragment program from a string
	GLuint load_program(GLenum program_type, const char *code)
	{
		GLuint program_id;
		glGenProgramsARB(1, &program_id);
		glBindProgramARB(program_type, program_id);
		glProgramStringARB(program_type, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei) strlen(code), (GLubyte *) code);

		GLint error_pos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &error_pos);
		if (error_pos != -1) {
			const GLubyte *error_string;
			error_string = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
			fprintf(stderr, "Program error at position: %d\n%s\n", error_pos, error_string);
		}
		return program_id;
	}
	void load_test_programs()
	{
		const char *vprog_code = 
		"!!ARBvp1.0\n"
		"OPTION NV_vertex_program3;\n"
		"PARAM mvp[4] = { state.matrix.mvp };\n"
		"PARAM scale = program.local[0];\n"
		"PARAM lod = program.local[1];\n"
		"TEMP pos, displace, texcoord;\n"
		"TEX displace, vertex.position, texture[0], 2D;\n"		    
		"MOV result.texcoord[0], texcoord;\n"
		"MAD result.color, {0.5,0.5,0.5,0},vertex.normal, {0.5,0.5,0.5,1.0};\n"
		"MOV result.color.w, 1.0;\n"
		"MOV pos,displace;\n"
		"MOV pos.w, 1.0;\n"
		"DP4 result.position.x, mvp[0], pos;\n"
		"DP4 result.position.y, mvp[1], pos;\n"
		"DP4 result.position.z, mvp[2], pos;\n"
		"DP4 result.position.w, mvp[3], pos;\n"
		"END\n";

		// fragment program
		const char *fprog_code =
		"!!ARBfp1.0\n"
		"TEMP tex0;\n"
		"MOV result.color, fragment.color;\n"
		"END\n";

		static int vprog_id = -1;
		static int fprog_id = -1;

		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glEnable(GL_VERTEX_PROGRAM_ARB);
		if ( vprog_id == -1 )
		{
			vprog_id = load_program(GL_VERTEX_PROGRAM_ARB, vprog_code);	get_GL_error();
			fprog_id = load_program(GL_FRAGMENT_PROGRAM_ARB, fprog_code);get_GL_error();
		}
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vprog_id);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fprog_id);
		float displace =1;
		float lod =1;
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, displace, 0, 0, 0);
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, lod, 0, 0, 0);
	}
	void unload_test_programs()
	{
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
		glDisable(GL_VERTEX_PROGRAM_ARB);
	}
};
// ---------------------------------------- //
