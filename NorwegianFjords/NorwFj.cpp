#include "NorwFj.h"

#include <vector>
#include <iostream>

#include <stb/stb_image.h>

#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/World.cpp

NorwFj::NorwFj()
{
}

NorwFj::~NorwFj()
{
}

void NorwFj::Init()
{
	//parameters related to surface generation
	no_of_generated_points = 6;	//number of points on a Bezier curve
	max_translate = 8.0f;			//for the translation surface, it's the distance between the first and the last curve
	max_rotate = glm::radians(360.0f);	//for the rotation surface, it's the angle between the first and the last curve
	max_pointX = 0;

	//define control points
	control_p1 = glm::vec3(rand() % 100 / 10.0f, 0, rand() % 100 / 10.0f);
	control_p2 = glm::vec3(rand() % 100 / 10.0f, 0, control_p1.z + rand() % 100 / 10.0f);
	control_p3 = glm::vec3(rand() % 100 / 10.0f, 0, control_p2.z + rand() % 100 / 10.0f);
	control_p4 = glm::vec3(rand() % 100 / 10.0f, 0, control_p3.z + rand() % 100 / 10.0f);
	control_p1_b1 = control_p4;
	control_p2_b1 = glm::vec3(rand() % 50 / 10.0f, 0, control_p1_b1.z + rand() % 100 / 10.0f);
	control_p3_b1 = glm::vec3(rand() % 50 / 10.0f, 0, control_p2_b1.z + rand() % 100 / 10.0f);
	control_p4_b1 = glm::vec3(rand() % 50 / 10.0f, 0, control_p3_b1.z + rand() % 100 / 10.0f);
	max_pointX = MAX(control_p2_b1.x, MAX(control_p3_b1.x, control_p4_b1.x));
	control_p1_b2 = control_p4;
	control_p2_b2 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p1_b2.z + rand() % 100 / 10.0f);
	control_p3_b2 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p2_b2.z + rand() % 100 / 10.0f);
	control_p4_b2 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p3_b2.z + rand() % 100 / 10.0f);
	max_pointX = MAX(control_p2_b2.x, MAX(control_p3_b2.x, control_p4_b2.x));
	control_p1_b3 = control_p4;
	control_p2_b3 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p1_b3.z + rand() % 100 / 10.0f);
	control_p3_b3 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p2_b3.z + rand() % 100 / 10.0f);
	control_p4_b3 = glm::vec3(max_pointX + rand() % 50 / 10.0f, 0, control_p3_b3.z + rand() % 100 / 10.0f);

	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(control_p2.x + 1.5f, 2, control_p2.z), glm::quat(glm::vec3(-20 * TO_RADIANS, 180 * TO_RADIANS, 0)));
	camera->Update();

	ToggleGroundPlane();

	const string textureLoc = "Source/Laboratoare/NorwFj/Textures/";

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "water_texture.jpg").c_str(), GL_REPEAT);
		mapTextures["water"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "mountain_texture.jpg").c_str(), GL_REPEAT);
		mapTextures["mountain"] = texture;
	}

	std::string shaderPath = "Source/Laboratoare/NorwFj/Shaders/";

	// Create a shader program for surface generation
	{
		Shader *shader = new Shader("MainRiverGeneration");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "MainRiver.GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for surface generation
	{
		Shader* shader = new Shader("Branch1Generation");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Branch1.GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for surface generation
	{
		Shader* shader = new Shader("Branch2Generation");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Branch2.GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for surface generation
	{
		Shader* shader = new Shader("Branch3Generation");
		shader->AddShader(shaderPath + "VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Branch3.GS.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader(shaderPath + "FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for rendering to texture
	{
		Shader* shader = new Shader("ShaderNormal");
		shader->AddShader(shaderPath + "Normal.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Normal.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a shader program for rendering to texture
	{
		Shader* shader = new Shader("BoatShader");
		shader->AddShader(shaderPath + "Boat.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader(shaderPath + "Boat.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Create a bogus mesh with 2 points (a line)
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-4.0, -2.5,  1.0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(-4.0, 5.5,  1.0), glm::vec3(0, 1, 0))
		};

		vector<unsigned short> indices =
		{
			0, 1
		};

		meshes["surface"] = new Mesh("generated initial surface points");
		meshes["surface"]->InitFromData(vertices, indices);
		meshes["surface"]->SetDrawMode(GL_LINES);
	}

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("boat");
		mesh->LoadMesh("Source/Laboratoare/NorwFj/Models/source", "gemiLow.fbx");
		meshes[mesh->GetMeshID()] = mesh;
	}

	std::string texturePath = "Source/Laboratoare/NorwFj/Textures/Cubemap/";

	cubeMapTextureID = UploadCubeMapTexture(
		texturePath + "posx.jpg",
		texturePath + "posy.jpg",
		texturePath + "posz.jpg",
		texturePath + "negx.jpg",
		texturePath + "negy.jpg",
		texturePath + "negz.jpg"
	);
}


void NorwFj::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void NorwFj::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader 
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Activate texture location 0
	glActiveTexture(GL_TEXTURE0);

	// Bind the texture1 ID
	glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Send texture uniform value
	glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

	// Activate texture location 1
	glActiveTexture(GL_TEXTURE1);

	// Bind the texture2 ID
	glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());

	// Send texture uniform value
	glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);

	// Activate texture location 2
	glActiveTexture(GL_TEXTURE2);

	// Bind the cubeMapTexture ID
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

	// Send texture uniform value
	glUniform1i(glGetUniformLocation(shader->program, "texture_cubemap"), 2);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);

}

