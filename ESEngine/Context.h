#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

class SceneManager;
class MouseManager;
class UIManager;
class ConsoleMemory;

class Context {
public:
	static ConsoleMemory* getConsoleMemory() { return consoleMemory; }
	static SceneManager* getSceneManager() { return sceneManager; }
	static MouseManager* getMouseManager() { return mouseManager; }
	static UIManager* getUIManager() { return uiManager; }
	static double getTime() { return currentTime; }
	static bool getHdrToggle() { return hdrToggle; }

	static void setConsoleMemory(ConsoleMemory *console) { consoleMemory = console; }
	static void setSceneManager(SceneManager *scene) { sceneManager = scene; }
	static void setMouseManager(MouseManager *mouse) { mouseManager = mouse; }
	static void setUIManager(UIManager *ui) { uiManager = ui; }
	static void setTime(double time) { currentTime = time; }
	static void setHdrToggle(bool isOn) { hdrToggle = isOn; }

private:
	static ConsoleMemory *consoleMemory;
	static SceneManager *sceneManager;
	static MouseManager *mouseManager;
	static UIManager *uiManager;
	static double currentTime;
	static bool hdrToggle;
};

#endif