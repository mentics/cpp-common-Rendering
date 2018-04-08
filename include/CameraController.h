#pragma once
#include "Camera.h"
#include "Quip.h"


namespace MenticsGame {
	inline glm::vec4 toGlmPoint(vect3 v) {
		return glm::vec4(v.x(), v.y(), v.z(), 1);
	}

	inline glm::vec4 toGlmVector(vect3 v) {
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

		QuipPtr player;



		CameraController(QuipPtr q) : player(q) {}

		void update(float aspectRatio);
		~CameraController();
	};

}