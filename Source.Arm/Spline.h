// ---------------------------------------- //
class SplineTex{public:

	int width,height;

	float *data_nbtpx;
	float *data_nbtpy;
	float *data_nbtpz;
	float *data_params;

	int num_params;
	int handle_params;
	int handle_pose;

	int handle_nbtpx;
	int handle_nbtpy;
	int handle_nbtpz;
	int handle_b;
	int handle_nbtpx_out;
	int handle_nbtpy_out;
	int handle_nbtpz_out;
	int handle_nbtpx_out2;
	int handle_nbtpy_out2;
	int handle_nbtpz_out2;

	void update_params()
	{
		if (handle_params==-1)
		{
			handle_params = GFX::NewFloatRectTex(num_params,height,(char*)data_params,true);
		}
		else
		{
			GLuint type = GL_TEXTURE_RECTANGLE_ARB;
			glBindTexture(type,handle_params);GFX::get_error();
			glTexSubImage2D(type,0,0,0,num_params,height,GL_RGBA,GL_FLOAT,data_params);
			GFX::get_error();
			glBindTexture(type,0);
		}
	}
};
// ---------------------------------------- //
class Spline { public:

	vec3f p1,p2,p3;
	vec3f d1,d2;
	vec3f normal;
	float angle , twist ;

	float center_distort;

	vec3f d1_before,d2_after;

	vec3f result_pos;
	vec3f result_normal;
	vec3f result_tan;
	vec3f result_cotan;

	vec3f bind_normal;
	vec3f bind_normal_cmp;
	float bind_angle;

	matrix44 result_matrix;

	matrix44 spline_space;
	matrix44 spline_space_inv;

	matrix44 spline_space_inv_parent;

	matrix44 spline_m0, spline_m0_inv;
	matrix44 spline_m1, spline_m1_inv;
	matrix44 spline_m1_init, spline_m1_init_inv;

	matrix44 spline_xform;

