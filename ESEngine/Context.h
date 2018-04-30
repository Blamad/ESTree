#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

class SceneManager;
class MouseManager;
class UIManager;
class ConsoleComponent;

class Context {
public:
	//static ConsoleComponent* getConsoleComponent() { return consoleComponent; }
	static SceneManager* getSceneManager() { return sceneManager; }
	static MouseManager* getMouseManager() { return mouseManager; }
	static UIManager* getUIManager() { return uiManager; }

	//static void setConsoleComponent(ConsoleComponent *console) { consoleComponent = console; }
	static void setSceneManager(SceneManager *scene) { sceneManager = scene; }
	static void setMouseManager(MouseManager *mouse) { mouseManager = mouse; }
	static void setUIManager(UIManager *ui) { uiManager = ui; }

private:
	//static ConsoleComponent *consoleComponent;
	static SceneManager *sceneManager;
	static MouseManager *mouseManager;
	static UIManager *uiManager;
};

#endif