
class ObjManager { public:
 
	std::vector<Material>	 materials;
	std::vector<Deformer*>	 objects;
	std::vector<ObjControl*> controls; // might contain copies of Deformer

	~ObjManager(){

		uint i;
		for ( i = 0 ; i<controls.size(); i++ )
		{
			delete( controls[i] );
		}
		for ( i = 0 ; i<objects.size(); i++ )
		{
			delete( objects[i] );
		}
		materials.clear();
		objects.clear();
		controls.clear();
	}
	int push_back( Geometry& geo )
	{
		//printf("push_back %s\n",geo.name.c_str());

		Deformer *obj = new Deformer();
		ObjControl *ctrl = new ObjControl();

		geo.init();

		obj->set_geometry( geo );
		ctrl->set_object ( obj );
		ctrl->init_name ( controls.size() );

		objects.push_back(obj);
		controls.push_back(ctrl);

		return controls.size()-1;
	}
	int push_back( Material& mat )
	{
		materials.push_back(mat);
		return materials.size();
	}
	int new_control_from( int i )
	{
		int ret ;

		if( controls[i]->obj_ref )
			ret = push_back( controls[i]->obj_ref->geo );
		else
			{printf("Error! controls[%d] not initialized\n",i);while(1)Sleep(100);;}
	
		return ret;
	}
	// -------------- materials ------------ //
	int get_material_index ( std::string& name )
	{
		for ( uint i = 0 ; i<materials.size(); i++ )
			if ( name.compare( materials[i].name ) == 0 ) return i;

		printf("couldnt find material %s\n",name.c_str() );
		return -1;
	}
	void print_materials ()
	{
		for(uint i=0;i<materials.size();i++)
		{
			printf("Material %i : %s\n",
				i,materials[i].name.c_str());
			printf("  Ambient RGB %2.2f %2.2f %2.2f\n",
				materials[i].ambient.x,
				materials[i].ambient.y,
				materials[i].ambient.z);
			printf("  Diffuse RGB %2.2f %2.2f %2.2f\n",
				materials[i].diffuse.x,
				materials[i].diffuse.y,
				materials[i].diffuse.z);
			printf("  Diff. Tex : %s , GL-Handle[%d]\n",
				materials[i].diffuse_map.filename.c_str() ,
				materials[i].diffuse_map.gl_handle			);
			printf("  Connector : %s\n",
				materials[i].con_def.get_type().c_str());
		}
/*		for ( uint i = 0 ; i<materials.size(); i++ )
			printf("Material[%d] = %s\n", i,materials[i].name.c_str() );*/
	}
	// -------------- objects -------------- //
	void print_objects ()
	{
		for ( uint i = 0 ; i<objects.size(); i++ )
			printf("Object[%d] = %s\n", i,objects[i]->name.c_str() );
	}
	int get_object_index ( std::string& name )
	{
		for ( uint i = 0 ; i<objects.size(); i++ )
			if ( name.compare( objects[i]->name ) == 0 ) return i;

		return -1;
	}
	// -------------- controls -------------- //
	void print_controls ()
	{
		for ( uint i = 0 ; i<controls.size(); i++ )
			printf("Control[%d] = %s\n", i,controls[i]->name.c_str() );
	}
	int get_control_index ( std::string& name )
	{
		for ( uint i = 0 ; i<controls.size(); i++ )
			if ( name.compare( controls[i]->name ) == 0 ) return i;

		return -1;
	}
	// ------------------------------------- //
	void draw()
	{
		for ( uint i = 0 ; i<controls.size(); i++ )
		{
			//printf("i:%d - %s\n",i,controls[i]->name.c_str());
			controls[i]->draw(&materials[0]);
		}
	}
	// ----------- String ------------- //
	std::string get_path ( std::string filename )
	{
		uint pos1 = filename.find_last_of( "/" );
		uint pos2 = filename.find_last_of( "\\" );

		if ( pos1 ==  std::string::npos ) pos1 = pos2;
		if ( pos1 ==  std::string::npos ) return "./";

		if (pos1 < filename.size())
		if (pos1 != 0)
			pos1++;

		//printf( "substr = %s\n" , filename.substr(0,pos1).c_str());

		return (filename.substr(0,pos1));
	}

	std::string get_pure_filename ( std::string filename )
	{
		uint pos1 = filename.find_last_of( "/" );
		uint pos2 = filename.find_last_of( "\\" );
		uint pos3 = filename.find_last_of( "." );

		if ( pos1 ==  std::string::npos ) pos1 = pos2;
		if ( pos1 ==  std::string::npos ) pos1 = 0;
		if ( pos1 < filename.size())
		if ( pos1 != 0 )pos1++;

		if ( pos3 == std::string::npos ) 
		{
			pos3 = filename.size();
		}		
		//printf( "input %s substr = %s\n" ,filename.c_str(), filename.substr(pos1,pos3-pos1).c_str());
		return (filename.substr(pos1,pos3-pos1));
	}

