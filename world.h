
#ifndef __GAME__WORLD_H
	#define __GAME__WORLD_H
	
	#include "tile.h"
	#include "struct.h"
	
	#define TUNA_ANIM 3
	#define TUNA_IDLE 2
	
	typedef struct {
		bool    ended;
		bool    stop;
		bool    lost;
		bool    rainbow;
		
		flt     starting_position;
		flt              position;
		
		layer_t background;
		layer_t ground;
		
		tile_t tiles[TILE_COUNT];
		
		var world_size;
		
		var** world_map;
		var*  world_heights;
		var*  world_collisions;
		
		layer_t tuna[TUNA_ANIM];
		
		var frame;
		flt uptime;
		flt tuna_position;
		
		var fps;
		
	} world_t;
	
	#include "lib/math.h"
	#include "lib/posix/rand.h"
	
	#include "terrain.h"
	
	void new_world(world_t* this, var seed, var level) {
		this->starting_position = FLOAT_ONE * 720 /* seconds */;
		
		this->ended             = false;
		this->stop              = false;
		this->lost              = false;
		this->rainbow           = false;
		
		new_layer       (&this->background);
		layer_set_bitmap(&this->background, PATH"bricks.bmp");
		this->background.stretch = true;
		layer_init      (&this->background);
		layer_set_size  (&this->background, ratio_x(BASE_PIXELS), ratio_y(BASE_PIXELS));
		layer_set_ya    (&this->background, 0);
		layer_set_layer (&this->background, 0);
		layer_set_rainbow(&this->background);
		
		new_layer       (&this->ground);
		layer_set_bitmap(&this->ground, ATLAS_PATH);
		this->ground.stretch = true;
		layer_init      (&this->ground);
		layer_set_size  (&this->ground, ratio_x(BASE_PIXELS), ratio_y(BASE_PIXELS));
		layer_set_ya    (&this->ground, FLOAT_TO_U64_MAX_MARGIN(GROUND_LEVEL) - (this->ground.height >> 1) - _UI64_MAX_MARGIN);
		layer_set_layer (&this->ground, 5);
		
		iterate (TILE_COUNT) {
			new_tile(&this->tiles[i], i);
			
		}
		
		this->frame           = 0;
		this->uptime          = FLOAT_ZERO;
		this->tuna_position   = FLOAT_ZERO;
		
		iterate (TUNA_ANIM) {
			new_layer       (&this->tuna[i]);
			layer_set_bitmap(&this->tuna[i], ATLAS_PATH);
			this->tuna[i].stretch = true;
			layer_init      (&this->tuna[i]);
			layer_set_layer (&this->tuna[i], 10);
			
			var j = i;
			var voffset = 0;
			
			if (j >= 2) {
				j -= 2;
				voffset++;
				
			}
			
			layer_set_size(&this->tuna[i], (ratio_x(BASE_PIXELS) >> 4) * (14 + (voffset << 1)), (ratio_y(BASE_PIXELS) >> 4) * 6);
			surface_scroll(&this->tuna[i].surface, (_UI64_MAX_MARGIN >> 3) * 7 * voffset + (_UI64_MAX_MARGIN >> 3) - _UI64_MAX_MARGIN, (_UI64_MAX_MARGIN >> 2) + (_UI64_MAX_MARGIN >> 3) * 3 * j - _UI64_MAX_MARGIN, (_UI64_MAX_MARGIN >> 3) * (7 + voffset), (_UI64_MAX_MARGIN >> 3) * 3);
			
		}
		
		// create world
		
		this->position = this->starting_position;
		create_world(this, seed, level);
		
	}
	
	bool world_update(world_t* this, var fps, flt delta) {
		this->fps = fps;
		
		this->uptime += delta;
		this->frame = (this->uptime / (FLOAT_ONE / TUNA_IDLE / 2)) % TUNA_IDLE;
		
		if (this->ended) {
			if (this->lost) {
				this->frame = 2;
				this->tuna_position += delta;
				
			} else {
				this->frame = 1;
				
			}
			
		} elif (this->stop) {
			this->frame = 1;
			this->tuna_position += delta >> 1;
			
			if (this->tuna_position > FLOAT_HALF) {
				this->ended = true;
				this->lost  = true;
				
			}
			
		}
		
		layer_update(&this->background, fps);
		
		if (!this->ended) {
			if (!this->stop) {
			    this->position -= MUL_FLOAT(delta, FLOAT(video_height()) / video_width()) * 2;
			    
			}
			
			if (this->position < FLOAT_ONE) {
				this->ended = true;
				return true;
				
			}
			
		} else {
			return true;
			
		} return false;
		
	}
	
	private void world_draw_layer(world_t* this, var speed, layer_t* layer) {
		flt repeated_position = (this->position * speed >> 1) % (FLOAT_ONE << 1);

        layer_set_xa(layer, FLOAT_TO_U64_MAX_MARGIN(repeated_position + FLOAT_ONE));
        layer_draw  (layer);

        layer_set_xa(layer, FLOAT_TO_U64_MAX_MARGIN(repeated_position));
		layer_draw  (layer);
		
		layer_set_xa(layer, FLOAT_TO_U64_MAX_MARGIN(repeated_position - FLOAT_ONE));
        layer_draw  (layer);
		
		layer_set_xa(layer, FLOAT_TO_U64_MAX_MARGIN(repeated_position - (FLOAT_ONE * 2)));
		layer_draw  (layer);
		
		layer_set_xa(layer, FLOAT_TO_U64_MAX_MARGIN(repeated_position - (FLOAT_ONE * 3)));
        layer_draw  (layer);
		
	}
	
	void world_draw(world_t* this, player_t* players, var player_count) {
		if (this->ended and not this->lost) {
			if (this->rainbow == false) {
				this->rainbow =  true;
				
				iterate (ANIM_FRAMES) {
					layer_set_rainbow(&players[0].layer[i]);
					
				}
				
			}
			
			iterate (ANIM_FRAMES) {
				layer_update(&players[0].layer[i], this->fps);
			}
			
		}
		
		world_draw_layer(this, 1, &this->background); // has to be speed = 1
		world_draw_layer(this, 2, &this->ground);
		
		var tile_width        =                  this->tiles[0].tile_width;
        flt tile_height_float = U64_MAX_TO_FLOAT(this->tiles[0].tile_height) * 200;
		
		const var TILES_IN_EACH_DIRECTION = _UI64_MAX_MARGIN / tile_width + 1;
		
		var index;
		#define SET_TILE_INDEX(j) index = FLOAT_TO_U64_MAX_MARGIN(this->starting_position - this->position) / tile_width + j + TILES_IN_EACH_DIRECTION;
		
		SET_TILE_INDEX(0)
		this->stop = false;
		
		iterate (player_count) {
		    var collision = min(this->world_collisions[index], this->world_collisions[index - 1]);
		    
            if (players[i].y > collision) {
                var delta = players[i].y - collision;
                bool touch_ground = true;
                
                if (delta > tile_height_float * players[i].state.impeed) {
                    this->stop = true;
                    collision = min(this->world_collisions[index - 1], this->world_collisions[index - 2]);
                    touch_ground = players[i].y > collision;
                    
                }
                
                if (touch_ground) {
                    players[i].y = collision;
                    player_touch_ground(&players[i]);
                    
                }
                
            } elif (players[i].y > ((-TILE_GET_Y(this->world_heights[index], this->tiles[0].tile_height, this->tiles[0].ground_level)) / (_UI64_MAX_MARGIN / FLOAT_ONE))) { // intersecting with tile
                switch (this->world_map[index]  [this->world_heights[index] - 1]) {
                    case TILE_GUM_START:
                    case TILE_GUM:
                    case TILE_GUM_END:
                    case TILE_GUM_SINGLE: {
                        player_gummify(&players[i]);
                        break;
                        
                    }
                    
                    case TILE_PENCIL_RUBBER:
                    case TILE_PENCIL_BODY:
                    case TILE_PENCIL_TIP: {
                        player_fly(&players[i]);
                        break;
                        
                    }
                    
                    case TILE_BOUNCE_ATTACK:
                    case TILE_BOUNCE_FALL: {
                        player_bounce(&players[i]);
                        break;
                        
                    }
                    
                    case TILE_BOTTLE_TOP:
                    case TILE_BOTTLE_BOTTOM: {
						player_default(&players[i]);
						break;
						
					}
                    
                }
                
            }
            
        }
        
        var offset = -FLOAT_TO_U64_MAX_MARGIN(this->starting_position - this->position) % tile_width;
        
		var j;
		for (j = -TILES_IN_EACH_DIRECTION; j <= TILES_IN_EACH_DIRECTION; j++) {
    		SET_TILE_INDEX(j)
    		
    		if (index >= this->world_size - 1) {
				this->ended = true;
				break;
				
			}
    		
			iterate                   (this->world_heights[index]) {
				tile_draw(&this->tiles[this->world_map    [index][i]], j + 1, i, offset);
				
			}
			
		}
		
		layer_set_x (&this->tuna[this->frame], FLOAT_TO_U64_MAX_MARGIN(this->tuna_position) - (this->tuna[0].width / 4 * 3) - _UI64_MAX_MARGIN);
		layer_set_ya(&this->tuna[this->frame], players[0].ya);
		layer_draw  (&this->tuna[this->frame]);
		
	}
	
	void dispose_world(world_t* this) {
		dispose_layer(&this->background);
		dispose_layer(&this->ground);
		
		iterate (TILE_COUNT) {
			dispose_tile(&this->tiles[i]);
			
		}
		
		iterate (TUNA_ANIM) {
			dispose_layer(&this->tuna[i]);
			
		}
		
		free_world(this);
		
	}
	
#endif
