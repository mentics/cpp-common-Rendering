#pragma once
#include "Camera.h"
#include "Quip.h"


namespace MenticsGame {
	glm::vec4 toGlmPoint(vect3 v) {
		return glm::vec4(v.x(), v.y(), v.z(), 1);
	}

	glm::vec4 toGlmVector(vect3 v) {
		return glm::vec4(v.x(), v.y(), v.z(), 0);
	}

	class CameraController 
	{
	public:
		Camera cam;

		glm::vec3 ray00;
		glm::vec3 ray10;
		glm::vec3 ray01;
		glm::vec3 ray11;

		QuipUniquePtr player;



		CameraController(QuipUniquePtr q) : player(std::move(q)) {}

		void update(float aspectRatio);
		~CameraController();
	};

}