#pragma once

#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <cstdint>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Mesh.hpp"
#include "Texture.hpp"
#include <memory>

	class RenderService;

	struct PushConstantData {
		alignas(16) glm::mat4 model;
	};

	template<typename T>
	struct Transform3 {
		glm::dvec3 pos {0.0f};
		glm::quat  rot {1, 0, 0, 0};
		glm::dvec3 scl {1.0f};
		bool dirty = true;
		glm::mat4 mat{ 1.0f };
		using vec = glm::vec<3, T, glm::defaultp>;

		inline double getX() const {return mat[3].x;}
		inline double getY() const {return mat[3].y;}

		inline void translate(vec delta) {
			mat[3] += glm::vec4(delta.x, delta.y, delta.z, 0.0f);
		}

		inline void setPosition(vec npos) {
			mat[3] = glm::vec4(npos.x, npos.y, npos.z, 1.0f);
		}

		inline void rotateLocal (double radians, vec axis) {
			glm::quat q = glm::angleAxis((float)radians, glm::normalize(glm::vec3(axis)));
			rot = glm::normalize(q * rot);
			dirty = true;
		}

		inline void rotateGlobal(double radians, vec axis) {
			glm::quat q = glm::angleAxis((float)radians, glm::normalize(glm::vec3(axis)));
			rot = glm::normalize(rot * q);
			dirty = true;
		}

		inline void setScale(vec s)
		{
			scl = s;
			dirty = true;
		}

		inline const glm::mat4& model()
		{
			if (!dirty) return mat;

			const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
			const glm::mat4 R = glm::mat4_cast(rot);
			const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scl));

			mat = T * R * S;
			dirty = false;

			return mat;
		}

		inline const glm::mat4& matrix() const { return mat;}
	};

	struct Material {
		std::shared_ptr<Texture2D> albedo;
		TextureFilter albedoFilter = TextureFilter::Linear;
	};

	class MeshObject {
	public:
		Transform3<double> transform;
		
		MeshObject() {};
		~MeshObject();

		MeshObject(const MeshObject&) = delete;
		MeshObject& operator=(const MeshObject&) = delete;

		void setMesh(std::shared_ptr<Mesh> mesh);
		void setMaterial(std::shared_ptr<Material> material);

		inline void setScissor(uint32_t width, uint32_t height) {
			scissorWidth = width;
			scissorHeight = height;
		}

		void updateMesh();
		void updateMaterial();

		inline const Mesh* getMesh() const {return mesh.get();}
		inline const Material* getMaterial() const {return material.get();}

		inline uint32_t getScissorWidth() {return scissorWidth;}
		inline uint32_t getScissorHeight() {return scissorHeight;}

		inline void tryFree() {
			if(!keepAfterUpload) {
				mesh.reset();
				material.reset();
			}
		}

		inline uint32_t getRenderId() const {return renderId;}

		inline void freeAfterUpload() {keepAfterUpload = false;}
		inline void setRenderId(uint32_t handle) {renderId = handle;}
		inline void setRenderScene(RenderService* scene) {renderScene = scene;}
	private:
		bool keepAfterUpload = true;
		bool attached = false;

		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		
		RenderService* renderScene = nullptr;
		uint32_t renderId = std::numeric_limits<uint32_t>().max(); // INVALID HANDLE //

		uint32_t scissorWidth = 0;
		uint32_t scissorHeight = 0;
	};