#pragma once

class SoundResource
{
public:
	Uint8 *buffer;
	Uint32 length;
	SDL_AudioSpec spec;
	SoundResource(const char *path, float volume = 1)
	{
		if (SDL_LoadWAV(path, &spec, &buffer, &length) == NULL){
			std::terminate();
		}

		if (volume != 1)
		{
			Sint16 *buffer16 = (Sint16 *)buffer;
			for (int s = 0; s < length / 2; s++)
			{
				buffer16[s] = buffer16[s] * volume;
			}
		}
	}

	~SoundResource()
	{
		SDL_FreeWAV(buffer);
	}
};

