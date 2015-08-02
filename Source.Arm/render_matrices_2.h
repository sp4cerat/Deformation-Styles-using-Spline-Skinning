	void render_matrices_2(void)
	{
		static bool init=true;
		static GLuint fb;
		static GLuint fbo_points[4];
		GLuint fb_type	= GL_TEXTURE_RECTANGLE_ARB;
		GLuint type		= GL_TEXTURE_RECTANGLE_ARB;

		static int fb_width  = spline_tex.width;
		static int fb_height = spline_tex.height;

		if(init)
		{
			glGenFramebuffersEXT(1,&fb); 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);

			fbo_points[0] = GFX::NewFloat16Tex(fb_width,fb_height,0,true);
			fbo_points[1] = GFX::NewFloat16Tex(fb_width,fb_height,0,true);
			fbo_points[2] = GFX::NewFloat16Tex(fb_width,fb_height,0,true);

			spline_tex.handle_nbtpx_out2 = fbo_points[0];
			spline_tex.handle_nbtpy_out2 = fbo_points[1];
			spline_tex.handle_nbtpz_out2 = fbo_points[2];

			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
											GL_COLOR_ATTACHMENT0_EXT, fb_type, 
											fbo_points[0], 0); 
			get_GL_error();
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
											GL_COLOR_ATTACHMENT1_EXT, fb_type, 
											fbo_points[1], 0); 
			get_GL_error();
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
											GL_COLOR_ATTACHMENT2_EXT, fb_type, 
											fbo_points[2], 0); 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			get_GL_error();

			init = false;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);
		get_GL_error();

		GLenum dbuffers[] = {
				GL_COLOR_ATTACHMENT0_EXT, 
				GL_COLOR_ATTACHMENT1_EXT,
				GL_COLOR_ATTACHMENT2_EXT
		};
		glDrawBuffers(3, dbuffers);get_GL_error();

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0,0,fb_width,fb_height); 	get_GL_error();
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity(); 
		glMatrixMode(GL_PROJECTION);glPushMatrix();	glLoadIdentity();
		gluOrtho2D(0.0,fb_width,0.0,fb_height);	get_GL_error();
		glMatrixMode(GL_MODELVIEW); 
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		// Bind Matrix
		glActiveTextureARB( GL_TEXTURE0 );
		glBindTexture(type, spline_tex.handle_nbtpx);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glActiveTextureARB( GL_TEXTURE1 );
		glBindTexture(type, spline_tex.handle_nbtpy);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glActiveTextureARB( GL_TEXTURE2 );
		glBindTexture(type, spline_tex.handle_nbtpz);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Spline Matrix
		glActiveTextureARB( GL_TEXTURE3 );
		glBindTexture(type, spline_tex.handle_nbtpx_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glActiveTextureARB( GL_TEXTURE4 );
		glBindTexture(type, spline_tex.handle_nbtpy_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glActiveTextureARB( GL_TEXTURE5 );
		glBindTexture(type, spline_tex.handle_nbtpz_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


		glActiveTextureARB( GL_TEXTURE0 );

		//glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);get_GL_error();

		glMatrixMode(GL_MODELVIEW);
		shader_matrices2->begin();

		shader_matrices2->setUniform1i("texBindRowX",0);
		shader_matrices2->setUniform1i("texBindRowY",1);
		shader_matrices2->setUniform1i("texBindRowZ",2);

		shader_matrices2->setUniform1i("texSplineRowX",3);
		shader_matrices2->setUniform1i("texSplineRowY",4);
		shader_matrices2->setUniform1i("texSplineRowZ",5);

		glBegin(GL_QUADS); 
		glColor3f(1,1,1);
		glMultiTexCoord2f( GL_TEXTURE0, 0.0, 0.0);
		glVertex2f(0, 0);
		glMultiTexCoord2f( GL_TEXTURE0, fb_width, 0.0);
		glVertex2f(fb_width,0);
		glMultiTexCoord2f( GL_TEXTURE0, fb_width, fb_height);
		glVertex2f(fb_width, fb_height);
		glMultiTexCoord2f( GL_TEXTURE0, 0.0, fb_height);
		glVertex2f(0, fb_height);
		glEnd();

		shader_matrices2->end();
		get_GL_error();

		glBindTexture(GL_TEXTURE_2D,0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		get_GL_error();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		glMatrixMode(GL_PROJECTION);glPopMatrix();
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
		glViewport(viewport[0],viewport[1],viewport[2],viewport[3]); 
		glDrawBuffer(GL_BACK);
	}
