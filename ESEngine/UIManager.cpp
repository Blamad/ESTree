#include "UIManager.h"

UIManager::UIManager() {
	addComponent(std::make_unique<DepthFramePreviewComponent>());
	depthPreviewComponent = (DepthFramePreviewComponent*)uiComponents[0].get();
	addComponent(std::make_unique<ConsoleComponent>());

	Context::setUIManager(this);
}

UIManager::~UIManager() {
	for (auto & component : uiComponents) {
		component.release();
	}
}

void UIManager::toggleDepthBufferComponent(bool enabled) {
	if (depthPreviewComponent != nullptr)
		depthPreviewComponent->enabled = enabled;
}

void UIManager::addComponent(std::unique_ptr<UIComponent> component) {
	uiComponents.push_back(std::move(component));
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