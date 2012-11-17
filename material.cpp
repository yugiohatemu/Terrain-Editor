#include "material.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

Material::~Material()
{
}

void Material::apply_gl(bool picking){
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}


PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl(bool picking) //const
{
//TODO: Remeber to turn on specular
	float diffuseBuffer[3] = {m_kd.R(),m_kd.G(),m_kd.B()};
	if(picking){
		diffuseBuffer[0] = 0;
		diffuseBuffer[1] = 0;
		diffuseBuffer[2] = 1;
	}

	float shineBuffer[1] = {m_shininess};
	//float specularBuffer[3] = {m_ks.R(),m_ks.G(),m_ks.B()};
	//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specularBuffer);	
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseBuffer);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shineBuffer);

}


