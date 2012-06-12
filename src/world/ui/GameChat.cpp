/*
 * GameChat.cpp
 *
 *  Created on: May 27, 2012
 *      Author: 100397561
 */

#include <SDL.h>

#include <net/packet.h>

#include "../../data/game_data.h"

#include "../../display/display.h"

#include "../../procedural/enemygen.h"

#include "../objects/PlayerInst.h"

#include "../GameState.h"

#include "GameChat.h"

static void print_dupe_string(const ChatMessage& cm, const font_data& font,
		const Pos& location, float alpha) {
	if (cm.exact_copies > 1)
		gl_printf(font, Colour(0, 191, 255, alpha * 255), location.x,
				location.y, " x%d", cm.exact_copies);
}

void ChatMessage::draw(const font_data& font, float alpha, int x, int y) const {
	Colour sendcol = sender_colour, msgcol = message_colour;
	sendcol.a *= alpha, msgcol.a *= alpha;
	Pos offset(0, 0);
	if (!sender.empty()) {
		offset = gl_printf(font, sendcol, x, y, "%s: ", sender.c_str());
		x += offset.x;
	}
	offset = gl_printf(font, msgcol, x, y, message.c_str());
	x += offset.x;
	print_dupe_string(*this, font, Pos(x, y), alpha);
}

bool ChatMessage::empty() const {
	return sender.empty() && message.empty();
}

void GameChat::add_message(const ChatMessage& cm) {
	bool dupe = false;

	if (!messages.empty()) {
		if (messages.back() == cm) {
			messages.back().exact_copies++;
			dupe = true;
		}
	}

	if (!dupe)
		messages.push_back(cm);

	fade_out = 1.0f;
	fade_out_rate = 0.005f;
}

void GameChat::add_message(const std::string& msg, const Colour& colour) {
	add_message(ChatMessage("", msg, Colour(), colour));
}

bool GameChat::is_typing_message() {
	return is_typing;
}

void GameChat::clear() {
	messages.clear();
}

