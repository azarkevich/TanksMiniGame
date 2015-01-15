#pragma once

class SoundResource
{
public:
	Uint8 *buffer;
	Uint32 length;
	SDL_AudioSpec spec;
	SoundResource(const char *path)
	{
		if (SDL_LoadWAV(path, &spec, &buffer, &length) == NULL){
			std::terminate();
		}
	}

	~SoundResource()
	{
		SDL_FreeWAV(buffer);
	}
};

