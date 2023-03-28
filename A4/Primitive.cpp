// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <glm/ext.hpp>
#include <iostream>
#include <cstdlib>
#include <random>




std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1);

// cite: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection.html
class Ray
{
public:
    Ray(const glm::vec3 &orig, const glm::vec3 &dir) : orig(orig), dir(dir)
    {
        invdir = glm::vec3(1/dir.x, 1/dir.y, 1/dir.z);
        sign[0] = (invdir.x < 0);
        sign[1] = (invdir.y < 0);
        sign[2] = (invdir.z < 0);
    }
    glm::vec3 orig, dir; // ray orig and dir
    glm::vec3 invdir;
    int sign[3];
};

class AABBox
{
public:
    AABBox(const glm::vec3 &b0, const glm::vec3 &b1) { bounds[0] = b0, bounds[1] = b1; }
    bool intersect(const Ray &r, float &t) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
        tmax = (bounds[1-r.sign[0]].x - r.orig.x) * r.invdir.x;
        tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
        tymax = (bounds[1-r.sign[1]].y - r.orig.y) * r.invdir.y;

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
        tmin = tymin;
        if (tymax < tmax)
        tmax = tymax;

        tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
        tzmax = (bounds[1-r.sign[2]].z - r.orig.z) * r.invdir.z;

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
        tmin = tzmin;
        if (tzmax < tmax)
        tmax = tzmax;

        t = tmin;

        if (t < 0) {
            t = tmax;
            if (t < 0) return false;
        }

        return true;
    }
    glm::vec3 bounds[2];
};


Primitive::~Primitive()
{
}

bool Primitive::hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) {
    return false;
}

bool NonhierSphere::hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) {
    glm::vec4 a_c = eye - glm::vec4(m_pos, 1.0f);

    double A = glm::dot(ray_direction, ray_direction);
    double B = glm::dot(ray_direction, a_c) * 2;
    double C = glm::dot(a_c, a_c) - m_radius * m_radius;

    double roots[2];
    int m_roots = quadraticRoots(A,B,C,roots);

    if (m_roots == 0) {
        return false;
    } else if (m_roots == 1) {
        *t = roots[0];
    } else {
        *t = glm::min(roots[0], roots[1]);
    }

    glm::vec4 intersection_point = eye +  (float) (*t) * ray_direction;
    *normal = intersection_point - glm::vec4(m_pos, 1.0f);

    //std::cout << "NonhierSphere::hit -- intersection_point " << glm::to_string(*normal) << std::endl;
    (*number_hit)++;
    return true;
}

bool NonhierBox::hit(glm::vec4 eye, glm::vec4 ray_direction, double* t, glm::vec4* normal, int* number_hit) {
    AABBox box(m_pos + glm::vec3(0.0f, 0.0f, 0.0f), m_pos + glm::vec3(m_size, m_size, m_size));
    float temp_t = 0;
    Ray ray (glm::vec3(eye.x, eye.y, eye.z), glm::vec3(ray_direction.x, ray_direction.y, ray_direction.z) );
    if (box.intersect(ray, temp_t)) {
        number_hit++;
        *t = (double) temp_t;
        glm::vec4 intersection_point = eye +  (float) (*t) * ray_direction;
        *normal = intersection_point - glm::vec4(m_pos, 1.0f);
        return true;
    }

    return false;
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}


Sphere::Sphere(): NonhierSphere(glm::dvec3(0.0f),1.0f)
{
}


Cube::Cube() : NonhierBox(glm::dvec3(0.0f),1.0f)
{
}
