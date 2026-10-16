/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** CookSprite
*/

#include "CookSprite.hpp"

namespace Plazza {

CookSprite::CookSprite(sf::Texture &idleTexture, sf::Texture &cookingTexture, sf::Texture &sleepingTexture,
    sf::Texture &chiefKissTexture, int frameWidth, int frameHeight, int idleFrames,
    int cookingFrames, int sleepingFrames, int chiefKissFrames, float frameTime, float x, float y)
    : _idle(idleTexture, frameWidth, frameHeight, idleFrames, frameTime),
    _cooking(cookingTexture, frameWidth, frameHeight, cookingFrames, frameTime),
    _sleeping(sleepingTexture, frameWidth, frameHeight, sleepingFrames, frameTime),
    _chiefKiss(chiefKissTexture, frameWidth, frameHeight, chiefKissFrames, frameTime, false),
    _current(_idle)
{
    _sprite.setTexture(_current.get().getTexture());
    _sprite.setTextureRect(_current.get().getFrameRect());
    _sprite.setScale(4.f, 4.f);
    _sprite.setPosition(x, y - static_cast<float>(frameHeight) * 4.f);
}

void CookSprite::rememberChiefKissToken(int token)
{
    _lastChiefKissToken = token;
}

void CookSprite::applyBaseState()
{
    std::reference_wrapper<Animation> next = _idle;

    switch (_baseState) {
        case FREE:
            next = _idle;
            break;
        case COOKING:
            next = _cooking;
            break;
        case WAITING_FOR_REFILL:
            next = _sleeping;
            break;
    }

    if (&next.get() != &_current.get()) {
        _current = next;
        _current.get().reset();
        _sprite.setTexture(_current.get().getTexture());
    }
}

void CookSprite::setState(CookState state) {
    _baseState = state;
    if (!_chiefKissPlaying)
        applyBaseState();
}

void CookSprite::triggerChiefKiss(int token)
{
    if (token <= _lastChiefKissToken)
        return;

    _lastChiefKissToken = token;
    _chiefKissPlaying = true;
    _current = _chiefKiss;
    _current.get().reset();
    _sprite.setTexture(_current.get().getTexture());
}

void CookSprite::update(float deltaTime) {
    _current.get().update(deltaTime);
    _sprite.setTextureRect(_current.get().getFrameRect());

    if (_chiefKissPlaying && _current.get().isFinished()) {
        _chiefKissPlaying = false;
        applyBaseState();
        _sprite.setTextureRect(_current.get().getFrameRect());
    }
}

void CookSprite::draw(sf::RenderWindow &window) const {
    window.draw(_sprite);
}

}
