// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual glm::vec3 get_color(const glm::vec3 & ambient,const Light* light, 
                              const glm::vec4 & normal, const glm::vec4 & eye,
                              const glm::vec4 & intersection, const glm::vec4 & ray_direction ) override;

  virtual ~PhongMaterial();
  glm::vec3 m_kd;


private:
  glm::vec3 m_ks;

  double m_shininess;
};
