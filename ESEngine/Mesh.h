#ifndef MESH_H
#define MESH_H

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <boost/foreach.hpp>

#include "ShaderManager.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Transform.h"
#include "Material.h"

#include "TextureBuffer.h"
#include "GLVertexArray.h"

#include "Context.h"
#include "Logger.h"

class Mesh : public Renderable {
public:
	Material material;
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	Mesh(std::shared_ptr<Shader> shader, int bufferUsage = GL_STATIC_DRAW) : bufferUsage(bufferUsage), Renderable(shader) {};
	Mesh(std::vector<Vertex> &vertices, std::vector<int> &indices, std::shared_ptr<Shader> shader, int vBufferSize = 0, int iBufferSize = 0, int bufferUsage = GL_STATIC_DRAW);
	Mesh(std::vector<Vertex> &vertices, std::vector<int> &indices, std::vector<std::shared_ptr<Shader>> shaders, int vBufferSize = 0, int iBufferSize = 0, int bufferUsage = GL_STATIC_DRAW);
	~Mesh();

	void draw(Renderer &renderer) override;
	void draw(Renderer &renderer, Shader *shader) override;

	void updateMesh();
	void showMeshWiring();
	void showNormalVisualisation();

	void hideMeshWiring();
	void hideNormalVisualisation();

protected:
	static Logger logger;

	bool initialized = false;
	int bufferUsage = GL_STATIC_DRAW;
	std::unique_ptr<VertexArray> vertexArray;

	std::shared_ptr<Shader> normalVisualisationShader;
	std::shared_ptr<Shader> meshWiringShader;

	bool isNormalsShaderEnabled = false;
	bool isMeshWiringShaderEnabled = false;

	void setupMeshes();
	void setupMesh(std::shared_ptr<Shader> shader);
	void addNewShader(std::shared_ptr<Shader> shader);

	Shader* getNormalsShader() {
		if (normalVisualisationShader == nullptr) {
			normalVisualisationShader = ShaderManager::getShader("Shaders/GeometryDebugShader.vert", "Shaders/GeometryDebugShader.frag", "Shaders/NormalVisualisationShader.geom");
		}
		return normalVisualisationShader.get();
	}

	Shader* getMeshWiringShader() {
		if (meshWiringShader == nullptr) {
			meshWiringShader = ShaderManager::getShader("Shaders/GeometryDebugShader.vert", "Shaders/GeometryDebugShader.frag", "Shaders/MeshWiringShader.geom");
		}
		return meshWiringShader.get();
	}

	int identifier() override { return 1; }
};

#endif