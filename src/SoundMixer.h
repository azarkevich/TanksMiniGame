#pragma once

class SoundMixer
{
	class SoundBuffer
	{
	public:
		Uint8 *buffer;
		Uint32 length;
		Uint32 position;
		SoundBuffer(Uint8 *buffer, Uint32 length)
			: buffer(buffer), length(length), position(0)
		{
		}
	};

	std::vector<SoundBuffer> _current;
public:
	SoundMixer();
	~SoundMixer();

	void Init();

	void AudioCallback(Uint8 *stream, int len);

	void Play(Uint8 *buffer, Uint32 length);

	bool IsEmpty()
	{
		return _current.size() == 0;
	}
};

extern SoundMixer g_mixer;
