#pragma once
#include "Agent.h"

namespace MenticsGame {
	class SelectionManager
	{
		AgentUniquePtr<TimePoint> selected;  
	public:
		SelectionManager(AgentUniquePtr<TimePoint> s);
		void setSelected(AgentUniquePtr<TimePoint> s);
		~SelectionManager();
	};
}
