// Termm-Fall 2020

#pragma once

#include "SceneNode.hpp"


class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

    virtual void render(const ShaderProgram & shader,
                        BatchInfoMap & batchInfoMap,
                        const glm::mat4 & viewMatrix,
                        std::vector<glm::mat4> & stack,
                        bool is_shadow);

    void updateShaderUniforms(
            const ShaderProgram & shader,
            const glm::mat4 & viewMatrix,
            std::vector<glm::mat4> & stack,
            bool is_shadow
    );


	Material material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
};
