#pragma once
#include "Agent.h"

namespace MenticsGame {
	class SelectionManager
	{
		AgentPtr<TimePoint> selected;  
	public:
		SelectionManager(AgentPtr<TimePoint> s);
		void setSelected(AgentPtr<TimePoint> s);
		~SelectionManager();
	};
}
