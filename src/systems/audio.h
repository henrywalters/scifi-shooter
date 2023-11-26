//
// Created by henry on 11/20/23.
//

#ifndef SCIFISHOOTER_AUDIO_H
#define SCIFISHOOTER_AUDIO_H

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

    AudioSystem();

    // Add a SourceComponent to an entity and set it up to play a stream
    void addSource(hg::Entity* entity, AudioChannel channel, std::string stream);

    // Checks if buffer exists in channel, and adds it if necessary
    hg::audio::buffer_t getBuffer(AudioChannel channel, std::string stream);

    // Set a source to play a different audio stream
    void updateSource(hg::Entity* entity, std::string stream);

    void setSourcePosition(hg::Entity* entity, hg::Vec3 pos, hg::Vec3 velocity);

    // Play an audio source!
    void playSource(hg::Entity* entity);

    void onUpdate(double dt);

private:

    std::unordered_map<AudioChannel, std::unique_ptr<hg::audio::Player>> m_players;
    std::unordered_map<AudioChannel, std::unordered_map<std::string, hg::audio::buffer_t>> m_buffers;
};

#endif //SCIFISHOOTER_AUDIO_H
