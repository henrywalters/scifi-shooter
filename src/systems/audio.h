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



class AudioSystem : public hg::System {
public:

    struct Source {
        AudioChannel channel;
        hg::audio::source_t id;
    };

    AudioSystem();

    void onUpdate(double dt);

    // Play all audio sources
    void play();

    // Pause all audio sources
    void pause();

    // Stop all audio sources
    void stop();

    // Play an audio source attached to an entity
    void play(hg::Entity* entity);

private:


    // Fetch an AudioSource from an entity, if it exists
    std::optional<Source> getSource(hg::Entity* entity);

    // Create a new Audio Source that can play a stream
    Source addSource(AudioChannel channel, std::string stream);

    // Checks if buffer exists in channel, and adds it if necessary
    hg::audio::buffer_t getBuffer(AudioChannel channel, std::string stream);

    // Set a source to play a different audio stream
    void updateSource(Source source, std::string stream);

    void setSourcePosition(Source source, hg::Vec3 pos, hg::Vec3 velocity);

    // Play an audio source!
    void playSource(Source source) const;

    std::unordered_map<AudioChannel, std::unique_ptr<hg::audio::Player>> m_players;
    std::unordered_map<AudioChannel, std::unordered_map<std::string, hg::audio::buffer_t>> m_buffers;

    std::unordered_map<hg::utils::uuid_t, Source> m_sources;

    bool m_played = false;
};

#endif //SCIFISHOOTER_AUDIO_H
