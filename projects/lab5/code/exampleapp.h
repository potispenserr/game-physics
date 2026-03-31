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

typedef struct Line {
	Vector4D start;
	Vector4D end;
	Line() {

	}
	Line(const Vector4D& newStart, const Vector4D& newEnd) : start(newStart), end(newEnd) 
	{} 
} Line;

typedef struct CollisionManifold {
	std::vector<Vector4D> contactPoints;
	float depth;
	bool isColliding;
	Vector4D collisionNormal;
}; 

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

	Interval getInterval(GraphicsNode& gn, const Vector4D axis);
	bool overlapingOnAxis(GraphicsNode& gn, GraphicsNode& gn2, Vector4D& axis, float& depth, bool& isNegative);
	CollisionManifold SATOnAABBs(GraphicsNode& gn, GraphicsNode& gn2);
	std::vector<Line> getEdges(GraphicsNode& gn);
	std::vector<Vector4D> checkEdgesToAABB(const std::vector<Line>& edges, GraphicsNode& gn);
	std::vector<Plane> getPlanes(GraphicsNode& gn);
	bool checkEdgesToPlane(const Plane& plane, const Line& edge, Vector4D& resultPoint);
	bool pointInAABB(const Vector4D& point, GraphicsNode& gn);


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