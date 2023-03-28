// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

class Primitive {
public:
  virtual ~Primitive();
  virtual bool hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal,  int* number_hit);
};


class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual bool hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) override;


private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual bool hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) override;


private:
  glm::vec3 m_pos;
  double m_size;
};


class Sphere : public NonhierSphere {
public:
  Sphere();
  virtual ~Sphere();
};

class Cube : public NonhierBox {
public:
  Cube();
  virtual ~Cube();
};
