// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Material.hpp"
#include "PhongMaterial.hpp"

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
);


glm::mat4 generate_ray(float nx, float ny, int d, glm::vec3 eye, glm::vec3 view, glm::vec3 up, double fovy);

glm::vec3 ray_color(glm::vec4 pworld, glm::vec4 eye, glm::vec4 ray_direction, glm::vec4 pk, int max_hits, const glm::vec3 & ambient,const std::list<Light *> & lights );

