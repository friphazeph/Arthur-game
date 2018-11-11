
#include "game.h"
#include "lib/fast/fast.h"
#include "lib/fast/text.h"

#define PLAYER_COUNT 1
#define GAME_SPEED   1

private bool has_clicked(event_list_t* this, device keyboard_device) {
	return this->pointer_click_type or (keyboard_device then *get_device(keyboard_device, "press scancode") orelse false);
	
}

private bool game(device keyboard_device, var seed, var level) {
	game_t __game;
	game_t*  game = (game_t*) &__game;
	
	new_game(game, PLAYER_COUNT, seed, level);
	video_clear_colour(-1, 0, -1, 0);
	
	bool ended = false;
	flt end_timer = FLOAT_ONE; // take 1 second to end
	
	var fps;
	always {
		fps = video_fps();
		get_events(&game->events);
		
		if (ended) {
			end_timer -= FLOAT_ONE / fps;
			
		}
		
		if (game->events.quit) {
			break;
			
		} if (has_clicked(&game->events, keyboard_device)) {
		    if (ended and end_timer <= FLOAT_ZERO) break;
		    else game->players[0].click = true;
		    
		}
		
		iterate (GAME_SPEED) {
			if (game_update(game, fps)) {
				ended = true;
				
			}
			
		}
		
		video_clear();
		game_draw(game);
		video_flip();
		
	}
	
	bool lost = game->world.lost;
	dispose_game(game);
	
	return lost;
	
}

#define AGSD_SIGNATURE 0xA65D
#define SAVE_PATH      PATH"arthur_game_save_data.agsd"

typedef struct {
	var signature;
	
	var level;
	var lost;
	
} save_t;

var main(void) {
	device texture_device  = is_device_supported("texture");
	device keyboard_device = is_device_supported("keyboard");
	
	font_t font = new_font(PATH"fonts/mono.ttf", _UI64_MAX / 30);
	
	text_t               play;
	new_text           (&play, font, "Appuyez pour jouer", theme_default(), 1);
	text_rainbow       (&play);
	text_point_to_point(&play, FLOAT_ONE, POSITION_BOTTOM, POSITION_CENTER);
	
	event_list_t events;
	bool click = false;
	
	save_t* save  = nullptr;
	var     bytes = 0;
	
	if (fs_support()) {
		if (fs_read(SAVE_PATH, (char**) &save, &bytes)) {
			print("WARNING Failed to open `%s`\n", SAVE_PATH);
			save = nullptr;
			
		} else {
			if (bytes != sizeof(save_t)) {
				print("WARNING File size (%lld bytes) is not the same as `sizeof(save_t)` (%lld bytes)\n", bytes, sizeof(save_t));
				save = nullptr;
				
			} else {
				if (save->signature != AGSD_SIGNATURE) {
					print("WARNING File signature (0x%llx) is not a correct AGSD signature (0x%llx)\n", save->signature, AGSD_SIGNATURE);
					save = nullptr;
					
				}
				
			}
			
		}
		
	} else {
		print("WARNING Your platform does not seem to support FS functions. You will not be able to save any progress\n");
		
	}
	
	if (save == nullptr) {
		save = (save_t*) malloc(sizeof(save_t));
		save->signature = AGSD_SIGNATURE;
		
		save->level = 0;
		save->lost  = 0;
		
	}
	
	var tick = 0;
	always {
		var fps = video_fps();
		
		if (tick++ > fps) {
			clear(0, 0, 0, 0);
			text_draw(&play, fps);
			
		}
		
		video_flip();
		get_events(&events);
		
		if (events.quit) {
			break;
			
		} if (has_clicked(&events, keyboard_device)) {
			if (not click) {
				click = true;
				
				if (texture_device) {
					bool sharp_status = true;
					send_device(texture_device, "sharp", (unsigned long long*) &sharp_status);
					
				} else {
					print("WARNING Texture device could not be found, textures may look blurry\n");
					
				}
				
				if (not game(keyboard_device, tick, save->level)) save->level++;
				else                                              save->lost++;
				
				if (fs_support() and fs_write(SAVE_PATH, (const char*) save, sizeof(save_t))) {
					print("WARNING Failed to save save file\n");
					
				}
				
				if (texture_device) {
					bool sharp_status = false;
					send_device(texture_device, "sharp", (unsigned long long*) &sharp_status);
					
				} else {
					print("WARNING Texture device could not be found, textures may look blurry\n");
					
				}
				
			}
			
		} else {
			click = false;
			
		}
		
	}
	
	dispose_text(&play);
	font_remove(font);
	
	mfree(save, sizeof(save_t));
	return 0;
	
}
