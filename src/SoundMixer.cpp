#include "stdafx.h"
#include "SoundMixer.h"

SoundMixer g_mixer;

static void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
	SoundMixer* self = (SoundMixer*)userdata;
	self->AudioCallback((Sint16 *)stream, len / 2);
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

void SoundMixer::Play(Uint8 *buffer, Uint32 length, float volume, bool cycle)
{
	_current.push_back(SoundBuffer((Sint16 *)buffer, length / 2, volume, cycle));
}

void SoundMixer::Pause(Uint8 *buffer, bool pauseOn)
{
	for (int i = 0; i < _current.size(); i++)
	{
		SoundBuffer &buff = _current[i];

		if (buff.buffer == (Sint16 *)buffer)
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

		if (buff.buffer == (Sint16 *)buffer)
		{
			_current.erase(_current.begin() + i);
			i--;
		}
	}
}

void SoundMixer::AudioCallback(Sint16 *stream, int len)
{
	if (_current.size() == 0)
		return;

	memset(stream, 0, len * 2);

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

			for (int s = 0; s < part; s++)
			{
				/*
				int sum = stream[stream_pos + s];
				Sint16 val = buff.buffer[buff.position + s];

				if (buff.volume != 1)
				{
					//val = (Sint16)(val * buff.volume);
				}

				sum += val;
				if (sum > 32767)
					sum = 32767;
				if (sum < -32768)
					sum = -32768;

				stream[stream_pos + s] = (Sint16)sum;
				*/

				float A = stream[stream_pos + s] / 32768.0;
				float B = buff.buffer[buff.position + s] / 32768.0;
				float C = (A + B) - A*B;

				stream[stream_pos + s] = C * 32768.0;
			}
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
