#pragma once

#include "Scalar.hpp"

struct Camera {
    Camera(int width, int height) : width(width), height(height) {};

    virtual void updateView() = 0;
    virtual void updateProjection() = 0;
    virtual void setResolution(int width, int height) = 0;

    inline Mat4 getProjview()          const {return projection * view;}
    inline const Mat4& getProjection() const {return projection;}
    inline const Mat4& getView()       const {return view;}
    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}

    inline void translate(Vec3 delta) {position+=delta;}
    inline void set(Vec3 pos)         {position = pos;}
protected:
    const scalar REBASE_GRANULARITY = 1000;

    int width, height;

    Vec3 position = Vec3(0, 0, 0);

    Mat4 projection = Mat4(1.0f);
    Mat4 view       = Mat4(1.0f);
};

struct CameraOrtho : public Camera { 
    CameraOrtho(int width, int height, float minZ = -100.0f , float maxZ = 100.0f);

    void updateProjection() override;
    void updateView() override;

    void setResolution(int width, int height) override {};
    void setZoom(float zoom);
    void addZoom(float delta);
private:
    float minZ = -1.0f;
    float maxZ =  1.0f;
    float zoom =  1.0f;
};

struct CameraProspective : public Camera {
    CameraProspective(
        int width, 
        int height, 
        float fov,
        float min,
        float max);

    void updateProjection() override;
    void updateView() override;

    void rotate(scalar x, scalar y, scalar z);
    
    void setZoom(float zoom);
    void addZoom(float delta);
    void setResolution(int width, int height) override {};
    void setFov(scalar fov) {};

	inline const Vec3& xdir() const { return x_dir; }
	inline const Vec3& ydir() const { return y_dir; }
	inline const Vec3& zdir() const { return z_dir; }
private:
    inline void setxdir(const glm::dvec3& dir) { x_dir = dir; }
	inline void setydir(const glm::dvec3& dir) { y_dir = dir; }
	inline void setzdir(const glm::dvec3& dir) { z_dir = dir; }

    void updateVectors();

	Vec3 x_dir = Vec3(1.0f);
	Vec3 y_dir = Vec3(1.0f);
	Vec3 z_dir = Vec3(1.0f);

	float fov = 85.0f;
    float min = -1.0f;
    float max =  1.0f;
    float zoom = 1.0f;

	Mat4 rotation = Mat4(1.0f);
};