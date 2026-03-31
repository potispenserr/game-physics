//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "plane.h"
#include "ray.h"

// HackFIX1337boi
#ifndef strncpy_s

typedef int errno_t;
typedef size_t rsize_t;

errno_t strncpy_s(char *__restrict__dest, rsize_t destsz,
                            const char *__restrict__src, rsize_t count)
{
    return !strncpy(__restrict__dest, __restrict__src, (destsz>count)?count:destsz);
}

#endif

const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec2 texCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view *  model * vec4(pos, 1.0);\n"
"   TexCoord = texCoord;\n"
"}\n";

const GLchar* ps =
"#version 430\n"
"out vec4 FragColor;\n"
"in vec4 color;\n"
"in vec2 TexCoord;"
"uniform sampler2D texture1;\n"
"uniform sampler2D texture2;\n"
"void main()\n"
"{\n"
"	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);\n"
"}\n";

using namespace Display;
namespace Example
{

	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::~ExampleApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	bool
		ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;
		window->SetKeyPressFunction([this](int32 asciikey, int32 argb, int32 status, int32 mod)
		{
			
		});

		

		GLfloat buf[] =
		{
			-0.5f,	-0.5f,	-1,			// pos 0
			1,		0,		0,		1,	// color 0
			0,		0.5f,	-1,			// pos 1
			0,		1,		0,		1,	// color 0
			0.5f,	-0.5f,	-1,			// pos 2
			0,		0,		1,		1	// color 0
		};
		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			
		this->window->SetNanoVGRender([this](NVGcontext * vg)
		{
			this->renderNano(vg);
		});
			
		}
		this->window->InitializeImGUI();
		return true;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void
		ExampleApp::Run()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//Uncomment the line below if you're not in a VM and then you will have an easier time moving the camera
		//window->SetInputMode(GLFW_CURSOR_DISABLED);

		float radFov = 45 * (PI / 180);

		projection = projection.perspective(radFov, 800.0f / 600.0f, 0.1f, 100.0f);

		cam.camPos = Vector4D(0.0f, 0.0f, 3.0f);
		cam.camTarget = Vector4D(0.0f, 0.0f, 0.0f);
		cam.camFront = Vector4D(0.0f, 0.0f, -1.0f);
		cam.camUp = Vector4D(0.0f, 1.0f, 0.0f);

		Vector4D lightPosition(-14.2f, 1.0f, 2.0f);

		
		GraphicsNode lightCube;

		std::shared_ptr<MeshResource> pointLightMesh = std::make_shared<MeshResource>();
		std::shared_ptr<TextureResource> lightTexPtr = std::make_shared<TextureResource>();
		std::shared_ptr<ShaderObject> pointLightShader = std::make_shared<ShaderObject>("./resources/shaders/LightCubeVS.vs", "./resources/shaders/LightCubeFS.fs");


		std::shared_ptr<MeshResource> objectMesh = std::make_shared<MeshResource>();
		std::shared_ptr<ShaderObject> lightShader = std::make_shared<ShaderObject>("./resources/shaders/NormalMappingVS.vs", "./resources/shaders/NormalMappingFS.fs");
		std::shared_ptr<TextureResource> texPtr = std::make_shared<TextureResource>();
		std::shared_ptr<TextureResource> normalMapPtr = std::make_shared<TextureResource>();

		std::shared_ptr<ShaderObject> rayShader = std::make_shared<ShaderObject>("./resources/shaders/RayShaderVS.vs", "./resources/shaders/RayShaderFS.fs");
		std::shared_ptr<ShaderObject> squareShader = std::make_shared<ShaderObject>("./resources/shaders/squareShaderVS.vs", "./resources/shaders/squareShaderFS.fs");


		pointLightMesh->loadObj("./resources/cube2.obj");
		objectMesh->loadObj("./resources/cube2.obj");

		lightCube.setMesh(pointLightMesh);
		lightCube.setShader(pointLightShader);
		lightCube.setTexture(lightTexPtr);
		lightCube.initTexture("./resources/container45.jpg");
		lightCube.setTransform(Matrix4D());


		gn.setMesh(objectMesh);
		gn.setShader(lightShader);
		gn.setAABBShader(rayShader);
		gn.setTexture(texPtr);
		//gn.setNormalMap(normalMapPtr);
		gn.initTexture("./resources/container45.jpg");
		gn.setTransform(Matrix4D());

		gn2.setMesh(objectMesh);
		gn2.setShader(lightShader);
		gn2.setAABBShader(rayShader);
		gn2.setTexture(texPtr);
		gn2.setTransform(Matrix4D());

		//Matrix4D gnTransform = Matrix4D::scale(Vector4D(0.5, 0.5, 0.5));
		Matrix4D gnTransform = gnTransform * Matrix4D::translation(Vector4D(2.0f, 0.0f, 0.0f));

		gn.setTransform(gnTransform);

		Matrix4D gn2Transform = Matrix4D::translation(Vector4D(2.0f, 1.0f, 1.5f));
		gn2.setTransform(gn2Transform);

		std::vector<GraphicsNode> gnList;
		gnList.push_back(gn);
		gnList.push_back(gn2);

		std::cout << "GN1 Max:" << gnList[0].maxBounds.x() << " " << gnList[0].maxBounds.y() << " " << gnList[0].maxBounds.z() << "\n";
		std::cout << "GN2 Max:" << gnList[1].maxBounds.x() << " " << gnList[1].maxBounds.y() << " " << gnList[1].maxBounds.z() << "\n";

		std::cout << "GN1 Min:" << gnList[0].minBounds.x() << " " << gnList[0].minBounds.y() << " " << gnList[0].minBounds.z() << "\n";
		std::cout << "GN2 Min:" << gnList[1].minBounds.x() << " " << gnList[1].minBounds.y() << " " << gnList[1].minBounds.z() << "\n";
		
		// gn.updateTransform(Matrix4D::scale(Vector4D(0.5, 0.5, 0.5)));
		// gn.updateTransform(Matrix4D::translation(Vector4D(2.0f, 0.0f, 0.0f)));

		//gn.updateTransform(Matrix4D::roty(160));
		// gn2.updateTransform(Matrix4D::scale(Vector4D(0.3, 0.3, 0.3)));
		// gn2.updateTransform(Matrix4D::translation(Vector4D(2.0f, 2.0f, 2.0f)));

		//rest plane
		std::vector<Vector4D> planePoints;
		planePoints.push_back(Vector4D(0.5f,  0.5f, 0.0f));
		planePoints.push_back(Vector4D(0.5f, -0.5f, 0.0f));
		planePoints.push_back(Vector4D(-0.5f, -0.5f, 0.0f));

		std::vector<Plane> planes;
		
		Matrix4D squareTransform;
		
		Vector4D squareColor = {0.5, 0.5, 0.5, 1};
		
		
		Plane testPlane(planePoints);

		//test ray
		std::vector<Ray> rays;
		Ray testRay(testPlane.getNormal(), testPlane.getNormal() * -50);
		testRay.rayColor = Vector4D(1.0f, 1.0f, 1.0f);
		//rays.push_back(testRay);
		//rays.push_back(Ray(Vector4D(0.0f, 0.0f, 0.0f), Vector4D(0.0f, 5.0f, 5.0f)));

		unsigned int cubeVBO;
		unsigned int cubeVAO;
		Matrix4D cubeTransform;
		setupCube(cubeVBO, cubeVAO);


		LightNode light = LightNode(lightShader);
		light.lightColor = Vector4D(1.0f, 1.0f, 1.0f);
		light.lightPos = lightPosition;
		light.intensity = 0.1;

		light.setupLighting();


		this->window->GetSize(width, height);
		//std::cout << "width: " << width << " height: " << height << "\n";

		float yaw = -90;
		float pitch = 0;

		float lastX = 512.0f;
		float lastY = 384.0f;
		bool firstRotation = true;

		std::map<std::string, Vector4D> hitResults;

		Vector4D squareHit = {50.0f, 0.0f};
		
		Vector4D hitVisualizerPosition;

		std::vector<GraphicsNode> collisionList;

		CollisionManifold collisionManifold;

		bool AABBRenderState = true;
		

		//render loop
		while (this->window->IsOpen())
		{

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;	
			lastFrame = currentFrame;

			for(int i = 0; i < gnList.size(); i++){
				gnList[i].AABBRenderState = AABBRenderState;
			}

			this->renderUI(squareHit, hitResults, AABBRenderState, collisionManifold);

			for(int i = 1; i < gnList.size(); i++){
				// check if collisions are happening on the AABBs
				if(gnList[i - 1].maxBounds.x() > gnList[i].minBounds.x() && gnList[i - 1].maxBounds.x() < gnList[i].maxBounds.x() ||
					gnList[i - 1].minBounds.x() > gnList[i].minBounds.x() && gnList[i - 1].minBounds.x() < gnList[i].maxBounds.x() ||
					gnList[i - 1].maxBounds.y() > gnList[i].minBounds.y() && gnList[i - 1].maxBounds.y() < gnList[i].maxBounds.y() ||
					gnList[i - 1].minBounds.y() > gnList[i].minBounds.y() && gnList[i - 1].minBounds.y() < gnList[i].maxBounds.y() ||
					gnList[i - 1].maxBounds.z() > gnList[i].minBounds.z() && gnList[i - 1].maxBounds.z() < gnList[i].maxBounds.z() ||
					gnList[i - 1].minBounds.z() > gnList[i].minBounds.z() && gnList[i - 1].minBounds.z() < gnList[i].maxBounds.z())
				{
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					//std::cout << "Collision maybe" << "\n";
					collisionManifold = SATOnAABBs(gnList[i - 1], gnList[i]);
					//std::cout << "Collision on AABBS is " << collisionManifold.isColliding << " frfr no cap" << "\n";
					if(collisionManifold.isColliding == true){
						gnList[i - 1].setAABBColor(Vector4D(1.0f, 0.0f, 0.0f));
						gnList[i].setAABBColor(Vector4D(1.0f, 0.0f, 0.0f));

					}
					else {
						gnList[i - 1].setAABBColor(Vector4D(1.0f, 1.0f, 1.0f));
						gnList[i].setAABBColor(Vector4D(1.0f, 1.0f, 1.0f));

					}
					
				}
				else{
					gnList[i - 1].setAABBColor(Vector4D(1.0f, 1.0f, 1.0f));
					gnList[i].setAABBColor(Vector4D(1.0f, 1.0f, 1.0f));
				}
			}

			window->SetKeyPressFunction([this, &light, &gnList](int32 asciikey, int32 argb, int32 status, int32 mod)
			{
				//std::cout << "asciikey: " << asciikey << " argb: " << argb << " status: " << status << " mod: " << mod << "\n";

				const float camSpeed = 5.0f * deltaTime;

				if (status == 1) {
					switch (asciikey) {
					case GLFW_KEY_W:
						cam.camPos = cam.camPos + cam.camFront * camSpeed;
						break;
					case GLFW_KEY_A:
						cam.camPos = cam.camPos - Vector4D::cross(cam.camFront, cam.camUp).norm() * camSpeed;
						break;
					case GLFW_KEY_S:
						cam.camPos = cam.camPos - cam.camFront * camSpeed;
						break;
					case GLFW_KEY_D:
						cam.camPos = cam.camPos + Vector4D::cross(cam.camFront, cam.camUp).norm() * camSpeed;
						break;
					case GLFW_KEY_J:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(-0.5f, 0.0f, 0.0f)));
						break;
					case GLFW_KEY_K:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.0f, 0.5f)));
						break;
					case GLFW_KEY_L:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.5f, 0.0f, 0.0f)));
						break;
					case GLFW_KEY_I:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.0f, -0.5f)));
						break;
					case GLFW_KEY_U:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, -0.5f, 0.0f)));
						break;
					case GLFW_KEY_O:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.5f, 0.0f)));
						break;

					case GLFW_KEY_Q:
						cam.camPos = cam.camPos + Vector4D(0.0f, 1.0f, 0.0f) * camSpeed;
						break;
					case GLFW_KEY_E:
						cam.camPos = cam.camPos + Vector4D(0.0f, -1.0f, 0.0f) * camSpeed;
						break;

					}


					
				}
				else if (status == 2) {
					switch (asciikey) {
					case GLFW_KEY_W:
						cam.camPos = cam.camPos + cam.camFront * camSpeed;
						break;
					case GLFW_KEY_A:
						cam.camPos = cam.camPos - Vector4D::cross(cam.camFront, cam.camUp).norm() * camSpeed;
						break;
					case GLFW_KEY_S:
						cam.camPos = cam.camPos - cam.camFront * camSpeed;
						break;

					case GLFW_KEY_D:
						cam.camPos = cam.camPos + Vector4D::cross(cam.camFront, cam.camUp).norm() * camSpeed;
						break;
					case GLFW_KEY_J:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(-0.5f, 0.0f, 0.0f)));
						break;
					case GLFW_KEY_K:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.0f, 0.5f)));
						break;
					case GLFW_KEY_L:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.5f, 0.0f, 0.0f)));
						break;
					case GLFW_KEY_I:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.0f, -0.5f)));
						break;
					case GLFW_KEY_U:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, -0.5f, 0.0f)));
						break;
					case GLFW_KEY_O:
						gnList[1].updateTransform(Matrix4D::translation(Vector4D(0.0f, 0.5f, 0.0f)));
						break;

					case GLFW_KEY_Q:
						cam.camPos = cam.camPos + Vector4D(0.0f, 1.0f, 0.0f) * camSpeed;
						break;
					case GLFW_KEY_E:
						cam.camPos = cam.camPos + Vector4D(0.0f, -1.0f, 0.0f) * camSpeed;
						break;
					
					}


				}

				if (asciikey == 256 && status == 1) {
					this->window->Close();
				}
			});
			

			window->SetMousePressFunction([&](int32 mousekey, int32 status, int32 keyc) {
				std::cout << "mousekey: " << mousekey << " status: " << status << " keyc: " << keyc << "\n";
				mousepress = mousekey;
				mousestatus = status;
				if(mousepress == 1 && status == 1) {
					double mouseX;
					double mouseY;
					window->GetCursorPosition(&mouseX, &mouseY);
					std::cout << "MouseX:" << mouseX << " MouseY: " << mouseY << "\n";
					hitResults.clear(); 

					Ray rayWorld;
					rayWorld.rayCast(mouseX, mouseY, cam.getView(), projection, width, height);
					//std::cout << "x: " << rayWorld.x() << "y: " << rayWorld.y() << "z: " << rayWorld.z() << "\n";
					//Ray newRay = Ray(Vector4D(rayWorld.x(), rayWorld.y() , rayWorld.z()), Vector4D(rayWorld.x() * 5.0f, rayWorld.y() * 5.0f, rayWorld.z() * 5.0f));
    				std::cout << "x: " << rayWorld.getRayDir().x() << "y: " << rayWorld.getRayDir().y() << "z: " << rayWorld.getRayDir().z() << "\n";

					std::srand(time(0));
					const float randomColorR = (std::rand() % 255) / 255.0f;
					const float randomColorG = (std::rand() % 255) / 255.0f;
					const float randomColorB = (std::rand() % 255) / 255.0f;
					rayWorld.rayColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
					Vector4D hitPoint;
					bool isSquareHit = rayWorld.Intersect(testPlane, hitPoint);
					if(isSquareHit){
						hitResults.insert({"Square", hitPoint});
						rayWorld.rayColor = Vector4D(1.0f, 0.0f, 0.0f, 1.0f);

					}
					Vector4D gnHitPoint;
					bool gnHit = false;
					gnHit = rayWorld.Intersect(gnList[0], gnHitPoint);
					if(gnHit == true){
						// std::cout << "gn hit" << "\n";
						// std::cout << gnHitPoint.x() << " " << gnHitPoint.y() << " " << gnHitPoint.z() << "\n";
						
						hitResults.insert({"GN", gnHitPoint});
						rayWorld.rayColor = Vector4D(1.0f, 0.0f, 0.0f, 1.0f);

					}

					if(hitResults.size() > 1){
						auto it = hitResults.begin();
						auto nearestCam = std::make_pair(it->first, it->second);
						for (it; it != hitResults.end(); ++it) {
							std::cout << it->first << ", " << it->second.x() << " " << it->second.y() << "" << it->second.z() << '\n';
							if(nearestCam.second.z() < it->second.z()){
								nearestCam = std::make_pair(it->first, it->second);
							}
						}
						std::cout << "The thing nearest cam was " << nearestCam.first << "\n";
						hitResults.clear();
						hitResults.insert(nearestCam);
						hitVisualizerPosition = nearestCam.second;
						//turn intersecting ray red
						rayWorld.rayColor = Vector4D(1.0f, 0.0f, 0.0f, 1.0f);
						
					}
					
					if(hitResults.find("Square") != hitResults.end()){
						std::cout << "ray hit inside the square" << "\n";
						hitVisualizerPosition = hitResults["Square"];
						squareColor = Vector4D(0.0f, 0.1f, 0.0f, 1);
					}
					
					if(hitResults.find("GN") != hitResults.end()){
						std::cout << "ray hit GN" << "\n";
						hitVisualizerPosition = hitResults["GN"];
						
					}
					if(gnHit == false && isSquareHit == false){
						rayWorld.rayColor = Vector4D(1.0f, 1.0f, 1.0f, 1.0f);
					}
					rays.push_back(rayWorld);
				}
				});

			window->SetMouseMoveFunction([&](float64 mousex, float64 mousey) {

				if (mousepress == 0 && mousestatus == 1) {


					if (firstRotation) {
						lastX = mousex;
						lastY = mousey;
						firstRotation = false;
					}


					float xoffset = mousex - lastX;
					float yoffset = lastY - mousey; 
					lastX = mousex;
					lastY = mousey;
					const float speed = 0.1f;
					xoffset *= speed;
					yoffset *= speed;

					yaw -= xoffset;
					pitch -= yoffset;
					

					if (pitch > 89.0f) {
						pitch = 89.0f;
					}
					if (pitch < -89.0f) {
						pitch = -89.0f;
					}

					Vector4D direction;
					float degrad = PI / 180;
					direction.x() = -cos(yaw * degrad) * cos(pitch * degrad);
					direction.y() = -sin(pitch * degrad);
					direction.z() = sin(yaw * degrad) * cos(pitch * degrad);

					cam.camFront = direction.norm();

				}
				
				});
			cam.setView();
			light.updateLighting(cam, projection, lightCube);
			for(GraphicsNode gNode : gnList){
				gNode.draw(cam, projection, light.lightPos);
				
			}
			//gn.draw(cam, projection, light.lightPos);
			lightCube.draw(cam, projection, light.lightPos);
			//gn2.draw(cam, projection, light.lightPos);
			


			//squareTransform = squareTransform.translation(testPlane.getPoint(0));
			

			squareShader.get()->use();
			squareShader.get()->setMat4(std::string("model"), squareTransform);
			squareShader.get()->setMat4(std::string("view"), cam.getView());
			squareShader.get()->setMat4(std::string("projection"), projection);
			squareShader.get()->setVec4(std::string("color"), squareColor);
			testPlane.draw();
			

			Matrix4D rayTransform;

			rayShader.get()->use();
			rayShader.get()->setMat4(std::string("model"), rayTransform);
			rayShader.get()->setMat4(std::string("view"), cam.getView());
			rayShader.get()->setMat4(std::string("projection"), projection);
			for(Ray ray : rays){
				rayShader.get()->setVec4(std::string("rayColor"), ray.rayColor);
				ray.draw();
			}
			
			// draw intersection point cube
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
			cubeTransform = Matrix4D::scale(Vector4D(0.1, 0.1, 0.1));
			Matrix4D newTrans = cubeTransform.translation(hitVisualizerPosition);
			cubeTransform = cubeTransform * newTrans;
			pointLightShader.get()->use();
			pointLightShader.get()->setMat4(std::string("model"), cubeTransform);
			pointLightShader.get()->setMat4(std::string("view"), cam.getView());
			pointLightShader.get()->setMat4(std::string("projection"), projection);
			glBindVertexArray(cubeVAO);
        	glDrawArrays(GL_TRIANGLES, 0, 36);
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

			if(collisionManifold.isColliding == true){
				Vector4D cubeColor = {1.0f, 1.0f, 1.0f};
				for(int i = 0; i < collisionManifold.contactPoints.size(); i++){
					cubeTransform.reset();
					cubeTransform = Matrix4D::scale(Vector4D(0.1, 0.1, 0.1));
					newTrans = cubeTransform.translation(collisionManifold.contactPoints[i]);
					cubeTransform = cubeTransform * newTrans;
					//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
					rayShader.get()->use();
					rayShader.get()->setMat4(std::string("model"), cubeTransform);
					rayShader.get()->setMat4(std::string("view"), cam.getView());
					rayShader.get()->setMat4(std::string("projection"), projection);
					rayShader.get()->setVec4(std::string("rayColor"), cubeColor);

					glBindVertexArray(cubeVAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
					// glBegin(GL_POINTS);
					// glVertex3f(collisionManifold.contactPoints[i].x(), collisionManifold.contactPoints[i].y(), collisionManifold.contactPoints[i].z());
					// glEnd();

					Ray normalRay = {Vector4D(0.0f, 0.0f, 0.0f), collisionManifold.collisionNormal * 5};
					normalRay.rayColor = Vector4D(1.0f, 1.0f, 1.0f);
					rayShader.get()->use();
					rayShader.get()->setMat4(std::string("model"), rayTransform);
					rayShader.get()->setMat4(std::string("view"), cam.getView());
					rayShader.get()->setMat4(std::string("projection"), projection);
					rayShader.get()->setVec4(std::string("rayColor"), normalRay.rayColor);
					normalRay.draw();

				}
			}

			//debug cubes
			Vector4D cubeColor;
			for(int i = 0; i < gnList.size(); i++){
				for(int j = 0; j < 3; j++){
					cubeTransform.reset();
					cubeTransform = Matrix4D::scale(Vector4D(0.1, 0.1, 0.1));
					if(j == 0){
						newTrans = cubeTransform.translation(gnList[i].minBounds);
						cubeColor = {0.0f, 1.0f, 0.0f};	
					}
					else if (j == 1){
						newTrans = cubeTransform.translation(gnList[i].maxBounds);
						cubeColor = {0.0f, 0.0f, 1.0f};
					}
					else if (j == 2){
						newTrans = cubeTransform.translation(gnList[i].AABBCenter);
						cubeColor = {1.0f, 0.0f, 0.0f};
					}
					cubeTransform = cubeTransform * newTrans;
					glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
					rayShader.get()->use();
					rayShader.get()->setMat4(std::string("model"), cubeTransform);
					rayShader.get()->setMat4(std::string("view"), cam.getView());
					rayShader.get()->setMat4(std::string("projection"), projection);
					rayShader.get()->setVec4(std::string("rayColor"), cubeColor);
	
					glBindVertexArray(cubeVAO);
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	
				}

			}

			//debug edges
			rayShader.get()->use();
			rayShader.get()->setMat4(std::string("view"), cam.getView());
			rayShader.get()->setMat4(std::string("projection"), projection);
			rayShader.get()->setVec4(std::string("rayColor"), Vector4D(0.75f, 0.1f, 0.36f));
			for(int i = 0; i < gnList.size(); i++){
				rayShader.get()->setMat4(std::string("model"), Matrix4D());
				glBegin(GL_LINES);
				std::vector<Line> edges = getEdges(gnList[i]);
				for(int j = 0; j < edges.size(); j++){
					Vector4D p1 = edges[j].start;
					Vector4D p2 = edges[j].end;
					glVertex3f(p1.x(), p1.y(), p1.z());
					glVertex3f(p2.x(), p2.y(), p2.z());
				}
				glEnd();

			}



			ImGui::Render();
			collisionList.clear();
			collisionManifold.contactPoints.clear();
			


			///     _             _          __  __ 
			///    | |           | |        / _|/ _|
			///  __| | ___    ___| |_ _   _| |_| |_ 
			/// / _` |/ _ \  / __| __| | | |  _|  _|
			///| (_| | (_) | \__ \ |_| |_| | | | |  
			/// \__,_|\___/  |___/\__|\__,_|_| |_|  
			


			this->window->SwapBuffers();
		}
		gn.clearMemory();
		//gn2.clearMemory();
		ImGui_ImplGlfwGL3_Shutdown();
    	ImGui::DestroyContext();
		glfwTerminate();
	}

    void ExampleApp::renderUI(Vector4D& hitPoint, std::map<std::string, Vector4D>& hitResults, bool& AABBRenderState, const CollisionManifold& cm)
    {
        ImGui_ImplGlfwGL3_NewFrame();
		bool show_demo_window = true;
    	bool show_another_window = false;
        
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Begin("Hello, world!");
			if(hitResults.find("Square") != hitResults.end()){
				Vector4D squareHit = hitResults["Square"];
				ImGui::Text("Square Hit:");
				std::string hitText = std::to_string(squareHit.x()) + " " + std::to_string(squareHit.y()) + " " + std::to_string(squareHit.z());
				ImGui::Text(hitText.c_str());
			}

			else{
				ImGui::Text("No square hit yet :(");
			}

			if(hitResults.find("GN") != hitResults.end()){
				Vector4D gnHit = hitResults["GN"];
				ImGui::Text("GraphicsNode Hit:");
				std::string hitText = std::to_string(gnHit.x()) + " " + std::to_string(gnHit.y()) + " " + std::to_string(gnHit.z());
				ImGui::Text(hitText.c_str());

			}

			else{
				ImGui::Text("No GraphicsNode hit yet :(");
				
			}

			if(cm.isColliding == true){
				ImGui::Text("The AABB collision normal is");
				std::string normalText = std::to_string(cm.collisionNormal.get(0)) + " " + std::to_string(cm.collisionNormal.get(1))
				 + " " + std::to_string(cm.collisionNormal.get(2));
				ImGui::Text(normalText.c_str());
			}
			else{
				ImGui::Text("No AABB collision yet");
			}

			if (ImGui::Button("Render AABB")){
                if(AABBRenderState == true){
					AABBRenderState = false;
				}
				else {
					AABBRenderState = true;
				}
			}
			ImGui::SameLine();
            ImGui::Text("AABB Rendering = %d", AABBRenderState);
			

            ImGui::Text("Average Frame Time %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
			ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
			ImGui::End();
        }

    }