	void set ( vec3f p1_in,vec3f p2_in,vec3f p3_in, float angle=0, float twist=0, matrix44& parent=matrix44() )
	{
		//center_distort = 0;
	/*	
		this->p1 = p1_in;
		this->p2 = p2_in;
		this->p3 = p3_in;
		d1 = p2 - p1;
		d2 = p3 - p2;
		vec3f d1n = d1; d1n.normalize();
		vec3f d2n = d2; d2n.normalize();
		vec3f delta = d2n-d1n;
		if ( delta.length() > 0.001 )
		{
			normal.cross(d1,d2);
		}
		else
		{
			if ( (d1-vec3f(1,1,1)).length() > 0.001 )
				normal.cross( d1, vec3f(1,1,1) );
			else
				normal.cross( d1, vec3f(0,0,1) );
		}		
		bind_normal_cmp = vec3f::normalize ( normal );
		*/
		
		vector3 pp1 = parent * vector3( p1_in.x,p1_in.y,p1_in.z);
		vector3 pp2 = parent * vector3( p2_in.x,p2_in.y,p2_in.z);
		vector3 pp3 = parent * vector3( p3_in.x,p3_in.y,p3_in.z);

		vec3f p1(pp1.x,pp1.y,pp1.z);
		vec3f p2(pp2.x,pp2.y,pp2.z);
		vec3f p3(pp3.x,pp3.y,pp3.z);

		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
		this->angle = angle;
		this->twist = twist;
		d1 = p2 - p1;
		d2 = p3 - p2;
	
		vec3f d1n = d1; d1n.normalize();
		vec3f d2n = d2; d2n.normalize();
		vec3f delta = d2n-d1n;

		float len_d1_d2 = d1.length()+d2.length();
		d1_before = d1n * len_d1_d2;
		d2_after  = d2n * len_d1_d2;
		
		if ( delta.length() > 0.001 )
		{
			normal.cross(d1,d2);
		}
		else
		{
			if ( (d1-vec3f(1,1,1)).length() > 0.001 )
				normal.cross( d1, vec3f(1,1,1) );
			else
				normal.cross( d1, vec3f(0,0,1) );
		}
		result_normal = normal = vec3f::normalize (normal);
		
		vec3f bnc = result_normal + p1;
		matrix44 parent_inv = parent; parent_inv.invert();
		vector3 nn = parent_inv * vector3( bnc.x,bnc.y,bnc.z);
		bind_normal_cmp = vec3f::normalize ( vec3f(nn.x,nn.y,nn.z) - p1_in );
		bind_angle = bind_normal.angle2( bind_normal_cmp , p2_in-p1_in );

		get_at(0);

		spline_space = result_matrix;

		delta = d1n * (d1.length()+d2.length());
		spline_space.m[2][0] = delta.x;
		spline_space.m[2][1] = delta.y;
		spline_space.m[2][2] = delta.z;

		spline_m0 = result_matrix;

		spline_m1_init = result_matrix; 
		spline_m1_init.m[3][0] = p1.x+delta.x;
		spline_m1_init.m[3][1] = p1.y+delta.y;
		spline_m1_init.m[3][2] = p1.z+delta.z;

		get_at(1);

		spline_m1 = result_matrix;

		spline_m0_inv      = spline_m0     ; spline_m0_inv.invert();
		spline_m1_inv      = spline_m1     ; spline_m1_inv.invert();
		spline_m1_init_inv = spline_m1_init; spline_m1_init_inv.invert();
		spline_space_inv   = spline_space  ; spline_space_inv.invert();

		// Transform Matrix of the spline
		spline_xform = spline_m1_init_inv * spline_m1;

		// Transform from World to Spline-space
		spline_space_inv_parent = parent * spline_space_inv;
	}
	void get_at ( float a , bool rotate=true)
	{
		if (a>1) a=1;
		if (a<0) a=0;

		float p=float(this->spline_hardness);//GFX::mouseY*5);
		float powa=pow(float(a),float(p));
		float pow1a=pow(float(1-a),float(p));

		float cos_a = a*powa;//cos ( 0.5f * M_PI * a );
		float sin_a = 1-(1-a)*pow1a;//sin ( 0.5f * M_PI * a );
		float cos_da = powa;//cos ( 0.5f * M_PI * a );
		float sin_da = pow1a;//sin ( 0.5f * M_PI * a );
		result_pos =  p1 + d1 * sin_a + d2 * cos_a;
		result_tan = (d1 * sin_da + d2 * cos_da).normalize();
		result_cotan.cross( result_tan,result_normal);
		//result_cotan = (d1 * sin_a + d2 * cos_a).normalize();

		matrix44 m;
		m.m[0][0] = result_normal.x;
		m.m[0][1] = result_normal.y;
		m.m[0][2] = result_normal.z;
		m.m[0][3] = 0;
		m.m[1][0] = result_cotan.x;
		m.m[1][1] = result_cotan.y;
		m.m[1][2] = result_cotan.z;
		m.m[1][3] = 0;
		m.m[2][0] = result_tan.x;
		m.m[2][1] = result_tan.y;
		m.m[2][2] = result_tan.z;
		m.m[2][3] = 0;
		m.m[3][0] = result_pos.x;
		m.m[3][1] = result_pos.y;
		m.m[3][2] = result_pos.z;
		m.m[3][3] = 1;

		
		if(rotate) 
		{
			matrix44 r;
			r.ident();
			r.rotate_z(angle+twist*a);//-bind_angle+
			result_matrix = r*m;
		}
		else
		{
			result_matrix = m;
		}

		result_normal.x = result_matrix.m[0][0];
		result_normal.y = result_matrix.m[0][1];
		result_normal.z = result_matrix.m[0][2];

		result_cotan.x = result_matrix.m[1][0];
		result_cotan.y = result_matrix.m[1][1];
		result_cotan.z = result_matrix.m[1][2];
	}
	void draw()
	{
		GFX::SetTex ( 0 );
//		GFX::Line (	p1,	p2 , vec3f (1,1,1) );
//		GFX::Line (	p2,	p3 , vec3f (1,1,1) );

		//center_distort = 1;

		get_at(0);
		vec3f distort = this->result_cotan * center_distort;

		vec3f pp1 = p1 + distort;
		vec3f pp3 = p3 + distort;

		glPushMatrix();
		glColor3f(0,0,0);
		glTranslatef(pp1.x,pp1.y,pp1.z);
		glutSolidSphere(0.1,7,7);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(pp3.x,pp3.y,pp3.z);
		glutSolidSphere(0.1,7,7);
		glPopMatrix();

		int segments = 10;

		static vec3f result_pos_old;

		for ( int i = 0 ; i <= segments ; i++)
		{
			float a = float(i)/float(segments);

			result_pos_old = result_pos+distort;

			get_at ( a , true);

			if (i>0)
				GFX::Line ( 
					result_pos_old,
					result_pos+distort,
					vec3f (0,0,0) );
/*
			GFX::Line ( 
				result_pos ,
				result_pos + result_normal*0.3,
				vec3f (1,0,0) );

			GFX::Line ( 
				result_pos ,
				result_pos + result_cotan*0.3,
				vec3f (0,0,1) );
				*/
		}
	}
	// one line = one spline

