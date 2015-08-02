uniform sampler2DRect texPoints;
uniform sampler2DRect texNWIO;

uniform sampler2DRect texSplineRowX;
uniform sampler2DRect texSplineRowY;
uniform sampler2DRect texSplineRowZ;

uniform sampler2DRect texBindRowX;
uniform sampler2DRect texBindRowY;
uniform sampler2DRect texBindRowZ;

uniform sampler2DRect texSplineBindRowX;
uniform sampler2DRect texSplineBindRowY;
uniform sampler2DRect texSplineBindRowZ;

uniform sampler2DRect texSplineParams;

uniform float tex_width;// = 16-1;
uniform float tex_height;
uniform float character;// = 16-1;

uniform float tex_deform_ofs;
uniform float tex_deform_height;
uniform float tex_deform_scale_ofs;

uniform float blend_map;
uniform float blend_twist;
uniform vec2 blend_direction;

varying vec2 textureCoord;
varying vec2 textureCoord1;
varying vec2 textureCoord2;
varying vec2 textureCoord3;
varying vec2 textureCoord4;

void get_matrix_fast_at (
	in vec2 matrixOfs,
	out vec4 row1,
	out vec4 row2,
	out vec4 row3)
{
	row1 = texture2DRect(texSplineBindRowX,matrixOfs); 
	row2 = texture2DRect(texSplineBindRowY,matrixOfs); 
	row3 = texture2DRect(texSplineBindRowZ,matrixOfs); 
}


