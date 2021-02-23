#pragma once

#include <Component/SimpleScene.h>

class NorwFj : public SimpleScene
{
	public:
		NorwFj();
		~NorwFj();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL);

		unsigned int UploadCubeMapTexture(const std::string& posx, const std::string& posy, const std::string& posz, const std::string& negx, const std::string& negy, const std::string& negz);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		glm::vec3 bezier(float t, glm::vec3 control_p1, glm::vec3 control_p2, glm::vec3 control_p3, glm::vec3 control_p4);

	
	protected:
		// info generated surface
		glm::vec3 control_p1, control_p2, control_p3, control_p4;
		glm::vec3 control_p1_b1, control_p2_b1, control_p3_b1, control_p4_b1;
		glm::vec3 control_p1_b2, control_p2_b2, control_p3_b2, control_p4_b2;
		glm::vec3 control_p1_b3, control_p2_b3, control_p3_b3, control_p4_b3;
		glm::vec3 pos, currentPoint;
		unsigned int no_of_generated_points;
		float max_translate, max_rotate, max_pointX;
		float translateX = 0, translateZ = 0;

		std::unordered_map<std::string, Texture2D*> mapTextures;
		int cubeMapTextureID;
		int index = 1;
		int reachPoint = 0;
};
