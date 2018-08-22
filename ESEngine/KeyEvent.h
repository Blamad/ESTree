#ifndef KEY_EVENT_H
#define KEY_EVENT_H

enum KeyState {
	PRESSED, RELEASED, REPEAT
};

class KeyEvent {
public:
	KeyEvent(int key, KeyState state) : key(key), state(state) {};
	int key;
	KeyState state;
};

#endif