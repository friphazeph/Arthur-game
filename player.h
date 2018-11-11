
#ifndef __GAME__PLAYER_H
	#define __GAME__PLAYER_H
	
	#include "lib/layer.h"
	#include "lib/fast/atlas.h"
	
	#include "tile.h"
	#include "size.h"
	#include "state.h"
	
	#define ANIM_FRAMES 6
	#define WALK_FRAMES 5
	
	typedef struct {
		bool    click;
		layer_t layer[ANIM_FRAMES];
		
		var frame;
		flt uptime;
		
		flt   vel_x; flt   vel_y;
		flt       x; flt       y;
		flt accel_x; flt accel_y;
		
		bool is_touching_ground;
		
	    flt     timer_to_default;
		state_t state;
		
		layer_t gum;
		bool    jump;
		
		var ya;
		
	} player_t;
	
	void new_player(player_t* this) {
		this->click = false;
        this->jump  = false;
		
		this->timer_to_default = FLOAT_ZERO;
		default_state(&this->state);
		
		this->frame  = 0;
		this->uptime = FLOAT_ZERO;
		
		this->x       = FLOAT_ZERO;
		this->y       = FLOAT_ZERO;
		
		this->vel_x   = FLOAT_ZERO;
		this->vel_y   = FLOAT_ZERO;
		
		this->accel_x = FLOAT_ZERO;
		this->accel_y = FLOAT_ONE * 10;
		
		iterate (ANIM_FRAMES) { // create a layer for each frame
			new_layer       (&this->layer[i]);
			layer_set_bitmap(&this->layer[i], ATLAS_PATH);
			this->layer[i].stretch = true;
			layer_init      (&this->layer[i]);
			layer_set_size  (&this->layer[i], ratio_x(BASE_PIXELS) >> 4, ratio_y(BASE_PIXELS) >> 3);
			layer_set_layer (&this->layer[i], 3);
			surface_scroll  (&this->layer[i].surface, -_UI64_MAX_MARGIN, (_UI64_MAX_MARGIN >> 2) * i - _UI64_MAX_MARGIN, _UI64_MAX_MARGIN >> 3, _UI64_MAX_MARGIN >> 2);
			
		}
		
		new_atlas_layer(&this->gum, BASE_PIXELS, ATLAS_PATH, 16, 16, TILE_GUM_FULL, 0);
		layer_set_layer(&this->gum, 4);
		
	}
	
	void player_touch_ground(player_t* this) {
	    this->vel_y = FLOAT_ZERO;
        this->is_touching_ground = true;
	    
	}
	
	void player_bounce(player_t* this) {
	    this->vel_y = -(FLOAT_ONE * 9);
	    
	}
	
	#define player_state_function(state_name, seconds) { \
	    this->timer_to_default = FLOAT_ONE *  seconds; \
	    state_name(&this->state); \
	}
	
	void player_gummify(player_t* this) player_state_function(sticky_state,  3)
	void player_fly    (player_t* this) player_state_function(flying_state,  4)
	void player_default(player_t* this) player_state_function(default_state, 0)
	
	void player_update(player_t* this, flt delta, var fps) {
		this->uptime += delta;
		this->frame = (this->uptime / (FLOAT_ONE / WALK_FRAMES / 4)) % WALK_FRAMES;
		
		if (this->timer_to_default >= FLOAT_ZERO) {
		    this->timer_to_default -= delta;
		    
		} else {
		    this->timer_to_default = FLOAT_ZERO - 1;
		    default_state(&this->state);
		    
		}
		
		if (this->click) {
		    this->click = false;
			
			if (this->is_touching_ground or this->state.can_double_jump) {
				if (not this->jump or not   this->state.can_double_jump) {
				    this->jump = true;
				    this->vel_y = -(FLOAT_ONE * 4);
				    
				}
				
			}
			
		} else {
            this->jump = false;
            
        }
		
		// physics
		
		if (this->accel_x < 0) this->vel_x -= MUL_FLOAT(-this->accel_x, delta);
		else                   this->vel_x += MUL_FLOAT( this->accel_x, delta);
		
		if (this->accel_y < 0) this->vel_y -= MUL_FLOAT(-this->accel_y, delta);
		else                   this->vel_y += this->state.vacc < 0 ? \
		                                      MUL_FLOAT(this->accel_y, delta) / (-this->state.vacc) : \
		                                      MUL_FLOAT(this->accel_y, delta) *   this->state.vacc;
		
		if (this->vel_x < 0) {
			this->vel_x +=          (-this->vel_x) / fps;
			this->x     -= MUL_FLOAT(-this->vel_x, delta);
		   
		} else {
			this->vel_x -=          (this->vel_x) / fps;
			this->x     += MUL_FLOAT(this->vel_x, delta);
		   
		}
		
		if (this->vel_y < 0) {
			this->vel_y +=          (-this->vel_y) / fps;
			this->y     -= MUL_FLOAT(-this->vel_y, delta);
		   
		} else {
			this->vel_y -=          (this->vel_y) / fps;
			this->y     += MUL_FLOAT(this->vel_y, delta);
		   
		}
		
		// collisions
		
		if (this->y     < -FLOAT_ONE) {
		    this->y     = -FLOAT_ONE;
		    this->vel_y =  FLOAT_ONE * 3; // ceiling bounce
		    
        }
        
        if (this->y > FLOAT_ONE - GROUND_LEVEL) {
			this->y = FLOAT_ONE - GROUND_LEVEL;
			player_touch_ground(this);
			
		} else {
			this->is_touching_ground = false;
			
		}
		
	}
	
	void player_draw(player_t* this, tile_t* tiles, bool collision) {
	    var x = 0; // FLOAT_TO_U64_MAX_MARGIN(this->x)
	    var y =       FLOAT_TO_U64_MAX_MARGIN(this->y);
	    
	    if (this->state.type == STICKY_STATE) {
	        layer_set_y   (&this->gum, -_UI64_MAX_MARGIN);
	        layer_set_xa  (&this->gum, 0);
	        layer_set_size(&this->gum, this->gum.width, _UI64_MAX_MARGIN - y);
	        layer_draw    (&this->gum);
	        
	    } elif (this->state.type == FLYING_STATE) {
			this->frame = 0;
			
	        var tile_width  =  tiles[0].tile_width;
	        var tile_height = -tiles[0].tile_height - y;
	        
	        tile_draw_raw(&tiles[TILE_PENCIL_RUBBER], -tile_width, tile_height, 6);
	        tile_draw_raw(&tiles[TILE_PENCIL_BODY],    0,          tile_height, 6);
	        tile_draw_raw(&tiles[TILE_PENCIL_TIP],     tile_width, tile_height, 6);
	        
	    }
		
		if (collision) {
			this->frame = 5;
			
		}
		
		this->ya = -y;
		
		layer_set_xa(&this->layer[this->frame], x);
		layer_set_y (&this->layer[this->frame], this->ya);
		
		layer_draw(&this->layer[this->frame]);
		
	}
	
	void dispose_player(player_t* this) {
		dispose_layer(&this->gum);
		
		iterate (ANIM_FRAMES) {
			dispose_layer(&this->layer[i]);
			
		}
		
	}
	
#endif
