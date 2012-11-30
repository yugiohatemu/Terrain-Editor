//Texture.hpp 
//Created by Yue Huang on Nov 18

#ifndef CS488_TEXTURE_HPP
#define CS488_TEXTURE_HPP

enum TEXTURE_TYPE{
	T_TERRAIN,
	T_WATER,
	T_SKYBOX
};


class Texture{
public:
	Texture();
	Texture(TEXTURE_TYPE t);
	~Texture();

	void bind_texture();
	void apply_texture();
	//void set_texture(std::string name, int format,unsigned int size);
	//unsigned char* get_data();
	
	unsigned int get_id();
	//void load_texture();
	void load_texture(char * name,int format, unsigned char data[]);
	void set_type(TEXTURE_TYPE t);

private:
	//string	m_name;
  	//int		m_format;
  	unsigned int	m_size;
	//unsigned char * m_data;
	unsigned int m_id;
	TEXTURE_TYPE m_type;
	
};




#endif // CS488_TEXTURE_HPP
