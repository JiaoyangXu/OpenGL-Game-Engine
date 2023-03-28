// Termm--Fall 2020

#include "Material.hpp"

Material::Material()
{}

Material::~Material()
{}

glm::vec3 Material::get_color(const glm::vec3 & ambient,const Light* light, 
                              const glm::vec4 & normal, const glm::vec4 & eye, 
                              const glm::vec4 & intersection, const glm::vec4 & ray_direction ) {
    return glm::vec3(1.0f);
}
