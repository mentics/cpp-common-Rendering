#include "stdafx.h"
#include "SelectionManager.h"

namespace MenticsGame {

template<typename TimeType>
void SelectionManager<TimeType>::setSelected(AgentP s) {
	selected = s;
}

}
