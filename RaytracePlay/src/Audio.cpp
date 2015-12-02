#include "audio.h"
#include "gl_core_4_4.h"
#include <iostream>
#include "intrin.h"

Audio* Audio::AM = nullptr;
int Audio::audio_listener_next_id = 0;

Audio::Audio()
{
	audio_listeners = std::vector<AudioListener>();
	audio_sources = std::vector<ALuint>();

	ALCdevice* audio_device = nullptr;
	ALCcontext* audio_context = nullptr;
}

bool Audio::Initailise()
{
	AM = new Audio();

	//get audio handle
	AM->audio_device = alcOpenDevice(NULL);
	if (!AM->audio_device)
	{
		printf("No Audio Device detected \n");
		return false;
	}

	//check if "ALC_ENUMERATION_EXT" devices are supported
	ALboolean enumeration;
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	{
		printf("Enumeration devices not supported");
		return false;
	}
	//output list of audio devices on the machine 
	const ALCchar *device = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	const ALCchar *next = device + 1;
	size_t len = 0;

	printf("Audio devices detected:\n");
	printf("---\n");
	while (device && *device != '\0' && next && *next != '\0')
	{
		printf("%s\n", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
	printf("---\n");

	AM->audio_context = alcCreateContext(AM->audio_device, NULL);
	if (!alcMakeContextCurrent(AM->audio_context))
	{
		printf("Failed to set audio context");
		return false;
	}

	return true;
	//ALCenum error;
	//error = alGetError();
	//if (error != AL_NO_ERROR)
	//{
	//
	//}
}

void Audio::Destroy()
{
	for (unsigned int i = 0; i < AM->audio_sources.size(); i++)
		alDeleteSources(1, &AM->audio_sources[i]);

	for (unsigned int i = 0; i < AM->audio_clips.size(); i++)
		alDeleteBuffers(1, &AM->audio_clips[i].buffer);
	
	AM->audio_listeners.clear();
	AM->audio_sources.clear();
	AM->audio_clips.clear();

	alcDestroyContext(AM->audio_context);
	alcCloseDevice(AM->audio_device);

	delete AM;
}

ALuint Audio::AddAudioSource(float pitch, float gain, glm::vec3 position, glm::vec3 velocity, bool looping)
{
	ALuint source;

	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
	alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	alSourcei(source, AL_LOOPING, looping);

	AM->audio_sources.push_back(source);
	return source; 
}

void Audio::UpdateAudioSource(ALuint source_id, float pitch, float gain, glm::vec3 position, glm::vec3 velocity, bool looping)
{
	for each(ALuint source in AM->audio_sources)
	{
		if (source_id == source)
		{
			alSourcef(source, AL_PITCH, pitch);
			alSourcef(source, AL_GAIN, gain);
			alSource3f(source, AL_POSITION, position.x, position.y, position.z);
			alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
			alSourcei(source, AL_LOOPING, looping);
		}
	}
	printf("%d not found\n", source_id);
	return;

}

int Audio::AddAudioListener(glm::vec3 position, glm::vec3 velocity,	glm::vec3 at_dir, glm::vec3 up_dir)
{
	AudioListener new_listener;

	new_listener.position = position;
	new_listener.velocity = velocity;
	new_listener.at_dir = at_dir;
	new_listener.up_dir = up_dir;
	new_listener.id = Audio::audio_listener_next_id++;

	if (AM->current_audio_listener == -1)
	{
		SetCurrentAudioListener(new_listener.id);
	}

	AM->audio_listeners.push_back(new_listener);
	return new_listener.id;
}

int Audio::GetCurrentAudioListener()
{
	return AM->current_audio_listener;
}

void Audio::SetCurrentAudioListener(int listener_id)
{
	for each (AudioListener listener in AM->audio_listeners)
	{
		if (listener.id == listener_id)
		{
			alListener3f(AL_POSITION, listener.position.x, listener.position.y, listener.position.z);
			alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y, listener.velocity.z);
			ALfloat orientation[] = { listener.at_dir.x, listener.at_dir.y, listener.at_dir.z,
									listener.up_dir.x, listener.up_dir.y, listener.up_dir.z };

			alListenerfv(AL_ORIENTATION, orientation);
			return;
		}
	}

}

ALuint Audio::AddAudioClip_wav(char* file_name)
{
	AudioClip new_clip = AudioClip();

	FILE* file = fopen(file_name, "r");

	char input_buffer[5];
	input_buffer[4] = '\0';

	//first checks header for file type
	if (fread(input_buffer, sizeof(char), 4, file) != 4 || strcmp(input_buffer, "RIFF") != 0)
	{
		printf("%s not .wav file \n", file_name);
		fclose(file);
		return -1;
	}
	file_read_int32_le(input_buffer, file);

	if (fread(input_buffer, sizeof(char), 4, file) != 4 || strcmp(input_buffer, "WAVE") != 0)
	{
		printf("%s is a .wave not .wav file \n", file_name);
		fclose(file);
		return -1;
	}
	
	if (fread(input_buffer, sizeof(char), 4, file) != 4 || strcmp(input_buffer, "fmt") != 0)
	{
		printf("%s not valid format in .wav \n", file_name);
		fclose(file);
		return -1;
	}

	file_read_int32_le(input_buffer, file);
	
	//get all the file info
	new_clip.audio_format = file_read_int16_le(input_buffer, file);
	new_clip.channels = file_read_int16_le(input_buffer, file);

	new_clip.sample_rate = file_read_int32_le(input_buffer, file);
	new_clip.byte_rate = file_read_int32_le(input_buffer, file);

	file_read_int16_le(input_buffer, file);

	new_clip.bitsPerSample = file_read_int16_le(input_buffer, file);

	//ignore other header info
	if (new_clip.audio_format != 16)
	{
		short extra_params = file_read_int16_le(input_buffer, file);
		file_ignore_bytes(file, extra_params);
	}

	//checks for data first
	if (fread(input_buffer, sizeof(char), 4, file) != 4 || strcmp(input_buffer, "data") != 0)
	{
		printf("%s not valid format in .wav \n", file_name);
		fclose(file);
		return -1;
	}
	int data_chunk_size = file_read_int32_le(input_buffer, file);
	unsigned char* buffer_data = file_allocate_and_read_bytes(file, (size_t)data_chunk_size);

	new_clip.duration = (float)data_chunk_size / new_clip.byte_rate;
	alBufferData(new_clip.buffer, GetFormatFromInfo(new_clip.channels, new_clip.bitsPerSample), buffer_data, data_chunk_size, new_clip.sample_rate);

	AM->audio_clips.push_back(new_clip);

	//clean up this GOD AWFUL MESS
	free(buffer_data);
	fclose(file);

	return new_clip.buffer;
}

void Audio::AttachAudioClipToAudioSource(ALuint source_id, ALuint clip_id)
{
	alSourcei(source_id, AL_BUFFER, clip_id);
	
	for (int i = 0; AM->audio_clips.size(); i++)
	{

	}
}

unsigned int Audio::file_read_int32_le(char* buffer, FILE* file)
{
	fread(buffer, 1, 4, file);
	unsigned int return_value = *(unsigned int*)(buffer);
	return _byteswap_ulong(return_value);
}

unsigned int Audio::file_read_int16_le(char* buffer, FILE* file)
{
	fread(buffer, 1, 2, file);
	unsigned int return_value = *(unsigned int*)(buffer);
	return _byteswap_ushort(return_value);
}

void Audio::file_ignore_bytes(FILE* file, int nbytes)
{
	//skips over bytes in input stream
	while (nbytes > 0)
	{
		fgetc(file);
		nbytes--;
	}
}

unsigned char* Audio::file_allocate_and_read_bytes(FILE* file, int chunk_size)
{
	unsigned char* bytes = (unsigned char*)malloc(chunk_size);
	size_t bytes_read = fread(bytes, 1, chunk_size, file);
	return bytes;

}

ALenum Audio::GetFormatFromInfo(short channels, short bits_per_sample)
{
	if (channels == 1)
	{
		if (bits_per_sample == 16)
			return AL_FORMAT_MONO16;
		else
			return AL_FORMAT_MONO8;
	}
	if (bits_per_sample == 16)
		return AL_FORMAT_STEREO16;
	else
		return AL_FORMAT_STEREO8;
}
