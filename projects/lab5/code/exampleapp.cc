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
		gn.setNormalMap(normalMapPtr);
		gn.initTexture("./resources/container45.jpg");
		gn.setTransform(Matrix4D());

		// gn2.setMesh(objectMesh);
		// gn2.setShader(lightShader);
		// gn2.setTexture(texPtr);s
		// gn2.setTransform(Matrix4D());


		 gn.updateTransform(Matrix4D::scale(Vector4D(0.1, 0.1, 0.1)));
		// gn.updateTransform(Matrix4D::roty(160));
		// gn2.updateTransform(Matrix4D::scale(Vector4D(0.3, 0.3, 0.3)));
		// gn2.updateTransform(Matrix4D::translation(Vector4D(2.0f, 2.0f, 2.0f)));

		//rest plane
		std::vector<Vector4D> planePoints;
		planePoints.push_back(Vector4D(0.5f,  0.5f, 0.0f));
		planePoints.push_back(Vector4D(0.5f, -0.5f, 0.0f));
		planePoints.push_back(Vector4D(-0.5f, -0.5f, 0.0f));

		Plane testPlane(planePoints);
		std::vector<Plane> planes;

		Matrix4D squareTransform;

		Vector4D squareColor = {0.5, 0.5, 0.5, 1};

		float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		float xMax = vertices[0];
		float xMin = vertices[0];

		float yMax = vertices[1];
		float yMin = vertices[1];

		float zMax = vertices[2];
		float zMin = vertices[2];
		for (int i = 8; i < 32; i += 8){
			if(xMax < vertices[i]){
				xMax = vertices[i];
			}
			if(xMin > vertices[i]){
				xMin = vertices[i];
			}

		}
		for (int i = 9; i < 32; i += 8){
			if(yMax < vertices[i]){
				yMax = vertices[i];
			}
			if(yMin > vertices[i]){
				yMin = vertices[i];
			}

		}
		for (int i = 10; i < 32; i += 8){
			if(zMax < vertices[i]){
				zMax = vertices[i];
			}
			if(zMin > vertices[i]){
				zMin = vertices[i];
			}

		} 

		unsigned int indices[] = {
			0, 1, 3, 
			1, 2, 3 
		};
		unsigned int VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//test ray
		std::vector<Ray> rays;
		Ray testRay(testPlane.getNormal(), testPlane.getNormal() * -50);
		testRay.rayColor = Vector4D(1.0f, 1.0f, 1.0f);
		rays.push_back(testRay);
		//rays.push_back(Ray(Vector4D(0.0f, 0.0f, 0.0f), Vector4D(0.0f, 5.0f, 5.0f)));




		LightNode light = LightNode(lightShader);
		light.lightColor = Vector4D(1.0f, 1.0f, 1.0f);
		light.lightPos = lightPosition;
		light.intensity = 0.1;

		light.setupLighting();


		this->window->GetSize(width, height);
		std::cout << "width: " << width << " height: " << height << "\n";

		float yaw = -90;
		float pitch = 0;

		float lastX = 512.0f;
		float lastY = 384.0f;
		bool firstRotation = true;

		Vector4D squareHit; 
		

		//render loop
		while (this->window->IsOpen())
		{
			//light.lightColor = Vector4D(1.0f, 1.0f, 1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;	
			lastFrame = currentFrame;

			this->renderUI(squareHit);
			

			/*light.intensity = (float)cos(glfwGetTime());
			std::cout << light.intensity << "\n";*/
			
			

			//std::cout << "Lightpos X:" << light.lightPos.x() << " Y:" << light.lightPos.y() << " Z:" << light.lightPos.z() << "\n";

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
					Ray rayWorld;
					rayWorld.rayCast(mouseX, mouseY, cam.getView(), projection, width, height);
					//std::cout << "x: " << rayWorld.x() << "y: " << rayWorld.y() << "z: " << rayWorld.z() << "\n";
					//Ray newRay = Ray(Vector4D(rayWorld.x(), rayWorld.y() , rayWorld.z()), Vector4D(rayWorld.x() * 5.0f, rayWorld.y() * 5.0f, rayWorld.z() * 5.0f));
					std::srand(time(0));
					const float randomColorR = (std::rand() % 255) / 255.0f;
					const float randomColorG = (std::rand() % 255) / 255.0f;
					const float randomColorB = (std::rand() % 255) / 255.0f;
					rayWorld.rayColor = Vector4D(randomColorR, randomColorG, randomColorB, 1.0f);
					Vector4D hitPoint = rayWorld.Intersect(testPlane);
					if(hitPoint.x() >= xMin && hitPoint.x() <= xMax && hitPoint.y() >= yMin && hitPoint.y() <= yMax){
						std::cout << "ray hit inside the square" << "\n";
						squareHit = hitPoint;
						squareColor = Vector4D(0.3f, 0, 0, 1);
					} 
					gn.setTransform(Matrix4D::scale(Vector4D(0.1, 0.1, 0.1)));
					gn.updateTransform(Matrix4D::translation(hitPoint));
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
			//gn2.draw(cam, projection, light.lightPos);


			//squareTransform = squareTransform.translation(testPlane.getPoint(0));

			squareShader.get()->use();
			squareShader.get()->setMat4(std::string("model"), squareTransform);
			squareShader.get()->setMat4(std::string("view"), cam.getView());
			squareShader.get()->setMat4(std::string("projection"), projection);
			squareShader.get()->setVec4(std::string("color"), squareColor);
			//Square Draw
			glBindVertexArray(VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			

			Matrix4D rayTransform;
			//ray draw
			//testRay.draw();
			rayShader.get()->use();
			rayShader.get()->setMat4(std::string("model"), rayTransform);
			rayShader.get()->setMat4(std::string("view"), cam.getView());
			rayShader.get()->setMat4(std::string("projection"), projection);
			rayShader.get()->setVec3(std::string("viewPosition"), cam.camPos);
			for(Ray ray : rays){
				rayShader.get()->setVec4(std::string("rayColor"), ray.rayColor);
				ray.draw();
			}

			ImGui::Render();


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

    void ExampleApp::renderUI(Vector4D hitPoint)
    {
        ImGui_ImplGlfwGL3_NewFrame();
		bool show_demo_window = true;
    	bool show_another_window = false;
        
        {
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Begin("Hello, world!");
			std::string hitText = std::string("Hit: ") + std::to_string(hitPoint.x()) + " " + std::to_string(hitPoint.y()) + " " + std::to_string(hitPoint.z());
            ImGui::Text(hitText.c_str());

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



} // namespace Example