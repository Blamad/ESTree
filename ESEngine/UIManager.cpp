#include "UIManager.h"

UIManager::UIManager() {
	addComponent(make_unique<DepthFramePreviewComponent>());
	addComponent(make_unique<ConsoleComponent>());
}

void UIManager::addComponent(unique_ptr<UIComponent> component) {
	uiComponents.push_back(move(component));
}

void UIManager::update(double &dt, InputState &inputState) {
	for (auto & component : uiComponents) {
		component->update(dt, inputState);
	}
}

void UIManager::draw() {
	for (auto & component : uiComponents) {
		component->draw();
	}
}