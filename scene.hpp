#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(bool picking = false);

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint();
  
	void pick(int id);
	int get_id(){
		return m_id;
	}
	void rotate_joint(char axis,int r);
	void pick_child(int id);
	void reset_joint();
	//bool selected;
protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;
	Matrix4x4 m_scale; //seperate to 
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;

	
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(bool bicking = false) ;

  virtual bool is_joint();

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };
	
  bool has_picked();
	
protected:
	double cur_x;
	double cur_y;
  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(bool picking = false) ;

  //const Material* get_material() const;
  Material* get_material(){return m_material;	};

  void set_material(Material* material)
  {
    m_material = material;
  }
	bool has_picked();
protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
