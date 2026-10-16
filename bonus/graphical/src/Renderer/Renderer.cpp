/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Renderer
*/

#include "Renderer.hpp"

namespace Plazza {

Renderer::Renderer(sf::RenderWindow &window, Receptionist &receptionist)
    : _window(window), _receptionist(receptionist),
      _windowWidth(static_cast<float>(window.getSize().x)),
      _windowHeight(static_cast<float>(window.getSize().y)),
      _halfWidth(_windowWidth / 2.f),
      _groundY(_windowHeight * COOK_GROUND_RATIO),
      _queueStartX(550.f), _clientSlotWidth(CLIENT_SLOT_WIDTH)
{
    loadAssets();
}

void Renderer::loadAssets() {
    if (!_backgroundTexture.loadFromFile("Assets/Textures/background.png"))
        throw AssetLoadError("Assets/Textures/background.png");

    _background.setTexture(_backgroundTexture);
    _background.setTextureRect(sf::IntRect(0, 0,
        static_cast<int>(_backgroundTexture.getSize().x),
        static_cast<int>(_backgroundTexture.getSize().y)));
    _background.setPosition(0.f, 0.f);
    _background.setScale(4.f, 4.f);
    _layoutWidth = _backgroundTexture.getSize().x * _background.getScale().x;
    _layoutHeight = _backgroundTexture.getSize().y * _background.getScale().y;
    _halfWidth = _layoutWidth / 2.f;
    _groundY = _layoutHeight * COOK_GROUND_RATIO;

    if (!_cookIdleTexture.loadFromFile("Assets/Textures/cook_idle.png"))
        throw AssetLoadError("Assets/Textures/cook_idle.png");
    if (!_cookCookingTexture.loadFromFile("Assets/Textures/cook_cooking.png"))
        throw AssetLoadError("Assets/Textures/cook_cooking.png");
    if (!_cookSleepingTexture.loadFromFile("Assets/Textures/cook_sleeping.png"))
        throw AssetLoadError("Assets/Textures/cook_sleeping.png");
    if (!_cookChiefKissTexture.loadFromFile("Assets/Textures/chief_s_kiss.png"))
        throw AssetLoadError("Assets/Textures/chief_s_kiss.png");
    if (!_font.loadFromFile("Assets/Fonts/Sauce_Tomato.otf"))
        throw AssetLoadError("Assets/Fonts/Sauce_Tomato.otf");
    if (!_clientIdleTexture.loadFromFile("Assets/Textures/client_waiting.png"))
        throw AssetLoadError("Assets/Textures/client_idle.png");
    if (!_ingredientsPanelTexture.loadFromFile("Assets/Textures/ingredients.png"))
        throw AssetLoadError("Assets/Textures/ingredients.png");
    if (!_exitButtonTexture.loadFromFile("Assets/Textures/exit_button.png"))
        throw AssetLoadError("Assets/Textures/exit_button.png");

    _clientGroundY = _layoutHeight * COOK_GROUND_RATIO;
    _kitchenNumber.setFont(_font);
    _kitchenNumber.setCharacterSize(KITCHEN_NUMBER_SIZE);
    _kitchenNumber.setPosition(_halfWidth + _halfWidth * 0.5f - 20.f, 10.f);
    _queueStartX = _halfWidth * 0.4f;
    _exitButton.setTexture(_exitButtonTexture);
    _exitButton.setScale(5.f, 5.f);
    _exitButton.setPosition(20.f, 20.f);

    _audioManager.configure(_exitButton.getGlobalBounds());

    _inputBar.setSize(sf::Vector2f(_layoutWidth * 0.5f, 40.f));
    _inputBar.setFillColor(sf::Color(0, 0, 0, 160));
    _inputBar.setOutlineColor(sf::Color::White);
    _inputBar.setOutlineThickness(1.f);
    _inputBar.setPosition(_layoutWidth * 0.25f, _layoutHeight - 60.f);
    _inputText.setFont(_font);
    _inputText.setCharacterSize(18);
    _inputText.setFillColor(sf::Color::White);
    _inputText.setPosition(_layoutWidth * 0.25f + 10.f, _layoutHeight - 52.f);

    _temporaryMessage.setFont(_font);
    _temporaryMessage.setCharacterSize(26);
    _temporaryMessage.setFillColor(sf::Color(255, 255, 255));
    _temporaryMessage.setPosition(_layoutWidth * 0.25f, _layoutHeight - 110.f);

    _ingredientPanel.initialize(_ingredientsPanelTexture, _font);
}

void Renderer::synchronizeCookSprites(
    const std::map<int, KitchenStatusSnapshot> &snapshots)
{
    for (const auto &[kitchenId, snap] : snapshots) {
        if (_cookSpritesByKitchen.find(kitchenId) != _cookSpritesByKitchen.end())
            continue;

        std::vector<CookSprite> row;
        int cookCount = snap.cookCount;
        float spawnStart = _halfWidth + COOK_SPAWN_MARGIN;
        float spawnEnd = _layoutWidth - COOK_SPAWN_MARGIN - (COOK_FRAME_WIDTH * 4.f);
        if (spawnEnd < spawnStart)
            spawnEnd = spawnStart;

        float spacing = 0.f;
        if (cookCount > 1)
            spacing = (spawnEnd - spawnStart) / static_cast<float>(cookCount - 1);

        for (int i = 0; i < cookCount; i++) {
            float x = spawnStart + spacing * static_cast<float>(i);
            row.emplace_back(_cookIdleTexture, _cookCookingTexture, _cookSleepingTexture,
                _cookChiefKissTexture, COOK_FRAME_WIDTH, COOK_FRAME_HEIGHT, COOK_FRAMES,
                COOK_FRAMES, COOK_FRAMES, COOK_FRAMES, COOK_FRAME_TIME, x, _groundY);
            if (i < static_cast<int>(snap.cookChiefKissTokens.size()))
                row.back().rememberChiefKissToken(snap.cookChiefKissTokens[i]);
        }
        _cookSpritesByKitchen[kitchenId] = std::move(row);
    }

    for (auto it = _cookSpritesByKitchen.begin(); it != _cookSpritesByKitchen.end(); ) {
        if (snapshots.find(it->first) == snapshots.end())
            it = _cookSpritesByKitchen.erase(it);
        else
            it++;
    }
}

void Renderer::update(float deltaTime) {
    auto snapshots = _receptionist.getSnapshots();
    synchronizeCookSprites(snapshots);
    _audioManager.update(snapshots);
    updateClients(deltaTime);

    if (_temporaryMessageTimer > 0.f) {
        _temporaryMessageTimer -= deltaTime;
        if (_temporaryMessageTimer <= 0.f) {
            _temporaryMessageTimer = 0.f;
            _temporaryMessage.setString("");
        }
    }

    std::vector<int> kitchenIds;
    for (const auto &[id, _] : snapshots)
        kitchenIds.push_back(id);
    std::sort(kitchenIds.begin(), kitchenIds.end());

    if (kitchenIds.empty()) {
        _displayedKitchenCount = 0;
        _kitchenNumber.setString("No kitchens");
        _ingredientPanel.clear();
        return;
    }

    _displayedKitchenCount = static_cast<int>(kitchenIds.size());
    if (_currentKitchenIndex >= static_cast<int>(kitchenIds.size()))
        _currentKitchenIndex = static_cast<int>(kitchenIds.size()) - 1;

    int currentId = kitchenIds[_currentKitchenIndex];
    const auto &snap = snapshots[currentId];
    auto &sprites = _cookSpritesByKitchen[currentId];

    _ingredientPanel.update(snap);

    for (size_t i = 0; i < sprites.size() && i < snap.cookWorking.size(); i++) {
        CookState state = FREE;

        if (i < snap.cookStates.size()) {
            switch (snap.cookStates[i]) {
                case FREE:
                    state = FREE;
                    break;
                case COOKING:
                    state = COOKING;
                    break;
                case WAITING_FOR_REFILL:
                    state = WAITING_FOR_REFILL;
                    break;
            }
        } else
            state = snap.cookWorking[i] ? COOKING : FREE;
        sprites[i].setState(state);

        if (i < snap.cookChiefKissTokens.size())
            sprites[i].triggerChiefKiss(snap.cookChiefKissTokens[i]);

        sprites[i].update(deltaTime);
    }

    _kitchenNumber.setString(
        std::to_string(_currentKitchenIndex + 1)
        + "/" + std::to_string(kitchenIds.size())
    );
}

void Renderer::draw() {
    _window.draw(_background);
    drawKitchenNavigator();
    _window.draw(_exitButton);
    _audioManager.draw(_window);
    _window.draw(_inputBar);
    _window.draw(_inputText);
    if (_temporaryMessageTimer > 0.f)
        _window.draw(_temporaryMessage);

    auto snapshots = _receptionist.getSnapshots();
    std::vector<int> kitchenIds;
    for (const auto &[id, _] : snapshots)
        kitchenIds.push_back(id);
    std::sort(kitchenIds.begin(), kitchenIds.end());

    if (!kitchenIds.empty() && _currentKitchenIndex < static_cast<int>(kitchenIds.size())) {
        int currentId = kitchenIds[_currentKitchenIndex];

        auto it = _cookSpritesByKitchen.find(currentId);
        if (it != _cookSpritesByKitchen.end())
            for (auto &sprite : it->second)
                sprite.draw(_window);
    }
    
    _ingredientPanel.draw(_window);
    drawClients();
}

void Renderer::showTransientMessage(const std::string &message, float durationSeconds)
{
    _temporaryMessage.setString(message);
    _temporaryMessageTimer = durationSeconds;
}

void Renderer::drawKitchenNavigator() {
    if (_displayedKitchenCount > 0)
        _kitchenNumber.setString(
            std::to_string(_currentKitchenIndex + 1)
            + "/" + std::to_string(_displayedKitchenCount)
        );
    else
        _kitchenNumber.setString("No kitchens");
    _window.draw(_kitchenNumber);
}

void Renderer::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::TextEntered) {
        uint32_t c = event.text.unicode;
        if (c == '\r' || c == '\n') {
            if (_onOrderSubmit && !_inputBuffer.empty()) {
                _lastOrder = _inputBuffer;
                _onOrderSubmit(_inputBuffer);
            }
            _inputBuffer.clear();
        } else if (c == 8) {
            if (!_inputBuffer.empty())
                _inputBuffer.pop_back();
        } else if (c >= 32 && c < 128)
            _inputBuffer += static_cast<char>(c);
        _inputText.setString("> " + _inputBuffer);
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Right) {
            if (_currentKitchenIndex < _displayedKitchenCount - 1)
                _currentKitchenIndex++;
        } else if (event.key.code == sf::Keyboard::Left) {
            if (_currentKitchenIndex > 0)
                _currentKitchenIndex--;
        } else if (event.key.code == sf::Keyboard::Up) {
            if (!_lastOrder.empty()) {
                _inputBuffer = _lastOrder;
                _inputText.setString("> " + _inputBuffer);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );
        if (_exitButton.getGlobalBounds().contains(mouse))
            _audioManager.stopAll(),
            _window.close();
        else
            _audioManager.handleEvent(event);
    }
}

void Renderer::spawnClient(const std::string &orderText, int orderId) {
    int index = static_cast<int>(_clients.size());
    float x = _queueStartX - static_cast<float>(index) * _clientSlotWidth;

    _clients.emplace_back(_clientIdleTexture, _font, orderText, orderId, x, _clientGroundY);
    _audioManager.playOrderCue();
}

void Renderer::markOrderFinished(int orderId) {
    for (auto &client : _clients) {
        if (client.getOrderId() == orderId) {
            client.markFinished();
            break;
        }
    }
}

void Renderer::updateClients(float deltaTime) {
    for (auto &client : _clients)
        client.update(deltaTime);

    size_t before = _clients.size();
    _clients.remove_if([](const Client &c) { return c.isDead(); });
    if (_clients.size() != before)
        reindexClients();
}

void Renderer::drawClients() {
    for (auto &client : _clients)
        if (!client.isOffScreen(0.f))
            client.draw(_window);
}

void Renderer::reindexClients() {
    int index = 0;

    for (auto &client : _clients) {
        float x = _queueStartX - static_cast<float>(index) * _clientSlotWidth;
        client.slideTo(x, index * 0.2f);
        index++;
    }
}

}
