#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#pragma once
#include "string.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics/Font.hpp>
#include <map>
#include <memory>

namespace GameEngine {

class ResourceManager {
protected:
    const string name;
    const string path;
public:
    ResourceManager() = default;
    ResourceManager(const string& _name, const string& _path)
        : name(_name), path(_path) {}
    virtual ~ResourceManager() = default;

    // 禁止拷贝
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // 返回 true 表示加载成功
    virtual bool LoadResource() = 0;
    virtual void UnloadResource() = 0;

    const string& GetName() const { return name; }
    const string& GetPath() const { return path; }
};

class TextureManager : public ResourceManager {
private:
    sf::Texture texture;
    int referenceCount = 0;
public:
    TextureManager() = default;
    TextureManager(const string& _name, const string& _path)
        : ResourceManager(_name, _path) {}
    ~TextureManager() override = default;

    bool LoadResource() override;
    void UnloadResource() override;

    sf::Texture* GetTexture();
    const sf::Texture* GetTexture() const;

    void AddReference() { ++referenceCount; }
    void RemoveReference() { if (referenceCount > 0) --referenceCount; }
    int GetReferenceCount() const { return referenceCount; }
};

class SpriteManager : public ResourceManager {
private:
    TextureManager* textureManager = nullptr;
    std::unique_ptr<sf::Sprite> sprite;
    sf::IntRect textureRect;
    bool hasTextureRect = false;
public:
    SpriteManager() = default;
    SpriteManager(const string& _name, const string& _path)
        : ResourceManager(_name, _path) {}
    ~SpriteManager() override = default;

    bool LoadResource() override;
    void UnloadResource() override;

    void SetTextureManager(TextureManager* tm);
    void SetTextureRect(const sf::IntRect& rect);

    sf::Sprite* GetSprite();
    const sf::Sprite* GetSprite() const;
};

class FontManager : public ResourceManager {
private:
    sf::Font font;
    int referenceCount = 0;
public:
    FontManager() = default;
    FontManager(const string& _name, const string& _path)
        : ResourceManager(_name, _path) {}
    ~FontManager() override = default;
    // Functions
    bool LoadResource() override;
    void UnloadResource() override;
    sf::Font* GetFont();
    const sf::Font* GetFont() const;
};

class AudioManager : public ResourceManager {
private:
    sf::SoundBuffer soundBuffer;
    std::unique_ptr<sf::Sound> sound;
public:
    AudioManager() = default;
    AudioManager(const string& _name, const string& _path)
        : ResourceManager(_name, _path) {}
    ~AudioManager() override = default;

    bool LoadResource() override;
    void UnloadResource() override;

    sf::Sound* GetSound();
    const sf::Sound* GetSound() const;
};

class ResourceCache {
private:
    static ResourceCache* instance;

    std::map<string, TextureManager*> textures;
    std::map<string, SpriteManager*> sprites;
    std::map<string, AudioManager*> audios;
    std::map<string, FontManager*> fonts;

    ResourceCache() = default;
    ~ResourceCache();

public:
    static ResourceCache* GetInstance();
    static void DestroyInstance();

    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;

    // --- 纹理管理 ---
    TextureManager* LoadTexture(const string& name, const string& path);
    TextureManager* GetTexture(const string& name);
    bool UnloadTexture(const string& name);

    // --- 精灵管理 ---
    SpriteManager* CreateSprite(
        const string& name,
        const string& textureName,
        const sf::IntRect* textureRect = nullptr
    );
    SpriteManager* GetSprite(const string& name);
    bool UnloadSprite(const string& name);

    // --- 音频管理 ---
    AudioManager* LoadAudio(const string& name, const string& path);
    AudioManager* GetAudio(const string& name);
    bool UnloadAudio(const string& name);

    // --- 全局管理 ---
    void UnloadAll();
};

} // namespace GameEngine

#endif // RESOURCE_MANAGER_HPP
 