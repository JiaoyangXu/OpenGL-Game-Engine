// Termm--Fall 2020

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
//#define RENDER_BOUNDING_VOLUMES

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  virtual bool hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) override;

  
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
