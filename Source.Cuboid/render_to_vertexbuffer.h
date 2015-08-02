	void render_to_vertexbuffer(void)
	{
//		glFinish();
//		glFlush();
		int t1  = timeGetTime();

		render_matrices();
		render_matrices_2();

//		glFinish();
//		glFlush();
		int t2  = timeGetTime();

		static bool init=true;
		static GLuint fb;
		static GLuint fbo_points[4];
		static GLuint fbo_colors;

		if(render_index!=0)return;

		GLuint fb_type = GL_TEXTURE_RECTANGLE_ARB;
		//GLuint fb_type = GL_TEXTURE_2D;

		if(init)
		{
			glGenFramebuffersEXT(1,&fb); 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);
			fbo_points[0] = GFX::NewFloatRectTex(tex_width,tex_height,0,true);
			glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
										GL_COLOR_ATTACHMENT0_EXT, 
										fb_type, 
										fbo_points[0], 0); 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
			init = false;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);
		/*
		glFramebufferTexture2DEXT(	GL_FRAMEBUFFER_EXT, 
									GL_COLOR_ATTACHMENT0_EXT, 
									fb_type, 
									fbo_points[0], 0); */
		//GLenum dbuffers[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
		//glDrawBuffers(1, dbuffers);get_GL_error();
		
		get_GL_error();

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glViewport(0,0,tex_width,tex_height); 	get_GL_error();
		glMatrixMode(GL_MODELVIEW); glPushMatrix();	glLoadIdentity(); 
		glMatrixMode(GL_PROJECTION);glPushMatrix();	glLoadIdentity();
		gluOrtho2D(0.0,tex_width,0.0,tex_height);	get_GL_error();
		glMatrixMode(GL_MODELVIEW); 
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		GLuint type = GL_TEXTURE_RECTANGLE_ARB;

		glActiveTextureARB( GL_TEXTURE0 );
		glBindTexture(type, tex_points);
		glActiveTextureARB( GL_TEXTURE1 );
		glBindTexture(type, tex_nwio);
		/*
		glBindTexture(type, tex_normals);
		glActiveTextureARB( GL_TEXTURE2 );
		glBindTexture(type, tex_weights);
		glActiveTextureARB( GL_TEXTURE3 );
		glBindTexture(type, tex_spline_ofs);
		glActiveTextureARB( GL_TEXTURE4 );
		glBindTexture(type, tex_indices);
		*/

		// Bind matrix
		glActiveTextureARB( GL_TEXTURE2 );
		glBindTexture(type, spline_tex.handle_nbtpx);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE3 );
		glBindTexture(type, spline_tex.handle_nbtpy);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE4 );
		glBindTexture(type, spline_tex.handle_nbtpz);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Spline matrix
		glActiveTextureARB( GL_TEXTURE5 );
		glBindTexture(type, spline_tex.handle_nbtpx_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE6 );
		glBindTexture(type, spline_tex.handle_nbtpy_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE7 );
		glBindTexture(type, spline_tex.handle_nbtpz_out);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Params
		glActiveTextureARB( GL_TEXTURE8 );
		glBindTexture(type, spline_tex.handle_params);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// SplineBind matrix
		glActiveTextureARB( GL_TEXTURE9 );
		glBindTexture(type, spline_tex.handle_nbtpx_out2);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE10);
		glBindTexture(type, spline_tex.handle_nbtpy_out2);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glActiveTextureARB( GL_TEXTURE11);
		glBindTexture(type, spline_tex.handle_nbtpz_out2);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glActiveTextureARB( GL_TEXTURE0 );

		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);get_GL_error();
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);get_GL_error();
		//glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);get_GL_error();

		float tex_1 = float(tex_height_real)/float(tex_height);

		for(int i=0;i<num_characters;i++)
		{	
			glMatrixMode(GL_MODELVIEW);
			shader_deform->begin();
			shader_deform->setUniform1i("texPoints",0);
			shader_deform->setUniform1i("texNWIO",1);

			shader_deform->setUniform1i("texBindRowX",2);
			shader_deform->setUniform1i("texBindRowY",3);
			shader_deform->setUniform1i("texBindRowZ",4);

			shader_deform->setUniform1i("texSplineRowX",5);
			shader_deform->setUniform1i("texSplineRowY",6);
			shader_deform->setUniform1i("texSplineRowZ",7);

			shader_deform->setUniform1i("texSplineParams",8);

			shader_deform->setUniform1f("tex_deform_ofs",float(0.5+num_characters*3));
			shader_deform->setUniform1f("tex_deform_height",float(63));
			//shader_deform->setUniform1f("tex_deform_scale_ofs",float(num_characters*3+64.5));

			shader_deform->setUniform1i("texSplineBindRowX",9);
			shader_deform->setUniform1i("texSplineBindRowY",10);
			shader_deform->setUniform1i("texSplineBindRowZ",11);

			shader_deform->setUniform1f("tex_width",float(spline_tex.width-1));

			shader_deform->setUniform1f("character",float(float(i*3)+0.5));

			float mul_x=1;
			float mul_y=1;
			mul_x=tex_width;
			mul_y=tex_height;

			glBegin(GL_QUADS); 
			glColor3f(1,1,1);

			glMultiTexCoord2f( GL_TEXTURE0, 0, 0);
			glVertex2f(0, 0);

			glMultiTexCoord2f( GL_TEXTURE0, tex_width, 0.0);
			glVertex2f(tex_width,0);

			glMultiTexCoord2f( GL_TEXTURE0, tex_width, tex_height_real);
			glVertex2f(tex_width, tex_height_real);

			glMultiTexCoord2f( GL_TEXTURE0, 0.0, tex_height_real);
			glVertex2f(0, tex_height_real);

			glEnd();

			shader_deform->end();
						
			// Copy			
			get_GL_error();

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();glLoadIdentity();	
			glMatrixMode(GL_MODELVIEW);

			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
			glRasterPos2i(0,0);
			glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, vbo_points_handle_array[i]);
			glReadPixels(0,0, tex_width,tex_height , GL_RGBA, GL_FLOAT, 0);get_GL_error();
			glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}
		glFinish();
		glFlush();
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

		int t3  = timeGetTime();

//		printf("Matrices:%3.3fms\n",float(t2-t1));
//		printf("Deform:%3.3fms\n",float(t3-t2));
	}

