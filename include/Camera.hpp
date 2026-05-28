#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    Camera(int width, int height) : width(width), height(height) {};

    virtual void updateView() = 0;
    virtual void updateProjection() = 0;
    virtual void setResolution(int width, int height) = 0;

    inline glm::mat4 getProjview()          const {return projection * view;}
    inline const glm::mat4& getProjection() const {return projection;}
    inline const glm::mat4& getView()       const {return view;}
    inline int getWidth() const {return width;}
    inline int getHeight() const {return height;}

    inline void translate(glm::vec3 delta) {position+=delta;}
    inline void set(glm::vec3 pos)         {position = pos;}
protected:
    const int REBASE_GRANULARITY = 1000;

    int width, height;

    glm::vec3 position   = glm::vec3(0, 0, 0);

    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view       = glm::mat4(1.0f);
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

    void rotate(float x, float y, float z);
    
    void setZoom(float zoom);
    void addZoom(float delta);
    void setResolution(int width, int height) override {};
    void setFov(float fov) {};

	inline const glm::vec3& xdir() const { return x_dir; }
	inline const glm::vec3& ydir() const { return y_dir; }
	inline const glm::vec3& zdir() const { return z_dir; }
private:
    inline void setxdir(const glm::vec3& dir) { x_dir = dir; }
	inline void setydir(const glm::vec3& dir) { y_dir = dir; }
	inline void setzdir(const glm::vec3& dir) { z_dir = dir; }

    void updateVectors();

	glm::vec3 x_dir = glm::vec3(1.0f);
	glm::vec3 y_dir = glm::vec3(1.0f);
	glm::vec3 z_dir = glm::vec3(1.0f);

	float fov = 85.0f;
    float min = -1.0f;
    float max =  1.0f;
    float zoom = 1.0f;

	glm::mat4 rotation = glm::mat4(1.0f);
};