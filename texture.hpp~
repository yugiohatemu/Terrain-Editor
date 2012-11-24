//Texture.hpp 
//Created by Yue Huang on Nov 18

#ifndef CS488_TEXTURE_HPP
#define CS488_TEXTURE_HPP

class Texture{
public:
	Texture();
	
	~Texture();

	void bind_texture();
	void apply_texture();
	void set_texture(char * name, int format,unsigned int size);
	unsigned char* get_data();
	void load_texture();


private:
	char *	m_name;
  	int		m_format;
  	unsigned int	m_size;
	unsigned char * m_data;
	unsigned int m_id;

	
};




#endif // CS488_TEXTURE_HPP
