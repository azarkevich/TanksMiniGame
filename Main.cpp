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
			
			//sockaddr_in client_addr;
			//int len = sizeof(client_addr);
			sock = accept(s, NULL, NULL);
//			sock = accept(s, (sockaddr *)&client_addr, &len);
			if(sock < 0)
			{
				cout << "Error in accept() " << sock << endl;
				exit(1);
			}
			
			cout << "Client connected." << endl;

			//closesocket(s);
			
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

	while(true)
	{
		int key = -1;
		while(SDL_PollEvent(&ev))
		{
			if((ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) || ev.type == SDL_QUIT)
			{
				exit(0);
			}

			if(ev.type == SDL_KEYDOWN)
			{
				key = ev.key.keysym.sym;
			}
		}
		
		int remote_key = -1;
		int local_key = -1;
		Uint32 system_ticks = 0;
		
		if(server || standalone)
		{
			system_ticks = SDL_GetTicks();
			local_key = key;
		}
		
		if(server)
		{
			int err;
			// server - local system
			err = send(sock, (const char *)&local_key, sizeof(local_key), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "send error: " << WSAGetLastError() << endl;
				exit(1);
			}
			err = send(sock, (const char *)&system_ticks, sizeof(system_ticks), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "send error: " << WSAGetLastError() << endl;
				exit(1);
			}
			err = recv(sock, (char *)&remote_key, sizeof(remote_key), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "recv error: " << WSAGetLastError() << endl;
				exit(1);
			}
		}
		else if(client)
		{
			int err;
			// client remote system
			remote_key = key;

			err = recv(sock, (char *)&local_key, sizeof(local_key), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "recv1 error: " << WSAGetLastError() << endl;
				exit(1);
			}
			recv(sock, (char *)&system_ticks, sizeof(system_ticks), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "recv2 error: " << WSAGetLastError() << endl;
				exit(1);
			}
			send(sock, (const char *)&remote_key, sizeof(remote_key), 0);
			if(err == SOCKET_ERROR)
			{
				cout << "send error: " << WSAGetLastError() << endl;
				exit(1);
			}
		}

		if(local_key != -1)
			w.handle_input(false, system_ticks, local_key);
		if(remote_key != -1)
			w.handle_input(true, system_ticks, remote_key);

		w.think(system_ticks);

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 200, 200, 200));

		w.draw(screen);

		SDL_Flip(screen);
	}

	for_each(TilesCache::main.begin(), TilesCache::main.end(), SDL_FreeSurface);

	if(server || client)
	{
		closesocket(sock);
		WSACleanup( );
	}

	return 0;
}
