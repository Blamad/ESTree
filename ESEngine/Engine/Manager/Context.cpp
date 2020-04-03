#include "Context.h"

SceneManager *Context::sceneManager = nullptr;
MouseManager *Context::mouseManager = nullptr;
UIManager *Context::uiManager = nullptr;
ConsoleMemory *Context::consoleMemory = nullptr;
double Context::currentTime = 0;
bool Context::hdrToggle = true;
long Context::frameCounter;
boost::posix_time::ptime Context::startTime;