	// ---------- Load Connectors ------------ //

	void save_con(const char* filename){

		printf ( "Saving Connections %s ... \n",filename);

		FILE* fn;
		if(filename==NULL)		return ;
		if((char)filename[0]==0)	return ;
		if ((fn = fopen(filename, "wb")) == NULL)
		{
			printf ( "File %s cant be saved\n" ,filename );
			return;
			//while(1);
		}

		uint i,j;

		fprintf(fn,"################################\n");
		fprintf(fn,"# Configuration Connectors for\n#\n");
		for ( i = 0;i<materials.size();i++ )
		{
			fprintf(fn,"# Material[%d]\t%s\n",i,materials[i].name.c_str());
		}
		fprintf(fn,"#\n################################\n");

		for ( i = 0;i<materials.size();i++ )
		{
			fprintf(fn,"\nConnector-Material\t%s\n",materials[i].name.c_str());
			fprintf(fn,"Connector-Type\t\t%s\n",materials[i].con_def.get_type().c_str());
			for ( j = 0;j<materials[i].con_def.compatible.size();j++ )
			{
				fprintf(fn,"Connector-To-Object\t%s\n",materials[i].con_def.compatible[j]->name.c_str());
			}
		}

		fprintf(fn,"\n################################\n");
		fprintf(fn,"# Configuration Objects\n#\n");
		for ( i = 0;i<objects.size();i++ )
		{
			fprintf(fn,"# Object[%d]\t%s\n",i,objects[i]->name.c_str());
		}
		fprintf(fn,"#\n################################\n");

		for ( i = 0;i<objects.size();i++ )
		{
			fprintf(fn,"\nObject-Name %s\n",objects[i]->name.c_str());

			Config &c = objects[i]->geo.config;
			fprintf(fn,"Object-deformable\t\t%s\n",
				(c.deformable ? "yes" : "no") );
			fprintf(fn,"Object-path\t\t\t%s\n",
				(c.path ? "yes" : "no") );
			fprintf(fn,"Object-scale_until_collision\t%s\n",
				(c.scale_until_collision ? "yes" : "no" ));
			fprintf(fn,"Object-twist_random_from\t%f\n",
				 c.twist_random_from);
			fprintf(fn,"Object-twist_random_to  \t%f\n",
				 c.twist_random_to);
			fprintf(fn,"Object-scale_random_from\t%f\n",
				 c.scale_random_from);
			fprintf(fn,"Object-scale_random_to  \t%f\n",
				 c.scale_random_to);
			fprintf(fn,"Object-bend_angle_max  \t%f\n",
				 c.bend_angle_max);
		}
		fprintf(fn,"\n\n");
		fprintf(fn,"################################\n");
		fprintf(fn,"# Connector Configuration Help\n");
		fprintf(fn,"################################\n#\n");
		fprintf(fn,"# Types:\n#\n");
		fprintf(fn,"# Connector-Type\tunused\n");
		fprintf(fn,"# Connector-Type\tincoming\n");
		fprintf(fn,"# Connector-Type\toutgoing\n#\n");
		fprintf(fn,"# Adding Objects as Link-Target:\n#\n");
		fprintf(fn,"# Connector-To-Object\t\tobjname1\n");
		fprintf(fn,"# Connector-To-Object\t\tobjname2\n");
		fprintf(fn,"#\n");
		fprintf(fn,"################################\n");
		fclose(fn);
	}


	// ---------- Load Connectors ------------ //

