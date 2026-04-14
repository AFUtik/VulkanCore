#include "Camera.hpp"

// Camera Ortho

CameraOrtho::CameraOrtho(int width, int height, float minZ, float maxZ) : Camera(width, height), minZ(minZ), maxZ(maxZ) 
{
	updateProjection();						  
}

void CameraOrtho::updateView() {
	glm::vec2 snapped = glm::floor(position * zoom) / zoom;
	this->view = glm::translate(Mat4(1.0f), -glm::vec3(snapped, 0.0f));
}

void CameraOrtho::updateProjection() {
	this->projection = glm::ortho(0.0f, width / zoom,
           					      height / zoom, 0.0f,
           					      minZ, maxZ);
}

void CameraOrtho::setZoom(float zoom) {
	this->zoom = zoom;
	updateProjection();
}

void CameraOrtho::addZoom(float delta) {
	float oldZoom = zoom;
    zoom += delta;

    glm::vec3 screenCenter = {
        width * 0.5f,
        height * 0.5f, 0.0f
    };

    glm::vec3 before = position + screenCenter / oldZoom;
    glm::vec3 after  = position + screenCenter / zoom;

    position += (before - after);

    updateProjection();
    updateView();
}

// Camera Prospective

CameraProspective::CameraProspective(
        int width, 
        int height, 
        float fov,
        float min,
        float max) : Camera(width, height), fov(fov), min(min), max(max) 
{
	updateProjection();
};

void CameraProspective::updateProjection() {
	this->projection = glm::perspective(
		fov,
		((float)width / (float)height) * zoom,
		min,
		max
	);
}

void CameraProspective::updateView() {
	this->view = glm::lookAt(
		position,
		position + z_dir,
		y_dir
	);
}

void CameraProspective::setZoom(float zoom) {
	this->zoom = zoom;
	updateProjection();
}

void CameraProspective::addZoom(float delta) {
	this->zoom += delta;
	updateProjection();
}

void CameraProspective::updateVectors() {
	x_dir = Vec3(rotation * Vec4(1, 0, 0, 1));
	y_dir = Vec3(rotation * Vec4(0, 1, 0, 1));
	z_dir = Vec3(rotation * Vec4(0, 0, -1, 1));
}

void CameraProspective::rotate(scalar x, scalar y, scalar z) {
	rotation = Mat4(1.0f);
	rotation = glm::rotate(rotation, z, Vec3(0, 0, 1));
	rotation = glm::rotate(rotation, y, Vec3(0, 1, 0));
	rotation = glm::rotate(rotation, x, Vec3(1, 0, 0));

	updateVectors();
}

