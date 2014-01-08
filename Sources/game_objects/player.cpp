#include "precomp.h"
#include "game_object_types.h"
#include "game_object_manager.h"
#include "game_object.h"
#include "player.h"
#include "../net/message.h"

clan::Sprite Player::m_rw, Player::m_lw, Player::m_uw, Player::m_dw;

Player::Player(uint32_t guid): GameObject(type(),guid)
{
	keys=add_property<uint32_t>("keys",0);

	clan::Contour contour;
	contour.get_points().push_back(clan::Pointf(0,0));
	contour.get_points().push_back(clan::Pointf(0,64));
	contour.get_points().push_back(clan::Pointf(64,64));
	contour.get_points().push_back(clan::Pointf(64,0));

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

bool Player::preload(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_rw = clan::Sprite::resource(canvas, "champ_rw", resources );
	m_lw = clan::Sprite::resource(canvas, "champ_lw", resources );
	m_uw = clan::Sprite::resource(canvas, "champ_uw", resources );
	m_dw = clan::Sprite::resource(canvas, "champ_dw", resources );

	return true;
}

void Player::free()
{
	m_rw = clan::Sprite();
	m_lw = clan::Sprite();
	m_uw = clan::Sprite();
	m_dw = clan::Sprite();
}

void Player::load(clan::Canvas & canvas, clan::ResourceManager & resources)
{
	m_sprite=m_dw;
	clan::Console::write_line("is null: %1",m_sprite.is_null());
}

void Player::update(const clan::GameTime & time)
{
	if(!m_sprite.is_null())
		m_sprite.update(time.get_time_elapsed_ms());

	if(keys&EUIKT_MOVE_LEFT)
	{
		m_sprite=m_lw;
		clan::vec2f v=m_pos;
		v.x-=32.0f * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
	}
	if(keys&EUIKT_MOVE_RIGHT)
	{
		m_sprite=m_rw;
		clan::vec2f v=m_pos;
		v.x+= 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
	}
	if(keys&EUIKT_MOVE_UP)
	{
		m_sprite=m_uw;
		clan::vec2f v=m_pos;
		v.y-= 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
	}
	if(keys&EUIKT_MOVE_DOWN)
	{
		m_sprite=m_dw;
		clan::vec2f v=m_pos;
		v.y+= 32.0f * (float)time.get_time_elapsed_ms()/900.0f;
		m_pos.set(v);
	}

	m_outline.set_translation(m_pos.get().x,m_pos.get().y);
}

void Player::render(clan::Canvas & c, const clan::vec2 & offset)
{
	if(!m_sprite.is_null())
	{
		m_sprite.draw(c, m_pos.get().x+offset.x, m_pos.get().y+offset.y);
		m_outline.draw(offset.x,offset.y,clan::Colorf(1,0,0,1),c);
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

}

clan::CollisionOutline & Player::get_outline()
{
	return m_outline;
}