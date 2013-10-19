#include "precomp.h"
#include "editor.h"
#include "tile_chunk.h"
#include "sprite_selection.h"

editor::editor(clan::DisplayWindow &display_window)
{
	m_window = display_window;
	m_sprite_selection = nullptr;
	m_selected_frame = -1;
	m_selected_layer = 0;
}

editor::~editor()
{

}

void editor::init_gui()
{
	m_window_manager = clan::GUIWindowManagerDirect(m_window, m_canvas);
	m_gui_manager = clan::GUIManager(m_window_manager, "Gfx/gui/aero");
	m_gui_root = new clan::GUIComponent(&m_gui_manager, clan::GUITopLevelDescription(clan::Rect(0,0,1024,720),true),"rootx");

	m_button_selection_window = new clan::Window(m_gui_root);
	m_button_selection_window->set_geometry(clan::Rect(50,100,clan::Size(150,200)));
	m_button_selection_window->set_visible(false);

	m_button_sprite = new clan::PushButton(m_button_selection_window);
	m_button_sprite->set_geometry(clan::Rect( 10, 60, clan::Size(80, 25)));
	m_button_sprite->func_clicked().set(this, &editor::on_button_clicked, m_button_sprite);
	m_button_sprite->set_text("Select frame");

	m_combo_layer = new clan::ComboBox(m_button_selection_window);
	m_combo_layer->set_geometry(clan::Rect( 10, 30, clan::Size(80, 25)));

	uint32_t i;
	for(i=0;i<GROUND_LAYER_COUNT;i++) 
		m_combo_menu_layer.insert_item("Ground "+clan::StringHelp::uint_to_text(i+1),i,i);
	m_combo_menu_layer.insert_separator(i);
	for(i++;i<GROUND_LAYER_COUNT+OBJECT_LAYER_COUNT+1;i++)
		m_combo_menu_layer.insert_item("Object "+clan::StringHelp::uint_to_text(i),i-1,i);
	m_combo_layer->set_popup_menu(m_combo_menu_layer);
	m_combo_layer->set_text("Select layer");

	m_combo_layer->func_item_selected().set(this,&editor::on_layer_select);

	//toolbar goes here
	/*m_ribbon = new clan::Ribbon(m_gui_root);
	m_ribbon->set_geometry(clan::Rect(0, 0, clan::Size(m_gui_root->get_content_box().get_width()/4, m_gui_root->get_content_box().get_height())));
	m_ribbon->get_menu()->add_item(clan::Image(m_canvas,"Gfx/gui/aero/Images/Ribbon/Tab.png"),"Labas1");
	m_ribbon->get_menu()->add_item(clan::Image(m_canvas,"Gfx/gui/aero/Images/Ribbon/Tab.png"),"Labas");*/

	

	m_sprite_selection_window = new clan::Window(m_gui_root);
	m_sprite_selection_window->set_geometry(clan::Rect(200,100,clan::Size(500,500)));
	m_sprite_selection_window->set_visible(false);
	
	m_sprite_selection = new SpriteSelection(m_sprite_selection_window);
	m_frame_select = m_sprite_selection->func_frame_selected().connect(this,&editor::on_frame_select);

	m_gui_root->update_layout();
}

void editor::init_level()
{
	m_tile_map = TileMap(m_canvas);
	m_tile_map.add_sprite(clan::Sprite::resource(m_canvas,"level_gfx",m_resources),0);
	m_tile_map.load("Level/Level.map");
}

bool editor::init()
{
	m_run = true;
	m_canvas = clan::Canvas(m_window);
	
	// Setup resources
	m_resources = clan::XMLResourceManager::create(clan::XMLResourceDocument("resources.xml"));
	
	m_key_up = m_window.get_ic().get_keyboard().sig_key_up().connect(this, &editor::on_input);
	m_mouse_click = m_window.get_ic().get_mouse().sig_key_up().connect(this, &editor::on_input);
	m_mouse_move = m_window.get_ic().get_mouse().sig_pointer_move().connect(this, &editor::on_input);

	init_level();
	init_gui();
	
	return true;
}

void editor::edge_pan(const clan::vec2 & pos)
{
	int32_t sens=15;

	if (pos.x < 30) m_pan.x=-sens;
	else if (pos.x > m_window.get_viewport().get_width()-30) m_pan.x=sens;
	else m_pan.x=0;

	if (pos.y < 30) m_pan.y=-sens;
	else if (pos.y > m_window.get_viewport().get_height()-30) m_pan.y=sens;
	else m_pan.y=0;
}

