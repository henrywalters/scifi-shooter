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
    scene->entities.forEach<hg::audio::SourceComponent>([&](auto source, auto entity) {

    });
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

void AudioSystem::addSource(hg::Entity* entity, AudioChannel channel, std::string stream) {
    auto sourceC = entity->addComponent<hg::audio::SourceComponent>();
    auto buffer = getBuffer(channel, stream);
    auto source = m_players[channel]->addSource(entity->position());
    sourceC->channel = (int) channel;
    sourceC->source = source;
    sourceC->buffer = buffer;
    m_players[channel]->bindBuffer(buffer, source);
}

void AudioSystem::playSource(hg::Entity *entity) {
    auto sourceC = entity->getComponent<SourceComponent>();
    if (!sourceC) {
        return;
    }
    m_players[(AudioChannel) sourceC->channel]->playSource(sourceC->source);
}

void AudioSystem::updateSource(hg::Entity *entity, std::string stream) {
    auto sourceC = entity->getComponent<SourceComponent>();
    if (!sourceC) {
        return;
    }
    auto buffer = getBuffer((AudioChannel)sourceC->channel, stream);
    m_players[(AudioChannel)sourceC->channel]->bindBuffer(buffer, sourceC->source);
}

void AudioSystem::setSourcePosition(hg::Entity *entity, hg::Vec3 pos, hg::Vec3 velocity) {
    auto sourceC = entity->getComponent<SourceComponent>();
    if (!sourceC) {
        return;
    }
    m_players[(AudioChannel)sourceC->channel]->updateSource(sourceC->source, pos, velocity);
}
