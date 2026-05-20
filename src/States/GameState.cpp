#include "Game.hpp"
#include "States/GameState.hpp"
#include "ResourceManager.hpp"
#include <SFML/Window/Keyboard.hpp>
//#include <iostream>
#include <ctime>
#include <algorithm>

// ==================== GameState ====================

GameState::GameState(Game& game)
    : game(game)
    , rng(static_cast<unsigned>(std::time(nullptr)))
    , obstacleYDist(200.f, 432.f)   // 障碍物Y范围
{
    // 加载纹理
    auto* cache = ResourceCache::GetInstance();
    auto* chickTex = cache->LoadTexture(string("chick"), string("assets/sprites/chick.png"));
    auto* obsTex = cache->LoadTexture(string("obstacle"), string("assets/sprites/spike.png"));
    if (!chickTex) {
        // std::cerr << "[GameState] Failed to load chick texture!" << std::endl;
    }
    if (!obsTex) {
        // std::cerr << "[GameState] Failed to load obstacle texture!" << std::endl;
    }
    if (!chick.loadSounds("assets/sounds/jump.wav", "assets/sounds/hit.wav")) {
        // std::cerr << "[GameState] Failed to load chick sounds!" << std::endl;
    }

    // 加载背景音乐
    if (!bgm.openFromFile("assets/music/bgm.mp3")) {
        // std::cerr << "[GameState] Failed to load bgm!" << std::endl;
    }
    bgm.setLooping(true);
    bgm.play();

    // 创建障碍物纹理
    obstacleTexLoaded = obstacleTexture.loadFromFile("assets/sprites/spike.png");

    // 加载字体
    if (font.openFromFile("assets/fonts/unifont.otf")) {
        fontLoaded = true;
    } else {
        std::cerr << "Font Loaded Failed!" << std::endl;
    }

    // 初始化 Chick（14帧动画，精灵表水平排列）
    // sprite sheet: 4200x300, 14 frames → each 300x300
    float chickScale = 80.f / 300.f;
    chick.init(chickTex ? chickTex->GetTexture() : nullptr,
               14,                                       // frameCount
               100.f,                                    // startX
               500.f - 80.f,                             // startY
               500.f,                                    // groundY
               chickScale);
    // 地面
    ground.setSize({800.f, 100.f});
    ground.setPosition({0.f, 500.f});
    ground.setFillColor(sf::Color(100, 60, 20));  // 棕色地面
}

void GameState::handleInput() {
    while (auto event = game.getWindow().pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            game.getWindow().close();
        }
        // 暂停中只响应 ESC 和 q(quit)
        if (paused) {
            bgm.pause();
            static bool escWasPressed = false;
            static bool qWasPressed = false;
            bool escNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
            bool qNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
            if (escNow && !escWasPressed) {
                paused = false;
                bgm.setLooping(true);
                bgm.play();
            }
            else if (qNow && !qWasPressed) {
                game.getWindow().close();
            }
            qWasPressed = qNow;
            escWasPressed = escNow;
            return;
        }

        // 传递给 chick
        chick.handleInput();

        // ESC → 暂停（带边沿检测防连触发）
        static bool escPressed = false;
        bool escNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
        if (escNow && !escPressed) {
            paused = true;
        }
        escPressed = escNow;
    }
}

void GameState::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    if (dt > 0.1f) dt = 0.1f;

    if (paused) return;

    if (chick.isDead()) {
        bgm.pause();
        // 死亡后等待一小段时间，按回车可以重新开始
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            chick.reset(100.f, 300.f);
            obstacles.clear();
            score = 0;
            spawnTimer = 0.f;
            spawnInterval = 2.0f;
            bgm.setLooping(true);
            bgm.play();
        }
        return;
    }

    // 更新 chick
    chick.update(dt);

    // 更新障碍物
    for (auto& obs : obstacles) {
        obs.update(dt);
    }

    // 障碍物生成
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.f;
        spawnObstacle();
        // 难度递增
        if (spawnInterval > minSpawnInterval) {
            spawnInterval -= difficultyIncrease;
        }
    }

    // 碰撞检测
    checkCollisions();

    // 清理出屏障碍物
    cleanupObstacles();
}

