/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** IngredientPanel
*/

#include "IngredientPanel.hpp"
#include "IPC/IPC.hpp"

namespace Plazza {

IngredientPanel::IngredientPanel()
{
}

void IngredientPanel::initialize(sf::Texture &panelTexture, const sf::Font &font)
{
    _panelSprite.setTexture(panelTexture);
    _panelSprite.setPosition(0.f, 0.f);
    _panelSprite.setScale(4.f, 4.f);

    std::vector<std::string> ingredients = {
        "tomato", "gruyere", "steak", "eggplant",
            "goatCheese", "ham", "dough", "mushroom"
    };

    float row1Y = 415.f;
    float row2Y = 550.f;
    std::vector<float> xPositions = {230.f, 365.f, 505.f, 637.f};

    for (size_t i = 0; i < ingredients.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);

        float y = (i < 4) ? row1Y : row2Y;
        float x = xPositions[i % 4];
        text.setPosition(960.f + x, y);
        _ingredientTexts[ingredients[i]] = text;
    }
    _initialized = true;
}

void IngredientPanel::update(const KitchenStatusSnapshot &snapshot)
{
    if (!_initialized)
        return;

    std::map<std::string, int> values = {
        {"dough", snapshot.dough},
        {"tomato", snapshot.tomato},
        {"gruyere", snapshot.gruyere},
        {"ham", snapshot.ham},
        {"mushroom", snapshot.mushroom},
        {"steak", snapshot.steak},
        {"eggplant", snapshot.eggplant},
        {"goatCheese", snapshot.goatCheese}
    };

    for (auto &[name, text] : _ingredientTexts) {
        if (values.find(name) != values.end()) {
            int value = values[name];
            if (value > 9)
                text.setString("9+");
            else
                text.setString(std::to_string(value));
        }
    }
}

void IngredientPanel::clear()
{
    for (auto &[_, text] : _ingredientTexts)
        text.setString("");
}

void IngredientPanel::draw(sf::RenderWindow &window) const
{
    if (!_initialized)
        return;

    window.draw(_panelSprite);
    for (const auto &[_, text] : _ingredientTexts)
        window.draw(text);
}

}