void GameChat::draw_player_chat(GameState* gs) const {
	const font_data& font = gs->primary_font();
	const int padding = 5;
	int line_sep = font.h + 2;

	int view_w = gs->window_view().width, view_h = gs->window_view().height;
	int chat_w = view_w, chat_h = 100;
	int chat_x = 0, chat_y = 0; //h - chat_h - TILE_SIZE;
	int text_x = chat_x + padding, text_y = chat_y + padding;

	gl_set_drawing_area(0, 0, view_w, view_h);

	gl_draw_rectangle(chat_x, chat_y, chat_w, chat_h,
			Colour(180, 180, 255, 50 * fade_out));

	bool draw_typed_message = is_typing || !typed_message.empty();

	int start_msg = 0;
	int message_space = chat_h - padding * 2
			- (draw_typed_message ? line_sep : 0);
	int msgs_in_screen = message_space / line_sep;
	if (messages.size() > msgs_in_screen) {
		start_msg = messages.size() - msgs_in_screen;
	}

	for (int i = start_msg; i < messages.size(); i++) {
		messages[i].draw(font, fade_out, text_x, text_y);
		text_y += line_sep;
	}

	if (draw_typed_message) {
		int type_y = chat_y + chat_h - padding - line_sep;
		gl_draw_line(chat_x, type_y, chat_x + chat_w, type_y,
				Colour(200, 200, 200, fade_out * 180));
		typed_message.draw(font, fade_out, text_x, type_y + padding - 1);
	}
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

void GameChat::step(GameState *gs) {
	std::string& msg = typed_message.message;

	if (show_chat)
		fade_out = 1.0f;
	else if (fade_out > 0.0f)
		fade_out -= fade_out_rate;

	if (repeat_steps_left > 0)
		repeat_steps_left--;
	else if (current_key != SDLK_FIRST) {
		/*Handle keys being held down*/
		if (is_typeable_keycode(current_key)) {
			msg += keycode_to_char(current_key, current_mod);
			repeat_steps_left = NEXT_REPEAT_STEP_AMNT;
		} else if (current_key == SDLK_BACKSPACE) {
			if (msg.empty()) {
				reset_typed_message();
				is_typing = false;
			} else {
				msg.resize(msg.size() - 1);
			}
			repeat_steps_left = NEXT_BACKSPACE_STEP_AMNT;
		}
	}
}
void GameChat::draw(GameState *gs) const {
	if (fade_out > 0.0f)
		draw_player_chat(gs);
}

static bool starts_with(const std::string& str, const char* prefix,
		const char** content) {
	int length = strlen(prefix);
	bool hasprefix = strncmp(str.c_str(), prefix, length) == 0;
	if (hasprefix) {
		*content = str.c_str() + length;
		return true;
	}
	return false;
}

bool GameChat::handle_special_commands(GameState* gs,
		const std::string& command) {
	ChatMessage printed;
	const char* content;
	PlayerInst* p = (PlayerInst*)gs->get_instance(gs->local_playerid());

	//Spawn monster
	if (starts_with(command, "!spawn ", &content)) {
		int enemy = get_enemy_by_name(content, false);
		if (enemy == -1) {
			printed.message = "No such monster, '" + std::string(content) + "'!";
			printed.message_colour = Colour(255, 50, 50);
		} else {
			printed.message = std::string(content) + " has spawned !";
			post_generate_enemy(gs, enemy);
			printed.message_colour = Colour(50, 255, 50);
		}
		add_message(printed);
		return true;
	}

	//Gain XP
	if (starts_with(command, "!gainxp ", &content)) {
		int xp = atoi(content);
		if (xp > 0 && xp < 50000){
			printed.message = std::string("You have gained ") + content + " experience.";
			printed.message_colour = Colour(255, 50, 50);
			p->gain_xp(gs, xp);
		} else {
			printed.message = "Invalid experience amount!";
			printed.message_colour = Colour(255, 50, 50);
		}
		add_message(printed);
		return true;
	}

	//Gain XP
	if (starts_with(command, "!item ", &content)) {
		char* rest = (char*)content;
		int amnt = strtol(content, &rest, 10);
		if (content == rest) amnt = 1;
		int item = get_item_by_name(content, false);
		if (item == -1) {
			printed.message = "No such item, '" + std::string(content) + "'!";
			printed.message_colour = Colour(255, 50, 50);
		} else {
			printed.message = std::string(content) + " put in your inventory !";
			p->stats().equipment.inventory.add(Item(item), amnt);
			post_generate_enemy(gs, item);
			printed.message_colour = Colour(50, 255, 50);
		}
		add_message(printed);
		return true;
	}

	return false;
}
void GameChat::toggle_chat(GameState* gs) {
	if (is_typing) {
		if (!typed_message.message.empty()) {
			typed_message.sender = local_sender;
			typed_message.sender_colour = Colour(37, 207, 240);
			const char* content = NULL;
			if (!handle_special_commands(gs, typed_message.message)) {
				add_message(typed_message);
			}
		} else {
			show_chat = false;
			fade_out_rate = 0.1f;
		}
		reset_typed_message();
		is_typing = false;
	} else {
		if (!show_chat)
			show_chat = true;
		else if (show_chat)
			is_typing = true;
	}
}
/*Returns whether has handled event completely or not*/
bool GameChat::handle_event(GameState* gs, SDL_Event *event) {
	int view_w = gs->window_view().width, view_h = gs->window_view().height;
	int chat_w = view_w, chat_h = 100;
	int chat_x = 0, chat_y = 0; //h - chat_h - TILE_SIZE;

	SDLKey keycode = event->key.keysym.sym;
	SDLMod keymod = event->key.keysym.mod;
	current_mod = keymod;
	switch (event->type) {
	case SDL_MOUSEBUTTONDOWN: {
		if (show_chat && event->button.button == SDL_BUTTON_LEFT
				&& gs->mouse_x() < chat_w && gs->mouse_y() < chat_h) {
			toggle_chat(gs);
			return true;
		}
		break;
	}
	case SDL_KEYUP: {
		if (current_key == keycode)
			current_key = SDLK_FIRST;
		/*Let GameState handle this as well*/
		break;
	}
	case SDL_KEYDOWN: {
		if (keycode == SDLK_RETURN) {
			toggle_chat(gs);
			return true;
		}
		if (is_typing) {
			std::string& msg = typed_message.message;
			if (is_typeable_keycode(keycode)) {
				msg += keycode_to_char(keycode, keymod);
				if (current_key != keycode) {
					current_key = keycode;
					repeat_steps_left = INITIAL_REPEAT_STEP_AMNT;
				}
				return true;
			}
			if (keycode == SDLK_BACKSPACE) {
				if (msg.empty()) {
					reset_typed_message();
					is_typing = false;
				} else {
					msg.resize(msg.size() - 1);
				}
				if (current_key != keycode) {
					current_key = keycode;
					repeat_steps_left = INITIAL_REPEAT_STEP_AMNT;
				}
				return true;
			}
			if (keycode == SDLK_LCTRL || keycode == SDLK_RCTRL) {
				is_typing = false;
				return true;
			}
			if (keycode == SDLK_DELETE) {
				reset_typed_message();
				is_typing = false;
				return true;
			}
		}
		break;
	}
	}
	return false;
}
void GameChat::reset_typed_message() {
	typed_message.sender.clear();
	typed_message.message.clear();
}
GameChat::GameChat(const std::string& local_sender) :
		local_sender(local_sender), typed_message(std::string(), std::string()) {
	current_key = SDLK_UNKNOWN;
	current_mod = KMOD_NONE;
	reset_typed_message();
	show_chat = true;
	fade_out = 1.0f;
	fade_out_rate = 0.05f;
	is_typing = false;
//	messages.push_back(
//			ChatMessage("ludamad", "What's up!?\nGo eff off",
//					Colour(37, 207, 240)));
//	messages.push_back(ChatMessage("ciribot", "nm u", Colour(255, 69, 0)));

//	char buff[40];
//	for (int i = 0; i < 14; i++){
//		snprintf(buff, 40, "Message %d\n", i);
//		this->add_message(buff);
//	}
}

void packet_get_str(NetPacket& packet, std::string& str) {
	int size = packet.get_int();
	str.resize(size);
	packet.get_str(&str[0], size);
}
void ChatMessage::packet_add(NetPacket& packet) {
	packet.add_str(sender.c_str(), sender.size());
	packet.add_str(message.c_str(), message.size());
	packet.add(sender_colour);
	packet.add(message_colour);
}
void ChatMessage::packet_get(NetPacket& packet) {
	packet_get_str(packet, sender);
	packet_get_str(packet, message);
	packet.get(sender_colour);
	packet.get(message_colour);
}

