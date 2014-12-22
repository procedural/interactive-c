#define _BSD_SOURCE // usleep()
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include "game.h"

const char *GAME_LIBRARY = "./libgame.so";

struct game {
    void *handle;
    ino_t id;
    struct game_api api;
};

static void game_load(struct game *game)
{
    struct stat attr;
    if ((stat(GAME_LIBRARY, &attr) == 0) && (game->id != attr.st_ino)) {
        if (game->handle)
            dlclose(game->handle);
        void *handle = dlopen(GAME_LIBRARY, RTLD_NOW);
        if (handle) {
            game->handle = handle;
            game->id = attr.st_ino;
            game->api = *(struct game_api *)dlsym(game->handle, "GAME_API");
            game->api.init();
        } else {
            game->handle = NULL;
            game->id = 0;
        }
    }
}

int main(void)
{
    struct game game = {0};
    for (;;) {
        game_load(&game);
        if (game.handle)
            game.api.step();
        usleep(100000);
    }
    return 0;
}