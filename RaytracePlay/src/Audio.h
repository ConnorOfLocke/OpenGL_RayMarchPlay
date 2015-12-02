#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <alc.h>
#include <al.h>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

struct AudioSource
{
	float pitch;
	float gain;
	glm::vec3 position;
	glm::vec3 velocity;
	bool looping;

	int audio_clip_id = -1;
};

struct AudioListener
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 at_dir;
	glm::vec3 up_dir;
	int id;
};

struct AudioClip
{
	ALuint buffer;
	char* file_name;
	short audio_format;
	short channels;

	int sample_rate;
	int byte_rate;

	short bitsPerSample;
	float duration;
};

class Audio
{
public:
	static bool Initailise();
	//static void Clear();
	//static void DestroyAudioSource(int audio_id);
	//static void DestroyAudioSource(int audio_listener);
	//static void DestroyAudioSource(int audio_listener);
	static void Destroy();

	static ALuint AddAudioSource(float pitch,
								float gain,
								glm::vec3 position,
								glm::vec3 velocity, 
								bool looping);

	static void UpdateAudioSource(	ALuint source_id,
									float pitch,
									float gain,
									glm::vec3 position,
									glm::vec3 velocity,
									bool looping);

	static int AddAudioListener(glm::vec3 position, 
								glm::vec3 velocity,
								glm::vec3 at_dir,
								glm::vec3 up_dir);

	static void UpdateCurrentAudioListener();
	static void UpdateAudioListener(int listener_id);
	
	static int GetCurrentAudioListener();
	static void SetCurrentAudioListener(int listener_id);

	static ALuint AddAudioClip_wav(char* file_name);
	static void AttachAudioClipToAudioSource(ALuint source_id, ALuint clip_id);



private:
	Audio();

	//Audio Manager
	static Audio* AM;

	int current_audio_listener = -1;

	ALCdevice* audio_device;
	ALCcontext* audio_context;

	std::vector<AudioListener> audio_listeners;
	std::vector<ALuint> audio_sources;
	std::vector<AudioClip> audio_clips;

	static int audio_listener_next_id;

	static unsigned int file_read_int32_le(char* buffer, FILE* file);
	static unsigned int file_read_int16_le(char* buffer, FILE* file);
	static void file_ignore_bytes(FILE* file, int nbytes);
	static unsigned char* file_allocate_and_read_bytes(FILE* file, int chunk_size);
	static ALenum GetFormatFromInfo(short channels, short bits_per_sample);
};


#endif