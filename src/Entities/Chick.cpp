#include "Entity.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Chick::Chick() = default;
Chick::~Chick() = default;

void Chick::init(const sf::Texture* tex, int fCount,
                 float startX, float startY, float gndY,
                 float scale) {
    texture = tex;
    frameCount = fCount;
    groundY = gndY;

    // 初始化蛋容器
    eggs = std::make_unique<std::vector<Egg>>();

    if (texture) {
        sf::Vector2u texSize = texture->getSize();
        frameWidth = static_cast<int>(texSize.x) / frameCount;
        frameHeight = static_cast<int>(texSize.y);

        sprite = std::make_unique<sf::Sprite>(
            *texture,
            sf::IntRect({0, 0}, {frameWidth, frameHeight}));
        sprite->setScale({scale, scale});
        sprite->setPosition({startX, startY});
    }

    velocity = {0.f, 0.f};
    health = 3;
    isOnGround = true;
    invincible = false;
    invincibleTimer = 0.f;
    currentFrame = 0;
    animationTimer = 0.f;
}

// 音效加载
bool Chick::loadSounds(const std::string& jumpPath, const std::string& hitPath,
    const std::string& scorePath, const std::string& healPath, const std::string& explosionPath) {
    if (jumpBuffer.loadFromFile(jumpPath) && hitBuffer.loadFromFile(hitPath)
    && scoreBuffer.loadFromFile(scorePath) && healBuffer.loadFromFile(healPath) 
    && explosionBuffer.loadFromFile(explosionPath)) {
        jumpSound = std::make_unique<sf::Sound>(jumpBuffer);
        hitSound = std::make_unique<sf::Sound>(hitBuffer);
        scoreSound = std::make_unique<sf::Sound>(scoreBuffer);
        healSound = std::make_unique<sf::Sound>(healBuffer);
        explosionSound = std::make_unique<sf::Sound>(explosionBuffer);
        soundsLoaded = true;
        return true;
    }
    return false;
}

void Chick::handleInput() {
    if (!sprite) return;
    bool keyDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

    // 在地面时起跳
    if (keyDown && isOnGround) {
        velocity.y = jumpVelocity;
        isOnGround = false;
        isJumpKeyHeld = true;
        jumpHoldClock.restart();
        jumpHoldDuration = 0.f;
        if (soundsLoaded) {
            jumpSound->play();
        }
    }
    // 在空中时按住持续额外加力
    if (isJumpKeyHeld && keyDown && !isOnGround) {
        float dt = jumpHoldClock.restart().asSeconds();
        jumpHoldDuration += dt;
        if (jumpHoldDuration <= maxJumpHoldTime) {
            velocity.y -= jumpBoostForce * dt;
        }
    }
    // 结束蓄力
    if (!keyDown) {
        isJumpKeyHeld = false;
        jumpHoldDuration = 0.f;
    }

    // Z 键发射蛋 —— 边沿触发，防连发
    static bool zWasPressed = false;
    bool zNow = (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down));
    if (zNow && !zWasPressed && getCurrentEggs() > 0) {
        launchEgg();
    }
    zWasPressed = zNow;
}

void Chick::launchEgg() {
    if (!eggs || currentEggs <= 0) return;

    // 找第一个未发射的蛋进行发射
    for (auto& egg : *eggs) {
        if (!egg.isLaunched()) {
            sf::Vector2f chickPos = getPosition();
            egg.setPosition({chickPos.x, chickPos.y - 20.f});
            egg.launch({300.f, -250.f});
            currentEggs--;
            break;
        }
    }
}

void Chick::update(float dt) {
    if (!sprite) return;

    // 重力
    velocity.y += gravity * dt;
    
    // 更新位置
    sf::Vector2f pos = sprite->getPosition();
    pos.y += velocity.y * dt;

    // 水平边界（使用缩放后的宽度）
    float scaledWidth = frameWidth * sprite->getScale().x;
    if (pos.x < leftBound) pos.x = leftBound;
    if (pos.x > rightBound - scaledWidth) pos.x = rightBound - scaledWidth;

    // 地面碰撞（使用缩放后的高度）
    float scaledHeight = frameHeight * sprite->getScale().y;
    if (pos.y >= groundY - scaledHeight) {
        pos.y = groundY - scaledHeight;
        velocity.y = 0.f;
        isOnGround = true;
    }

    sprite->setPosition(pos);

    // 动画
    animationTimer += dt;
    if (animationTimer >= animationSpeed) {
        animationTimer -= animationSpeed;
        currentFrame = (currentFrame + 1) % frameCount;
        sprite->setTextureRect(sf::IntRect(
            {currentFrame * frameWidth, 0},
            {frameWidth, frameHeight}));
    }

    // 无敌时间处理
    if (invincible) {
        invincibleTimer -= dt;
        if (invincibleTimer <= 0.f) {
            invincible = false;
            invincibleTimer = 0.f;
        }
    }
}

void Chick::draw(sf::RenderWindow& window) {
    if (!sprite) return;

    // 无敌时闪烁效果（每0.1秒交替显示/隐藏）
    if (invincible) {
        float blink = std::fmod(invincibleTimer, 0.2f);
        if (blink > 0.1f) {
            window.draw(*sprite);
        }
    } else {
        window.draw(*sprite);
    }
}

std::vector<Egg>& Chick::getEggs() {
    return *eggs;
}

void Chick::addEgg(Egg egg) {
    if (eggs) {
        eggs->push_back(std::move(egg));
        currentEggs = static_cast<int>(eggs->size());
    }
}
sf::FloatRect Chick::getBounds() const {
    if (!sprite) return sf::FloatRect();
    // 碰撞体略小于精灵，手感更好
    sf::FloatRect bounds = sprite->getGlobalBounds();
    float shrinkX = bounds.size.x * 0.2f;
    float shrinkY = bounds.size.y * 0.1f;
    return sf::FloatRect(
        {bounds.position.x + shrinkX / 2.f, bounds.position.y + shrinkY},
        {bounds.size.x - shrinkX, bounds.size.y - shrinkY});
}

void Chick::takeDamage() {
    if (invincible) return;

    health--;
    invincible = true;
    invincibleTimer = invincibleDuration;

    if (soundsLoaded && health > 0) {
        hitSound->play();
    }

    if (health <= 0) {
        health = 0;
    }
}

void Chick::setHealth(int newHealth) { health = newHealth; }

void Chick::setNewEggs(int newEgg) { currentEggs = newEgg; }

int Chick::getMaxHealth() const { return maxHealth; }

void Chick::reset(float x, float y) {
    if (sprite) {
        sprite->setPosition({x, y});
    }
    velocity = {0.f, 0.f};
    health = 3;
    isOnGround = true;
    invincible = false;
    invincibleTimer = 0.f;
    currentFrame = 0;
    animationTimer = 0.f;
    // 重置蛋容器
    if (eggs) {
        eggs->clear();
    }
    currentEggs = 0;
}

void Chick::initEggs(const sf::Texture* eggTex) {
    if (!eggs) {
        eggs = std::make_unique<std::vector<Egg>>();
    }
    eggs->clear();
    currentEggs = static_cast<int>(eggs->size());
}
