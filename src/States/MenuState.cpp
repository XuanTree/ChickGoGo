#include "Game.hpp"
#include "States/GameState.hpp"
#include "States/MenuState.hpp"

// ==================== MenuState ====================

MenuState::MenuState(Game& game) : game(game) {
    if (font.openFromFile("assets/fonts/unifont.otf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Font loaded failed ! " << std::endl;
    }
}

void MenuState::handleInput() {
    while (auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            game.changeState(std::make_unique<GameState>(game));
        }
    }
}

void MenuState::update(sf::Time deltaTime) {
    // 菜单无动态内容
}

void MenuState::draw() {
    auto& window = game.getWindow();

    window.clear(sf::Color(30, 30, 50));

    if (fontLoaded) {
        // 标题
        sf::Text title(font);
        title.setString(U"鸡仔冲冲冲");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color(255, 220, 50));
        title.setOutlineColor(sf::Color::Black);
        title.setOutlineThickness(2.f);
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin({tb.size.x / 2.f, tb.size.y / 2.f});
        title.setPosition({400.f, 200.f});
        window.draw(title);

        // 提示
        sf::Text hint(font);
        hint.setString(U"按Enter键开始游戏");
        hint.setCharacterSize(24);
        hint.setFillColor(sf::Color::White);
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
        hint.setPosition({400.f, 300.f});
        window.draw(hint);

        // 操作说明
        sf::Text controls(font);
        controls.setString(U"空格/W/Up: 跳过障碍物,按住可以跳得更高!!");
        controls.setCharacterSize(20);
        controls.setFillColor(sf::Color(180, 180, 180));
        sf::FloatRect cb = controls.getLocalBounds();
        controls.setOrigin({cb.size.x / 2.f, cb.size.y / 2.f});
        controls.setPosition({400.f, 400.f});
        window.draw(controls);
    }
}
