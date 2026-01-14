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
#include "Matrix4D.h"
#include "Vector4D.h"
#include "LightNode.h"
#include "ray.h"
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
	
	void renderUI();

	void renderNano(NVGcontext * vg);

	Ray rayCast(double xpos, double ypos, Matrix4D view, Matrix4D projection, unsigned int screenW, unsigned int screnH, Vector4D camPos);

	Vector4D unproject(Vector4D& viewportPoint, Vector4D& viewportOrigin, Vector4D& viewportSize, Matrix4D& view, Matrix4D& projection);

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
};
} // namespace Example