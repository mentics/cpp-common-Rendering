#include "stdafx.h"
#include "SelectionManager.h"

namespace MenticsGame {
	SelectionManager::SelectionManager(AgentPtr<TimePoint> s) : selected(s)
	{
	}

	void MenticsGame::SelectionManager::setSelected(AgentPtr<TimePoint> s)
	{
		selected = s;
	}


	SelectionManager::~SelectionManager()
	{
	}
}
