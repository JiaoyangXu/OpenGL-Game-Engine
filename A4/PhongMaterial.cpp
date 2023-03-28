// Termm--Fall 2020

#include "PhongMaterial.hpp"
#include <glm/ext.hpp>
#include <iostream>


PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::get_color(const glm::vec3 & ambient,const Light* light, 
                              const glm::vec4 & normal, const glm::vec4 & eye, 
							  const glm::vec4 & intersection, const glm::vec4 & ray_direction ) {
	
	glm::vec4 L = normalize(ray_direction);
	glm::vec4 N = normalize(normal);
	glm::vec4 V = normalize(eye - intersection);
	glm::vec4 H = normalize(  (V+L)  / glm::length(V + L)  );
	
	// glm::vec4 R = normalize(2 * N * dot(N, L) - L) ;

	float distance = glm::length(ray_direction);

	float intensity = 1.0f / (light->falloff[0] + light->falloff[1]*distance + light->falloff[2]*distance*distance );
	glm::vec4 ID = glm::vec4(intensity * light->colour, 1.0f);	

	// ambinent
	// glm::vec4 color = glm::vec4(ambient, 1.0f) * glm::vec4(m_kd, 1.0f);


	
	//diffuse
	 glm::vec4 color = glm::vec4(m_kd,1.0f) * (glm::dot(L, N)) * ID;


	// std::cout << "Before is " << glm::to_string(color) << std::endl  ;

	//spectular
	 color += glm::vec4(m_ks,1.0f) * glm::pow(glm::dot(H, N)  , (float) m_shininess) * ID;

	 //std::cout << "After is " << glm::to_string(color) <<std::endl;
	
	return glm::vec3(color.x, color.y, color.z);
}