void get_rect_deformed (
	in float shift,
	in vec4 direction_twist_bend,
	in vec4 pos,
	in vec4 normal,
	in vec2 matrixOfs1,
	in vec2 matrixOfs_round_scale,
	in vec2 matrixOfs_rect_scale,// scale offset
	out vec3 pos_def_out,
	out vec3 normal_def_out)
{
	mat4 bind_matrix;
	mat4 deform_matrix;
	mat4 spline_matrix;
	mat4 final_matrix;

	bind_matrix[0] = texture2DRect(texBindRowX,matrixOfs1); 
	bind_matrix[1] = texture2DRect(texBindRowY,matrixOfs1);
	bind_matrix[2] = texture2DRect(texBindRowZ,matrixOfs1);
	bind_matrix[3] = vec4(0.0,0.0,0.0,1.0);
	
	vec4 pos_splinespace;
	pos_splinespace.x = dot ( bind_matrix[0], pos);
	pos_splinespace.y = dot ( bind_matrix[1], pos);
	pos_splinespace.z = dot ( bind_matrix[2], pos);
	pos_splinespace.w = 1;
	
	vec4 nrm_splinespace = bind_matrix*normal;
	nrm_splinespace.x = dot ( bind_matrix[0].xyz, normal.xyz);
	nrm_splinespace.y = dot ( bind_matrix[1].xyz, normal.xyz);
	nrm_splinespace.z = dot ( bind_matrix[2].xyz, normal.xyz);
	nrm_splinespace.w = 0;
	
	float len = length(pos_splinespace.xy);
	vec2  sincos = pos_splinespace.xy/len;
	float angle = dot(sincos.xy,direction_twist_bend.xy);
	
	float angle_blend_front = max(angle,0);
	float angle_blend_side  = max(1-abs(angle),angle_blend_front);
	float angle_blend_twist = direction_twist_bend.z ;
	
	float bend_weight = direction_twist_bend.w;

	vec4 bend_front_vector = texture2DRect(texBindRowX,matrixOfs_rect_scale);
	vec4 bend_side_vector  = texture2DRect(texBindRowY,matrixOfs_rect_scale);
	vec4 bend_round_vector = texture2DRect(texBindRowZ,matrixOfs_rect_scale);
	
	vec4 scale_front=mix (vec4(0,1,0,1),bend_front_vector,bend_weight*angle_blend_front);
	vec4 scale_side=mix (vec4(0,1,0,1),bend_side_vector,bend_weight);
	
	scale_front = mix (scale_front,bend_round_vector,angle_blend_twist);
	scale_side  = mix (scale_side ,bend_round_vector,angle_blend_twist);

	vec4 bend_tex_front = texture2DRect(texBindRowX,matrixOfs_round_scale); 	
	vec4 bend_tex_side  = texture2DRect(texBindRowY,matrixOfs_round_scale); 	
	vec4 bend_tex_twist = texture2DRect(texBindRowZ,matrixOfs_round_scale); 	
	
	vec4 surface;
	surface = mix (vec4(0,1,0,1),bend_tex_twist,angle_blend_twist);
	surface = mix (surface		,bend_tex_side ,bend_weight*angle_blend_side);
	surface = mix (surface		,bend_tex_front,bend_weight*angle_blend_front);//
	
	pos_splinespace.xy = pos_splinespace.xy + direction_twist_bend.xy * shift;	
		
	vec3 n1; n1.xyz = nrm_splinespace.xyz;	
	pos_splinespace.xy = pos_splinespace.xy * surface.w;
	vec3 n6;
	n6.x = n1.x * sincos.y - n1.y * sincos.x;
	n6.y = dot(n1.xy , sincos.xy);// n1.y * sincos.y + n1.x * sincos.x;
	n6.z = n1.z;	
	n6.xz = n6.xz * surface.w;
	vec2 rot_yz = normalize ( surface.yz );
	vec2 rot_xy = normalize ( surface.xy );	
	vec3 n7;
	n7.y = dot(n6.yz , rot_yz.xy);//n6.y * rot_yz.x + n6.z * rot_yz.y;
	n7.z = n6.z * rot_yz.x - n6.y * rot_yz.y;
	n7.x = n6.x;
	vec3 n8;
	n8.x = n7.x * rot_xy.y + n7.y * rot_xy.x;
	n8.y = n7.y * rot_xy.y - n7.x * rot_xy.x;
	n8.z = n7.z;
	nrm_splinespace.x = n8.x * sincos.y + n8.y * sincos.x;
	nrm_splinespace.y = n8.y * sincos.y - n8.x * sincos.x;
	nrm_splinespace.z = n8.z;
	
	
	//vec3 normal_def;//normal_def.xyz = nrm_splinespace.xyz;
	
			
	vec2 pos_rot;
	pos_rot.x = dot(pos_splinespace.xy , direction_twist_bend.xy);
	pos_rot.y = pos_splinespace.y * direction_twist_bend.x - pos_splinespace.x * direction_twist_bend.y ;	
	pos_rot.x = pos_rot.x * scale_front.w;
	pos_rot.y = pos_rot.y * scale_side.w;
	float sign_mul = pos_rot.y;
	pos_splinespace.x = pos_rot.x * direction_twist_bend.x - pos_rot.y * direction_twist_bend.y ;
	pos_splinespace.y = dot(pos_rot.yx , direction_twist_bend.xy);//pos_rot.y * direction_twist_bend.x + pos_rot.x * direction_twist_bend.y ;
	
	vec3 normal_def,n2,n3,n4,n5;
		
		
	n2.x = dot(nrm_splinespace.xy , direction_twist_bend.xy);//nrm_splinespace.x * direction_twist_bend.x + nrm_splinespace.y * direction_twist_bend.y ;
	n2.y = nrm_splinespace.y * direction_twist_bend.x - nrm_splinespace.x * direction_twist_bend.y ;
	n2.z = nrm_splinespace.z;
	n2.yz = n2.yz * scale_front.w;
	vec2 rot_xz = ( scale_front.yz );// normalize
	n3.x = dot(n2.xz , rot_xz.xy);//n2.x * rot_xz.x + n2.z * rot_xz.y;
	n3.z = dot(vec2(n2.z,-n2.x),rot_xz.xy);//n2.z * rot_xz.x - n2.x * rot_xz.y;
	n3.y = n2.y;	
	n3.xz = n3.xz * scale_side.w;
	vec2 rot_yx = ( scale_side.yx );//normalize
	rot_yx.y = rot_yx.y * sign_mul;
	n4.y = n3.y * rot_yx.x - n3.z * rot_yx.y;//	n4.y = dot(vec2(n3.y,-n3.z),rot_yx.xy);//
	n4.z = dot(n3.zy , rot_yx.xy);//n3.z * rot_yx.x + n3.y * rot_yx.y;
	n4.x = n3.x;	
	normal_def.x = dot(vec2(n4.x,-n4.y) , direction_twist_bend.xy);//normal_def.x = n4.x * direction_twist_bend.x - n4.y * direction_twist_bend.y ;
	normal_def.y = dot(n4.yx , direction_twist_bend.xy);//n4.y * direction_twist_bend.x + n4.x * direction_twist_bend.y ;
	normal_def.z = n4.z ;
	
		
	spline_matrix[0] = texture2DRect(texSplineRowX,matrixOfs1); 
	spline_matrix[1] = texture2DRect(texSplineRowY,matrixOfs1);
	spline_matrix[2] = texture2DRect(texSplineRowZ,matrixOfs1);

	pos_def_out.x = dot ( spline_matrix[0], pos_splinespace);
	pos_def_out.y = dot ( spline_matrix[1], pos_splinespace);
	pos_def_out.z = dot ( spline_matrix[2], pos_splinespace);
	
	normal_def_out.x = dot ( spline_matrix[0].xyz, normal_def.xyz);
	normal_def_out.y = dot ( spline_matrix[1].xyz, normal_def.xyz);
	normal_def_out.z = dot ( spline_matrix[2].xyz, normal_def.xyz);
	
	normal_def_out.xyz = normalize(normal_def_out.xyz);
}


