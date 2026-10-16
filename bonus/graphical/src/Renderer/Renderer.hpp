/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Renderer
*/

#pragma once

#include "CookSprite.hpp"
#include "IngredientPanel.hpp"
#include "AudioManager.hpp"
#include "Kitchen.hpp"
#include "Receptionist.hpp"
#include "Errors.hpp"
#include "Client.hpp"

#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <memory>
#include <functional>
#include <string>

#define COOK_FRAME_WIDTH  64
#define COOK_FRAME_HEIGHT  64
#define COOK_FRAMES  7
#define COOK_FRAME_TIME  0.15f
#define COOK_GROUND_RATIO  0.9
#define COOK_SPAWN_MARGIN  50.f
#define KITCHEN_NUMBER_SIZE  25

namespace Plazza {
    class Renderer {
        public:
            Renderer(sf::RenderWindow &window, Receptionist &receptionist);
            ~Renderer() = default;
            void update(float deltaTime);
            void draw();
            void handleEvent(const sf::Event &event);
            void spawnClient(const std::string &orderText, int orderId);
            void markOrderFinished(int orderId);
            void showTransientMessage(const std::string &message, float durationSeconds);
            void setOrderCallback(const std::function<void(const std::string&)> &cb) { _onOrderSubmit = cb; }
            int getDisplayedKitchenCount() const { return _displayedKitchenCount; }
            void setKitchenCount(int count) { _displayedKitchenCount = count; }
            void synchronizeCookSprites(const std::map<int, KitchenStatusSnapshot> &snapshots);

            AudioManager _audioManager;

        private:
            void loadAssets();
            void drawKitchenNavigator();
            void updateClients(float deltaTime);
            void drawClients();
            void reindexClients();

            sf::RenderWindow &_window;
            Receptionist &_receptionist;
            float _windowWidth;
            float _windowHeight;
            float _halfWidth;
            float _groundY;

            sf::Texture _backgroundTexture;
            sf::Sprite _background;
            sf::Font _font;
            sf::Text _kitchenNumber;

            std::map<int, std::vector<CookSprite>> _cookSpritesByKitchen;
            int _currentKitchenIndex = 0;
            sf::Texture _cookIdleTexture;
            sf::Texture _cookCookingTexture;
            sf::Texture _cookSleepingTexture;
            sf::Texture _cookChiefKissTexture;
            sf::Texture _clientIdleTexture;
            sf::Texture _ingredientsPanelTexture;
            std::list<Client> _clients;
            float _clientGroundY;
            float _queueStartX;
            float _clientSlotWidth;

            sf::Texture _exitButtonTexture;
            sf::Sprite _exitButton;
            sf::RectangleShape _inputBar;
            sf::Text _inputText;
            std::string _inputBuffer;
            std::function<void(const std::string&)> _onOrderSubmit;
            std::string _lastOrder;
            sf::Text _temporaryMessage;
            float _temporaryMessageTimer = 0.f;

            IngredientPanel _ingredientPanel;

            float _layoutWidth = 0.f;
            float _layoutHeight = 0.f;

            int _displayedKitchenCount = 0;
    };
}
