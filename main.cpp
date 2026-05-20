#include "PlatformUtils.hpp"
#include "Game.hpp"

int main() {
    findAndSetAssetsDirectory(); // 跨平台兼容编译

    // 启动!
    Game game;
    game.run(game);
    return 0;
}