void editor::draw_world_axis()
{
	int32_t w = m_window.get_viewport().get_width(), h = m_window.get_viewport().get_height();

	//chunk and tile separators
	for(int32_t i=0; i<=w; i++)
		if((i+m_pos.x)%CHUNK_EDGE_LENGTH_PIXELS==0) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(i,0),clan::vec2(i,h)),clan::Colorf::yellow);
		else if((i+m_pos.x)%TILE_SIZE==0) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(i,0),clan::vec2(i,h)),clan::Colorf::blue);

	for(int32_t i=0; i<=h; i++)
		if((i+m_pos.y)%CHUNK_EDGE_LENGTH_PIXELS==0) 
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,i),clan::vec2(w,i)),clan::Colorf::yellow);
		else if((i+m_pos.y)%TILE_SIZE==0)
			m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,i),clan::vec2(w,i)),clan::Colorf::blue);
		
	//origin axis
	if(m_pos.x<=w && m_pos.x+w>=0) m_canvas.draw_line(clan::LineSegment2f(clan::vec2(-m_pos.x,0),clan::vec2(-m_pos.x,h)),clan::Colorf::red);
	if(m_pos.y<=h && m_pos.y+h>=0) m_canvas.draw_line(clan::LineSegment2f(clan::vec2(0,-m_pos.y),clan::vec2(w,-m_pos.y)),clan::Colorf::red);
}

bool editor::run()
{
	if(m_run)
	{
		m_game_time.update();
		m_canvas.clear();

		m_pos+=m_pan;
		m_tile_map.render(m_pos);
		draw_world_axis();

		///render gui
		m_gui_manager.process_messages(0);
		m_gui_manager.render_windows();
		m_window_manager.get_canvas(NULL).flush();

		m_window.flip(1);
		clan::KeepAlive::process();
	}

	return m_run;
}

bool editor::pause()
{
	State::pause();

	m_key_up.disable();
	return true;
}
bool editor::resume()
{
	State::resume();

	m_key_up.enable();
	return true;
}
bool editor::exit()
{
	m_tile_map.save("Level/Level.map");
	m_key_up.destroy();
	
	return true;
}

void editor::on_frame_select(int32_t frame)
{
	//Tile & t = m_tile_map.get_chunk(clan::vec2(0,0)).get_tile(clan::vec2(0,0),0);
	m_selected_frame = frame;
}

void editor::on_layer_select(int32_t layer)
{
	clan::Console::write_line("Selected layer:%1",layer); //DEBUG
	m_selected_layer = layer;
}

void editor::on_input(const clan::InputEvent & e)
{
	switch(e.device.get_type())
	{
		case clan::InputDevice::keyboard:
		{
			
			if(e.id == clan::keycode_escape)
				m_run = false;
			else if (e.id == clan::keycode_d)
				clan::Console::write_line("x:%1 y:%2", m_pos.x,m_pos.y); //DEBUG
			else if (e.id == clan::keycode_e)
				m_button_selection_window->set_visible(!m_button_selection_window->is_visible());
			break;
		}
		case clan::InputDevice::pointer:
		{
			if(m_gui_root->get_component_at(e.mouse_pos)!=m_gui_root) break;

			if (e.id == clan::mouse_left)
				change_tile_sprite(e.mouse_pos);
			else if (e.id == clan::mouse_right)
			{
				change_tile_sprite(e.mouse_pos,true);
			}
			if (e.type == clan::InputEvent::pointer_moved)
			{
				edge_pan(e.mouse_pos);
			}
			break;
		}
	}
}

void editor::change_tile_sprite(const clan::vec2 & pos, bool remove)
{
	clan::vec2 chunk_pos=pixel_to_chunk_pos(pos+m_pos);
	clan::vec2 tile_pos=pixel_to_tile_pos(pos+m_pos);
	clan::Console::write_line("chunk: x:%1 y:%2\ntile: x:%3 y:%4",chunk_pos.x, chunk_pos.y, tile_pos.x, tile_pos.y); //DEBUG

	TileChunk c=m_tile_map.get_chunk(chunk_pos);
	if(c.is_null()) 
		c = m_tile_map.add_chunk(chunk_pos);

	if(remove)
			c.get_tile(tile_pos,m_selected_layer).type=ETT_NO_TILE;
	else if(m_selected_frame!=-1 && m_selected_layer!=-1)
	{
		c.get_tile(tile_pos,m_selected_layer).type=ETT_NORMAL;
		c.get_tile(tile_pos,m_selected_layer).sprite_frame=m_selected_frame;
	}
}

void editor::on_button_clicked(clan::PushButton * btn)
{
	if(btn==m_button_sprite)
	{
		m_sprite_selection->set_sprite(m_tile_map.get_sprite(0));
		m_sprite_selection_window->set_visible(!m_sprite_selection_window->is_visible());
	}
}
