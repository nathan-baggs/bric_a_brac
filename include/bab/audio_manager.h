#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "audio_clip.h"

namespace bab
{

/**
 * Class to handle all things related to audio.
 */
class AudioManager
{
  public:
    /**
     * Construct a new AudioManager.
     */
    AudioManager();

    /**
     * AudioManager specific cleanup.
     */
    ~AudioManager();

    /**
     * Load a WAV audio file. Will return the same object for the same filename.
     *
     * @param filename
     *   The name of the audio clip to load.
     *
     * @returns
     *   AudioClip object for loaded file.
     */
    const AudioClip *load(const std::string &filename);

  private:
    /** Map of filenames to loaded audio clip objects. */
    std::unordered_map<std::string, AudioClip> clips_;

    /** Handle to the device that will play audio. */
    std::uint32_t device_id_;

    /** The specification for the device. */
    ::SDL_AudioSpec spec_;

    /** Collection of loaded audio data. */
    std::vector<std::byte *> loaded_buffers_;
};

}
