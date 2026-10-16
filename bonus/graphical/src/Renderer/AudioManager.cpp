/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** AudioManager
*/

#include "AudioManager.hpp"
#include "Errors.hpp"

namespace Plazza {

void AudioManager::configure(const sf::FloatRect &exitButtonBounds)
{
    if (_configured)
        return;

    loadAssets();
    _soundButton.setTexture(_soundButtonTexture);
    _soundButton.setScale(3.f, 3.f);
    _soundButton.setPosition(exitButtonBounds.left + exitButtonBounds.width + 15.f, exitButtonBounds.top);
    updateSoundButtonTexture();
    _configured = true;
}

void AudioManager::loadAssets()
{
    if (!_soundButtonTexture.loadFromFile("Assets/Textures/sound_button.png"))
        throw AssetLoadError("Assets/Textures/sound_button.png");

    if (!_backgroundMusic.openFromFile("Assets/Audios/LeFestin.mp3"))
        throw AssetLoadError("Assets/Audios/LeFestin.mp3");
    _backgroundMusic.setLoop(true);
    _backgroundMusic.setVolume(35.f);
    _backgroundMusic.play();

    if (!_cookingMusic.openFromFile("Assets/Audios/cooking.mp3"))
        throw AssetLoadError("Assets/Audios/cooking.mp3");
    _cookingMusic.setLoop(true);
    _cookingMusic.setVolume(85.f);

    if (!_orderSound.openFromFile("Assets/Audios/ding.mp3"))
        throw AssetLoadError("Assets/Audios/ding.mp3");
    _orderSound.setVolume(15.f);
}

bool AudioManager::isCookingActive(const std::map<int, KitchenStatusSnapshot> &snapshots) const
{
    for (const auto &[_, snapshot] : snapshots) {
        for (char working : snapshot.cookWorking)
            if (working)
                return true;
    }
    return false;
}

void AudioManager::updateMusicVolumes()
{
    _backgroundMusic.setVolume(_audioEnabled ? 35.f : 0.f);
    _cookingMusic.setVolume(_audioEnabled ? 85.f : 0.f);
}

void AudioManager::update(const std::map<int, KitchenStatusSnapshot> &snapshots)
{
    _cookingActive = isCookingActive(snapshots);

    if (_backgroundMusic.getStatus() != sf::SoundSource::Playing)
        _backgroundMusic.play();

    if (_cookingActive) {
        if (_cookingMusic.getStatus() != sf::SoundSource::Playing)
            _cookingMusic.play();
    } else if (_cookingMusic.getStatus() == sf::SoundSource::Playing) {
        _cookingMusic.stop();
    }

    updateMusicVolumes();
}

void AudioManager::draw(sf::RenderWindow &window) const
{
    window.draw(_soundButton);
}

void AudioManager::updateSoundButtonTexture()
{
    sf::Vector2u size = _soundButtonTexture.getSize();
    int halfWidth = static_cast<int>(size.x / 2u);
    int rectX = _audioEnabled ? 0 : halfWidth;

    _soundButton.setTextureRect(sf::IntRect(rectX, 0, halfWidth, static_cast<int>(size.y)));
}

void AudioManager::handleEvent(const sf::Event &event)
{
    if (!_configured || event.type != sf::Event::MouseButtonPressed
        || event.mouseButton.button != sf::Mouse::Left)
        return;

    sf::Vector2f mouse(
        static_cast<float>(event.mouseButton.x),
        static_cast<float>(event.mouseButton.y)
    );

    if (!_soundButton.getGlobalBounds().contains(mouse))
        return;

    _audioEnabled = !_audioEnabled;
    updateSoundButtonTexture();
    updateMusicVolumes();
}

void AudioManager::playOrderCue()
{
    if (!_audioEnabled)
        return;

    _orderSound.stop();
    _orderSound.play();
}

void AudioManager::stopAll()
{
    _backgroundMusic.stop();
    _cookingMusic.stop();
    _orderSound.stop();
}

}