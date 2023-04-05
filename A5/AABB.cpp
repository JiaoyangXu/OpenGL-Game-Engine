#include "AABB.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>

using namespace std;
using namespace glm;


void AABB::calculate_temp_position(const vec3& curr) {
      m_position.x = (m_position.x > curr.x)? curr.x : m_position.x;
      m_position.y = (m_position.y > curr.y)? curr.y : m_position.y;
      m_position.z = (m_position.z > curr.z)? curr.z : m_position.z;

      m_max_position.x = (m_max_position.x < curr.x)? curr.x : m_max_position.x;
      m_max_position.y = (m_max_position.y < curr.y)? curr.y : m_max_position.y;
      m_max_position.z = (m_max_position.z < curr.z)? curr.z : m_max_position.z;
}

bool AABB::check_collision(const AABB* two) {
      bool collisionX = m_updated_position.x + m_size.x >= two->m_updated_position.x &&
            two->m_updated_position.x + two->m_size.x >= m_updated_position.x;
      bool collisionY = m_updated_position.y + m_size.y >= two->m_updated_position.y &&
            two->m_updated_position.y + two->m_size.y >= m_updated_position.y;
      
      return collisionX && collisionY;
}

void AABB::translate(const glm::vec3& trans) {
      m_updated_position.x = m_position.x + trans.x;
      m_updated_position.y = m_position.y + trans.y;
      m_updated_position.z = m_position.z + trans.z;
}

void AABB::force_translate(const glm::vec3& trans) {
      m_position.x = m_position.x + trans.x;
      m_position.y = m_position.y + trans.y;
      m_position.z = m_position.z + trans.z;
      std::cout << to_string(m_position) << std::endl;
}

AABB::AABB(vector<vec3> vertices) {
      if (vertices.size() == 0 ) {
            return;
      }
      m_position = vertices.at(0);
      m_max_position = vertices.at(0);
      
      for (vec3 vertex : vertices) {
            calculate_temp_position(vertex);
      }
      m_size.x = m_max_position.x - m_position.x;
      m_size.y = m_max_position.y - m_position.y;
      m_size.z = m_max_position.z - m_position.z;
      
      m_updated_position = m_position;

      std::cout << to_string(m_position) << std::endl;

}