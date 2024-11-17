#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <winsock2.h>
#include <windows.h>

#include "nuklear_cross.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define NUMBER_SIZE (2 * sizeof(float))
//#include <net/ethernet.h>
#define INITIAL_SIZE 6
static const float ratio[] = {120, 150};
static char buffer_1[64]; // высота над уровнем моря
static char buffer_2[64]; // температура

static int buffer_1_len;
static int buffer_2_len;
float dist[BUFFER_SIZE];
int flag_pusk = 0;
void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int func(struct nk_context *ctx)
{
//--------------------------------------------
// Сокет
//--------------------------------------------
 WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    float numbers[2];
    float result_array[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error_exit("WSAStartup failed");
    }

      // Создаем UDP сокет
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Ошибка при создании сокета. Код ошибки: %d\n", WSAGetLastError());
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Настраиваем адрес сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Принимаем сообщения с любого IP
    server_addr.sin_port = htons(PORT); // Указываем порт

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Ошибка при привязке сокета. Код ошибки: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return EXIT_FAILURE;
    }
    // if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    //     WSACleanup();
    //     error_exit("Socket creation failed");
    // }

    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(PORT);
    // server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    //     closesocket(sockfd);
    //     WSACleanup();
    //     error_exit("Connect failed");
    // }
//--------------------------------------------
// Сокет конец
//--------------------------------------------
    static int show_menu = nk_true;
    static int titlebar = nk_true;
    static int border = nk_true;
    static int resize = nk_true;
    static int movable = nk_true;
    static int no_scrollbar = nk_false;
    static int scale_left = nk_false;
    static nk_flags window_flags = 0;
    static int minimizable = nk_true;

    /* popups */
    static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
    static int show_app_about = nk_false;

    window_flags = 0;
    ctx->style.window.header.align = header_align;
    if (border)
        window_flags |= NK_WINDOW_BORDER;
    if (resize)
        window_flags |= NK_WINDOW_SCALABLE;
    if (movable)
        window_flags |= NK_WINDOW_MOVABLE;
    if (no_scrollbar)
        window_flags |= NK_WINDOW_NO_SCROLLBAR;
    if (scale_left)
        window_flags |= NK_WINDOW_SCALE_LEFT;
    if (minimizable)
        window_flags |= NK_WINDOW_MINIMIZABLE;
    if (nk_begin(ctx, "Func", nk_rect(10, 10, 400, 600), window_flags))
    {
        float Altitude = 0;    // высота над уровнем моря
        float Temperature = 0; // температура
        nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
        nk_label(ctx, "Altitude:", NK_TEXT_LEFT); // высота над уровнем моря
        nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer_1, &buffer_1_len, 64, nk_filter_float);

        nk_label(ctx, "Temperature:", NK_TEXT_LEFT); // температура
        nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer_2, &buffer_2_len, 64, nk_filter_float);

        nk_layout_row_static(ctx, 30, 100, 3);
        if (nk_button_label(ctx, "Change Data"))
        {
            fprintf(stdout, "Button pressed!\n");
            Altitude = atof(buffer_1);
            Temperature = atof(buffer_2);
            numbers[0] = Altitude;
            numbers[1] = Temperature;
            // отправляем на сервер 2 значения 
            sendto(sockfd, numbers, NUMBER_SIZE, 0,
               (struct sockaddr *)&client_addr, addr_len);
            //send(sockfd, (char*)numbers, sizeof(numbers), 0);
        }
    if (!flag_pusk)
    {
        if (nk_button_label(ctx, "Start"))
        {
            // принимаем дистанцию из сокета
            int bytes_received = recv(sockfd, (char*)result_array, sizeof(result_array), 0);
            int n = recvfrom(sockfd, result_array, BUFFER_SIZE, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        ///----------------------------------------------
            float max = result_array[0], min = result_array[0];
            for (int i = 0; i < BUFFER_SIZE; i++)
            {
                if (result_array[i] > max)
                {
                    max = result_array[i];
                }
                if (result_array[i] < min)
                {
                    min = result_array[i];
                }
            }
        int line_index = -1;
            int i;
            int index = -1;
            struct nk_rect bounds;

            /* line chart */
            index = -1;
            nk_layout_row_dynamic(ctx, 100, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin(ctx, NK_CHART_LINES, BUFFER_SIZE, min, max))
            {
                for (i = 0; i < BUFFER_SIZE; ++i)
                {
                    nk_flags res = nk_chart_push(ctx, result_array[i]);
                    if (res & NK_CHART_HOVERING)
                        index = (int)i;
                    if (res & NK_CHART_CLICKED)
                    {
                        line_index = (int)i;
                    }
                }
                nk_chart_end(ctx);
            }
            if (index != -1)
                nk_tooltipf(ctx, "Value: %f", result_array[index]);
            if (line_index != -1)
            {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %f", result_array[line_index]);
            }
            nk_layout_row_static(ctx, 30, 160, 1);
            if (nk_button_label(ctx, "Delete Selected value"))
            {
                line_index = -1;
            }
           // nk_tree_pop(ctx);  
    
            flag_pusk = 1;
                closesocket(sockfd);
                 WSACleanup();
        }
    }
    else
    {
        if (nk_button_label(ctx, "Stop"))
        {
            flag_pusk = 0;
        }
    }
    }
    nk_end(ctx);
    return !nk_window_is_closed(ctx, "Func");
}