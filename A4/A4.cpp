// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"

glm::mat4 generate_ray(float nx, float ny, float d, glm::vec3 eye, glm::vec3 view, glm::vec3 up, double fovy) {
	glm::mat4 T;
	// std::cout << "nx is " << nx * (-1) / 2 << std::endl;
	// std::cout << "ny is " << ny * (-1) / 2 << std::endl;

	glm::mat4 T1 = glm::translate(glm::mat4(), glm::vec3(-nx / 2, -ny / 2, d) );

	// std::cout << "T1 is " << glm::to_string(T1) << std::endl;

	float theta = (float) glm::radians(fovy);
	float h = 2 * d * glm::tan(theta / 2);
	float w = h * nx / ny;
	
	glm::mat4 T2 = glm::scale(glm::mat4(), glm::vec3(-h / ny, -w / nx, 1.0f) );

	glm::vec3 vec_w = view / glm::length(view);
	glm::vec3 vec_u = glm::cross(up, vec_w) / glm::length( glm::cross(up, vec_w) );
	glm::vec3 vec_v = glm::cross(vec_w, vec_u);

	glm::mat4 T3 = glm::mat4 (
		vec_u.x, vec_u.y, vec_u.z, 0,
		vec_v.x, vec_v.y, vec_v.z, 0,
		vec_w.x, vec_w.y, vec_w.z, 0,
		0, 0, 0, 1
	);

	glm::mat4 T4 = glm::mat4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0, 
		eye.x, eye.y, eye.z, 1
	);

// 	    std::cout << "T1 is  " <<  glm::to_string(T1) << std::endl;

//   std::cout << "T2 is  " <<  glm::to_string(T2) << std::endl;

//   std::cout << "T3 is  " <<  glm::to_string(T3) << std::endl;

//   std::cout << "T4 is  " <<  glm::to_string(T4) << std::endl;

// 	std::cout << "\n\n" << std::endl;
	
	return T4 * T3 * T2 * T1;
}

glm::vec3 ray_color(SceneNode * root, glm::vec4 pworld, glm::vec4 eye, 
					glm::vec4 ray_direction, glm::vec4 pk, int max_hits,
					const glm::vec3 & ambient,const std::list<Light *> & lights) {

	glm::vec4 kd, ks, ke;
	glm::vec3 col;
	glm::vec4 normal;
	double t; 
	int material_type;
	Material* hit_material = nullptr;
	int counter = 0;

	if (root->hit(eye, ray_direction, &t, &normal, &material_type, &hit_material , &counter)) {
		if (hit_material) {
			normal = glm::normalize(normal);
			

			glm::vec4 intersection = eye + glm::vec4( t*ray_direction.x, t*ray_direction.y, t*ray_direction.z, 0.0f   );
			glm::vec4 normal_on_material;
			double new_t = 0;
			
			PhongMaterial* m_material = static_cast<PhongMaterial*>(hit_material);

			glm::vec3 a = m_material->m_kd;

			col += ambient * a; 


			for (Light* light : lights) {

				glm::vec4 shadow_ray = glm::vec4(light->position, 1.0f) - intersection;
						
				//ray_direction = glm::vec4(light->position, 1.0f) - intersection;
				col += hit_material->get_color(ambient,light,normal,eye, intersection, shadow_ray );

				counter = 0;
				
			}
			// std::cout << glm::to_string(col) << std::endl;
		} 
		
	} else {
		float random_index = ray_direction.y;


		col = glm::vec3(1.0f, 0.5f + 0.5 * random_index, 0.0f );
		// std::cout << "ray direction is " << -random_index << std::endl;

	}
	return col;
}


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
) {
  // Fill in raytracing code here...    
  std::cout << "F20: Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std::cout <<")" << std::endl;

	float h =  (float) image.height();
	float w =  (float) image.width();

	glm::dvec3 deye = glm::dvec3(eye);
  	glm::dvec3 dview = glm::dvec3(view);
  	glm::dvec3 dup = glm::dvec3(up);


	// generate ray
	glm::mat4 ray_transform_vector = generate_ray(w, h, 1.0f, eye, view, up, fovy );
	

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			glm::vec4 pk = glm::vec4(x, y, 0, 1.0f);

			glm::vec4 pworld = ray_transform_vector * pk;

			glm::vec4 ray_direction = pworld - glm::vec4(eye, 1.0f);

			//std::cout << glm::to_string(ray_direction) << std::endl;


			glm::vec3 rgb = ray_color(root, pworld, glm::vec4(eye, 1.0f), ray_direction, pk, 5, ambient,lights );


			// Red: 
			image(x, y, 0) = (double) rgb.x;
			// Green: 
			image(x, y, 1) = (double) rgb.y;
			// Blue: 
			image(x, y, 2) = (double) rgb.z;
		}
	}

}
