class ObjControl { public:

	Deformer* obj_ref;
	Deformer* obj_copy;
	std::string name;

	Spline spline;

	vec3f pos,scale,axis;float angle;

	ObjControl(){ init(); }

	~ObjControl(){
		clear();
	}

	void init(){
		this->name = "empty";
		this->obj_ref=NULL;
		this->obj_copy=NULL;
		this->pos = vec3f (0,0,0);
		this->scale = vec3f (1,1,1);
		this->axis = vec3f (1,0,0);
		this->angle = 0;
	}
	void clear()
	{
		if ( obj_copy )
		{
			delete(obj_copy);
		}
	}
	void set_object( Deformer* obj )
	{
		obj_ref = obj;
		init_name();
/*
		vec3f bbmin = obj->geo.bb_min;
		vec3f bbmax = obj->geo.bb_max;
		vec3f bbmid = (bbmin + bbmax)/2;

		vec3f p1 = vec3f( bbmid.x,bbmid.y,bbmin.z );
		vec3f d  = vec3f( 0,0,(bbmax.z-bbmin.z)/2 );
		set_spline( p1,(p1+d),(p1+d+d),0,0 );
*/
		//printf("assigned %s,%d points\n",obj.name.c_str(),obj.geo.points.size());
	}
	void draw(Material *mats)
	{
		if (!obj_ref) return;

		glPushMatrix();
		glTranslatef( pos.x,pos.y,pos.z );
		glRotatef( angle, axis.x,axis.y,axis.z);
		glScalef( scale.x,scale.y,scale.z );

		obj_ref->draw(mats,&spline);

		glPopMatrix();
	}
	void init_name(int i=-1)
	{
		if (!obj_ref){ this->name = "empty";return; }
		if ( i== -1 )
			this->name = obj_ref->name;
		else
		{
			this->name = int_to_str(i) + std::string("-") + obj_ref->name;
		}
	}
}; 
