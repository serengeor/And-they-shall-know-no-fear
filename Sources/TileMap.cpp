#include "precomp.h"

#include "TileMap.h"
#include "TileChunk.h"

class TileMap_Impl
{
    public:

    TileMap_Impl(Canvas c)
    {
        m_canvas = c;
        m_unique_id = 0;
    }

    bool add_sprite(Sprite spr, uint8_t id)
    {
        if(m_sprites.find(id)==m_sprites.end())
        {
            m_sprites[id]=spr;
            return true;
        }

        return false;
    }

    Sprite get_sprite(uint8_t id)
    {
        auto spr = m_sprites.find(id);
        if(spr!=m_sprites.end())
            return spr->second;

        return Sprite();
    }

    void remove_sprite(uint8_t id)
    {
        m_sprites.erase(id);
    }

    bool is_chunk_visible(const vec2 & chunk_pos, const Rect & render_rect)
    {
        Rect chunk_rect(chunk_pos.x,chunk_pos.y,Sizex<int32_t>(CHUNK_SIZE,CHUNK_SIZE));
        return chunk_rect.is_overlapped(render_rect);
    }

    TileChunk add_chunk(TileMap m, const vec2 & pos )
    {
        if(m_chunks.find(pos)==m_chunks.end())
        {
            TileChunk c(m,m_unique_id);
            m_chunks[pos]=c;
            m_unique_id++;
            std::cout << "Chunk added." << std::endl;
            return c;
        }
        else
            return TileChunk();
    }


    TileChunk get_chunk(const vec2 & pos)
    {
        auto r = m_chunks.find(pos);
        if(r!=m_chunks.end())
        {
            return r->second;
        }

        return TileChunk();
    }


    Canvas & get_canvas()
    {
        return m_canvas;
    }

    void render(const vec2 & pos)
    {
        int w = m_canvas.get_width();
        int h = m_canvas.get_height();

        int rx = (pos.x-CHUNK_SIZE+1)/CHUNK_SIZE;
        int ry = (pos.y-CHUNK_SIZE+1)/CHUNK_SIZE;
        int rx2 = (pos.x+w+CHUNK_SIZE+1)/CHUNK_SIZE;
        int ry2 = (pos.y+h+CHUNK_SIZE+1)/CHUNK_SIZE;
        TileChunk c;

		std::string s = "x1: ";
		s+=StringHelp::int_to_text(rx);

		s+= " y1: ";
		s+=StringHelp::int_to_text(ry);
		
		s+= "\nx2: ";
		s+=StringHelp::int_to_text(rx2);
		
		s+= " y2: ";
		s+=StringHelp::int_to_text(ry2);

        c = get_chunk(vec2(0,0));
        
		for(int y = ry; y < ry2; y++)
		for(int x = ry; x < rx2; x++)
		{
			c = get_chunk(vec2(x,y));

			if(!c.is_null())
			{
				if(!c.is_batched())
					c.batch();

				for(int32_t i = 0; i < LAYER_COUNT; i++)
					c.draw_chunk(m_canvas,vec2(x*CHUNK_SIZE,y*CHUNK_SIZE)-pos,i);
			}
		}

		m_font.draw_text(m_canvas,20,20,s,Colorf::black);
    }

    protected:
    Canvas      m_canvas;
    uint32_t    m_unique_id;

	Font		m_font;

    std::map<vec2, TileChunk>  m_chunks;
    std::map<uint8_t, Sprite>           m_sprites;
};

TileMap::TileMap()
{

}

TileMap::TileMap(Canvas & c)
{
    impl=std::shared_ptr<TileMap_Impl>(new TileMap_Impl(c));
}

TileMap::~TileMap(){}

bool TileMap::add_sprite(Sprite spr, uint8_t id)
{
    impl->add_sprite(spr,id);

    return false;
}

Sprite TileMap::get_sprite(uint8_t id)
{
    return impl->get_sprite(id);
}

void TileMap::remove_sprite(uint8_t id)
{
    impl->remove_sprite(id);
}

bool TileMap::is_chunk_visible(const vec2 & chunk_pos, const Rect & render_rect)
{
    return impl->is_chunk_visible(chunk_pos,render_rect);
}

TileChunk TileMap::add_chunk( const vec2 & pos )
{
    return impl->add_chunk(*this,pos);
}


TileChunk TileMap::get_chunk(const vec2 & pos)
{
    return impl->get_chunk(pos);
}

Canvas & TileMap::get_canvas()
{
	return impl->get_canvas();
}

void TileMap::render(const vec2 & pos)
{
    impl->render(pos);
}