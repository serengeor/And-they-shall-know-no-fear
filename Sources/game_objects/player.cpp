#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_object.h"
#include "player.h"
#include "throwable_object.h"
#include "../net/message.h"

clan::Sprite Player::s_rw, 
			 Player::s_lw, 
			 Player::s_uw, 
			 Player::s_dw,
			 Player::s_h;

Player::Player(uint32_t guid): GameObject(type(),guid)
{
	keys=add_property<uint32_t>("keys",0);
	life=add_property<uint32_t>("life",100);
	name=add_property<std::string>("name");
	vel =add_property<float>("vel",41);

	clan::Contour contour;
	contour.get_points().push_back(clan::Pointf(16,13));
	contour.get_points().push_back(clan::Pointf(16,63));
	contour.get_points().push_back(clan::Pointf(47,63));
	contour.get_points().push_back(clan::Pointf(47,13));

	m_outline.get_contours().push_back(contour);
	m_outline.calculate_radius();
	m_outline.calculate_smallest_enclosing_discs();
	
	m_next_attack_time = 0;
}

Player::~Player()
{

}

uint32_t Player::get_next_attack_time()
{
	return m_next_attack_time;
}

void Player::set_next_attack_time(uint32_t time)
{
	m_next_attack_time = time;
}

clan::Colorf Player::get_hp_bar_color()
{
	float r,g;
	r = 1.0f - (float)life/100;
	g = (float)life/100;
	return clan::Colorf(r,g,0.0f,1.0f);
}

bool Player::preload(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	s_rw = clan::Sprite::resource(canvas, "champ_rw", resources );
	s_lw = clan::Sprite::resource(canvas, "champ_lw", resources );
	s_uw = clan::Sprite::resource(canvas, "champ_uw", resources );
	s_dw = clan::Sprite::resource(canvas, "champ_dw", resources );

	s_h = clan::Sprite::resource(canvas, "hp_bar", resources );

	return true;
}

void Player::free()
{
	s_rw = clan::Sprite();
	s_lw = clan::Sprite();
	s_uw = clan::Sprite();
	s_dw = clan::Sprite();

	s_h = clan::Sprite();
}

void Player::load(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_rw = s_rw.clone();
	m_lw = s_lw.clone();
	m_uw = s_uw.clone();
	m_dw = s_dw.clone();

	m_h = s_h.clone();

	m_sprite=m_dw;
	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void Player::update(const clan::GameTime & time)
{
	if(!m_sprite.is_null())
		m_sprite.update(time.get_time_elapsed_ms());
	//m_pos.data().x += 16.0f * (float)time.get_time_elapsed_ms()/1000.0f;


	if(keys&EUIKT_MOVE_LEFT)
	{
		m_sprite=m_lw;

		#if defined GAME_SERVER
		clan::vec2f v=m_pos;
		v.x-= vel * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
		#endif
	}
	if(keys&EUIKT_MOVE_RIGHT)
	{
		m_sprite=m_rw;
		#if defined GAME_SERVER
		clan::vec2f v=m_pos;
		v.x+= vel * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
		#endif
	}
	if(keys&EUIKT_MOVE_UP)
	{
		m_sprite=m_uw;
		#if defined GAME_SERVER
		clan::vec2f v=m_pos;
		v.y-= vel * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
		#endif
	}
	if(keys&EUIKT_MOVE_DOWN)
	{
		m_sprite=m_dw;
		#if defined GAME_SERVER
		clan::vec2f v=m_pos;
		v.y+= vel * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
		#endif
	}

	m_outline.set_translation(m_pos.get().x,m_pos.get().y);
}

void Player::render(clan::Canvas & c, const clan::vec2 & offset)
{
	if(!m_sprite.is_null())
	{
		m_sprite.draw(c, m_pos.get().x+offset.x, m_pos.get().y+offset.y);
		c.fill_rect(m_pos.get().x+offset.x+10, m_pos.get().y+offset.y-1,  m_pos.get().x+offset.x+10+(44*life/100), m_pos.get().y+offset.y+8,  get_hp_bar_color());
		m_h.draw(c,m_pos.get().x+offset.x, m_pos.get().y+offset.y-5);
		m_outline.draw(offset.x,offset.y,clan::Colorf(1,0,0,1),c);
		
		static clan::Font title(c,"Ariel",12);
		title.draw_text(c, m_pos.get().x+offset.x,m_pos.get().y+offset.y-10, name, clan::Colorf(0.0f,0.0f,1.0f));
	}
}

void Player::on_message(const Message & msg)
{
	if(msg.get_type()==MSGC_INPUT)
	{
		const MSGC_Input & input = static_cast<const MSGC_Input &>(msg);
		keys = input.keys;
		clan::log_event("input_ev","keys: %1;", keys);
	}
}

void Player::on_collide(GameObject * obj)
{
	if(obj->is_alive() && obj->get_type()==EGOT_THROWABLE_OBJECT)
	{
		if(static_cast<ThrowableObject*>(obj)->get_owner_guid()==this->get_guid())
			return;

		obj->set_is_alive(false);

		if(this->life>10)
		{
			this->life = this->life - 10;
		}
		else
		{
			this->life=0;
		}
	}
}

clan::CollisionOutline & Player::get_outline()
{
	return m_outline;
}