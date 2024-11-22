#ifndef MAIN_H
#define MAIN_H

#define PORT 8080
#define BUFFER_SIZE 100
#define NUMBER_SIZE (2 * sizeof(float))

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
struct nk_context;
//static
//int overview(struct nk_context *ctx);
int func(struct nk_context *ctx);

#endif //MAIN_H