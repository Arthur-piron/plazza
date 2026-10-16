/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Client
*/

#pragma once

#include "Animation.hpp"

#include <SFML/Graphics.hpp>
#include <string>

#define CLIENT_FRAME_WIDTH 64
#define CLIENT_FRAME_HEIGHT 64
#define CLIENT_FRAME_COUNT 7
#define CLIENT_FRAME_TIME 0.15f
#define CLIENT_FADE_SPEED 180.f
#define BANNER_PADDING 6.f
#define BANNER_CHAR_SIZE 13
#define CLIENT_SLOT_WIDTH  (CLIENT_FRAME_WIDTH * 2.f)
#define CLIENT_SLIDE_SPEED 300.f

namespace Plazza {
    class Client {
        public:
            Client(sf::Texture &idleTexture, sf::Font &font,
                const std::string &orderText, int orderId, float x, float groundY);
            void update(float deltaTime);
            void draw(sf::RenderWindow &window) const;
            void markFinished() { _dead = true; }
            bool isDead() const { return _dead; }
            int getOrderId() const {return _orderId; }
            void slideTo(float targetX, float delay = 0.f);
            bool isOffScreen(float leftBound) const;

        private:
            void updateBannerPosition();

            Animation _animation;
            sf::Sprite _sprite;
            sf::RectangleShape _bannerBackground;
            sf::Text _bannerText;
            int _orderId;
            bool _dead = false;
            float _targetX = 0.f;
            float _slideDelay = 0.f;
    };
}