void main(void)
{
	vec2 tc1 = textureCoord*vec2(4.0,1.0)+vec2(-0.5-1.0,-0.5);
	vec2 tc2 = tc1+vec2(1.0,0.0);
	vec2 tc3 = tc1+vec2(2.0,0.0);
	vec2 tc4 = tc1+vec2(3.0,0.0);

	vec4 pos		= texture2DRect(texPoints,textureCoord) ;
	vec3 normal		= texture2DRect(texNWIO,tc1).xyz * 2.0 - 1.0;
	vec4 weights	= texture2DRect(texNWIO,tc2) ;
	vec4 index		= texture2DRect(texNWIO,tc3)*255.0+character;
	vec4 splinepos	= texture2DRect(texNWIO,tc4);
	vec4 offsets	= splinepos*tex_width;
	
	float lin_a = weights.w;
	//weights.z = 1-(weights.x+weights.y);
	
	vec4 m1row1,m1row2,m1row3;
	vec4 m2row1,m2row2,m2row3;
	vec4 m3row1,m3row2,m3row3;
		
	vec3 f = frac(offsets.xyz);
	float a = splinepos.x;
	float a01 = clamp(a,0,1);
	float a21 = a*2-1;
//	float a_mul = 1-(-pow(1-abs(a21),0.5)*0.5+1);
	
	float rot   = texture2DRect(texSplineParams,vec2(0,index.x)).w ;
	float twist = 0.3*abs(texture2DRect(texSplineParams,vec2(1,index.x)).w) ;
	float dir_x = texture2DRect(texSplineParams,vec2(2,index.x)).w ;
	float dir_y = texture2DRect(texSplineParams,vec2(3,index.x)).w ;
	vec4 snormal = texture2DRect(texSplineParams,vec2(4,index.x)) ;
	vec4 params5 = texture2DRect(texSplineParams,vec2(5,index.x)) ;
	float shift  = params5.w ;
	float style  = params5.x ;
	float bendangle = snormal.w ;

	vec3 pos1_out;
	vec3 normal1_out;
	
	float ofsx = lin_a * tex_width *(1.0-1.0/16.0)+1.0/32.0;
	//float ofsx = offsets.x * (1.0-1.0/16.0)+1.0/32.0;

	vec3 p1,p2,p3;
	vec3 n1,n2,n3;

	get_rect_deformed(
		shift,
		vec4 ( dir_x,dir_y,twist,bendangle),
		vec4 ( pos.xyz,1),
		vec4 ( normal.xyz,0),
		vec2 (offsets.x	,index.x),
		vec2 (ofsx,style*65 + tex_deform_height*(frac(splinepos.w*(1.0-1.0/32.0)+1.0/64.0))+tex_deform_ofs),
		vec2 (ofsx,style*65 +tex_deform_scale_ofs),
		p1,
		n1	);
	pos1_out = pos1_out + snormal.xyz * shift;
	
		/*
	get_matrix_fast_at(
		vec2 (offsets.x,index.x),
		m1row1,m1row2,m1row3);
		*/
		
	get_matrix_fast_at(
		vec2 (offsets.y,index.y),
		m2row1,m2row2,m2row3);
		
	get_matrix_fast_at(
		vec2 (offsets.z,index.z),
		m3row1,m3row2,m3row3);
/*
	vec4 row1,row2,row3;
	row1 = m2row1*weights.y+m3row1*weights.z;
	row2 = m2row2*weights.y+m3row2*weights.z;
	row3 = m2row3*weights.y+m3row3*weights.z;
//	row1 = m1row1*weights.x+m2row1*weights.y+m3row1*weights.z;
//	row2 = m1row2*weights.x+m2row2*weights.y+m3row2*weights.z;
//	row3 = m1row3*weights.x+m2row3*weights.y+m3row3*weights.z;
	
	vec4 pos_out;
	vec3 normal_out;
	pos_out.x = dot ( pos,row1 );
	pos_out.y = dot ( pos,row2 );
	pos_out.z = dot ( pos,row3 );
	normal_out.x = dot ( normal.xyz ,row1.xyz );
	normal_out.y = dot ( normal.xyz ,row2.xyz );
	normal_out.z = dot ( normal.xyz ,row3.xyz );
	normal_out.xyz = mix ( normal_out.xyz ,normal1_out.xyz ,  weights.x);
	pos_out.xyz = mix ( pos_out.xyz ,pos1_out.xyz ,  weights.x);
*/
	
//	p1.x = dot ( pos,m1row1 );
//	p1.y = dot ( pos,m1row2 );
//	p1.z = dot ( pos,m1row3 );
	p2.x = dot ( pos,m2row1 );
	p2.y = dot ( pos,m2row2 );
	p2.z = dot ( pos,m2row3 );
	p3.x = dot ( pos,m3row1 );
	p3.y = dot ( pos,m3row2 );
	p3.z = dot ( pos,m3row3 );
	
//	n1.x = dot ( normal.xyz,m1row1.xyz );
//	n1.y = dot ( normal.xyz,m1row2.xyz );
//	n1.z = dot ( normal.xyz,m1row3.xyz );
	n2.x = dot ( normal.xyz,m2row1.xyz );
	n2.y = dot ( normal.xyz,m2row2.xyz );
	n2.z = dot ( normal.xyz,m2row3.xyz );
	n3.x = dot ( normal.xyz,m3row1.xyz );
	n3.y = dot ( normal.xyz,m3row2.xyz );
	n3.z = dot ( normal.xyz,m3row3.xyz );
	
	vec4 pos_out;
	vec3 normal_out;
	
	normal_out  = n1* weights.x + n2 * weights.y + n3 * weights.z;
	pos_out.xyz = p1* weights.x + p2 * weights.y + p3 * weights.z;;

	pos_out.w = dot(floor(normal_out * 127 + 127.5), vec3(1.0 / 256.0, 1.0, 256.0));
	gl_FragData[0] = pos_out ;
}
