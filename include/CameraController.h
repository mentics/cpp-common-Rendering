#pragma once
#include "Camera.h"
#include "WorldModel.h"
#include "Quip.h"

namespace MenticsGame {

template<typename TimeType>
class CameraController {
public:
	Camera cam;

	glm::vec3 ray00;
	glm::vec3 ray10;
	glm::vec3 ray01;
	glm::vec3 ray11;

	QuipWP player;

	CameraController(QuipWP q) : player(q) {}

	void update(float aspectRatio);
};

template CameraController<uint64_t>;

}