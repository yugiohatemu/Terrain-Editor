#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

#define OBJ_SPHERE 1

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl(bool picking) const = 0;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual void walk_gl(bool picking) const;
};

#endif
