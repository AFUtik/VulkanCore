#include "Camera.hpp"

// Camera Ortho

CameraOrtho::CameraOrtho(int width, int height, float minZ, float maxZ) : Camera(width, height), minZ(minZ), maxZ(maxZ) 
{
	updateProjection();						  
}

void CameraOrtho::updateView() {
	glm::vec2 snapped = glm::floor(position * zoom) / zoom;
	this->view        = glm::translate(glm::mat4(1.0f), -glm::vec3(snapped, 0.0f));
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

	zoom = std::fmax(zoom, 0.01f);

    glm::vec3 screenCenter = {
        width * 0.5f,
        height * 0.5f,
        0.0f
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
	x_dir = glm::vec3(rotation * glm::vec4(1, 0, 0, 1));
	y_dir = glm::vec3(rotation * glm::vec4(0, 1, 0, 1));
	z_dir = glm::vec3(rotation * glm::vec4(0, 0, -1, 1));
}

void CameraProspective::rotate(float x, float y, float z) {
	rotation = glm::mat4(1.0f);
	rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
	rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, x, glm::vec3(1, 0, 0));

	updateVectors();
}

