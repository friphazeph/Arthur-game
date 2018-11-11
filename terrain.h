
#ifndef __GAME__TERRAIN_H
	#define __GAME__TERRAIN_H
	
	void create_world(world_t* this, var seed, var level) {
		this->world_size = 100 + 200 * level;
		
		this->world_map        = (var**) malloc(this->world_size * sizeof(var*));
		this->world_heights    = (var*)  malloc(this->world_size * sizeof(var));
		this->world_collisions = (var*)  malloc(this->world_size * sizeof(var));
		
		var cube_size;
		var cube_material;
		var struct_height;
		
		var struct_count = 0;
		
		var current_structure = 0;
		rand_t random;
		
		srand(&random, seed);
		
		iterate (this->world_size) {
			struct_count++;
			this->world_heights[i] = 0;
			
			switch (current_structure) {
				case STRUCT_TABLE: {
					if (struct_count > cube_size) current_structure      = 0;
					else                          this->world_heights[i] = struct_height;
					
					break;
					
				} case STRUCT_CHAIR: {
					if   (struct_count >  cube_size) current_structure      = 0;
					elif (struct_count == cube_size) this->world_heights[i] = struct_height << 1;
					else                             this->world_heights[i] = struct_height;
					
					break;
					
				} case STRUCT_BOTTLE: {
					if   (struct_count >  cube_size) current_structure      = 0;
					else                             this->world_heights[i] = struct_height;
					
					break;
					
				}
				
				case STRUCT_BOUNCE:
				case STRUCT_PENCIL:
				case STRUCT_GUM: {
					if (struct_count > cube_size) current_structure      = 0;
					else                          this->world_heights[i] = 1;
					
					break;
					
				} case STRUCT_CUBE: {
					if (struct_count > cube_size) current_structure      = 0;
					else                          this->world_heights[i] = cube_size;
					
					break;
					
				} case STRUCT_STAIRS: {
					if (struct_count > cube_size) current_structure      = 0;
					else                          this->world_heights[i] = struct_count;
					
					break;
					
				} 
				
			}
			
			if (not current_structure and rand(&random) % 7) {
			   struct_count = 0;
			   
			   pass: {
				   if (level > STRUCT_LAST) {
					   level = STRUCT_LAST;
					   
				   }
				   
				   current_structure = rand(&random) % (STRUCT_FIRST + level - 1) + STRUCT_FIRST;
				   
				   
				}
			   
			   if (((current_structure == STRUCT_PENCIL) and      rand(&random) % 5)  or \
				   ((current_structure == STRUCT_BOUNCE) and not (rand(&random) % 5)) or \
				   ((current_structure == STRUCT_STAIRS) and      rand(&random) % 3)  or \
				   ((current_structure == STRUCT_BOTTLE) and      rand(&random) % 3)  or \
				   ((current_structure == STRUCT_GUM)    and not (rand(&random) % 6)) or \
				   ((current_structure == STRUCT_TABLE)  and not (rand(&random) % 4)) or \
				   ((current_structure == STRUCT_CHAIR)  and not (rand(&random) % 6))
				   ) {
				   goto pass;
				   
				}
			   
			   switch (current_structure) {
				   case STRUCT_CHAIR: {
					   struct_height = rand(&random) % 2 + 2;
					   cube_size     = struct_height + 1;
					   
					   break;
					   
					} case STRUCT_TABLE: {
					   struct_height = rand(&random) % 4 + 2;
					   cube_size     = rand(&random) % 5 + 3;
					   
					   break;
					   
					} case STRUCT_GUM: {
					   cube_size = rand(&random) % 3 + 1;
					   break;
					   
					} case STRUCT_BOTTLE: {
					   cube_size = 1;
					   struct_height = 2;
					   
					   break;
					   
					} case STRUCT_BOUNCE: {
					   cube_size = 2;
					   break;
					   
					} case STRUCT_PENCIL: {
					   cube_size = rand(&random) % 3 + 3;
					   break;
					   
					} case STRUCT_STAIRS:
					case STRUCT_CUBE: {
					   cube_size     = rand(&random) % 4 + 2;
					   cube_material = TILE_FLOOR;
					   
					   break;
					   
					}
				   
				}
				
			}
			
			var collision_height = this->world_heights[i];
			
			switch (current_structure) {
				case STRUCT_BOUNCE: {
					collision_height = 0;
					break;
					
				} case STRUCT_PENCIL: {
					collision_height = 0;
					break;
					
				} case STRUCT_GUM: {
					collision_height = 0;
					break;
					
				} case STRUCT_BOTTLE: {
					collision_height = 0;
					break;
					
				}
				
			}
			
			this->world_collisions[i] = ((-TILE_GET_Y(collision_height, this->tiles[0].tile_height, this->tiles[0].ground_level)) / (_UI64_MAX_MARGIN / FLOAT_ONE));
			this->world_map       [i] = (var*) malloc(this->world_heights[i] * sizeof(var));
			
			var j;
			for (j = 0; j < this->world_heights[i]; j++) {
				this->world_map[i][j] = TILE_AIR;
				
				switch (current_structure) {
					case STRUCT_STAIRS:
					case STRUCT_CUBE: {
						if (j + 1 == this->world_heights[i]) this->world_map[i][j] = TILE_STAIR_TRAIL;
						else                                 this->world_map[i][j] = TILE_STAIR_FULL;
						
						break;
						
					} case STRUCT_TABLE: {
						if   (j + 1 == this->world_heights[i])                this->world_map[i][j] = TILE_TABLE_TOP;
						elif (struct_count == cube_size || struct_count == 1) this->world_map[i][j] = TILE_TABLE_LEG;
						
						break;
						
					} case STRUCT_CHAIR: {
						if   (struct_count == cube_size)       this->world_map[i][j] = j + 1 < this->world_heights[i] >> 1 ? TILE_CHAIR_LEG : TILE_CHAIR_BACK;
						elif (j + 1 == this->world_heights[i]) this->world_map[i][j] = TILE_CHAIR_SEAT;
						elif (struct_count == 1)               this->world_map[i][j] = TILE_CHAIR_LEG;
						
						break;
						
					} case STRUCT_GUM: {
						if   (cube_size    == 1)         this->world_map[i][j] = TILE_GUM_SINGLE;
						elif (struct_count == 1)         this->world_map[i][j] = TILE_GUM_START;
						elif (struct_count == cube_size) this->world_map[i][j] = TILE_GUM_END;
						else                             this->world_map[i][j] = TILE_GUM;
						
						break;
						
					} case STRUCT_PENCIL: {
						if   (struct_count == 1)         this->world_map[i][j] = TILE_PENCIL_RUBBER;
						elif (struct_count == cube_size) this->world_map[i][j] = TILE_PENCIL_TIP;
						else                             this->world_map[i][j] = TILE_PENCIL_BODY;
						
						break;
						
					} case STRUCT_BOUNCE: {
						if   (struct_count == 1)         this->world_map[i][j] = TILE_BOUNCE_ATTACK;
						elif (struct_count == cube_size) this->world_map[i][j] = TILE_BOUNCE_FALL;
						
						break;
						
					} case STRUCT_BOTTLE: {
						this->world_map[i][j] = j == 0 then TILE_BOTTLE_BOTTOM orelse TILE_BOTTLE_TOP;
						break;
						
					}
					
				}
				
			}
			
		}
		
	}
	
	void free_world(world_t* this) {
		iterate (this->world_size) {
			mfree(this->world_map[i], this->world_heights[i] * sizeof(var));
			
		}
		
		mfree(this->world_map,        this->world_size * sizeof(var*));
		mfree(this->world_heights,    this->world_size * sizeof(var));
		mfree(this->world_collisions, this->world_size * sizeof(var));
		
	}
	
#endif
