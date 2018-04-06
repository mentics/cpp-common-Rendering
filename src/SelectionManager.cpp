#include "stdafx.h"
#include "SelectionManager.h"

namespace MenticsGame {
	SelectionManager::SelectionManager(AgentUniquePtr<TimePoint> s) : selected(std::move(s))
	{
	}

	void MenticsGame::SelectionManager::setSelected(AgentUniquePtr<TimePoint> s)
	{
		selected = std::move(s);
	}


	SelectionManager::~SelectionManager()
	{
	}
}
