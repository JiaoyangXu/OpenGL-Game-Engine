// Termm-Fall 2020

#pragma once


#include "Material.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>
#include <vector>


class Shadow {
public:
      Shadow(const ShaderProgram& shader);

      unsigned int initShadowMap();
      glm::mat4 initLightSpaceMatrix();
private:
      ShaderProgram m_shader;
};
