#pragma once

#include "state.h"
#include "TileMap.h"

class editor: public State
{
public:
	editor(clan::DisplayWindow &display_window);
	~editor();

	bool init();
	bool run();
	bool exit();

	bool pause();
	bool resume();

protected:
	///event funcs
<<<<<<< HEAD
	void on_key_up(const clan::InputEvent & e);
=======
	void on_input(const InputEvent & e);
>>>>>>> origin/tile_editor

	///game specific funcs
	void init_level();

protected:
	bool m_run;
	clan::Slot m_key_up;
	clan::Slot m_mouse_click;

	clan::ResourceManager	m_resources;
	clan::DisplayWindow		m_window;
	clan::Canvas			m_canvas;
	clan::GameTime			m_game_time;

	///Kameros pozicija
	clan::vec2 m_pos;

	///game
	TileMap m_tile_map;
};
