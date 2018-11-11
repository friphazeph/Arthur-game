
#ifndef __GAME__TILE_H
	#define __GAME__TILE_H
	
	#include "lib/layer.h"
	#include "size.h"
	
	#define TILE_VOID          0
	#define TILE_DIRT          1
	#define TILE_GRASS         2
	#define TILE_FLOOR         3
	#define TILE_HALF          4
	#define TILE_AIR           5
	
	#define TILE_GUM_START     6
	#define TILE_GUM           7
	#define TILE_GUM_END       8
	#define TILE_GUM_SINGLE    9
	#define TILE_GUM_FULL      10
	
	#define TILE_TABLE_LEG     11
	#define TILE_TABLE_TOP     12
	
	#define TILE_CHAIR_SEAT    13
	#define TILE_CHAIR_LEG     14
	#define TILE_CHAIR_BACK    15
	
	#define TILE_PENCIL_RUBBER 17
	#define TILE_PENCIL_BODY   18
	#define TILE_PENCIL_TIP    19
	
	#define TILE_BOUNCE_ATTACK 20
	#define TILE_BOUNCE_FALL   21
	
	#define TILE_STAIR_TRAIL   22
	#define TILE_STAIR_FULL    23
	
	#define TILE_BOTTLE_BOTTOM 24
	#define TILE_BOTTLE_TOP    25
	
	#define TILE_COUNT 26
	
	typedef struct {
		var     type;
		layer_t layer;
		
		var tile_width;
		var tile_height;
		
		var ground_level;
		
	} tile_t;
	
	#define DEFAULT_TILE_LAYER 2
	
	tile_t* new_tile(tile_t* this, var type) {
		this->type = type;
		
		this->tile_width  = ratio_x(BASE_PIXELS) >> 4;
		this->tile_height = ratio_y(BASE_PIXELS) >> 4;
		
		this->ground_level = FLOAT_TO_U64_MAX_MARGIN(GROUND_LEVEL);
		
		if (this->type != TILE_AIR) {
			new_layer       (&this->layer);
			layer_set_bitmap(&this->layer, ATLAS_PATH);
			this->layer.stretch = true;
			layer_init      (&this->layer);
			layer_set_size  (&this->layer, this->tile_width, this->tile_height);
			layer_set_layer (&this->layer, DEFAULT_TILE_LAYER);
			
			var voffset = 0;
			
			if (type >= 16) {
				type -= 16;
				voffset++;
				
			}
			
			surface_scroll(&this->layer.surface, -_UI64_MAX_MARGIN + (_UI64_MAX_MARGIN >> 3) * type, \
												 -_UI64_MAX_MARGIN + (_UI64_MAX_MARGIN >> 3) * voffset, \
												  _UI64_MAX_MARGIN >> 3, \
												  _UI64_MAX_MARGIN >> 3);
																	  
		}
		
		return this;
		
	}
	
	#define TILE_GET_Y(y, tile_height, ground_level) (y * tile_height - _UI64_MAX_MARGIN + ground_level)
	
	void tile_draw_raw(tile_t* this, var x, var y, var layer) {
		if (this->type == TILE_AIR) {
			return;
			
		}
		
		layer_set_xa(&this->layer, x);
		layer_set_y (&this->layer, y);
		
		layer_set_layer(&this->layer, layer);
		layer_draw     (&this->layer);
		
	}
	
	void tile_draw(tile_t* this, var x, var y, var world_position) {
		tile_draw_raw(this, x * this->tile_width + world_position, TILE_GET_Y(y, this->tile_height, this->ground_level), DEFAULT_TILE_LAYER);
		
	}
	
	void dispose_tile(tile_t* this) {
		if (this->type != TILE_AIR) {
			dispose_layer(&this->layer);
			
		}
		
	}
	
#endif
