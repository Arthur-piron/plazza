/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** IngredientPanel
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

namespace Plazza {
    struct KitchenStatusSnapshot;

    class IngredientPanel {
        public:
            IngredientPanel();
            ~IngredientPanel() = default;

            void initialize(sf::Texture &panelTexture, const sf::Font &font);
            void update(const KitchenStatusSnapshot &snapshot);
            void clear();
            void draw(sf::RenderWindow &window) const;

        private:
            sf::Sprite _panelSprite;
            std::map<std::string, sf::Text> _ingredientTexts;
            bool _initialized = false;
    };
}