	void load_con(const char* filename){

		printf ( "Loading Connections %s ... \n",filename);

		FILE* fn;
		if(filename==NULL)		return ;
		if((char)filename[0]==0)	return ;
		if ((fn = fopen(filename, "rb")) == NULL)
		{
			printf ( "File %s not found!\n" ,filename );
			return;
			//while(1);
		}
		char line[1000];
		memset ( line,0,1000 );

		Material* mat=NULL;
		Deformer* obj=NULL;
		char str[200];
		float val;

		while(fgets( line, 1000, fn ) != NULL)
		{
			switch (line[0])
			{
				case 'C': /* ---- Connector Config ---- */

				if(sscanf(line,"Connector-Material %s",str)==1) 
				{
					int i = get_material_index ( std::string(str) );

					if ( i>=0 )
						mat = &materials[i];
					else
					{
						printf("Material %s not found\n",str);
						printf("Available are:\n");
						this->print_materials();
						while(1);;
					}
				}else
				if( mat && sscanf(line,"Connector-To-Object %s",str)==1) 
				{
					int i = this->get_object_index (std::string(str));
					if ( i>=0 )
						mat->con_def.compatible.push_back( objects[i] );
					else
					{
						printf("Object %s not found\n",str);
						printf("Available are:\n");
						this->print_objects();
						while(1);;
					}
				}else
				if( mat && sscanf(line,"Connector-Type %s",str)==1) 
				{
					mat->con_def.type = 
						mat->con_def.get_type ( std::string(str) );
				}
				break;

				case 'O':/* ---- Object Config ---- */

				if(sscanf(line,"Object-Name %s",str)==1) 
				{
					int i = get_object_index ( std::string(str) );

					if ( i>=0 ) 
						obj = objects[i];
					else
					{
						printf("Object %s not found\n",str);
						printf("Available are:\n");
						this->print_objects();
						while(1);;
					}
				}else
				if(obj && sscanf(line,"Object-deformable %s",str)==1) 
				{
					if ( string(str).compare("yes")==0 )
						obj->geo.config.deformable = true;
					else
						obj->geo.config.deformable = false;
				}else
				if(obj && sscanf(line,"Object-path %s",str)==1) 
				{
					if ( string(str).compare("yes")==0 )
						obj->geo.config.path = true;
					else
						obj->geo.config.path = false;
				}else
				if(obj && sscanf(line,"Object-scale_until_collision %s",str)==1) 
				{
					if ( string(str).compare("yes")==0 )
						obj->geo.config.scale_until_collision = true;
					else
						obj->geo.config.scale_until_collision = false;
				}
				else if(obj && sscanf(line,"Object-twist_random_from %f",&val)==1) 
					obj->geo.config.twist_random_from = val;
				else if(obj && sscanf(line,"Object-twist_random_to %f",&val)==1) 
					obj->geo.config.twist_random_to = val;
				else if(obj && sscanf(line,"Object-scale_random_from %f",&val)==1) 
					obj->geo.config.scale_random_from = val;
				else if(obj && sscanf(line,"Object-scale_random_to %f",&val)==1) 
					obj->geo.config.scale_random_to = val;
				else if(obj && sscanf(line,"Object-bend_angle_max %f",&val)==1) 
					obj->geo.config.bend_angle_max = val;
				break;
			}
		}
		fclose(fn);

		for ( uint i = 0; i < objects.size() ; i++ )
			objects[i]->geo.init_connectors(&materials[0]);
	}

	// ---------- Load Mtl ------------ //

	void load_mtl(const char* filename , std::string prefix){

		printf ( "Loading Materials %s ... \n",filename);

		std::string path = get_path( filename );

		FILE* fn;
		if(filename==NULL)		return ;
		if((char)filename[0]==0)	return ;
		if ((fn = fopen(filename, "rb")) == NULL)
		{
			printf ( "File %s not found!\n" ,filename );
			while(1);
		}
		char line[1000];
		memset ( line,0,1000 );

		Material mat,empty_mat;
		char str[200];
		vec3f color;
		bool first = true;

		while(fgets( line, 1000, fn ) != NULL)
		{
			switch (line[0] )
			{
				case 'n':
					char material_str[200];
					if(sscanf(line,"newmtl %s",material_str)==1) 
					{
						if (!first)
							materials.push_back ( mat );

						mat = empty_mat;
						mat.name = prefix + std::string(material_str);
						first = false;
					}
				break;
				case 'm':
				if(sscanf(line,"map_Kd %s",str)==1) 
				{
					std::string s = path + std::string(str);
					Texture texture;
					texture.filename = s;
					texture.gl_handle = GFX::LoadTex( s.c_str() );
					mat.diffuse_map = texture;
				}
				break;
				case 'K':
					if(sscanf(line,"Kd %f %f %f",
						&color.x,&color.y,&color.z)==3) 
					{
						mat.diffuse = color;
					}
					if(sscanf(line,"Ka %f %f %f",
						&color.x,&color.y,&color.z)==3) 
					{
						mat.ambient = color;
					}
				break;
				case 'T':
					if(sscanf(line,"Tf %f %f %f",
						&color.x,&color.y,&color.z)==3) 
					{
						mat.specular = color;
					}
				break;
			}
		}
		if (!first)
			materials.push_back ( mat );

		fclose(fn);
	}

