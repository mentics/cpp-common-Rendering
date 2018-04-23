#include "stdafx-rendering.h"
#include "SelectionManager.h"

namespace MenticsGame {

template<typename TimeType>
void SelectionManager<TimeType>::setSelected(AgentWP s) {
    selected = s;
}

}
