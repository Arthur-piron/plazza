/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Animation
*/

#include "Animation.hpp"

namespace Plazza {

Animation::Animation(sf::Texture &texture, int frameWidth, int frameHeight, int frameCount, float frameTime, bool loop)
    : _texture(texture), _frameRect(0, 0, frameWidth, frameHeight), _frameWidth(frameWidth),
      _frameCount(frameCount), _frameTime(frameTime), _loop(loop)
{
}

void Animation::update(float deltaTime) {
    if (_finished || _frameCount <= 1)
        return;

    _timer += deltaTime;
    while (_timer >= _frameTime && !_finished) {
        _timer -= _frameTime;
        if (_loop)
            _currentFrame = (_currentFrame + 1) % _frameCount;
        else if (_currentFrame + 1 >= _frameCount) {
            _currentFrame = _frameCount - 1;
            _finished = true;
        } else
            _currentFrame++;
        _frameRect.left = _currentFrame * _frameWidth;
    }
}

void Animation::reset() {
    _currentFrame = 0;
    _timer = 0.f;
    _frameRect.left = 0;
    _finished = false;
}

}
