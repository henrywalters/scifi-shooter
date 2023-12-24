//
// Created by henry on 11/22/23.
//
#include "audio.h"
#include <hagame/audio/player.h>
#include <hagame/core/scene.h>
#include <hagame/core/assets.h>

using namespace hg::audio;

AudioSystem::AudioSystem()
{
    m_players.insert(std::make_pair(AudioChannel::Music, std::make_unique<Player>()));
    m_players.insert(std::make_pair(AudioChannel::Sfx, std::make_unique<Player>()));

    m_players[AudioChannel::Music]->run();
    m_players[AudioChannel::Sfx]->run();
}

void AudioSystem::onUpdate(double dt) {

    int initialized = 0;
    int sources = 0;

    scene->entities.forEach<hg::audio::SourceComponent>([&](SourceComponent* source, auto entity) {

        if (!hg::hasAudioSource(source->streamName)) {
            std::cout << "Audio source does not exist: " << source->streamName << "\n";
            return;
        }

        sources++;

        if (m_sources.find(source->id()) == m_sources.end()) {
            m_sources.insert(std::make_pair(source->id(), addSource((AudioChannel) source->channel, source->streamName)));
        }
        auto audioSource = m_sources[source->id()];

        auto player = m_players[audioSource.channel].get();
        auto rawSource = player->getSource(audioSource.id);

        if (!rawSource) { // Source event hasn't been processed yet!
            return;
        }

        initialized++;

        auto settings = rawSource->settings();

        if (source->looping != settings.looping) {
            player->updateSource(audioSource.id, source->looping);
        }
        if (source->pitch != settings.pitch || source->gain != settings.gain) {
            player->updateSource(audioSource.id, source->pitch, source->gain);
        }
        if (source->velocity != settings.velocity || entity->position() != settings.position) {
            player->updateSource(audioSource.id, source->velocity, entity->position());
        }
    });

    if (initialized == sources && !m_played) {
        play();
    }
}

std::optional<AudioSource> AudioSystem::getSource(hg::Entity *entity) {
    auto source = entity->getComponent<SourceComponent>();
    if (!source) {
        return std::nullopt;
    }
    return m_sources[source->id()];
}

hg::audio::buffer_t AudioSystem::getBuffer(AudioChannel channel, std::string stream) {
    if (m_buffers.find(channel) == m_buffers.end()) {
        m_buffers.insert(std::make_pair(channel, std::unordered_map<std::string, buffer_t>()));
    }

    if (m_buffers[channel].find(stream) == m_buffers[channel].end()) {
        m_buffers[channel].insert(std::make_pair(stream, m_players[channel]->addBuffer(hg::getAudioStream(stream))));
    }

    return m_buffers[channel][stream];
}

AudioSource AudioSystem::addSource(AudioChannel channel, std::string stream) {
    AudioSource source;
    auto buffer = getBuffer(channel, stream);
    source.id = m_players[channel]->addSource();
    source.channel = channel;
    m_players[channel]->bindBuffer(buffer, source.id);
    return source;
}

void AudioSystem::playSource(AudioSource source) const {
    m_players.at((AudioChannel) source.channel)->playSource(source.id);
}

void AudioSystem::updateSource(AudioSource source, std::string stream) {
    auto buffer = getBuffer((AudioChannel)source.channel, stream);
    m_players[(AudioChannel)source.channel]->bindBuffer(buffer, source.id);
}

void AudioSystem::setSourcePosition(AudioSource source, hg::Vec3 pos, hg::Vec3 velocity) {
    m_players[(AudioChannel)source.channel]->updateSource(source.id, pos, velocity);
}

void AudioSystem::play() {
    scene->entities.forEach<SourceComponent>([&](SourceComponent* source, hg::Entity* entity) {
        AudioSource audioSource = m_sources[source->id()];
        Source* rawSource = m_players[audioSource.channel]->getSource(audioSource.id);
        SourceState state = rawSource->state();
        if (state != SourceState::Playing) {
            m_players[audioSource.channel]->playSource(audioSource.id);
        }
    });
    m_played = true;
}

void AudioSystem::pause() {
    scene->entities.forEach<SourceComponent>([&](SourceComponent* source, hg::Entity* entity) {
        auto audioSource = m_sources[source->id()];
        auto rawSource = m_players[audioSource.channel]->getSource(audioSource.id);
        if (rawSource->playing()) {
            m_players[audioSource.channel]->pauseSource(audioSource.id);
        }
    });
}

void AudioSystem::stop() {
    scene->entities.forEach<SourceComponent>([&](SourceComponent* source, hg::Entity* entity) {
        auto audioSource = m_sources[source->id()];
        auto rawSource = m_players[audioSource.channel]->getSource(audioSource.id);
        m_players[audioSource.channel]->stopSource(audioSource.id);
    });
    m_played = false;
}