void ExampleApp::renderNano(NVGcontext * vg)
{
	
	
	nvgSave(vg);

	nvgBeginPath(vg);
	nvgCircle(vg,600, 100, 50);
	NVGpaint paint;
	//paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	

	nvgRestore(vg);
}

void ExampleApp::setupCube(unsigned int& VBO, unsigned int& VAO)
{
	float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void ExampleApp::renderCube()
{
}

Interval ExampleApp::getInterval(const GraphicsNode &gn, const Vector4D axis)
{
	Vector4D max = gn.maxBounds;
	Vector4D min = gn.minBounds;
	//AABB vertices
	Vector4D verts[] = {
		Vector4D(min.x(), max.y(), min.z()), // top front left
		Vector4D(min.x(), max.y(), max.z()), // top back left

		Vector4D(min.x(), min.y(), min.z()), // bottom front left
		Vector4D(min.x(), min.y(), max.z()), // bottom back left

		Vector4D(max.x(), max.y(), min.z()), // top front right
		Vector4D(max.x(), max.y(), max.z()), // top back right

		Vector4D(max.x(), min.y(), min.z()), // bottom front right
		Vector4D(max.x(), min.y(), max.z())  // bottom back right
	};
	
	Interval result;
	result.max = -__FLT_MAX__;
	result.min = __FLT_MAX__;

	for (int i = 0; i < 8; i++){
		float projection = Vector4D::dot(axis, verts[i]);
		result.min = std::min(projection, result.min);
		result.max = std::max(projection, result.max);
	}


    return result;
}

bool ExampleApp::overlapingOnAxis(const GraphicsNode &gn, const GraphicsNode &gn2, Vector4D &axis, float& depth, bool& isNegative)
{
	Interval first = getInterval(gn, axis.norm());
	Interval second = getInterval(gn2, axis.norm());

	if (second.max < first.min) {
		return false;
	}
	if(second.min > first.max) {
		return false;
	}


	float negativePen = second.max - first.min;
	float positivePen = first.max - second.min;
	
	isNegative = negativePen < positivePen;
	depth = std::min(negativePen, positivePen);
	return true;

}

CollisionManifold ExampleApp::SATOnAABBs(const GraphicsNode &gn, const GraphicsNode &gn2)
{
	CollisionManifold collisionResult;
	Vector4D testAxis[13] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f),
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f),
	};
	for (int i = 0; i < 3; i++){
		testAxis[4 + i * 3 + 0] = Vector4D::cross(testAxis[i], testAxis[0]);
		testAxis[4 + i * 3 + 1] = Vector4D::cross(testAxis[i], testAxis[1]);
		testAxis[4 + i * 3 + 2] = Vector4D::cross(testAxis[i], testAxis[2]);
	}
	float smallestPenDepth = __FLT_MAX__;
	Vector4D smallestPenAxis;
	Vector4D collisionNormal;
	
	
	for (int i = 0; i < 13; i++){
		bool negativePenetration;
		float resultantPenDepth;
		if(Vector4D::dot(testAxis[i], testAxis[i]) < 0.001f) {
			continue;
		}
		if(!overlapingOnAxis(gn, gn2, testAxis[i], resultantPenDepth, negativePenetration)){
			std::cout << "no collision" << "\n";
			return collisionResult;
		}
		if(resultantPenDepth <= 0.0f){
			return collisionResult;
		}

		if(resultantPenDepth < smallestPenDepth) {
			smallestPenDepth = resultantPenDepth;
			smallestPenAxis = testAxis[i] * (negativePenetration ? -1.0f : 1.0f);

			collisionNormal = smallestPenAxis;
			collisionResult.depth = smallestPenDepth;
		}
	}
	if(collisionNormal == Vector4D(0.0f, 0.0f, 0.0f)){
		return collisionResult;
	}

	//std::cout << "hit normal is X: " << collisionNormal.x() << " Y: " << collisionNormal.y() << " Z: " << collisionNormal.z() << "\n";
	Vector4D collisionAxis = collisionNormal.norm();
	collisionResult.collisionNormal = collisionAxis;

	std::vector<Vector4D> collisions1 = checkEdgesToAABB(getEdges(gn2), gn);
	std::vector<Vector4D> collisions2 = checkEdgesToAABB(getEdges(gn), gn2);
	collisionResult.contactPoints.reserve(collisions1.size() + collisions2.size());
	for(int i = 0; i < collisions2.size(); i++){
		collisionResult.contactPoints.push_back(collisions2[i]);
	}
	for(int i = 0; i < collisions1.size(); i++){
		collisionResult.contactPoints.push_back(collisions1[i]);
	}

	Interval interval = getInterval(gn, collisionAxis);
	float distance = (interval.max - interval.min) * 0.5f - collisionResult.depth * 0.5f;
	
	Vector4D planePoint = gn.AABBCenter + collisionAxis * distance;
	
	for (int i = collisionResult.contactPoints.size() - 1; i >= 0; --i){
		Vector4D contactPoint = collisionResult.contactPoints[i];
		collisionResult.contactPoints[i] = contactPoint + (collisionAxis * 
		Vector4D::dot(collisionAxis, planePoint - contactPoint));

		for (int i = collisionResult.contactPoints.size() - 1; i >= 0; --i){
			Vector4D contactPoint = collisionResult.contactPoints[i];

			//check if point is larger than the min x, y and z values and smaller than the max x, y and z values of the gn
			bool isWithinGN1 = contactPoint.x() >= gn.minBounds.get(0) && contactPoint.get(0) <= gn.maxBounds.get(0) &&
				contactPoint.get(1) >= gn.minBounds.get(1) && contactPoint.get(1) <= gn.maxBounds.get(1) &&
				contactPoint.get(2) >= gn.minBounds.get(2) && contactPoint.get(2) <= gn.maxBounds.get(2);

			bool isWithinGN2 = contactPoint.get(0) >= gn2.minBounds.get(0) && contactPoint.get(0) <= gn2.maxBounds.get(0) &&
				contactPoint.get(1) >= gn2.minBounds.get(1) && contactPoint.get(1) <= gn2.maxBounds.get(1) &&
				contactPoint.get(2) >= gn2.minBounds.get(2) && contactPoint.get(2) <= gn2.maxBounds.get(2);

			if(isWithinGN1 && isWithinGN2){

					//std::cout << "Contact point " << i << " is good" << "\n";
					continue;
				}
				collisionResult.contactPoints.erase(collisionResult.contactPoints.begin() + i);
				//std::cout << "Contact point " << i << " is not good at all" << "\n";

				//std::cout << "isWithinGN1 " << isWithinGN1 << " isWithinGN2 " << isWithinGN2 << "\n";
				
		}

	}


	collisionResult.isColliding = true;


    return collisionResult;
}

