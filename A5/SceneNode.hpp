// Termm-Fall 2020

#pragma once

#include "Material.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/ObjFileDecoder.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include <vector>
#include "AABB.hpp"

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);

    void configure_AABB(const char * objFilePath);

    virtual void render(const ShaderProgram & shader,
                BatchInfoMap & batchInfoMap,
                const glm::mat4 & viewMatrix,
                std::vector<glm::mat4> & stack,
                bool is_shadow);



	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    static bool do_picking;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;
    AABB* m_AABB;

    SceneNode* parent;


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
