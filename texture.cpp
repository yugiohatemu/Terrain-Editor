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
	m_format = GL_RGB;
}

void Texture::set_texture(char * name, int format,unsigned int size){
	m_name = name;
	m_format =format;
	m_size = size;
}


Texture::~Texture(){
}

unsigned char* Texture::get_data(){
	return m_data;
}

unsigned int Texture::get_id(){
	return m_id;
}
void Texture::bind_texture(){
	//Bind the texture
	glGenTextures (1, &m_id);
	glBindTexture (GL_TEXTURE_2D, m_id);
	/*if(m_format == GL_RGB){
		m_data = (unsigned char *) malloc(m_size * m_size * 3);
	}else if(m_format == GL_ALPHA){
		m_data = (unsigned char *) malloc(m_size * m_size);
	}*/
	
	load_texture();
   	gluBuild2DMipmaps (GL_TEXTURE_2D, m_format,m_size, m_size, m_format,GL_UNSIGNED_BYTE, m_data);
	
	//Texture mapping format
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Replace format
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//Free data
	free(m_data);
}

void Texture::apply_texture(){
	glBindTexture (GL_TEXTURE_2D, m_id);
	
}

//http://www.lousodrome.net/opengl/ function for loading jpeg
void Texture::load_texture (){
		if(m_format == GL_RGB){
		m_data = (unsigned char *) malloc(m_size * m_size * 3);
	}else if(m_format == GL_ALPHA){
		m_data = (unsigned char *) malloc(m_size * m_size);
	}		

		FILE *fd;
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr jerr;
		unsigned char * line;

		cinfo.err = jpeg_std_error (&jerr);
		jpeg_create_decompress (&cinfo);
		
		//If open file fails
		if (0 == (fd = fopen(m_name, "rb"))){
			std::cerr<<"File open error"<<std::endl;
		}

		jpeg_stdio_src (&cinfo, fd);
		jpeg_read_header (&cinfo, TRUE);
		if ((cinfo.image_width != m_size) || (cinfo.image_height !=  m_size)){
			std::cerr<<"Image size does not match"<<std::endl;
		}
		if (GL_RGB == m_format){
		 	 if (cinfo.out_color_space == JCS_GRAYSCALE){
				std::cerr<<"Image color error"<<std::endl;
			}
		}else if (cinfo.out_color_space != JCS_GRAYSCALE){
		  	std::cerr<<"Image format error"<<std::endl;
		}
		jpeg_start_decompress (&cinfo);

		while (cinfo.output_scanline < cinfo.output_height){
		  	line = m_data +(GL_RGB == m_format ? 3 * m_size : m_size) * cinfo.output_scanline;
		  	jpeg_read_scanlines (&cinfo, &line, 1);
		}

		jpeg_finish_decompress (&cinfo);
		jpeg_destroy_decompress (&cinfo);

}