std::vector<Line> ExampleApp::getEdges(const GraphicsNode &gn)
{
	std::vector<Line> edges;
	edges.reserve(12);
	Vector4D max = gn.maxBounds;
	Vector4D min = gn.minBounds;
	Vector4D verts[] = {
		Vector4D(min.x(), max.y(), min.z()), // top front left
		Vector4D(min.x(), max.y(), max.z()), // top back left

		Vector4D(min.x(), min.y(), min.z()), // bottom front left
		Vector4D(min.x(), min.y(), max.z()), // bottom back left

		Vector4D(max.x(), max.y(), min.z()), // top front right
		Vector4D(max.x(), max.y(), max.z()), // top back right

		Vector4D(max.x(), min.y(), min.z()), // bottom front right
		Vector4D(max.x(), min.y(), max.z())  // bottom back right
	};

	int edgeIndices[][2] = {
		{0, 1}, {0, 2}, {1, 3}, {3, 2}, // left side face
		{5, 4}, {4, 6}, {5, 7}, {7, 6}, // right side face
		{0, 4}, {2, 6}, 				// front side face (the others are shared with other faces)
		{1, 5}, {3, 7}					// back side face
	};

	for (int i = 0; i < 12; ++i){
		edges.push_back(Line(verts[edgeIndices[i][0]], 
			verts[edgeIndices[i][1]]));

	}

    return edges;
}

