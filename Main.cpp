// Tanks-SDL.cpp : Defines the entry point for the console application.
//

#include "StdAfx.h"
#include "World.h"
#include <Winsock2.h>

vector<SDL_Surface *> TilesCache::main;

vector<SDL_Surface*> load_sprites(const char *file,
		int width, int height,
		int border_width, int border_height,
		int color_key_r, int color_key_g, int color_key_b)
{
	vector<SDL_Surface*> sprites;
	SDL_Surface* multi = SDL_LoadBMP(file);
	if(multi == NULL)
	{
		fprintf(stderr, "Couldn't load sprites from: %s, error: %s\n", file, SDL_GetError());
		exit(1);
	}

	SDL_Surface *tmp = SDL_CreateRGBSurface(
		SDL_HWSURFACE|SDL_SRCCOLORKEY,
		width,
		height,
		32,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000);
	if(tmp == NULL)
	{
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		exit(1);
	}

	int cols =  (multi->w - border_width) / (width + border_width);
	int rows =  (multi->h - border_height) / (height + border_height);

	for(int row=0;row<rows;row++)
	{
		for(int col=0;col<cols;col++)
		{
			SDL_Rect srcr;
			srcr.x = border_width + (width + border_width)*col;
			srcr.y = border_height + (height + border_height)*row;
			srcr.w = width;
			srcr.h = height;

			SDL_BlitSurface(multi, &srcr, tmp, NULL);

			SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, SDL_MapRGB(tmp->format, color_key_r, color_key_g, color_key_b));

			sprites.push_back(SDL_DisplayFormat(tmp));
		}
	}

	return sprites;
}

void recv_keys(SOCKET sock, vector<SDLKey> &keys)
{
	keys.clear();

	// read count of keys at peer
	int count;
	if(recv(sock, (char *)&count, sizeof(count), 0) == SOCKET_ERROR)
	{
		cout << "recv error: " << WSAGetLastError() << endl;
		exit(1);
	}
	if(count == 0)
		return;

	// read all keys from peer
	keys.resize(count);
	if(recv(sock, (char *)&keys[0], sizeof(SDLKey) * count, 0) == SOCKET_ERROR)
	{
		cout << "recv error: " << WSAGetLastError() << endl;
		exit(1);
	}
}

void send_keys(SOCKET sock, vector<SDLKey> &keys)
{
	int count = keys.size();
	if(send(sock, (char *)&count, sizeof(count), 0) == SOCKET_ERROR)
	{
		cout << "send error: " << WSAGetLastError() << endl;
		exit(1);
	}

	if(count == 0)
		return;

	// send key array
	if(send(sock, (const char *)&keys[0], sizeof(SDLKey) * count, 0) == SOCKET_ERROR)
	{
		cout << "sned error: " << WSAGetLastError() << endl;
		exit(1);
	}
}

int main(int argc, char **argv)
{
	/* Initialize the SDL library */
	if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 )
	{
        	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        	exit(1);
	}

	// network init
	bool server = false;
	bool client = false;
	bool standalone = false;
	SOCKET sock = -1;
	if(argc > 1)
	{
		WORD wVersionRequested = MAKEWORD(2, 2);

		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);
		if ( err != 0 )
		{
			cerr << "Can't initialize WinSock." << endl;
			return 1;
		}

		if(strcmp(argv[1], "-server") == 0)
		{
			//server
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (s == INVALID_SOCKET)
			{
				cout << "Error in socket()" << endl;
				exit(1);
			}

			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(12345);
			addr.sin_addr.s_addr = INADDR_ANY;

			int ret = bind(s, (SOCKADDR *)&addr, sizeof(addr));
			if(ret != 0)
			{
				cout << "Error in bind() " << ret << endl;
				exit(1);
			}
			
			ret = listen(s, 1);
			if(ret != 0)
			{
				cout << "Error in listen() " << ret << endl;
				exit(1);
			}

			cout << "Server wait for client." << endl;
			
			sock = accept(s, NULL, NULL);
			if(sock < 0)
			{
				cout << "Error in accept() " << sock << endl;
				exit(1);
			}
			
			cout << "Client connected." << endl;

			closesocket(s);
			
			server = true;
		}
		else if(strcmp(argv[1], "-connect") == 0)
		{
			if(argc < 3)
			{
				cerr << "Usage: tanks -connect <ip or host name>" << endl;
				exit(1);
			}
			
			//client
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (sock == INVALID_SOCKET)
			{
				cout << "Error in socket()" << endl;
				exit(1);
			}

			hostent *he = gethostbyname(argv[2]);
			if (he == NULL)
			{
				cerr << "Invalid host: " << argv[2] << endl;
				exit(1);
			}
			
			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(12345);
			memcpy(&addr.sin_addr, he->h_addr, he->h_length);

			cout << "Try connect to " << argv[2] << endl;
			
			if (connect(sock, (SOCKADDR *)&addr, sizeof(addr)) != 0)
			{
				cerr << "Connect failed" << endl;
				exit(1);
			}
			
			cout << "Connected to server." << endl;
			
			client = true;
		}
	}
	
	standalone = !client && !server;
	
	/* Clean up on exit */
	atexit(SDL_Quit);

	SDL_Surface *screen = SDL_SetVideoMode(800, 600, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);//  | SDL_FULLSCREEN);
	if (screen == NULL)
	{
        	fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        	exit(1);
	}

	SDL_WM_SetCaption("Tanks", "t");

	TilesCache::main = load_sprites("resources/sprites.bmp", 32, 32, 1, 1, 0, 0, 0);

	World w(screen->w, screen->h);
	w.set_level("resources/first.level");
	w.network_game = !standalone;

	SDL_Event ev;

	WorldTime::now = 0;

	int network_exchange_initial = 20; // each 20 frames == once per 200 ms
	int network_exchange = network_exchange_initial;
	vector<SDLKey> keys;
	vector<SDLKey> remote_keys;

	while(true)
	{
		Uint32 start_loop = SDL_GetTicks();

		while(SDL_PollEvent(&ev))
		{
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) || ev.type == SDL_QUIT)
			{
				exit(0);
			}

			if(ev.type == SDL_KEYDOWN)
			{
				keys.push_back(ev.key.keysym.sym);
			}
		}

		if(network_exchange-- == 0)
		{
			network_exchange = network_exchange_initial;

			int err;
			if(server)
			{
				// read all keys from client, then send to client self keys
				recv_keys(sock, remote_keys);
				send_keys(sock, keys);
			}
			else if(client)
			{
				// send keys and then read read all keys from server
				send_keys(sock, keys);
				recv_keys(sock, remote_keys);
			}

			// handle keypress
			for(unsigned int i=0;i<keys.size();i++)
				w.handle_input(client, keys[i]);

			for(unsigned int i=0;i<remote_keys.size();i++)
				w.handle_input(!client, remote_keys[i]);

			keys.clear();
			remote_keys.clear();
		}

		w.think();

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 200, 200, 200));

		w.draw(screen);

		SDL_Flip(screen);

		// time not going if pause
		if(w.is_paused() == false)
			WorldTime::now++;

		// each loop 10 ms. if we too fast - sleep. if we too slow - do nothing.. it's a life.
		Uint32 time_diff = SDL_GetTicks() - start_loop;
		if(time_diff < 10)
		{
			SDL_Delay(10 - time_diff);
		}
	}

	for_each(TilesCache::main.begin(), TilesCache::main.end(), SDL_FreeSurface);

	if(server || client)
	{
		closesocket(sock);
		WSACleanup( );
	}

	return 0;
}
