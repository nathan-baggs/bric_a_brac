#include "audio_clip.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

#include "SDL.h"

namespace bab
{

AudioClip::AudioClip(std::uint32_t device_id, std::span<std::byte> audio_data)
    : device_id_(device_id)
    , audio_data_(audio_data)
{
}

void AudioClip::play() const
{
    assert(::SDL_QueueAudio(device_id_, audio_data_.data(), audio_data_.size()) == 0);
    ::SDL_PauseAudioDevice(device_id_, 0);
}

}
