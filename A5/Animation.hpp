// Termm-Fall 2020

#pragma once


class Animation {
public:
      Animation(float velocity, float time_per_animation);
      float interpolated_transform(float time_per_frame);
private:
      float m_velocity;
      float m_time_per_animation;
      //unsigned int accumulated_time;
};
