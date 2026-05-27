#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2018 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include "mesh.h"
#include "TextureResource.h"
#include "GraphicsNode.h"
#include "ShaderObject.h"
#include "Camera.h"
#include "Vector4D.h"
#include "Matrix4D.h"
#include "LightNode.h"
#include "ray.h"
#include "CollisionManifold.h"
#include "PhysicsSystem.h"
#include "RigidBodyVolume.h"


namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open();
	/// run app
	void Run();
	private:
	
	void renderUI(Vector4D& hitPoint, std::map<std::string, Vector4D>& hitResults, bool& AABBRenderState, const CollisionManifold& cm);

	void renderNano(NVGcontext * vg);

	void setupCube(unsigned int& VBO, unsigned int& VAO);

	void renderCube();

	void setupBodies();


	GLuint program;
	GLuint vertexShader;
	GLuint pixelShader;
	GLuint triangle;
	Display::Window* window;
	MeshResource mesh;
	TextureResource tex;
	TextureResource tex2;
	GraphicsNode gn;
	GraphicsNode gn2;
	std::vector<GraphicsNode> gnList;
	Camera cam;
	float deltaTime;
	float lastFrame;
	float currentFrame;
	int mousepress;
	int mousestatus;
	PhysicsSystem physics;
	std::vector<RigidBodyVolume> bodyVolumes;


	Matrix4D projection;
	int width = 0;
	int height = 0;
};
} // namespace Example