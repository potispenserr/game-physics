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
		gn.setTexture(texPtr);
		//gn.setNormalMap(normalMapPtr);
		gn.initTexture("./resources/container45.jpg");
		gn.setTransform(Matrix4D());

		gn2.setMesh(objectMesh);
		gn2.setShader(lightShader);
		gn2.setTexture(texPtr);
		gn2.setTransform(Matrix4D());

		Matrix4D gnTransform = Matrix4D::scale(Vector4D(0.5, 0.5, 0.5));
		gnTransform = gnTransform * Matrix4D::translation(Vector4D(2.0f, 0.0f, 0.0f));

		//Gather the transforms into one matrix otherwise all hell breaks loose
		gn.updateTransform(gnTransform);

		Matrix4D gn2Transform = Matrix4D::scale(Vector4D(0.5, 0.5, 0.5)) * Matrix4D::translation(Vector4D(2.9f, 0.0f, 0.0f));
		gn2.updateTransform(gn2Transform);

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
		rays.push_back(testRay);
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
		

		//render loop
		while (this->window->IsOpen())
		{

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;	
			lastFrame = currentFrame;

			this->renderUI(squareHit, hitResults, gn.AABBRenderState);

			for(int i = 1; i < gnList.size(); i++){
				// check if collisions are happening on the AABBs right side
				if(gnList[i - 1].maxBounds.x() > gnList[i].minBounds.x() && gnList[i - 1].maxBounds.x() < gnList[i].maxBounds.x()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision right" << "\n";
				}
				// check if collisions are not happening on the AABBs left side
				else if(gnList[i - 1].minBounds.x() > gnList[i].minBounds.x() && gnList[i - 1].minBounds.x() < gnList[i].maxBounds.x()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision left" << "\n";
				}
				// top side this time
				else if(gnList[i - 1].maxBounds.y() > gnList[i].minBounds.y() && gnList[i - 1].maxBounds.y() < gnList[i].maxBounds.y()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision top" << "\n";
				}
				// bottom side
				else if(gnList[i - 1].minBounds.y() > gnList[i].minBounds.y() && gnList[i - 1].minBounds.y() < gnList[i].maxBounds.y()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision bottom" << "\n";
				}
				// it's front side time
				else if(gnList[i - 1].maxBounds.z() > gnList[i].minBounds.z() && gnList[i - 1].maxBounds.z() < gnList[i].maxBounds.z()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision front" << "\n";
				}
				// now it's back side collisions
				else if(gnList[i - 1].minBounds.z() > gnList[i].minBounds.z() && gnList[i - 1].minBounds.z() < gnList[i].maxBounds.z()){
					collisionList.push_back(gnList[i - 1]);
					collisionList.push_back(gnList[i]);
					std::cout << "Collision back" << "\n";
				}

			}



			window->SetKeyPressFunction([this, &light](int32 asciikey, int32 argb, int32 status, int32 mod)
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
						light.lightPos = light.lightPos - Vector4D(0.5f, 0.0f, 0.0f);
						break;
					case GLFW_KEY_K:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.0f, 0.5f);
						break;
					case GLFW_KEY_L:
						light.lightPos = light.lightPos + Vector4D(0.5f, 0.0f, 0.0f);
						break;
					case GLFW_KEY_I:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.0f, -0.5f);
						break;
					case GLFW_KEY_U:
						light.lightPos = light.lightPos + Vector4D(0.0f, -0.5f, 0.0f);
						break;
					case GLFW_KEY_O:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.5f, 0.0f);
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
						light.lightPos = light.lightPos - Vector4D(0.5f, 0.0f, 0.0f);
						break;
					case GLFW_KEY_K:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.0f, 0.5f);
						break;
					case GLFW_KEY_L:
						light.lightPos = light.lightPos + Vector4D(0.5f, 0.0f, 0.0f);
						break;
					case GLFW_KEY_I:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.0f, -0.5f);
						break;
					case GLFW_KEY_U:
						light.lightPos = light.lightPos + Vector4D(0.0f, -0.5f, 0.0f);
						break;
					case GLFW_KEY_O:
						light.lightPos = light.lightPos + Vector4D(0.0f, 0.5f, 0.0f);
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
					gnHit = rayWorld.Intersect(gn, gnHitPoint);
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
			gn.draw(cam, projection, light.lightPos);
			lightCube.draw(cam, projection, light.lightPos);
			gn2.draw(cam, projection, light.lightPos);
			


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
			rayShader.get()->setVec3(std::string("viewPosition"), cam.camPos);
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



			ImGui::Render();
			collisionList.clear();


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

    void ExampleApp::renderUI(Vector4D& hitPoint, std::map<std::string, Vector4D>& hitResults, bool& AABBRenderState)
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

} // namespace Example