glm::vec3 NorwFj::bezier(float t, glm::vec3 control_p1, glm::vec3 control_p2, glm::vec3 control_p3, glm::vec3 control_p4)
{
	return control_p1 * pow((1 - t), 3) + control_p2 * glm::vec3(3) * t * pow((1 - t), 2) + control_p3 * glm::vec3(3) * pow(t, 2) * (1 - t) + control_p4 * pow(t, 3);
}


void NorwFj::Update(float deltaTimeSeconds)
{
	ClearScreen();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	auto camera = GetSceneCamera();

	{
		Shader* shader = shaders["MainRiverGeneration"];
		shader->Use();

		// send uniforms to shaders
		glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4"), control_p4.x, control_p4.y, control_p4.z);

		// send number of generated points to aproximate the curve to shaders
		glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);

		// draw the reflextion on the mesh
		auto cameraPosition = camera->transform->GetWorldPosition();

		int loc_camera = shader->GetUniformLocation("camera_position");
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		Mesh* mesh = meshes["surface"];
		// draw the object
		RenderSimpleMesh(mesh, shader, glm::mat4(1), mapTextures["water"], mapTextures["mountain"]);
	}

	{
		Shader* shader = shaders["Branch1Generation"];
		shader->Use();

		// send uniforms to shaders
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b1"), control_p1_b1.x, control_p1_b1.y, control_p1_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b1"), control_p2_b1.x, control_p2_b1.y, control_p2_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b1"), control_p3_b1.x, control_p3_b1.y, control_p3_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b1"), control_p4_b1.x, control_p4_b1.y, control_p4_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b2"), control_p1_b2.x, control_p1_b2.y, control_p1_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b2"), control_p2_b2.x, control_p2_b2.y, control_p2_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b2"), control_p3_b2.x, control_p3_b2.y, control_p3_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b2"), control_p4_b2.x, control_p4_b2.y, control_p4_b2.z);

		// send number of generated points to aproximate the curve to shaders
		glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);

		// draw the reflextion on the mesh
		auto cameraPosition = camera->transform->GetWorldPosition();

		int loc_camera = shader->GetUniformLocation("camera_position");
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		Mesh* mesh = meshes["surface"];
		// draw the object
		RenderSimpleMesh(mesh, shader, glm::mat4(1), mapTextures["water"], mapTextures["mountain"]);
	}

	{
		Shader* shader = shaders["Branch2Generation"];
		shader->Use();

		// send uniforms to shaders
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b1"), control_p1_b1.x, control_p1_b1.y, control_p1_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b1"), control_p2_b1.x, control_p2_b1.y, control_p2_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b1"), control_p3_b1.x, control_p3_b1.y, control_p3_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b1"), control_p4_b1.x, control_p4_b1.y, control_p4_b1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b2"), control_p1_b2.x, control_p1_b2.y, control_p1_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b2"), control_p2_b2.x, control_p2_b2.y, control_p2_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b2"), control_p3_b2.x, control_p3_b2.y, control_p3_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b2"), control_p4_b2.x, control_p4_b2.y, control_p4_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b3"), control_p1_b3.x, control_p1_b3.y, control_p1_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b3"), control_p2_b3.x, control_p2_b3.y, control_p2_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b3"), control_p3_b3.x, control_p3_b3.y, control_p3_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b3"), control_p4_b3.x, control_p4_b3.y, control_p4_b3.z);

		// send number of generated points to aproximate the curve to shaders
		glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);

		// draw the reflextion on the mesh
		auto cameraPosition = camera->transform->GetWorldPosition();

		int loc_camera = shader->GetUniformLocation("camera_position");
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		Mesh* mesh = meshes["surface"];
		// draw the object
		RenderSimpleMesh(mesh, shader, glm::mat4(1), mapTextures["water"], mapTextures["mountain"]);
	}

	{
		Shader* shader = shaders["Branch3Generation"];
		shader->Use();

		// send uniforms to shaders
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b2"), control_p1_b2.x, control_p1_b2.y, control_p1_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b2"), control_p2_b2.x, control_p2_b2.y, control_p2_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b2"), control_p3_b2.x, control_p3_b2.y, control_p3_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b2"), control_p4_b2.x, control_p4_b2.y, control_p4_b2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p1_b3"), control_p1_b3.x, control_p1_b3.y, control_p1_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2_b3"), control_p2_b3.x, control_p2_b3.y, control_p2_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3_b3"), control_p3_b3.x, control_p3_b3.y, control_p3_b3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4_b3"), control_p4_b3.x, control_p4_b3.y, control_p4_b3.z);

		// send number of generated points to aproximate the curve to shaders
		glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);

		// draw the reflextion on the mesh
		auto cameraPosition = camera->transform->GetWorldPosition();

		int loc_camera = shader->GetUniformLocation("camera_position");
		glUniform3f(loc_camera, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		Mesh* mesh = meshes["surface"];
		// draw the object
		RenderSimpleMesh(mesh, shader, glm::mat4(1), mapTextures["water"], mapTextures["mountain"]);
	}

	// draw the cubemap
	{
		Shader* shader = shaders["ShaderNormal"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(100));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["cube"]->Render();
	}

	// draw the boat
	{
		Shader* shader = shaders["BoatShader"];
		shader->Use();

		// send uniforms to shaders
		glUniform3f(glGetUniformLocation(shader->program, "control_p1"), control_p1.x, control_p1.y, control_p1.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p2"), control_p2.x, control_p2.y, control_p2.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p3"), control_p3.x, control_p3.y, control_p3.z);
		glUniform3f(glGetUniformLocation(shader->program, "control_p4"), control_p4.x, control_p4.y, control_p4.z);

		// send number of generated points to aproximate the curve to shaders
		glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);

		// send the time value
		glUniform1f(glGetUniformLocation(shader->program, "time"), Engine::GetElapsedTime());

		Mesh* mesh = meshes["boat"];
		glm::mat4 modelMatrix = glm::mat4(1);
		pos = glm::vec3(control_p1.x + 0.75f, control_p1.y + 0.25f, control_p1.z);
		modelMatrix = glm::translate(modelMatrix, pos);

		currentPoint = bezier(float(index % no_of_generated_points), control_p1, control_p2, control_p3, control_p4);
		translateX += deltaTimeSeconds * currentPoint.x / 20;
		translateZ += deltaTimeSeconds * currentPoint.z / 20;
		if ((translateX >= pos.x || translateZ >= pos.z) || reachPoint == 1)
		{
			index++;
			reachPoint = 1;
			translateX = 0.75f;
			translateZ = pos.z;
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(translateX, 0, translateZ));

		modelMatrix = glm::rotate(modelMatrix, RADIANS(-90.0f), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
		// draw the object
		// RenderSimpleMesh(mesh, shader, modelMatrix, mapTextures["water"], mapTextures["mountain"]);
		RenderMesh(mesh, shader, modelMatrix);
	}
	
}

void NorwFj::FrameEnd()
{
	// DrawCoordinatSystem();
}

unsigned int NorwFj::UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// Create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);

	// Bind the texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void NorwFj::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
};

void NorwFj::OnKeyPress(int key, int mods)
{
	
	// change number of generated points
	if (key == GLFW_KEY_EQUAL)
	{
		no_of_generated_points++;
	}

	if (key == GLFW_KEY_MINUS)
	{
		no_of_generated_points--;
	}
};

void NorwFj::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void NorwFj::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void NorwFj::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void NorwFj::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void NorwFj::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void NorwFj::OnWindowResize(int width, int height)
{
	// treat window resize event
}
