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

class RenderSystem;

namespace myvk {
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

		inline glm::mat4& model()
		{
			if (!dirty) return mat;

			const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
			const glm::mat4 R = glm::mat4_cast(rot);
			const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scl));

			mat = T * R * S;
			dirty = false;

			return mat;
		}
	};

	struct Material {
		std::shared_ptr<Texture2D> albedo;
	};

	class MeshObject {
	public:
		uint32_t objectId;

		Transform3<double> transform;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		MeshObject() {};
		~MeshObject() {};

		MeshObject(const MeshObject&) = delete;
		MeshObject& operator=(const MeshObject&) = delete;
	};
}