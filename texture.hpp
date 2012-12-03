//Texture.hpp 
//Created by Yue Huang on Nov 18

#ifndef CS488_TEXTURE_HPP
#define CS488_TEXTURE_HPP

enum TEXTURE_TYPE{
	T_TERRAIN,
	T_WATER,
	T_SKYBOX_TOP,
	T_SKYBOX_LEFT,
	T_SKYBOX_RIGHT,
	T_SKYBOX_FRONT,
	T_SKYBOX_BACK,
	T_GRASS_BUMP,
	T_SAND_BUMP,
	T_SNOW_BUMP
};


class Texture{
public:
	Texture();
	Texture(TEXTURE_TYPE t);
	~Texture();

	void bind_texture();
	void apply_texture();
	
	unsigned int get_id();
	void load_texture(char * name,int format, unsigned char data[]);
	void set_type(TEXTURE_TYPE t);

private:
  	unsigned int	m_size;
	unsigned int m_id;
	TEXTURE_TYPE m_type;
	
};




#endif // CS488_TEXTURE_HPP
