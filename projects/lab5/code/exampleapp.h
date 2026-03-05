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
typedef struct Interval {
	float max;
	float min;
} Interval;

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
	
	void renderUI(Vector4D& hitPoint, std::map<std::string, Vector4D>& hitResults, bool& AABBRenderState);

	void renderNano(NVGcontext * vg);

	void setupCube(unsigned int& VBO, unsigned int& VAO);

	void renderCube();

	Interval getInterval(const GraphicsNode& gn, const Vector4D axis);
	bool overlapingOnAxis(const GraphicsNode& gn, const GraphicsNode& gn2, const Vector4D& axis, float& depth, bool& isNegative);
	bool SATOnAABBs(const GraphicsNode& gn, const GraphicsNode& gn2);

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
	Camera cam;
	float deltaTime;
	float lastFrame;
	float currentFrame;
	int mousepress;
	int mousestatus;

	Matrix4D projection;
	int width = 0;
	int height = 0;
};
} // namespace Example