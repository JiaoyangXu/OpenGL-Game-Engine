// Termm--Fall 2020

#include "GeometryNode.hpp"
#include <glm/ext.hpp>


//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

bool GeometryNode::hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, 
						int* material_type,  Material** hit_material, int* number_hit) {
	glm::vec4 transformed_eye = invtrans * eye;
	glm::vec4 transformed_ray_direction = invtrans * ray_direction;

	bool result = false;

	for (SceneNode* child : children) {
		if (child->hit(transformed_eye, transformed_ray_direction, t, normal, material_type, hit_material, number_hit)) {
			result = true;
		}
	}

	if (m_primitive->hit(eye, ray_direction, t, normal, number_hit)) {
		//std::cout << "hit" << std::endl;
		*hit_material = m_material;
		result = true;
	}


	if (result) {
		*normal = trans * (*normal);
	}

	return result;
}
