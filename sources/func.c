#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "nuklear_cross.h"


#include "main.h"

#define INITIAL_SIZE 6
static const float ratio[] = {120, 150};
static char buffer_1[64]; // высота над уровнем моря
static char buffer_2[64]; // температура

static int buffer_1_len;
static int buffer_2_len;
// float dist[BUFFER_SIZE]; // BUFFER_SIZE 1024
int flag_pusk = 0;
void error_exit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
float numbers[2];
float result_array[BUFFER_SIZE]; // BUFFER_SIZE 100

extern SOCKET sockfd;
extern struct sockaddr_in server_addr, client_addr;
extern int addr_len;
int flag_first_start = 1;
float value;
int current_index = 0;
int templ = 0;


int func(struct nk_context *ctx)
{
    if (flag_first_start == 1)
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            result_array[i] = 0;
        }
        flag_first_start = 0;
    }
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
        if (nk_button_label(ctx, "Send Data!"))
        {
            fprintf(stdout, "Button pressed!\n");
            Altitude = atof(buffer_1);
            Temperature = atof(buffer_2);
            numbers[0] = Altitude;
            numbers[1] = Temperature;
            // отправляем на сервер 2 значения
            sendto(sockfd, (char *)numbers, NUMBER_SIZE, 0,
                   (struct sockaddr *)&client_addr, addr_len);
            // send(sockfd, (char*)numbers, sizeof(numbers), 0);
        }
        int line_index = -1;
        int i;
        int index = -1;
        float max = result_array[0], min = result_array[0];
        struct nk_rect bounds;
        if (!flag_pusk)
        {

            if (nk_button_label(ctx, "Start"))
            {
                flag_pusk = 1;
            }
        }
        else
        {
            // принимаем дистанцию из сокета
            // int bytes_received = recv(sockfd, (char*)result_array, sizeof(result_array), 0);
            // int n = recvfrom(sockfd, result_array, BUFFER_SIZE, 0,\
                         (struct sockaddr *)&client_addr, &addr_len);
            // int bytes_received = recvfrom(sockfd, (char*)&value, sizeof(float), 0,(struct sockaddr *)&client_addr, &addr_len);

            int bytes_received = recv(sockfd, (char *)&value, sizeof(float), 0);
            if (bytes_received < 0)
            {
                printf("recv failed func.c :: 125\n");
                //perror("recv failed");
            }
            else if (bytes_received == 0)
            {
                printf("Connection closed by the peer\n");
            }
            else
            {
                printf("Received %d bytes\n", bytes_received);
            }
            ///----------------------------------------------
            if (templ == 1000)
            {
                if (current_index >= BUFFER_SIZE)
                {
                    // Сдвигаем элементы влево на одну позицию
                    memmove(result_array, result_array + 1, (BUFFER_SIZE - 1) * sizeof(float));
                    // Записываем новое значение в последнюю ячейку
                    result_array[BUFFER_SIZE - 1] = value;
                }
                else
                {
                    // Если буфер еще не переполнен, просто добавляем новое значение
                    result_array[current_index] = value;
                    current_index++;
                }
                templ = 0;
            }
            templ++;

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
            /* line chart */
            index = -1;
            nk_layout_row_dynamic(ctx, 150, 1);
            bounds = nk_widget_bounds(ctx);
            if (nk_chart_begin(ctx, NK_CHART_LINES, BUFFER_SIZE, min, max + 1))
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
            if (nk_button_label(ctx, "Stop"))
            {
                flag_pusk = 0;
            }
        }
    }
    // closesocket(sockfd);
    // WSACleanup();
    nk_end(ctx);
    return !nk_window_is_closed(ctx, "Func");
}