#include "scene.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <map>


int i = 1; //this is use for picking
std::vector<int> pick_list;
std::vector<int> child_pick_list;
const double ROTATE_RATIO = 1.0;
int rotate_x , rotate_y = 0;
int head_joint_id;
bool reset = false;
std::map<int, bool> pick_map;
std::map<int, bool> pick_child_map;

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
	m_id = i;
	i+=1;
	if(name == "headjoint"){
		head_joint_id = m_id;
	}
	//selected = false;
	//std::cerr<<m_id<<std::endl;
}

SceneNode::~SceneNode(){
}

void SceneNode::walk_gl(bool picking) {
  ChildList::iterator it;
	glPushMatrix();
	glMultMatrixd(m_trans.transpose().begin());
	glLoadName(m_id);
	
  for(it=m_children.begin() ; it != m_children.end(); it++ ){	
			//glPushName(i);
			(*it)->walk_gl(picking);		
			//i += 1;	
			//glPopName();
	}
	reset = false;
	
	glPopMatrix();
}

//+1, increase, -1, decrease, 0 doesnt change
void SceneNode::rotate_joint(char axis,int r){
	if(axis == 'x'){
		rotate_x = r;
	}else if(axis == 'y'){
		rotate_y = r;
	}
}

void SceneNode::reset_joint(){
		//pick_list.clear();
		//child_pick_list.clear();
		rotate_x = 0;
		rotate_y = 0;
		std::map<int,bool>::iterator it;
		for(it = pick_map.begin();it !=pick_map.end();it++){
			(*it).second = false;
		}
		for(it = pick_child_map.begin();it !=pick_child_map.end();it++){
			(*it).second = false;
		}
		reset = true;
		//std::cerr<<"not even here?"<<std::endl;
}

void SceneNode::rotate(char axis, double angle){
	Matrix4x4 r;
	//std::cerr << "Stub: rotate "<< m_name <<" on " <<axis << " by " << angle << std::endl;
	double c = cos(angle* M_PI/180);
	double s = sin(angle* M_PI/180);
  if(axis == 'x'){
		Vector4D row1(1,0,0,0);
		Vector4D row2(0,c,-s,0);
		Vector4D row3(0,s,c,0);
		Vector4D row4(0,0,0,1);
		r = Matrix4x4(row1,row2,row3,row4);
	}else if(axis == 'y'){
		Vector4D row1(c,0,s,0);
		Vector4D row2(0,1,0,0);
		Vector4D row3(-s,0,c,0);
		Vector4D row4(0,0,0,1);
		r = Matrix4x4(row1,row2,row3,row4);
	}else if(axis == 'z'){ //z axis
		Vector4D row1(c,-s,0,0);
		Vector4D row2(s,c,0,0);
		Vector4D row3(0,0,1,0);
		Vector4D row4(0,0,0,1);
		r = Matrix4x4(row1,row2,row3,row4);
	}
	r = m_trans*r;
	
	set_transform(r);
}

void SceneNode::scale(const Vector3D& amount){
  //std::cerr << "Stub: Scale " << m_name << " by " << amount << std::endl;
	//Matrix4x4 s;
	Vector4D row1(amount[0],0,0,0);
	Vector4D row2(0,amount[1],0,0);
	Vector4D row3(0,0,amount[2],0);
	Vector4D row4(0,0,0,1);
	//s = Matrix4x4(row1,row2,row3,row4);	
	//s = m_trans*s;
	m_scale = Matrix4x4(row1,row2,row3,row4);
	//set_transform(s); 
  
}

void SceneNode::translate(const Vector3D& amount){
  //std::cerr << "Stub: Translate " << m_name << " by " << amount << std::endl;
	Matrix4x4 t;
	Vector4D row1(1.0,0.0,0.0,amount[0]);
	Vector4D row2(0.0,1.0,0.0,amount[1]);
	Vector4D row3(0.0,0.0,1.0,amount[2]);
	Vector4D row4(0.0,0.0,0.0,1.0);
	t = Matrix4x4(row1,row2,row3,row4);	
	t = m_trans*t; 

	set_transform(t);
}



bool SceneNode::is_joint() {
  return false;
}
/*
void SceneNode::pick(int id){
	//check if id is in, remove
	std::vector<int>::iterator it;
	for(it = pick_list.begin();it<pick_list.end();it++){
		if((*it) == id){ //also need to check whether this id is a joint node
			pick_list.erase(it);
			pick_child(id +1 );
			return ;
		}
	}
	
	pick_list.push_back(id);
	pick_child(id+1);
	
}

void SceneNode::pick_child(int id){
	//check if id is in, remove
	std::vector<int>::iterator it;

	for(it = child_pick_list.begin();it<child_pick_list.end();it++){
		if((*it) == id){
			child_pick_list.erase(it);
			//std::cerr<<"Unselect "<<id<<std::endl;
			return ;
		}
	}
	//cant find, insert
	//std::cerr<<"Select "<<id<<std::endl;
	child_pick_list.push_back(id);
}
*/

