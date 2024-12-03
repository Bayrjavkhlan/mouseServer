#include <libwebsockets.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                    void *in, size_t len) {
    static Display *display = NULL;
    static Window root;
    static int screen_width, screen_height;

    if (!display) {
        display = XOpenDisplay(NULL);
        root = DefaultRootWindow(display);
        screen_width = DisplayWidth(display, 0);
        screen_height = DisplayHeight(display, 0);
    }

    switch (reason) {
        case LWS_CALLBACK_RECEIVE: {
            double x, y, z;
            if (sscanf((char *)in, "{\"x\":%lf,\"y\":%lf,\"z\":%lf}", &x, &y, &z) == 3) {
                // Print the received values to the terminal
                printf("Received data: x = %.2f, y = %.2f, z = %.2f\n", x, y, z);

                int cur_x, cur_y;
                Window child;
                int root_x, root_y;
                unsigned int mask;

                XQueryPointer(display, root, &root, &child, &root_x, &root_y, &cur_x, &cur_y, &mask);

                int move_x = cur_x + (int)(x * 10); // Scale
                int move_y = cur_y - (int)(y * 10); // Invert Y-axis

                move_x = move_x < 0 ? 0 : move_x > screen_width ? screen_width : move_x;
                move_y = move_y < 0 ? 0 : move_y > screen_height ? screen_height : move_y;

                XWarpPointer(display, None, root, 0, 0, 0, 0, move_x, move_y);
                XFlush(display);
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

int main(void) {
    struct lws_protocols protocols[] = {
        {"mouse-move-protocol", callback, 0, 128},
        {NULL, NULL, 0, 0}  // Termination
    };

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    info.port = 8080;
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Failed to create WebSocket context\n");
        return -1;
    }

    printf("Server running on ws://0.0.0.0:8080\n");
    while (1) {
        lws_service(context, 0);
    }

    lws_context_destroy(context);
    return 0;
}
