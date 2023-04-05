// Termm-Fall 2020

#include "GeometryNode.hpp"

#include "cs488-framework/MathUtils.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>



using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}


void GeometryNode::render(const ShaderProgram & shader,
                       BatchInfoMap & batchInfoMap,
                       const glm::mat4 & viewMatrix,
                       std::vector<glm::mat4> & stack,
                       bool is_shadow) {

    //cout << "GeometryNode::render" << endl;

    //stack.push_back(trans);

    for (SceneNode* node : children) {
        node->render(shader, batchInfoMap, viewMatrix, stack, is_shadow);
    }

    if (m_nodeType != NodeType::GeometryNode)
        return;


    updateShaderUniforms(shader,viewMatrix,stack, is_shadow );


    // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
    BatchInfo batchInfo = batchInfoMap[meshId];

    //-- Now render the mesh:
    shader.enable();
    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
    shader.disable();

    //stack.pop_back();
}

void GeometryNode::updateShaderUniforms(const ShaderProgram &shader, const glm::mat4 &viewMatrix, std::vector<glm::mat4>  &stack, bool is_shadow) {
    shader.enable();
    {
        //-- Set ModelView matrix:
        GLint location = shader.getUniformLocation("ModelView");
        mat4 modelView = viewMatrix * trans;
        for (auto i : stack) {
            modelView = i * modelView;
        }
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
        CHECK_GL_ERRORS;

    if (!is_shadow) {
        if( do_picking ) {
           // cout << "false colour" <<endl;
            float r = float(m_nodeId & 0xff) / 255.0f;
            float g = float((m_nodeId>>8) & 0xff) / 255.0f;
            float b = float((m_nodeId>>16) & 0xff) / 255.0f;

            location = shader.getUniformLocation("material.kd");
            glUniform3f( location, r, g, b );
            CHECK_GL_ERRORS;
        } else {
            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;


            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = material.kd;
            if (isSelected) {
                kd = {1,1,0};
            }
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
        }

    }
        
    }
    shader.disable();
}
