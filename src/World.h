#pragma once

#include "Object.h"
#include "Tank.h"
#include "Bullet.h"
#include "Bush.h"
#include "Wall.h"
#include "Explode.h"
#include "Flag.h"

#define GAME_MODE_START 0
#define GAME_MODE_PLAY 1
#define GAME_MODE_PAUSE 2
#define GAME_MODE_LOSE 3
#define GAME_MODE_WIN 4

class World
{
	int _show_bb;
	vector< pair<Uint32, Object *> > add_queue;
	vector<Object *> _objs;
	// save removed objects until end game, because tanks can save pointer to target after its removal
	vector<Object *> removed_objs;
	void delete_objects();

	void move_obj(Object *o);
	void show_bb(SDL_Surface *s);

	void try_move_tank(Tank *t);
	bool is_possible_position(Object *o);
	int game_mode;
	int player_start_x, player_start_y;
	int remote_player_start_x, remote_player_start_y;
	Uint32 respawn_player_at;
	Uint32 respawn_remote_player_at;
	int lives;
	int remote_lives;
	SDL_Surface *mode_image[5];

	int _levelIndex;
	void load_current_level();
	
	Sprite heart;

public:
	bool is_paused() {return game_mode == GAME_MODE_PAUSE; };
	bool network_game;
	Flag *player_flag;
	Flag *enimy_flag;
	Tank *_player;
	Tank *_remote_player;
	vector< vector<Object *> > EnvMap;
	bool EnvMapChanged;
	SDL_Rect bounds;

	World(int w, int h);
	~World();

	void add_explode(int x, int y);
	
	void add_object(Object *o)
	{
		_objs.push_back(o);
	}
	void add_object_delay(Object *o, Uint32 delay)
	{
		add_queue.push_back(make_pair(WorldTime::now + delay, o));
	}
	void add_explode_area(SDL_Rect box, int count);
	void handle_input(bool remote_player, int key);
	void think();
	void draw(SDL_Surface *s);
};
