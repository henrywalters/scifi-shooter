//
// Created by henry on 11/22/23.
//
#include "audio.h"
#include <hagame/audio/player.h>
#include <hagame/core/scene.h>
#include <hagame/core/assets.h>
#include <hagame/utils/profiler.h>

using namespace hg::audio;
using namespace hg::utils;

AudioSystem::AudioSystem()
{
    m_players.insert(std::make_pair(AudioChannel::Music, std::make_unique<Player>()));
    m_players.insert(std::make_pair(AudioChannel::Sfx, std::make_unique<Player>()));

    m_players[AudioChannel::Music]->run();
    m_players[AudioChannel::Sfx]->run();
}

void AudioSystem::onUpdate(double dt) {

    Profiler::Start("Audio::onUpdate");

    int initialized = 0;
    int sources = 0;

    scene->entities.forEach<AudioSource>([&](auto* source, auto entity) {

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

    //if (initialized == sources && !m_played) {
    //    play();
    //}

    Profiler::End("Audio::onUpdate");
}

std::optional<AudioSystem::Source> AudioSystem::getSource(hg::Entity *entity) {
    auto source = entity->getComponent<AudioSource>();
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

AudioSystem::Source AudioSystem::addSource(AudioChannel channel, std::string stream) {
    Source source;
    auto buffer = getBuffer(channel, stream);
    source.id = m_players[channel]->addSource();
    source.channel = channel;
    m_players[channel]->bindBuffer(buffer, source.id);
    return source;
}

void AudioSystem::playSource(AudioSystem::Source source) const {
    m_players.at((AudioChannel) source.channel)->playSource(source.id);
}

void AudioSystem::updateSource(AudioSystem::Source source, std::string stream) {
    auto buffer = getBuffer((AudioChannel)source.channel, stream);
    m_players[(AudioChannel)source.channel]->bindBuffer(buffer, source.id);
}

void AudioSystem::setSourcePosition(AudioSystem::Source source, hg::Vec3 pos, hg::Vec3 velocity) {
    m_players[(AudioChannel)source.channel]->updateSource(source.id, pos, velocity);
}

void AudioSystem::play() {
    scene->entities.forEach<AudioSource>([&](AudioSource* source, hg::Entity* entity) {
        AudioSystem::Source audioSource = m_sources[source->id()];
        SourceState state = m_players[audioSource.channel]->getSource(audioSource.id)->state();
        if (state != SourceState::Playing) {
            m_players[audioSource.channel]->playSource(audioSource.id);
        }
    });
    m_played = true;
}

void AudioSystem::pause() {
    scene->entities.forEach<AudioSource>([&](AudioSource* source, hg::Entity* entity) {
        auto audioSource = m_sources[source->id()];
        auto rawSource = m_players[audioSource.channel]->getSource(audioSource.id);
        if (rawSource->playing()) {
            m_players[audioSource.channel]->pauseSource(audioSource.id);
        }
    });
}

void AudioSystem::stop() {
    scene->entities.forEach<AudioSource>([&](AudioSource* source, hg::Entity* entity) {
        auto audioSource = m_sources[source->id()];
        m_players[audioSource.channel]->stopSource(audioSource.id);
    });
    m_played = false;
}

void AudioSystem::play(hg::Entity *entity) {
    auto source = getSource(entity);
    if (!source) {
        return;
    }
    playSource(source.value());
}