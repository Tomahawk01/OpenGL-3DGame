#pragma once

#include "StopEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "MouseButtonEvent.h"

#include <variant>

namespace Game {

	using Event = std::variant<StopEvent, KeyEvent, MouseEvent, MouseButtonEvent>;

}
