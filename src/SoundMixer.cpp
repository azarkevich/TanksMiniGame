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

void SoundMixer::Play(Uint8 *buffer, Uint32 length, Uint8 volume, bool cycle)
{
	_current.push_back(SoundBuffer(buffer, length, volume, cycle));
}

void SoundMixer::Pause(Uint8 *buffer, bool pauseOn)
{
	for (int i = 0; i < _current.size(); i++)
	{
		SoundBuffer &buff = _current[i];

		if (buff.buffer == buffer)
		{
			buff.paused = pauseOn;
		}
	}
}

void SoundMixer::Remove(Uint8 *buffer)
{
	for (int i = 0; i < _current.size(); i++)
	{
		SoundBuffer &buff = _current[i];

		if (buff.buffer == buffer)
		{
			_current.erase(_current.begin() + i);
			i--;
		}
	}
}

void SoundMixer::AudioCallback(Uint8 *stream, int len)
{
	if (_current.size() == 0)
		return;

	for (int i = 0; i < _current.size(); i++)
	{
		SoundBuffer &buff = _current[i];

		if (buff.paused)
			continue;

		int copy = len;
		int stream_pos = 0;
		while (copy > 0)
		{
			// copy necessary amount of information
			int part = copy;

			// or copy to buffer end if here is no enough data
			if (buff.length < (buff.position + copy))
				part = buff.length - buff.position;

			SDL_MixAudio(stream + stream_pos, buff.buffer + buff.position, part, buff.volume);
			buff.position += part;
			copy -= part;
			stream_pos += part;

			// reset position if we at end of buffer and play 
			if (buff.cycle && buff.position >= buff.length)
			{
				buff.position = 0;
			}

			// if no enough data for copy - exit even if copy still > 0
			if (buff.position >= buff.length)
			{
				break;
			}
		}

		// check if buffer should be removed
		if (buff.position >= buff.length)
		{
			_current.erase(_current.begin() + i);
			i--;
		}
	}
}