	void sample_mats(matrix44 *m,int num)
	{
		for(int a = 0;a<num;a++)
		{
			this->get_at(float(a)/float(num-1),false);
			m[a]=this->result_matrix;
			m[a].invert_simpler();
		}
	}

	int deformation_style;
	float spline_hardness;
	float spline_twist_adjust;

	void set_style(int i)
	{
		deformation_style = i;
	}
	void set_hardness(float f)
	{
		spline_hardness = f;
	}
	void set_twist_adjust(float f)
	{
		spline_twist_adjust = f;
	}
	void set_params_to_tex(SplineTex &tex, int y)
	{
		int ofs = y*tex.num_params*4; // 5 params, 4 floats per param

		tex.data_params[ofs++]	=	p1.x;
		tex.data_params[ofs++]	=	p1.y;
		tex.data_params[ofs++]	=	p1.z;
		tex.data_params[ofs++]	=	angle-bind_angle;

		//printf("an:%3.3f %3.3f\n",angle,bind_angle);

		tex.data_params[ofs++]	=	d1.x;
		tex.data_params[ofs++]	=	d1.y;
		tex.data_params[ofs++]	=	d1.z;
		tex.data_params[ofs++]	=	twist;

		tex.data_params[ofs++]	=	d2.x;
		tex.data_params[ofs++]	=	d2.y;
		tex.data_params[ofs++]	=	d2.z;
		tex.data_params[ofs++]	=	-sin(angle-bind_angle);

		tex.data_params[ofs++]	=	normal.x;
		tex.data_params[ofs++]	=	normal.y;
		tex.data_params[ofs++]	=	normal.z;
		tex.data_params[ofs++]	=	-cos(angle-bind_angle);

		get_at(0,true);
		matrix44 r;
		r.ident();
		r.rotate_z(bind_angle);//-bind_angle+
		r = result_matrix;

		float angle = 2*(d1.angle(d2) / M_PI); angle = angle;
		tex.data_params[ofs++]	=	r.m[1][0];//result_cotan.x;
		tex.data_params[ofs++]	=	r.m[1][1];//result_cotan.y;
		tex.data_params[ofs++]	=	r.m[1][2];//result_cotan.z;
		tex.data_params[ofs++]	=  ( angle > 1 ) ? 1 : angle ;

		this->center_distort = 0.0;//006;
		tex.data_params[ofs++]	=	deformation_style;//
		tex.data_params[ofs++]	=	spline_hardness;
		tex.data_params[ofs++]	=	spline_twist_adjust;
		tex.data_params[ofs++]	=	center_distort ;// center distort
	}
	void sample(SplineTex &tex, int y,matrix44 *mats=NULL)
	{
		GFX::mouseY = 0;
		for(int a = 0;a<tex.width;a++)
		{
			float a01 = float(a)/float(tex.width-1);
			int ofs = (a + y * tex.width)*4;

			this->get_at(a01,false);
			matrix44 m = this->result_matrix;

			matrix44 r;
			r.rotate_z((-bind_angle+angle+twist*a01));

			if (mats) m = mats[a]*r*m;
			else
			{
				m.invert_simpler();
			}

			for (int i=0;i<3;i++)
			for (int j=0;j<3;j++)
			{
				if ( m.m[i][j] < -1 ) m.m[i][j] = -1;
				if ( m.m[i][j] >  1 ) m.m[i][j] =  1;
			}
			
			float scale =256;

			tex.data_nbtpx[ofs+0] = m.m[0][0];//*scale;
			tex.data_nbtpx[ofs+1] = m.m[1][0];//*scale;
			tex.data_nbtpx[ofs+2] = m.m[2][0];//*scale;
			tex.data_nbtpx[ofs+3] = m.m[3][0];//*scale;

			tex.data_nbtpy[ofs+0] = m.m[0][1];//*scale;
			tex.data_nbtpy[ofs+1] = m.m[1][1];//*scale;
			tex.data_nbtpy[ofs+2] = m.m[2][1];//*scale;
			tex.data_nbtpy[ofs+3] = m.m[3][1];//*scale;

			tex.data_nbtpz[ofs+0] = m.m[0][2];//*scale;
			tex.data_nbtpz[ofs+1] = m.m[1][2];//*scale;
			tex.data_nbtpz[ofs+2] = m.m[2][2];//*scale;
			tex.data_nbtpz[ofs+3] = m.m[3][2];//*scale;

		}
		set_params_to_tex( tex, y);
	}

