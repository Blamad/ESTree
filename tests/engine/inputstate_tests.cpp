#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <memory>

#include "GraphicEngine/Input/InputState.h"

TEST_CASE("InputState: can be created, filled and destroyed") {
	// GLWindow owns it through a unique_ptr; the destructor must not free its by-value members.
	auto state = std::make_unique<InputState>();
	state->pushMousePosition(1.0, 2.0);
	state->pushMouseScroll(0.0, 1.0);
	state->pushMouseClick(ClickEvent(MOUSE_BUTTON_LEFT, true, Point2d(1.0, 2.0)));
	state->pushKeyEvent(65, PRESSED);
	state->pushCharEvent('a');

	CHECK(state->getMousePositionEvents().size() == 1);
	CHECK(state->getMouseClickEvents().size() == 1);
	CHECK(state->isKeyPressed(65));

	state->clearEvents();
	CHECK(state->getMousePositionEvents().empty());
	CHECK(state->getKeyboardEvents().empty());

	state.reset();
}