std::vector<Vector4D> ExampleApp::checkEdgesToAABB(const std::vector<Line> &edges, const GraphicsNode& gn)
{
	std::vector<Vector4D> results;
	results.reserve(edges.size() * 3);
	Vector4D intersection;

	std::vector<Plane> planes = getPlanes(gn);

	for (int i = 0; i < planes.size(); ++i) {
		for (int j = 0; j < edges.size(); ++j){
			if(checkEdgesToPlane(planes[i], edges[j], intersection)){
				if(pointInAABB(intersection, gn)){
					results.push_back(intersection);
				}
			}
		}
	}
    return results;
}

std::vector<Plane> ExampleApp::getPlanes(const GraphicsNode& gn)
{
	std::vector<Plane> planes;
	planes.resize(6);
	Vector4D axis[] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f)
	};

	Vector4D newCenter = gn.AABBCenter;
	Vector4D size = gn.AABBSize;
	size = size * 0.5f; // we need to use half extents here
	planes[0] = Plane(axis[0], Vector4D::dot(axis[0], (newCenter + axis[0] * size[0])));
	planes[1] = Plane(axis[0] * -1.0f, -Vector4D::dot(axis[0], (newCenter - axis[0]) * size[0]));
	planes[2] = Plane(axis[1], Vector4D::dot(axis[1], (newCenter + axis[1] * size[1])));
	planes[3] = Plane(axis[1]  * -1.0f, -Vector4D::dot(axis[1], (newCenter - axis[1] * size[1])));
	planes[4] = Plane(axis[2], Vector4D::dot(axis[2], (newCenter + axis[2] * size[2])));
	planes[5] = Plane(axis[2]  * -1.0f, -Vector4D::dot(axis[2], (newCenter - axis[2] * size[2])));

    return planes;
}

