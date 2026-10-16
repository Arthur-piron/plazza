/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Client
*/

#include "Client.hpp"

namespace Plazza {

Client::Client(sf::Texture &idleTexture, sf::Font &font,
    const std::string &orderText, int orderId, float x, float groundY)
    : _animation(idleTexture, CLIENT_FRAME_WIDTH, CLIENT_FRAME_HEIGHT,
        CLIENT_FRAME_COUNT, CLIENT_FRAME_TIME), _orderId(orderId)
{
    _sprite.setTexture(idleTexture);
    _sprite.setTextureRect(_animation.getFrameRect());
    _sprite.setScale(4.f, 4.f);
    _sprite.setPosition(x, groundY - static_cast<float>(CLIENT_FRAME_HEIGHT) * 4.f);

    _bannerText.setFont(font);
    _bannerText.setString(orderText);
    _bannerText.setCharacterSize(BANNER_CHAR_SIZE);
    _bannerText.setFillColor(sf::Color::Black);

    sf::FloatRect textBounds = _bannerText.getLocalBounds();
    _bannerBackground.setSize(sf::Vector2f(
        textBounds.width  + BANNER_PADDING * 2.f,
        textBounds.height + BANNER_PADDING * 2.f
    ));
    _bannerBackground.setFillColor(sf::Color::White);
    _bannerBackground.setOutlineColor(sf::Color(180, 180, 180));
    _bannerBackground.setOutlineThickness(1.f);

    updateBannerPosition();
    _targetX = x;
}

void Client::updateBannerPosition() {
    sf::Vector2f spritePos  = _sprite.getPosition();
    sf::Vector2f bannerSize = _bannerBackground.getSize();

    float bannerX = spritePos.x + (CLIENT_FRAME_WIDTH * 3.f / 2.f) - (bannerSize.x / 2.f);
    float bannerY = spritePos.y - bannerSize.y - 6.f;

    _bannerBackground.setPosition(bannerX, bannerY);
    _bannerText.setPosition(bannerX + BANNER_PADDING, bannerY + BANNER_PADDING);
}

void Client::update(float deltaTime) {
    if (_dead)
        return;

    if (_slideDelay > 0.f) {
        _slideDelay -= deltaTime;
        return;
    }

    float currentX = _sprite.getPosition().x;
    if (std::abs(currentX - _targetX) > 1.f) {
        float direction = (_targetX > currentX) ? 1.f : -1.f;
        float move = CLIENT_SLIDE_SPEED * deltaTime;
        float newX = currentX + direction * move;

        if (std::abs(newX - _targetX) > std::abs(currentX - _targetX))
            newX = _targetX;
        _sprite.setPosition(newX, _sprite.getPosition().y);
        updateBannerPosition();
    }
    _animation.update(deltaTime);
    _sprite.setTextureRect(_animation.getFrameRect());
}

void Client::draw(sf::RenderWindow &window) const {
    if (_dead)
        return;

    window.draw(_bannerBackground);
    window.draw(_bannerText);
    window.draw(_sprite);
}

void Client::slideTo(float targetX, float delay) {
    _targetX = targetX;
    _slideDelay = delay;
}

bool Client::isOffScreen(float leftBound) const {
    return _sprite.getPosition().x
        + CLIENT_SLOT_WIDTH < leftBound;
}

}
