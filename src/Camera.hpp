#pragma once

#include "glm/ext/vector_double3.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace glm;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;
};

struct Plane {
    glm::vec3 normal;
    float d;

    float distance(const glm::vec3& point) const {
        return glm::dot(normal, point) + d;
    }
};

class Frustum {
public:
    Plane planes[6];
    
    void update(const glm::mat4& m) {
        glm::vec4 row0 = glm::vec4(m[0][0], m[1][0], m[2][0], m[3][0]);
        glm::vec4 row1 = glm::vec4(m[0][1], m[1][1], m[2][1], m[3][1]);
        glm::vec4 row2 = glm::vec4(m[0][2], m[1][2], m[2][2], m[3][2]);
        glm::vec4 row3 = glm::vec4(m[0][3], m[1][3], m[2][3], m[3][3]);

        glm::vec4 left   = row3 + row0;
        glm::vec4 right  = row3 - row0;
        glm::vec4 bottom = row3 + row1;
        glm::vec4 top    = row3 - row1;
        glm::vec4 near   = row3 + row2;
        glm::vec4 far    = row3 - row2;

        auto setPlane = [&](int i, const glm::vec4& p) {
            planes[i].normal = glm::vec3(p.x, p.y, p.z);
            planes[i].d = p.w;
            float len = glm::length(planes[i].normal);
            planes[i].normal /= len;
            planes[i].d /= len;
        };

        setPlane(0, left);
        setPlane(1, right);
        setPlane(2, bottom);
        setPlane(3, top);
        setPlane(4, near);
        setPlane(5, far);
    }

    bool pointInFrustum(const glm::vec3& point) const {
        for (int i = 0; i < 6; i++) {
            if (planes[i].distance(point) < 0) return false;
        }
        return true;
    }

    bool boxInFrustum(const AABB &aabb) const {
        for (int i = 0; i < 6; i++) {
            glm::vec3 p = aabb.min;
            if (planes[i].normal.x >= 0) p.x = aabb.max.x;
            if (planes[i].normal.y >= 0) p.y = aabb.max.y;
            if (planes[i].normal.z >= 0) p.z = aabb.max.z;
            if (planes[i].distance(p) < -1e-4f) return false;
        }
        return true;
    }
};

class Camera {
	Frustum frustum;

	const double REBASE_GRANULARITY = 1000.0;

	void updateVectors();

	int width = 0;
	int height = 0;

	dvec3 x_dir;
	dvec3 y_dir;
	dvec3 z_dir;

	dvec3 originPosition;
	dvec3 offsetPosition;
	dvec3 accumulatedShift = dvec3(0.0);

	bool frustum_flag = true;
	bool rebased_dirty = true;

	float fov;
	mat4 rotation;
	mat4 view;
	mat4 proj;

	mat4 prospective;
    mat4 ortho;

	dvec3 getRebaseShift();
public:
	Camera(int width, int height, dvec3 position, float fov);

    inline void setWidth(int width) {this->width = width;}
    inline void setHeight(int height) {this->height = height;}

	//inline Frustum& getFrustum() { return frustum; }

	inline void setxdir(const glm::dvec3& dir) { x_dir = dir; }
	inline void setydir(const glm::dvec3& dir) { y_dir = dir; }
	inline void setzdir(const glm::dvec3& dir) { z_dir = dir; }

	inline const dvec3& xdir() { return x_dir; }
	inline const dvec3& ydir() { return y_dir; }
	inline const dvec3& zdir() { return z_dir; }

	inline void setRotation(const glm::mat4& rot) { rotation = rot; }

	inline dvec3& getPosition() { return originPosition; }

	void translate(const glm::dvec3& dp);
	void set(const glm::dvec3& p);
	void rotate(float x, float y, float z);

	void update();
    void clear();

	const mat4& getProjviewProspective();
    const mat4& getProjviewOrtho();

	vec3 getViewDir();

	inline const dvec3& getWorldShift() const { return accumulatedShift; };
	void originRebase();

	inline bool isRebased() { return rebased_dirty; }
};