bool ExampleApp::checkEdgesToPlane(const Plane &plane, const Line &edge, Vector4D &resultPoint)
{
	Vector4D edgeVector = edge.end - edge.start;
	
	float normalEdge = Vector4D::dot(plane.getNormal(), edge.start);
	float normalEdgeAngle = Vector4D::dot(plane.getNormal(), edgeVector);

	if(normalEdgeAngle == 0){
		return false;
	}

	float t = (plane.getDistance() - normalEdge) / normalEdgeAngle;
	if(t >= 0.0f && t <= 1.0f) {
		resultPoint = edge.start + edgeVector * t;
		return true;
	}

    return false;
}

bool ExampleApp::pointInAABB(const Vector4D &point, const GraphicsNode &gn)
{
	Vector4D direction = point - gn.AABBCenter; //direction to point
	Vector4D axis[] = {
		Vector4D(1.0f, 0.0f, 0.0f),
		Vector4D(0.0f, 1.0f, 0.0f),
		Vector4D(0.0f, 0.0f, 1.0f)

	};
	for(int i = 0; i < 3; ++i){
		float distance = Vector4D::dot(direction, axis[i]);
		
		if(distance > gn.AABBSize.get(i)){
			return false;
		}

		if(distance < -gn.AABBSize.get(i)){
			return false;
		}

	}

    return true;
}

} // namespace Example