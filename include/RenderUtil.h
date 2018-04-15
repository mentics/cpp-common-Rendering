#pragma once

#include "MenticsMath.h"
#include <glm/glm.hpp>

namespace MenticsGame {

inline glm::vec4 toGlmPoint(vect3 v) {
	return glm::vec4(v.x(), v.y(), v.z(), 1);
}

inline glm::vec4 toGlmVector(vect3 v) {
	return glm::vec4(v.x(), v.y(), v.z(), 0);
}

}
