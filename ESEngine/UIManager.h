#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <memory>
#include <vector>
#include "UIComponent.h"

#include "DepthFramePreviewComponent.h"
#include "ConsoleComponent.h"

using namespace std;

class UIManager {
public:
	UIManager();

	void addComponent(unique_ptr<UIComponent> component);
	void draw();
	void update(double &dt, InputState &inputState);

private:
	vector<unique_ptr<UIComponent>> uiComponents;
};

#endif