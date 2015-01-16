#include "stdafx.h"
#include "SoundMixer.h"

SoundMixer g_mixer;

static void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
	SoundMixer* self = (SoundMixer*)userdata;
	self->AudioCallback(stream, len);
}

SoundMixer::SoundMixer()
{
}

SoundMixer::~SoundMixer()
{
	SDL_CloseAudio();
}

void SoundMixer::Init()
{
	// the specs of our piece of music
	SDL_AudioSpec audio_spec;

	// set the callback function
	audio_spec.callback = my_audio_callback;
	audio_spec.userdata = this;
	audio_spec.format = AUDIO_S16;
	audio_spec.freq = 44100;
	audio_spec.channels = 2;
	audio_spec.samples = 1024;

	/* Open the audio device */
	if (SDL_OpenAudio(&audio_spec, NULL) < 0)
	{
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		std::terminate();
	}

	SDL_PauseAudio(0);
}

void SoundMixer::Play(Uint8 *buffer, Uint32 length, Uint8 volume)
{
	_current.push_back(SoundBuffer(buffer, length, volume));
}

void SoundMixer::AudioCallback(Uint8 *stream, int len)
{
	if (_current.size() == 0)
		return;

	for (int i = 0; i < _current.size(); i++)
	{
		SoundBuffer &buff = _current[i];

		int copy = len;

		if (buff.length < (buff.position + len))
			copy = buff.length - buff.position;

		if (copy == 0)
		{
			_current.erase(_current.begin() + i);
			i--;
			continue;
		}

		SDL_MixAudio(stream, buff.buffer + buff.position, copy, buff.volume);
		buff.position += copy;

		if (buff.length < buff.position)
		{
			_current.erase(_current.begin() + i);
			i--;
			continue;
		}
	}
}
