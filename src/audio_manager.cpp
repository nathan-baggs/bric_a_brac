#include "audio_manager.h"

#include <cassert>
#include <cstdint>
#include <span>
#include <string>
#include <unordered_map>

#include "SDL.h"

#include "audio_clip.h"

namespace bab
{

AudioManager::AudioManager()
    : clips_()
    , device_id_(0u)
    , spec_()
    , loaded_buffers_()
{
    // setup SDL for just audio
    ::SDL_Init(SDL_INIT_AUDIO);
    device_id_ = ::SDL_OpenAudioDevice(nullptr, 0, &spec_, nullptr, 0);
    assert(device_id_ != 0u);
}

AudioManager::~AudioManager()
{
    for (auto *buffer : loaded_buffers_)
    {
        ::SDL_FreeWAV(reinterpret_cast<std::uint8_t *>(buffer));
    }
}

const AudioClip *AudioManager::load(const std::string &filename)
{
    std::uint32_t length = 0u;
    std::byte *buffer = nullptr;

    assert(
        ::SDL_LoadWAV("assets/box-crash.wav", &spec_, reinterpret_cast<std::uint8_t **>(&buffer), &length) != nullptr);
    const auto [clip, _] = clips_.try_emplace(filename, device_id_, std::span<std::byte>{buffer, length});

    loaded_buffers_.push_back(buffer);

    return std::addressof(clip->second);
}

}