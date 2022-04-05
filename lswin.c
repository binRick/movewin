/* ========================================================================
 * lswin.c - list windows
 * Andrew Ho (andrew@zeuscat.com)
 *
 * Copyright (c) 2014-2020, Andrew Ho.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the author nor the names of its contributors may
 * be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ========================================================================
 */

#include "winutils.h"

#include "parson.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ME "lswin"
#define USAGE "usage: " ME " [-h] [-l] [-j] [-i id] [title]\n"
#define FULL_USAGE USAGE \
    "    -h       display this help text and exit\n" \
    "    -l       long display, include window ID column in output\n" \
    "    -j       json output mode\n" \
    "    -i id    show only windows with this window ID (-1 for all)\n" \
    "    title    pattern to match \"Application - Title\" against\n"

typedef struct {
    int longDisplay;   /* include window ID column in output */
    int id;            /* show only windows with this window ID (-1 for all) */
    int numFound;      /* out parameter, number of windows found */
    int jsonMode;      /* json output mode */
} LsWinCtx;



/* Callback for EnumerateWindows() prints title of each window it encounters */
void PrintWindow(CFDictionaryRef window, void *ctxPtr) {
    LsWinCtx *ctx = (LsWinCtx *)ctxPtr;
    int windowId = CFDictionaryGetInt(window, kCGWindowNumber);
    char *appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
    char *windowName = CFDictionaryCopyCString(window, kCGWindowName);
    char *title = windowTitle(appName, windowName);
    CGPoint position = CGWindowGetPosition(window);
    CGSize size = CGWindowGetSize(window);

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    char *pretty_serialized_string = NULL;
    json_object_set_string(root_object, "appName", appName);
    json_object_set_string(root_object, "windowName", windowName);
    json_object_set_string(root_object, "title", title);
    json_object_set_number(root_object, "windowId", windowId);
    json_object_dotset_number(root_object, "size.height", (int)size.height);
    json_object_dotset_number(root_object, "size.width", (int)size.width);
    json_object_dotset_number(root_object, "position.x", (int)position.x);
    json_object_dotset_number(root_object, "position.y", (int)position.y);
    pretty_serialized_string = json_serialize_to_string_pretty(root_value);
    serialized_string = json_serialize_to_string(root_value);


    if(ctx->id == -1 || ctx->id == windowId) {
        if(ctx->jsonMode){
            printf("%s",serialized_string);
        }else if(ctx->longDisplay) {
            printf(
                "%s - %s %d %d %d %d %d\n", title,
                appName,
                (int)windowId,
                (int)position.x, (int)position.y,
                (int)size.width, (int)size.height
            );
        }else {
            printf("%d\n", windowId);
        }
        ctx->numFound++;
    }
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    free(title);
    free(windowName);
    free(appName);
}

int main(int argc, char **argv) {
    LsWinCtx ctx;
    int ch;
    char *pattern = NULL;

#define DIE(msg) { fprintf(stderr, ME ": " msg "\n"); exit(1); }
#define DIE_OPT(msg) \
    { fprintf(stderr, ME ": " msg " -- %c\n" USAGE, optopt); return 1; }

    /* Parse and sanitize command line arguments */
    ctx.longDisplay = 0;
    ctx.id = -1;
    ctx.numFound = 0;
    while((ch = getopt(argc, argv, ":jhli:")) != -1) {
        switch(ch) {
            case 'j':
                ctx.jsonMode = 1;
                break;
            case 'h':
                 printf(FULL_USAGE);
                 return 0;
            case 'l':
                ctx.longDisplay = 1;
                break;
            case 'i':
                ctx.id = atoi(optarg);
                break;
            case ':':
                DIE_OPT("option requires an argument");
            default:
                DIE_OPT("illegal option");
         }
    }
    argc -= optind;
    argv += optind;
    if(argc > 0) pattern = argv[0];

    /* Die if we are not authorized to do screen recording */
    if(!isAuthorizedForScreenRecording()) DIE("not authorized to do screen recording");

    /* Print matching windows */
    EnumerateWindows(pattern, PrintWindow, (void *)&ctx);

    /* Return success if found any windows, or no windows but also no query */
    return (ctx.numFound > 0 || (pattern == NULL && ctx.id == -1)) ? 0 : 1;

#undef DIE_OPT
}


/* ======================================================================== */
