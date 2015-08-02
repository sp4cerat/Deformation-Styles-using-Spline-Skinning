// ---------------------------------------- //
class Deformer {

public:

	std::string name ;
	Geometry geo;

	Deformer(){	init (); }
	~Deformer()
	{ 
		clear();
	}
	void init()
	{
		name = "empty";
	}

	void clear ()
	{
		geo.clear();
	}
	void set_geometry ( Geometry geo )
	{
		this->geo = geo;
		this->name = geo.name;
		geo.init();
	}
	
	void draw (Material *materials, Spline* spline)
	{
		if ( spline )
		{
			geo.draw( materials , spline );
			//spline->draw();
		}
		else
			geo.draw( materials );
	}
};
// ---------------------------------------- //

