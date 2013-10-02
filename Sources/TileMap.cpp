#include "precomp.h"

#include "TileMap.h"
#include "TileChunk.h"

class TileMap_Impl
{
    public:

    TileMap_Impl(clan::Canvas c)
    {
        m_canvas = c;
    }

    bool add_sprite(clan::Sprite spr, uint8_t id)
    {
        if(m_sprites.find(id)==m_sprites.end())
        {
            m_sprites[id]=spr;
            return true;
        }

        return false;
    }

    clan::Sprite get_sprite(uint8_t id)
    {
        auto spr = m_sprites.find(id);
        if(spr!=m_sprites.end())
            return spr->second;

        return clan::Sprite();
    }

    void remove_sprite(uint8_t id)
    {
        m_sprites.erase(id);
    }

    bool is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect)
    {
        clan::Rect chunk_rect(chunk_pos.x,chunk_pos.y, clan::Sizex<int32_t>(CHUNK_EDGE_LENGTH_PIXELS,CHUNK_EDGE_LENGTH_PIXELS));
        return chunk_rect.is_overlapped(render_rect);
    }

    TileChunk add_chunk(TileMap m, const clan::vec2 & pos )
    {
        if(m_chunks.find(pos)==m_chunks.end())
        {
            TileChunk c(m);
            m_chunks[pos]=c;
            std::cout << "Chunk added." << std::endl;
            return c;
        }
        else
            return TileChunk();
    }


    TileChunk get_chunk(const clan::vec2 & pos)
    {
        auto r = m_chunks.find(pos);
        if(r!=m_chunks.end())
        {
            return r->second;
        }

        return TileChunk();
    }

	void erase_chunk( const clan::vec2 & pos )
	{
		m_chunks.erase(pos);
	}

    clan::Canvas & get_canvas()
    {
        return m_canvas;
    }

    void render(const clan::vec2 & pos)
    {
        int w = m_canvas.get_width();
        int h = m_canvas.get_height();

        int rx = (pos.x-CHUNK_EDGE_LENGTH_PIXELS+1)/CHUNK_EDGE_LENGTH_PIXELS;
        int ry = (pos.y-CHUNK_EDGE_LENGTH_PIXELS+1)/CHUNK_EDGE_LENGTH_PIXELS;
        int rx2 = (pos.x+w+CHUNK_EDGE_LENGTH_PIXELS+1)/CHUNK_EDGE_LENGTH_PIXELS;
        int ry2 = (pos.y+h+CHUNK_EDGE_LENGTH_PIXELS+1)/CHUNK_EDGE_LENGTH_PIXELS;
        TileChunk c;

		std::string s = "x1: ";
		s+=clan::StringHelp::int_to_text(rx);

		s+= " y1: ";
		s+=clan::StringHelp::int_to_text(ry);
		
		s+= "\nx2: ";
		s+=clan::StringHelp::int_to_text(rx2);
		
		s+= " y2: ";
		s+=clan::StringHelp::int_to_text(ry2);

        c = get_chunk(clan::vec2(0,0));
        
		for(int y = ry; y < ry2; y++)
		for(int x = ry; x < rx2; x++)
		{
			c = get_chunk(clan::vec2(x,y));

			if(!c.is_null())
			{
				if(!c.is_batched())
					c.batch();

				for(int32_t i = 0; i < LAYER_COUNT; i++)
					c.draw_chunk(m_canvas,clan::vec2(x*CHUNK_EDGE_LENGTH_PIXELS,y*CHUNK_EDGE_LENGTH_PIXELS)-pos,i,false);
			}
		}

		m_font.draw_text(m_canvas,20,20,s,clan::Colorf::white);
    }

    protected:
    clan::Canvas      m_canvas;

	clan::Font		m_font;

    std::map<clan::vec2, TileChunk>  m_chunks;
    std::map<uint8_t, clan::Sprite>           m_sprites;
};

TileMap::TileMap()
{

}

TileMap::TileMap(clan::Canvas & c)
{
    impl=std::shared_ptr<TileMap_Impl>(new TileMap_Impl(c));
}

TileMap::~TileMap(){}

bool TileMap::add_sprite(clan::Sprite spr, uint8_t id)
{
    impl->add_sprite(spr,id);

    return false;
}

clan::Sprite TileMap::get_sprite(uint8_t id)
{
    return impl->get_sprite(id);
}

void TileMap::remove_sprite(uint8_t id)
{
    impl->remove_sprite(id);
}

bool TileMap::is_chunk_visible(const clan::vec2 & chunk_pos, const clan::Rect & render_rect)
{
    return impl->is_chunk_visible(chunk_pos,render_rect);
}

TileChunk TileMap::add_chunk( const clan::vec2 & pos )
{
    return impl->add_chunk(*this,pos);
}


TileChunk TileMap::get_chunk(const clan::vec2 & pos)
{
    return impl->get_chunk(pos);
}

void TileMap::erase_chunk(const clan::vec2 & pos)
{
    return impl->erase_chunk(pos);
}

clan::Canvas & TileMap::get_canvas()
{
	return impl->get_canvas();
}

void TileMap::render(const clan::vec2 & pos)
{
    impl->render(pos);
}
