/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** CookSprite
*/

#pragma once

#include "Animation.hpp"
#include "Cook.hpp"

#include <SFML/Graphics.hpp>
#include <functional>

namespace Plazza {
    class CookSprite {
        public:
            CookSprite(sf::Texture &idleTexture, sf::Texture &cookingTexture, sf::Texture &sleepingTexture,
                sf::Texture &chiefKissTexture, int frameWidth, int frameHeight, int idleFrames,
                int cookingFrames, int sleepingFrames, int chiefKissFrames, float frameTime,
                float x, float y);
            void rememberChiefKissToken(int token);
            void setState(CookState state);
            void triggerChiefKiss(int token);
            void update(float deltaTime);
            void draw(sf::RenderWindow &window) const;

        private:
            void applyBaseState();

            Animation _idle;
            Animation _cooking;
            Animation _sleeping;
            Animation _chiefKiss;
            std::reference_wrapper<Animation> _current;
            CookState _baseState = FREE;
            int _lastChiefKissToken = 0;
            bool _chiefKissPlaying = false;
            sf::Sprite _sprite;
    };
}