void GameState::spawnObstacle() {
    Obstacle obs;
    obs.init(&obstacleTexture,
             850.f,                        // 屏幕右侧外
             obstacleYDist(rng),           // 随机Y
             200.f + score * 2.f,          // 速度随分数递增
             800.f);
    obstacles.push_back(std::move(obs));
}

void GameState::checkCollisions() {
    if (chick.isInvincible()) return;

    sf::FloatRect chickBounds = chick.getBounds();

    for (auto& obs : obstacles) {
        if (chickBounds.findIntersection(obs.getBounds())) {
            chick.takeDamage();
            chick.getHitSound().play();
        }

        // 计分：鸡仔完全越过障碍才记一分
        if (!obs.hasScored() && obs.getRightEdge() < chick.getPosition().x) {
            obs.markScored();
            if (obs.getBounds().position.y > chick.getPosition().y) {
                score++;
            }
        }
    }
}

void GameState::cleanupObstacles() {
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
                       [](const Obstacle& o) { return o.isOffScreen(); }),
        obstacles.end());
}

void GameState::draw() {
    auto& window = game.getWindow();

    // 背景
    window.clear(sf::Color(135, 206, 235));  // 天蓝色

    // 地面
    window.draw(ground);

    // 障碍物
    for (auto& obs : obstacles) {
        obs.draw(window);
    }

    // chick
    chick.draw(window);

    // HUD
    if (fontLoaded) {
        // 生命
        sf::Text healthText(font);
        healthText.setString("HP: " + std::to_string(chick.getHealth()));
        healthText.setCharacterSize(24);
        healthText.setFillColor(sf::Color::White);
        healthText.setOutlineColor(sf::Color::Black);
        healthText.setOutlineThickness(1.2f);
        healthText.setPosition({ 10.f, 40.f });
        window.draw(healthText);

        // 分数
        sf::Text scoreText(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineColor(sf::Color::Black);
        scoreText.setOutlineThickness(1.2f);
        scoreText.setPosition({ 10.f, 70.f });
        window.draw(scoreText);

        // 暂停
        sf::Text pauseHint(font);
        pauseHint.setString("ESC");
        pauseHint.setCharacterSize(24);
        pauseHint.setFillColor(sf::Color::Yellow);
        pauseHint.setOutlineColor(sf::Color::Black);
        pauseHint.setOutlineThickness(1.5f);
        pauseHint.setPosition({ 10.f, 10.f });
        window.draw(pauseHint);
    }

    // 暂停遮罩
    if (paused) {
        sf::RectangleShape overlay({800.f, 600.f});
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
            pauseText.setOrigin({pb.size.x / 2.f, pb.size.y / 2.f});
            pauseText.setPosition({400.f, 250.f});
            window.draw(pauseText);

            sf::Text hint(font);
            hint.setString(U"按ESC继续 或 按Q退出");
            hint.setCharacterSize(24);
            hint.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect hb = hint.getLocalBounds();
            hint.setOrigin({hb.size.x / 2.f, hb.size.y / 2.f});
            hint.setPosition({400.f, 320.f});
            window.draw(hint);
        }
    }

    // 死亡画面
    if (chick.isDead()) {
        sf::RectangleShape overlay({800.f, 600.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));

        window.draw(overlay);

        if (fontLoaded) {
            sf::Text deadText(font);
            deadText.setString("GAME OVER");
            deadText.setCharacterSize(48);
            deadText.setFillColor(sf::Color::Red);
            deadText.setOutlineColor(sf::Color::Black);
            deadText.setOutlineThickness(2.f);
            sf::FloatRect bounds = deadText.getLocalBounds();
            deadText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
            deadText.setPosition({400.f, 250.f});
            window.draw(deadText);

            sf::Text restartText(font);
            restartText.setString(U"按Enter键重来");
            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            sf::FloatRect rb = restartText.getLocalBounds();
            restartText.setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});
            restartText.setPosition({400.f, 320.f});
            window.draw(restartText);
        }
    }
}
