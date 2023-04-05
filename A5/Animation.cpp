#include "Animation.hpp"

Animation::Animation(float velocity, float time_per_animation) {
      m_velocity = velocity;
      m_time_per_animation = time_per_animation;
}

float Animation::interpolated_transform(float time_per_frame) {
      float change = time_per_frame / m_time_per_animation;
      float updated_velocity = m_velocity * (1 - change);
      return updated_velocity;
}