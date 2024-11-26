
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "main.h"
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_opengl.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
//#include <fcntl.h>
#include <ws2tcpip.h>

#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_SDL_GL2_IMPLEMENTATION
#include "nuklear_cross.h"
#include "nuklear_sdl_gl2.h"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

WSADATA wsaData;
SOCKET sockfd;
struct sockaddr_in server_addr, client_addr;
int addr_len = sizeof(client_addr);

int main(int argc, char *argv[])
{
    //--------------------------------------------
    // Сокет
    //--------------------------------------------
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("main::55 WSAStartup failed");
    }

    // Создаем UDP сокет
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        printf("error creating socket. error code: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

   u_long mode = 1; // 1 для неблокирующего режима
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        perror("ioctlsocket failed");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Принимаем сообщения с любого IP
    server_addr.sin_port = htons(PORT);       // Указываем порт 8080

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Error while binding socket. Error code: %d\n", WSAGetLastError()); // ошибка при привязке сокета
        closesocket(sockfd);
        WSACleanup();
        return EXIT_FAILURE;
    }

    /* Platform */
    SDL_Window *win;
    SDL_GLContext glContext;
    int win_width, win_height;
    int running = 1;

    /* GUI */
    struct nk_context *ctx;
    struct nk_colorf bg;

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    win = SDL_CreateWindow("Demo",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    glContext = SDL_GL_CreateContext(win);
    SDL_GetWindowSize(win, &win_width, &win_height);

    /* GUI */
    ctx = nk_sdl_init(win);
    {
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin(&atlas);

        nk_sdl_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        /*nk_style_set_font(ctx, &roboto->handle)*/;
    }

    // bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt))
        {
            if (evt.type == SDL_QUIT)
                goto cleanup;
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        func(ctx);
        /* ----------------------------------------- */

        /* Draw */
        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        // glClearColor(bg.r, bg.g, bg.b, bg.a);
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON);
        SDL_GL_SwapWindow(win);
    }
    //--------------------------------------------
    // Закрываем Сокет
    //--------------------------------------------
    closesocket(sockfd);
    WSACleanup();

cleanup:
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
// struct my_nkc_app {
//     struct nkc* nkcHandle;
// };
// void mainLoop(void* loopArg)
// {
// struct my_nkc_app* myapp = (struct my_nkc_app*)loopArg;
//     struct nk_context *ctx = nkc_get_ctx(myapp->nkcHandle);

//     union nkc_event e = nkc_poll_events(myapp->nkcHandle);
//     if( (e.type == NKC_EWINDOW) && (e.window.param == NKC_EQUIT) ){
//         nkc_stop_main_loop(myapp->nkcHandle);
//     }

//     func(ctx);
//     nkc_render(myapp->nkcHandle, nk_rgb(40,40,40) );
// }
//     int main(int argc, char *argv[]){
//     struct my_nkc_app myapp;
//     struct nkc nkcx; /* Allocate memory for Nuklear+ handle */
//     myapp.nkcHandle = &nkcx;

//     if( nkc_init( myapp.nkcHandle, "Nuklear+ Example", 640, 480, NKC_WIN_NORMAL ) )
//     {
//         nkc_set_main_loop(myapp.nkcHandle, mainLoop, (void*)&myapp );
//     } else
//     {
//         printf("Can't init NKC\n");
//     }

//     nkc_shutdown( myapp.nkcHandle );
//     return 0;
// }