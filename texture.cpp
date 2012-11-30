#include "texture.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <iostream>

Texture::Texture(){
	m_id = 0;
	//m_format = GL_RGB;
}

Texture::Texture(TEXTURE_TYPE t){
	m_type = t;
	
}

void Texture::set_type(TEXTURE_TYPE t){
	m_type = t;
	//m_id = id;
}
/*
void Texture::set_texture(std::string name, int format,unsigned int size){
	m_name = name;
	m_format =format;
	m_size = size;
}*/

Texture::~Texture(){
}
/*
unsigned char* Texture::get_data(){
	return m_data;
}
*/
unsigned int Texture::get_id(){
	return m_id;
}

void Texture::bind_texture(){
	//Bind the texture
	if(m_type == T_WATER){
		m_size = 256;

		unsigned char total_texture[4 * m_size * m_size];
		unsigned char caustic_texture[3 * m_size * m_size];
		unsigned char alpha_texture[ m_size * m_size];

		load_texture("reflection.jpg",GL_RGB,caustic_texture);
		load_texture("alpha.jpg",GL_ALPHA,alpha_texture);
		
		for (int i = 0; i < m_size * m_size; i++){
		  total_texture[4 * i] = caustic_texture[3 * i];
		  total_texture[4 * i + 1] = caustic_texture[3 * i + 1];
		  total_texture[4 * i + 2] = caustic_texture[3 * i + 2];
		  total_texture[4 * i + 3] = alpha_texture[i];
    	}
		glGenTextures (1, &m_id);
		glBindTexture (GL_TEXTURE_2D, m_id);
		
		gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, m_size, m_size, GL_RGBA, GL_UNSIGNED_BYTE,  total_texture);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glEnable (GL_TEXTURE_GEN_S);
		glEnable (GL_TEXTURE_GEN_T);
		glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		

	}else{
		glGenTextures (1, &m_id);
		glBindTexture (GL_TEXTURE_2D, m_id);
		
		if(m_type == T_TERRAIN ){
			m_size = 64;
			unsigned char total_texture [3 * m_size * m_size];
			load_texture("marble.jpg",GL_RGB,total_texture);
			gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGB,m_size, m_size, GL_RGB,GL_UNSIGNED_BYTE, total_texture);
			
		}else if(m_type == T_SKYBOX){
			m_size = 512;
			unsigned char total_texture [3 * m_size * m_size];
			load_texture("otop6.jpg",GL_RGB,total_texture);
			gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGB,m_size, m_size, GL_RGB,GL_UNSIGNED_BYTE, total_texture);
			
		}
			
		//Texture mapping format
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//Modulate format
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		
	}
}

void Texture::apply_texture(){
	//std::cerr<<m_id<<std::endl;
	glBindTexture (GL_TEXTURE_2D, m_id);
	if(m_type == T_WATER){
		glEnable (GL_TEXTURE_GEN_S);
		glEnable (GL_TEXTURE_GEN_T);
	}else{
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
	}
}

//http://www.lousodrome.net/opengl/ function for loading jpeg
void Texture::load_texture (char* name,int format, unsigned char data[]){
		/*if(m_format == GL_RGB){
			data = (unsigned char *) malloc(m_size * m_size * 3);
		}else if(m_format == GL_ALPHA){
			data = (unsigned char *) malloc(m_size * m_size);
		}*/		

		FILE *fd;
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		unsigned char * line;

		cinfo.err = jpeg_std_error (&jerr);
		jpeg_create_decompress (&cinfo);
		
		//If open file fails
		if (0 == (fd = fopen(name, "rb"))){
			std::cerr<<"File open error"<<std::endl;
		}

		jpeg_stdio_src (&cinfo, fd);
		jpeg_read_header (&cinfo, TRUE);
		if ((cinfo.image_width != m_size) || (cinfo.image_height !=  m_size)){
			std::cerr<<"Image size does not match"<<std::endl;
		}
		if (GL_RGB == format){
		 	 if (cinfo.out_color_space == JCS_GRAYSCALE){
				std::cerr<<"Image color error"<<std::endl;
			}
		}else if (cinfo.out_color_space != JCS_GRAYSCALE){
		  	std::cerr<<"Image format error"<<std::endl;
		}
		jpeg_start_decompress (&cinfo);

		while (cinfo.output_scanline < cinfo.output_height){
		  	line = data +(GL_RGB == format ? 3 * m_size : m_size) * cinfo.output_scanline;
		  	jpeg_read_scanlines (&cinfo, &line, 1);
		}

		jpeg_finish_decompress (&cinfo);
		jpeg_destroy_decompress (&cinfo);

}


