#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include <chrono>

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
		
	static void increaseFrameCounter() { frameCounter++; }
	static long getFrameCounterValue() { return frameCounter; }
	static void clearFrameCounter() { frameCounter = 0; }

	typedef std::chrono::steady_clock::time_point TimePoint;
	static void setStartTime() { startTime = std::chrono::steady_clock::now(); }
	static TimePoint getStartTime() { return startTime; }

private:
	static ConsoleMemory *consoleMemory;
	static SceneManager *sceneManager;
	static MouseManager *mouseManager;
	static UIManager *uiManager;
	static double currentTime;
	static bool hdrToggle;

	static long frameCounter;
	static TimePoint startTime;
};

#endif