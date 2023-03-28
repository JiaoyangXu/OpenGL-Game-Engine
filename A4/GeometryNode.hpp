// Termm--Fall 2020

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );

	virtual bool hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, 
					int* material_type,  Material** hit_material, int* number_hit) override;


	Material *m_material;
	Primitive *m_primitive;
};
