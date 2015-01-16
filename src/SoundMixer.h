#pragma once

class SoundMixer
{
	class SoundBuffer
	{
	public:
		float volume;
		Sint16 *buffer;
		Uint32 length;
		Uint32 position;
		bool cycle;
		bool paused;
		SoundBuffer(Sint16 *buffer, Uint32 length, float volume, bool cycle)
			: volume(volume), buffer(buffer), length(length), position(0), cycle(cycle), paused(false)
		{
		}
	};

	std::vector<SoundBuffer> _current;
public:
	SoundMixer();
	~SoundMixer();

	void Init();

	void AudioCallback(Sint16 *stream, int len);

	void Play(Uint8 *buffer, Uint32 length, float volume = 1.0f, bool cycle = false);
	void Pause(Uint8 *buffer, bool pauseOn);
	void Remove(Uint8 *buffer);

	bool IsEmpty()
	{
		return _current.size() == 0;
	}
};

extern SoundMixer g_mixer;
