/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** AudioManager
*/

#pragma once

#include "IPC/IPC.hpp"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>

namespace Plazza {
    class AudioManager {
        public:
            AudioManager() = default;

            void configure(const sf::FloatRect &exitButtonBounds);
            void update(const std::map<int, KitchenStatusSnapshot> &snapshots);
            void draw(sf::RenderWindow &window) const;
            void handleEvent(const sf::Event &event);
            void playOrderCue();
            void stopAll();

        private:
            void loadAssets();
            void updateMusicVolumes();
            void updateSoundButtonTexture();
            bool isCookingActive(const std::map<int, KitchenStatusSnapshot> &snapshots) const;

            bool _configured = false;
            bool _audioEnabled = true;
            bool _cookingActive = false;

            sf::Texture _soundButtonTexture;
            sf::Sprite _soundButton;

            sf::Music _backgroundMusic;
            sf::Music _cookingMusic;
            sf::Music _orderSound;
    };
}