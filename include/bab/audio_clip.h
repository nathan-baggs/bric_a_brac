#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace bab
{

/**
 * Class wrapping a loaded audio clip that can be played.
 */
class AudioClip
{
  public:
    /**
     * Play the audio clip.
     */
    void play() const;

  private:
    // allow AudioManager to construct this object
    friend class AudioManager;

    // allow this object to be emplaced into an unordered_map
    friend std::pair<const std::string, AudioClip>;

    /**
     * Construct a new AudioClip, private so only AudioManager can call.
     *
     * @param device_id
     *   Handle to the device that will play the audio.
     *
     * @param audio_date
     *   The loaded audio data.
     */
    AudioClip(std::uint32_t device_id, std::span<std::byte> audio_data);

    /** Handle to device that will play audio. */
    std::uint32_t device_id_;

    /** Loaded audio data. */
    std::span<std::byte> audio_data_;
};

}
