// Termm-Fall 2020

#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace std;


class AABB {
public:
      AABB(vector<glm::vec3> vertices);
      bool check_collision(const AABB* two);

      void calculate_temp_position(const glm::vec3& curr);

      void translate(const glm::vec3& trans);

      void force_translate(const glm::vec3& trans);

      glm::vec3 m_updated_position;
      glm::vec3 m_position;
      glm::vec3 m_max_position;
      glm::vec3 m_size;
};
