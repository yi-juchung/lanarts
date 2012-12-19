/*
 * TextField.cpp:
 *  Interface component for entering text.
 *  This includes stuff like entering name before play, etc.
 */

#include <SDL.h>

#include "TextField.h"

/*Handle key repeating, in milliseconds*/
const int INITIAL_REPEAT_STEP_AMNT = 560;
const int NEXT_REPEAT_STEP_AMNT = 70;
const int NEXT_BACKSPACE_STEP_AMNT = 42;
const int NO_REPEAT_AMNT = -1;

TextField::TextField(int max_length, const std::string & default_text) :
		_max_length(max_length), _text(default_text) {
	clear_keystate();
}

void TextField::clear_keystate() {
	_current_key = SDLK_UNKNOWN;
	_current_mod = KMOD_NONE;
	_repeat_cooldown = NO_REPEAT_AMNT;
}
void TextField::clear() {
	_text.clear();
	clear_keystate();
}

static char keycode_to_char(SDLKey keycode, SDLMod keymod) {
	const char DIGIT_SYMBOLS[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*',
			'(' };
	const char MISC_SYMBOLS[][2] = { { '`', '~' }, { ',', '<' }, { '.', '>' }, {
			'/', '?' }, { ';', ':' }, { '\'', '"' }, { '[', '{' }, { ']', '}' },
			{ '\\', '|' }, { '-', '_' }, { '=', '+' } };
	bool hitcaps = (keymod & KMOD_CAPS);
	bool hitshift = (keymod & (KMOD_LSHIFT | KMOD_RSHIFT));
	if ((hitcaps != hitshift) && isalpha(keycode)) {
		return toupper(keycode);
	} else if (hitshift) {
		if (isdigit(keycode))
			return DIGIT_SYMBOLS[keycode - '0'];
		for (int i = 0; i < sizeof(MISC_SYMBOLS) / sizeof(char) / 2; i++) {
			if (keycode == MISC_SYMBOLS[i][0])
				return MISC_SYMBOLS[i][1];
		}
	}
	return keycode;
}
static bool is_typeable_keycode(SDLKey keycode) {
	return (keycode >= SDLK_SPACE && keycode <= SDLK_z);
}

bool TextField::_has_repeat_cooldown() {
	int ms = _repeat_timer.get_microseconds() / 1000;
	bool has_cooldown = ms < _repeat_cooldown;
	return has_cooldown;
}

void TextField::_reset_repeat_cooldown(int cooldownms) {
	_repeat_timer.start();
	_repeat_cooldown = cooldownms;

}
/*Returns whether has handled event*/
void TextField::step() {
	if (!_has_repeat_cooldown() && _current_key != SDLK_FIRST) {
		/*Handle keys being held down*/
		if (is_typeable_keycode(_current_key)) {
			if (_text.size() < _max_length) {
				_text += keycode_to_char(_current_key, _current_mod);
			}
			_reset_repeat_cooldown(NEXT_REPEAT_STEP_AMNT);
		} else if (_current_key == SDLK_BACKSPACE) {
			if (!_text.empty()) {
				_text.resize(_text.size() - 1);
			}
			_reset_repeat_cooldown(NEXT_BACKSPACE_STEP_AMNT);
		}
	}
}

bool TextField::handle_event(SDL_Event* event) {
	SDLKey keycode = event->key.keysym.sym;
	SDLMod keymod = event->key.keysym.mod;
	_current_mod = keymod;
	switch (event->type) {
	case SDL_KEYUP: {
		/*Since the key-up isnt truly an action, we respond but pretend we didn't handle it*/
		if (_current_key == keycode) {
			_current_key = SDLK_FIRST;
		}
		break;
	}
	case SDL_KEYDOWN: {
		if (is_typeable_keycode(keycode)) {
			if (_text.size() < _max_length) {
				_text += keycode_to_char(keycode, keymod);
			}
			if (_current_key != keycode) {
				_current_key = keycode;
				_reset_repeat_cooldown(INITIAL_REPEAT_STEP_AMNT);
			}
			return true;
		}
		if (keycode == SDLK_BACKSPACE) {
			if (!_text.empty()) {
				_text.resize(_text.size() - 1);
			}
			if (_current_key != keycode) {
				_current_key = keycode;
				_reset_repeat_cooldown(INITIAL_REPEAT_STEP_AMNT);
			}
			return true;
		}
		if (keycode == SDLK_DELETE) {
			_text.clear();
			return true;
		}
		break;
	}
	}
	return false;
}
