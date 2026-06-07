#include "Entity.hpp"
#include <SFML/Window/Keyboard.hpp>

void Egg::init(const sf::Texture* tex, float spawnX, float spawnY) {
    eggTexture = tex;
    startX = spawnX;
    startY = spawnY;

    if (eggTexture) {
        eggSprite = std::make_unique<sf::Sprite>(
            *eggTexture);
        eggSprite->setPosition({spawnX, spawnY});
    }

    eggVelocity = {0.f, 0.f};
    isHitBy = false;
    launched = false;
    dead = false;
}

void Egg::setPosition(sf::Vector2f pos) {
    if (eggSprite) {
        eggSprite->setPosition(pos);
    }
}

void Egg::launch(sf::Vector2f velocity) {
    launched = true;
    eggVelocity = velocity;
}

void Egg::update(float dt) {
    if (!eggSprite || !launched || dead) return;

    // 只对已发射的蛋施加重力
    eggVelocity.y += dropVelocity * dt;

    // 更新位置
    sf::Vector2f pos = eggSprite->getPosition();
    pos += eggVelocity * dt;
    eggSprite->setPosition(pos);
}

void Egg::draw(sf::RenderWindow& window) {
    if (!eggSprite || !launched || dead) return;

    // 绘制蛋
    window.draw(*eggSprite);
}

sf::FloatRect Egg::getBounds() const {
    if (!eggSprite) return sf::FloatRect();
    return eggSprite->getGlobalBounds();
}

bool Egg::markDead() {
    dead = true;
    return true;
}