	float get_offset_result_angle;
	float get_offset_result_distance;

	float get_lin_offset()
	{
		float a = (result_pos-p1).length() / (p3-p1).length();

		if (a>1)return 1;
		if (a<0)return 0;

		return a;
	}

	float get_offset(vec3f pos, float a = 0.5f, float add_a=0.25f)
	{
		if(add_a<0.001)
		{
			matrix44 res_inv = result_matrix; res_inv.invert_simpler();
			vec3f inplane = res_inv * vector3 (pos.x,pos.y,pos.z);
			get_offset_result_angle = 
				0.75+0.5+atan2(inplane.y,inplane.x)/(2*M_PI);

			if(get_offset_result_angle > 1 ) get_offset_result_angle-=1.0f;

			float border = 1.0/128.0 ;
			get_offset_result_angle = get_offset_result_angle * (1.0-2*border)+border;

			if(get_offset_result_angle<border)
			{
				get_offset_result_angle = border;
				get_offset_result_distance = 0;
			}

			if(get_offset_result_angle>1-border)
			{
				get_offset_result_angle = 1-border;
				get_offset_result_distance = 0;
			}

			
			return a;
		}

		GFX::mouseY = 0;

		if (add_a==0.25f/2)
		{
			this->get_at(0,false);

			vec3f w(	pos.x-result_pos.x,
						pos.y-result_pos.y,
						pos.z-result_pos.z);

			float d = w.dot(this->result_tan);

			if (d<0) return 0;

			this->get_at(1,false);

			w = vec3f(	pos.x-result_pos.x,
						pos.y-result_pos.y,
						pos.z-result_pos.z);

			d = w.dot(this->result_tan);

			if (d>0) return 1;

		//	return 0.5;
		}

		this->get_at(a,false);

		vec3f w(	pos.x-result_pos.x,
					pos.y-result_pos.y,
					pos.z-result_pos.z);

		float d = w.dot(this->result_tan);

		if (d>0) 
			return get_offset(pos,a+add_a,add_a/2);
		else
			return get_offset(pos,a-add_a,add_a/2);
	}
};//class spline
// ---------------------------------------- //
