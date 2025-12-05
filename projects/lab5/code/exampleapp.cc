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

			this->window->SetUiRender([this]()
		{
			this->renderUI();
		});
		this->window->SetNanoVGRender([this](NVGcontext * vg)
		{
			this->renderNano(vg);
		});
			
		}
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

		Matrix4D projection = projection.perspective(radFov, 800.0f / 600.0f, 0.1f, 100.0f);

		cam.camPos = Vector4D(0.0f, 0.0f, 3.0f);
		cam.camTarget = Vector4D(0.0f, 0.0f, 0.0f);
		cam.camFront = Vector4D(0.0f, 0.0f, -1.0f);
		cam.camUp = Vector4D(0.0f, 1.0f, 0.0f);

		Vector4D lightPosition(-14.2f, 1.0f, 2.0f);

		
		GraphicsNode lightCube;

		std::shared_ptr<MeshResource> pointLightMesh = std::make_shared<MeshResource>();
		std::shared_ptr<TextureResource> lightTexPtr = std::make_shared<TextureResource>();
		std::shared_ptr<ShaderObject> pointLightShader = std::make_shared<ShaderObject>("./resources/LightCubeVS.vs", "./resources/LightCubeFS.fs");


		std::shared_ptr<MeshResource> objectMesh = std::make_shared<MeshResource>();
		std::shared_ptr<ShaderObject> lightShader = std::make_shared<ShaderObject>("./resources/NormalMappingVS.vs", "./resources/NormalMappingFS.fs");
		std::shared_ptr<TextureResource> texPtr = std::make_shared<TextureResource>();
		std::shared_ptr<TextureResource> normalMapPtr = std::make_shared<TextureResource>();


		pointLightMesh->loadObj("./resources/cube2.obj");
		objectMesh->loadGLTF("./resources/cube.gltf");

		lightCube.setMesh(pointLightMesh);
		lightCube.setShader(pointLightShader);
		lightCube.setTexture(lightTexPtr);
		lightCube.initTexture("./resources/container2fixed.png");
		lightCube.setTransform(Matrix4D());


		gn.setMesh(objectMesh);
		gn.setShader(lightShader);
		gn.setTexture(texPtr);
		gn.setNormalMap(normalMapPtr);
		gn.initTexture("");
		gn.setTransform(Matrix4D());

		// gn2.setMesh(objectMesh);
		// gn2.setShader(lightShader);
		// gn2.setTexture(texPtr);s
		// gn2.setTransform(Matrix4D());

		//gn.updateTransform(Matrix4D::scale(Vector4D(0.05, 0.05, 0.05)));

		gn.updateTransform(Matrix4D::scale(Vector4D(1.3, 1.3, 1.3)));
		gn.updateTransform(Matrix4D::roty(160));
		// gn2.updateTransform(Matrix4D::scale(Vector4D(0.3, 0.3, 0.3)));
		// gn2.updateTransform(Matrix4D::translation(Vector4D(2.0f, 2.0f, 2.0f)));


		

		LightNode light = LightNode(lightShader);
		light.lightColor = Vector4D(1.0f, 1.0f, 1.0f);
		light.lightPos = lightPosition;
		light.intensity = 0.1;

		light.setupLighting();


		Matrix4D rot;



		int width;
		int height;

		this->window->GetSize(width, height);
		std::cout << "width: " << width << " height: " << height << "\n";

		float yaw = -90;
		float pitch = 0;

		float lastX = 512.0f;
		float lastY = 384.0f;
		bool firstRotation = true;
		

		//render loop
		while (this->window->IsOpen())
		{
			//light.lightColor = Vector4D(1.0f, 1.0f, 1.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			this->window->Update();
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;	
			lastFrame = currentFrame;

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
			

			window->SetMousePressFunction([this](int32 mousekey, int32 status, int32 keyc) {
				std::cout << "mousekey: " << mousekey << " status: " << status << " keyc: " << keyc << "\n";
				mousepress = mousekey;
				mousestatus = status;
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
		glfwTerminate();
	}
void ExampleApp::renderUI() {
	static float f = 0.0f;
	static int counter = 0;
	bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);
	ImGui::End();


}

void ExampleApp::renderNano(NVGcontext * vg)
{
	
	
	nvgSave(vg);

	nvgBeginPath(vg);
	nvgCircle(vg,600, 100, 50);
	NVGpaint paint;
	paint = nvgLinearGradient(vg, 600, 100, 650, 150, nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 255, 0, 255));
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	

	nvgRestore(vg);
}
}// namespace Example