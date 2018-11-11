
#ifndef __GAME__GAME_H
	#define __GAME__GAME_H
	
	#define GROUND_LEVEL (FLOAT_ONE >> 2)
	#define ATLAS_PATH   (PATH"REMME.bmp")
	
	#include "player.h"
	#include "world.h"
	
	#define MAX_PLAYER_COUNT 8
	
	typedef struct {
		event_list_t events;
		world_t      world;
		
		var          player_count;
		player_t     players[MAX_PLAYER_COUNT];
		
	} game_t;
	
	void new_game(game_t* this, var player_count, var seed, var level) {
		this->player_count = player_count;
		new_world(&this->world, seed, level);
		
		iterate (this->player_count) {
			new_player(&this->players[i]);
			
		}
		
	}
	
	bool game_update(game_t* this, var fps) {
		flt delta = FLOAT_ONE / fps;
		world_update(&this->world, fps, delta);
		
		iterate (this->player_count) {
			player_update(&this->players[i], delta, fps);
			
		}
		
		return this->world.ended;
		
	}
	
	void game_draw(game_t* this) {
		world_draw(&this->world, this->players, this->player_count);
		
		if (this->world.lost) {
			return;
			
		}
		
		iterate (this->player_count) {
			player_draw(&this->players[i], this->world.tiles, this->world.stop);
			
		}
		
	}
	
	void dispose_game(game_t* this) {
		dispose_world(&this->world);
		
		iterate (this->player_count) {
			dispose_player(&this->players[i]);
			
		}
		
	}
	
#endif
