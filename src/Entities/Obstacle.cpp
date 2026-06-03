#include "Entity.hpp"

void Obstacle::init(const sf::Texture* tex, float x, float y,
                    float spd, float screenW) {
    texture = tex;
    speed = spd;
    screenWidth = screenW;

    if (texture) {
        sprite = std::make_unique<sf::Sprite>(*texture);
        sprite->setPosition({x, y});
    }

    scored = false;
    consumed = false;
}

void Obstacle::update(float dt) {
    if (!sprite) return;
    sf::Vector2f pos = sprite->getPosition();
    pos.x -= speed * dt;
    sprite->setPosition(pos);
}

void Obstacle::draw(sf::RenderWindow& window) {
    if (sprite) {
        window.draw(*sprite);
    }
}

sf::FloatRect Obstacle::getBounds() const {
    if (!sprite) return sf::FloatRect();
    return sprite->getGlobalBounds();
}

bool Obstacle::isOffScreen() const {
    if (!sprite) return true;
    sf::FloatRect bounds = sprite->getGlobalBounds();
    return bounds.position.x + bounds.size.x < 0.f;
}

float Obstacle::getRightEdge() const {
    if (!sprite) return 0.f;
    return sprite->getPosition().x + sprite->getGlobalBounds().size.x;
}
