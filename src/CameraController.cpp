#include "stdafx.h"
#include "CameraController.h"

namespace MenticsGame {
	void CameraController::update(float aspectRatio)
	{
		ray00 = glm::normalize(cam.GetViewMatrix() * glm::vec4(-aspectRatio, -1, 1.75, 0));
		ray10 = glm::normalize(cam.GetViewMatrix() * glm::vec4(aspectRatio, -1, 1.75, 0));
		ray01 = glm::normalize(cam.GetViewMatrix() * glm::vec4(-aspectRatio, 1, 1.75, 0));
		ray11 = glm::normalize(cam.GetViewMatrix() * glm::vec4(aspectRatio, 1, 1.75, 0));
	}
	CameraController::~CameraController()
	{
	}
}