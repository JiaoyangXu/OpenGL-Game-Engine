// Termm--Fall 2020
#include <glm/glm.hpp>
#include <list>
#include "Light.hpp"


#pragma once

class Material {
public:
  virtual ~Material();
  virtual glm::vec3 get_color(const glm::vec3 & ambient,const Light* light, 
                              const glm::vec4 & normal, const glm::vec4 & eye, 
                              const glm::vec4 & intersection, const glm::vec4 & ray_direction );

protected:
  Material();
};
