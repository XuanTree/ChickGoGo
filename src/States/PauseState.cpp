#include "Game.hpp"
#include "States/PauseState.hpp"
#include <iostream>

// ==================== PauseState ====================

PauseState::PauseState(Game& game) : game(game) {
    if (!font.openFromFile("assets/fonts/unifont.otf")) {
        std::cerr << "Failed to load Fonts!" << std::endl;
    } else {
        fontLoaded = true;
    }
}

void PauseState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        game.popState();
    }
}

void PauseState::update(sf::Time deltaTime) {
    // 暂停无动态内容
}

void PauseState::draw() {
    auto& window = game.getWindow();

    // 不清理窗口，让底层 GameState 的画面保持

    // 半透明遮罩
    sf::RectangleShape overlay({ 800.f, 600.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    if (fontLoaded) {
        sf::Text pauseText(font);
        pauseText.setString(U"暂停");
        pauseText.setCharacterSize(48);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setOutlineColor(sf::Color::Black);
        pauseText.setOutlineThickness(2.f);
        sf::FloatRect pb = pauseText.getLocalBounds();
        pauseText.setOrigin({ pb.size.x / 2.f, pb.size.y / 2.f });
        pauseText.setPosition({ 400.f, 250.f });
        window.draw(pauseText);

        sf::Text hint(font);
        hint.setString(U"按ESC继续，或者Q退出");
        hint.setCharacterSize(24);
        hint.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setOrigin({ hb.size.x / 2.f, hb.size.y / 2.f });
        hint.setPosition({ 400.f, 320.f });
        window.draw(hint);
    }
}
