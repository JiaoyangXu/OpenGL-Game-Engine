// Termm--Fall 2020

#include <iostream>
#include <fstream>
#include <cmath>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

constexpr float kEpsilon = 1e-8;


Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{

	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs(fname.c_str()  );

	while( ifs >> code ) {
		

		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}


// cite https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html
bool rayTriangleIntersect(
    const glm::vec3 &orig, const glm::vec3 &dir,
    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
    float &t)
{
    // compute the plane's normal
    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    // no need to normalize
    glm::vec3 N = glm::cross(v0v1, v0v2);
    float area2 = glm::length(N);
 
    // Step 1: finding P

    // check if the ray and plane are parallel.
    float NdotRayDirection = glm::dot(N,dir); 
    if (fabs(NdotRayDirection) < kEpsilon) // almost 0
        return false; // they are parallel, so they don't intersect! 

    // compute d parameter using equation 2
    float d = -1 * glm::dot(N,v0); 
    
    // compute t (equation 3)
    t = -1 * (glm::dot(N,orig) + d) / NdotRayDirection;
    
    // check if the triangle is behind the ray
    if (t < 0) return false; // the triangle is behind
 
    // compute the intersection point using equation 1
    glm::vec3 P = orig + t * dir;
 
    // Step 2: inside-outside test
    glm::vec3 C; // vector perpendicular to triangle's plane
 
    // edge 0
    glm::vec3 edge0 = v1 - v0; 
    glm::vec3 vp0 = P - v0;
    C = glm::cross(edge0, vp0);
    if ( glm::dot(N,C) < 0) return false; // P is on the right side
 
    // edge 1
    glm::vec3 edge1 = v2 - v1; 
    glm::vec3 vp1 = P - v1;
    C = glm::cross(edge1, vp1);
    if ( glm::dot(N,C) < 0)  return false; // P is on the right side
 
    // edge 2
    glm::vec3 edge2 = v0 - v2; 
    glm::vec3 vp2 = P - v2;
    C = glm::cross(edge2, vp2);
    if (glm::dot(N,C) < 0) return false; // P is on the right side;

    return true; // this ray hits the triangle
}

bool Mesh::hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) {
	bool result = false;

	for (Triangle triangle : m_faces) {
		// std::cout << "ta" << std::endl;
		float temp_t = 0;
		if (rayTriangleIntersect( glm::vec3(eye.x, eye.y, eye.z), glm::vec3(ray_direction.x, ray_direction.y, ray_direction.z),
								m_vertices[triangle.v1],  m_vertices[triangle.v2],  m_vertices[triangle.v3], temp_t )) {
			// std::cout << " rayTriangleIntersect" << std::endl;
			*t = (double) temp_t;
			(*number_hit)++;
			*normal = glm::vec4(
				glm::cross(m_vertices[triangle.v1] - m_vertices[triangle.v2], m_vertices[triangle.v2] - m_vertices[triangle.v3]), 1.0f							
			);
			return true;
		}
	}

	return false;
}


std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
