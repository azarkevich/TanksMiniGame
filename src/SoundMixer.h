#pragma once

class SoundMixer
{
	class SoundBuffer
	{
	public:
		Uint8 volume;
		Uint8 *buffer;
		Uint32 length;
		Uint32 position;
		bool cycle;
		bool paused;
		SoundBuffer(Uint8 *buffer, Uint32 length, Uint8 volume, bool cycle)
			: volume(volume), buffer(buffer), length(length), position(0), cycle(cycle), paused(false)
		{
		}
	};

	std::vector<SoundBuffer> _current;
public:
	SoundMixer();
	~SoundMixer();

	void Init();

	void AudioCallback(Uint8 *stream, int len);

	void Play(Uint8 *buffer, Uint32 length, Uint8 volume = SDL_MIX_MAXVOLUME, bool cycle = false);
	void Pause(Uint8 *buffer, bool pauseOn);
	void Remove(Uint8 *buffer);

	bool IsEmpty()
	{
		return _current.size() == 0;
	}
};

extern SoundMixer g_mixer;
