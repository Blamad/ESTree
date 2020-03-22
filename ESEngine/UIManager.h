#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <memory>
#include <vector>
#include "UIComponent.h"

#include "DepthFramePreviewComponent.h"
#include "ConsoleComponent.h"
#include "Context.h"

class UIManager {
public:
	UIManager();
	~UIManager();

	void toggleDepthBufferComponent(bool enabled);

	void addComponent(std::unique_ptr<UIComponent> component);
	void draw();
	void update(double &dt, InputState &inputState);

private:
	DepthFramePreviewComponent *depthPreviewComponent;
	std::vector<std::unique_ptr<UIComponent>> uiComponents;
};

#endif