	void load_obj(const char* filename){

		printf ( "\nLoading Objects %s ... \n",filename);

		get_pure_filename ( std::string(filename) );

		std::string path = get_path( filename );
		FILE* fn;
		if(filename==NULL)		return ;
		if((char)filename[0]==0)	return ;
		if ((fn = fopen(filename, "rb")) == NULL)
		{
			printf ( "File %s not found!\n" ,filename );
			while(1);
		}
		char line[1000];
		memset ( line,0,1000 );

		int material= -1;
		int vertex_cnt=0;
		int tex_coords_cnt=0;
		int normals_cnt=0;

		char state='-',state_before='-';

		Geometry geo;

		static int counter=0;counter++;

		std::string mtl_prefix = int_to_str(counter) + "-";

		while(fgets( line, 1000, fn ) != NULL)
		{
			if ( line[0] == 'v' )
			if ( line[1] == ' ' )
			{
				state_before = state;state ='v';
			}

			if ( state == 'v' )
			if ( state_before == 'f' )
			{
				push_back( geo );

				vertex_cnt    += geo.points.size();
				tex_coords_cnt+= geo.tex_coords.size();
				normals_cnt   += geo.normals.size();

				geo.clear();
			}

			vec3f val; 

			if ( line[0] == 'v' )
			{
				if ( line[1] == ' ' )
				if(sscanf(line,"v %f %f %f",
					&val.x,	&val.y,	&val.z)==3) 
				{
						geo.points.push_back ( val );
				}

				if ( line[1] == 't' )
					if(sscanf(line,"vt %f %f",
						&val.x,	&val.y)==2) 
							geo.tex_coords.push_back ( val );

				if ( line[1] == 'n' )
					if(sscanf(line,"vn %f %f %f",
						&val.x,	&val.y,	&val.z)==3) 
							geo.normals.push_back ( val );
			}

			int integers[9];
			
			if ( line[0] == 'f' )
			{
				Triangle tri;
				bool tri_ok = false;

				if(sscanf(line,"f %d %d %d",
					&integers[0],&integers[1],&integers[2])==3) 
				{
					tri_ok = true;
				}else
				if(sscanf(line,"f %d//%d %d//%d %d//%d",
					&integers[0],&integers[3],
					&integers[1],&integers[4],
					&integers[2],&integers[5])==6) 
				{
					tri_ok = true;
				}else
				if(sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",
					&integers[0],&integers[6],&integers[3],
					&integers[1],&integers[7],&integers[4],
					&integers[2],&integers[8],&integers[5])==9) 
				{
					tri_ok = true;
				}
				else
				{
					printf("unrecognized sequence\n");
					printf("%s\n",line);
					while(1);
				}

				if ( tri_ok )
				{
					tri.points[1] = integers[0]-1-vertex_cnt;
					tri.points[0] = integers[1]-1-vertex_cnt;
					tri.points[2] = integers[2]-1-vertex_cnt;

					tri.normals[1] = integers[3]-1-normals_cnt;
					tri.normals[0] = integers[4]-1-normals_cnt;
					tri.normals[2] = integers[5]-1-normals_cnt;

					tri.tex_coords[1] = integers[6]-1-tex_coords_cnt;
					tri.tex_coords[0] = integers[7]-1-tex_coords_cnt;
					tri.tex_coords[2] = integers[8]-1-tex_coords_cnt;
					
					if ( (uint)tri.points[1] >= geo.points.size())
					{
						int a = integers[0]-vertex_cnt;
						int b = geo.points.size();
						printf("Error at f%d: %d-%d > %d!\n",
							geo.triangles.size(),
							tri.points[1],vertex_cnt , 
							geo.points.size());
					}

					tri.material = material;
					geo.triangles.push_back ( tri );

					state_before = state;
					state ='f';
				}
			}
			if ( line[0] == 'm' )
			{
				char mtl_file_str[200];
				if(sscanf(line,"mtllib %s",mtl_file_str)==1)
				{
					std::string s = path + std::string(mtl_file_str);
					this->load_mtl ( s.c_str() , mtl_prefix );
				}
			}
			if ( line[0] == 'u' )
			{
				char material_str[300];
				if(sscanf(line,"usemtl %s",material_str)==1)
				{
					material = get_material_index ( 
						mtl_prefix+std::string(material_str) );

					if ( material == -1 )
					{
						printf("Available are:\n");
						this->print_materials();
						while(1);;
					}
				}
			}
			if ( line[0] == 'g' )
			{
				char name_str[300];
				if(sscanf(line,"g %s",name_str)==1)
				{
					geo.name = name_str;
					//printf("group:%s\n",name_str );
				}
			}
		}
		fclose(fn);

		this->push_back( geo );
		
/*
		this->load_con ((
				this->get_path(filename)
			+	this->get_pure_filename(filename)
			+	std::string(".con")).c_str(), mtl_prefix );

		for ( uint i = obj_from; i < obj_to ; i++ )
			objects[i]->geo.init_connectors(&materials[0]);
*/		
		printf ( "Read %s - %d points , %d faces\n" , 
			filename , geo.points.size() , geo.triangles.size() );
	}

};
