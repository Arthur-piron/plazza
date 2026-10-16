/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Animation
*/

#pragma once

#include <SFML/Graphics.hpp>

namespace Plazza {
    class Animation {
        public:
            Animation(sf::Texture &texture, int frameWidth, int frameHeight, int frameCount, float frameTime, bool loop = true);
            void update(float deltaTime);
            void reset();

            const sf::IntRect &getFrameRect() const { return _frameRect; }
            sf::Texture &getTexture() const { return _texture; }
            bool isFinished() const { return _finished; }

        private:
            sf::Texture &_texture;
            sf::IntRect _frameRect;
            int _frameWidth;
            int _frameCount;
            int _currentFrame = 0;
            float _frameTime;
            float _timer = 0.f;
            bool _loop;
            bool _finished = false;
    };
}
