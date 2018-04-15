#pragma once
#include "Agent.h"

namespace MenticsGame {

template<typename TimeType>
class SelectionManager {
	AgentP selected;  

public:
	SelectionManager(AgentP s) : selected(s) {}

	void setSelected(AgentP s);
};

}