void SceneNode::pick(int id){
	//check if id is in, remove
	//std::cerr<<pick_map.size()<<std::endl;
	if(pick_map.find(id)!=pick_map.end()){
		pick_map[id] = !pick_map[id];
		//std::cerr<<"Pick "<<id<<" "<<pick_map[id] <<std::endl;
		pick_child_map[id+1] = !pick_child_map[id+1];
	}
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
	pick_map[m_id] = false;
}

JointNode::~JointNode()
{
}

//only check(direct descendent) is picked, then do rotation
//for convenience, suppose a joint would only have one direct child 
//otherwise "hard" to consider whether being picked
//Remeber to do the same on when writing lua file
bool JointNode::has_picked(){
	std::vector<int>::iterator it;
	for(it = pick_list.begin();it<pick_list.end();it++){
		if(m_id == (*it)){
			return true;
		}
	}
	return false;
}

/*bool JointNode::has_picked(){
	
	return pick_map[m_
}*/


void JointNode::walk_gl(bool picking) {
	//joint node are hidden, use to indicate whether the cildren are jointable
	
	//bool movable = (picking && has_picked());
	bool movable = (picking && pick_map[m_id]);
	//std::cerr<<"shoudl come"<<std::endl;
	//here, check if we need to update the m_trans matrix
	if(movable){
		
		cur_x += rotate_x * ROTATE_RATIO;
		
		if(cur_x>m_joint_x.max){
			cur_x = m_joint_x.max;
		}else if(cur_x<m_joint_x.min){
			cur_x = m_joint_x.min;
			//should stop accumulating after certain point?
		}else{
			rotate('x', rotate_x * ROTATE_RATIO);
		}
		
		if(m_id == head_joint_id){ //Only head rotate around y axis
			cur_y += rotate_y * ROTATE_RATIO;
			if(cur_y>m_joint_y.max){
				cur_y = m_joint_y.max;
			}else if(cur_y<m_joint_y.min){
				cur_y = m_joint_y.min;
			}else{
				rotate('y', rotate_y * ROTATE_RATIO);
			}	
		}
	}
	
	
	glPushMatrix(); 
	if(reset){
		//std::cerr<<"Call"<<std::endl;
		glMultMatrixd(m_invtrans.transpose().begin());
		cur_x = m_joint_x.init;
		cur_y = m_joint_y.init;
	}else{
		glMultMatrixd(m_trans.transpose().begin());
	}
	glLoadName(m_id);
	
	ChildList::iterator it;	
	
	for(it=m_children.begin() ; it != m_children.end(); it++ ){
			(*it)->walk_gl(picking);			
	}
	glPopMatrix();

}

/*int r_x = rotate_x * ROTATE_RATIO;
		if(cur_x+r_x>m_joint_x.max){	
			std::cerr<<"should come here"<<std::endl;			
			r_x = m_joint_x.max - cur_x;
			cur_x = m_joint_x.max;
		}else if(cur_x<m_joint_x.min){
			r_x =  cur_x- m_joint_x.max;
			cur_x = m_joint_x.min;
		}
		rotate('x', r_x);*/
		
/*int r_y = rotate_y * ROTATE_RATIO;
			if(cur_y+r_y>m_joint_y.max){			
			r_y = m_joint_y.max - cur_y;
			cur_y = m_joint_y.max;
			}else if(cur_y<m_joint_y.min){
			r_y =  cur_y- m_joint_y.max;
			cur_y = m_joint_y.min;
			}
			rotate('y', r_y);*/

bool JointNode::is_joint()
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max){
	//std::cerr<<"Stub: "<<m_name<< " joint x"<<" "<<min<<" "<<init<<" "<<max<<std::endl;
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
	//add initial rotate
	rotate('x',init);
}

void JointNode::set_joint_y(double min, double init, double max){
 //std::cerr<<"Stub: "<<m_name<<" joint y"<<" "<<min<<" "<<init<<" "<<max<<std::endl;
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
	//add initial rotate
	rotate('z',init);
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),m_primitive(primitive){
	pick_child_map[m_id] = false;
}

GeometryNode::~GeometryNode()
{

}


bool GeometryNode::has_picked(){
	std::vector<int>::iterator it;
	for(it = child_pick_list.begin();it<child_pick_list.end();it++){
		if(m_id == (*it)){
			return true;
		}
	}
	return false;
}

void GeometryNode::walk_gl(bool picking)
{
	glPushMatrix(); 
	glMultMatrixd(m_trans.transpose().begin());
	
	//std::cerr<<has_picked();
	//std::cerr<<has_picked()<<std::endl;
	//bool picked = (has_picked() && picking);
	bool picked = (picking&&pick_child_map[m_id]);
	//avoid normal change
	glPushMatrix();
	glMultMatrixd(m_scale.transpose().begin());	
	m_material->apply_gl(picked);	 	
  m_primitive->walk_gl(picking);
	glPopMatrix();

	glLoadName(m_id);

	ChildList::iterator it;	
	for(it=m_children.begin() ; it != m_children.end(); it++ ){
			(*it)->walk_gl(picking);			
	}
	
	glPopMatrix(); 
	
}
 //int i = 1;
	//std::cerr<<m_id<<" " <<is_joint()<<std::endl;
