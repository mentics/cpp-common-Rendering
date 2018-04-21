#pragma once
#include "WorldModel.h"

namespace MenticsGame {

template<typename TimeType>
class SelectionManager {
	AgentWP selected;  

public:
	SelectionManager(AgentWP s) : selected(s) {}

	void setSelected(AgentWP s);
};

}
