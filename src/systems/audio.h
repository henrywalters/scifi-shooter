//
// Created by henry on 11/20/23.
//

#ifndef SCIFISHOOTER_AUDIO_H
#define SCIFISHOOTER_AUDIO_H

#include <optional>
#include <hagame/core/system.h>
#include <hagame/audio/player.h>
#include <hagame/core/entity.h>
#include <hagame/audio/components/source.h>
#include <hagame/audio/components/listener.h>

enum class AudioChannel {
    Music,
    Sfx,
};

struct AudioSource {
    AudioChannel channel;
    hg::audio::source_t id;
};

class AudioSystem : public hg::System {
public:

    AudioSystem();

    // Fetch an AudioSource from an entity, if it exists
    std::optional<AudioSource> getSource(hg::Entity* entity);

    // Create a new Audio Source that can play a stream
    AudioSource addSource(AudioChannel channel, std::string stream);

    // Checks if buffer exists in channel, and adds it if necessary
    hg::audio::buffer_t getBuffer(AudioChannel channel, std::string stream);

    // Set a source to play a different audio stream
    void updateSource(AudioSource source, std::string stream);

    void setSourcePosition(AudioSource source, hg::Vec3 pos, hg::Vec3 velocity);

    // Play an audio source!
    void playSource(AudioSource source) const;

    void onUpdate(double dt);

private:

    std::unordered_map<AudioChannel, std::unique_ptr<hg::audio::Player>> m_players;
    std::unordered_map<AudioChannel, std::unordered_map<std::string, hg::audio::buffer_t>> m_buffers;

    std::unordered_map<hg::utils::uuid_t, AudioSource> m_sources;
};

#endif //SCIFISHOOTER_AUDIO_H
