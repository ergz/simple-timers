/* nuklear - 1.32.0 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
 #include "nuklear_glfw_gl3.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define MAX_TIMERS

static void error_callback(int e, const char *d)
{
    printf("Error: %d: %s\n", e, d);
}

typedef struct Timer {
    int id;
    char status; // is the timer going?
    int seconds; 
    int minutes;
    int hours;
} Timer;

typedef struct TimerGrid {
    int active_timers;
    Timer *timers[MAX_TIMERS];
} TimerGrid;

Timer *timer_create(int id, char status, int seconds, int minutes, int hours) 
{
    Timer *timer = malloc(sizeof(Timer));
    assert(timer != NULL);
    
    timer->id = id; 
    timer->status = status; // a: off b: init c: running
    timer->seconds = seconds;
    timer->minutes = minutes;
    timer->hours = hours;

    return timer;
}

// initialize the timer grid 
TimerGrid *init_timer_grid() 
{
    TimerGrid *grid = malloc(sizeof(TimerGrid)); // allocate all mem at once?

    grid->active_timers = 0;

    return grid;
}

// add a timer to the timer grid
void add_timer_to_grid(TimerGrid *grid, Timer *timer)
{
    // if (grid->active_timers > MAX_TIMERS) {
    //     fprintf(stdout, "ERROR: too many timers!\n");
    //     exit(1);
    // }

    grid->timers[grid->active_timers] = timer;
    grid->active_timers += 1; 
}

// NOTE make this happen
void remove_timer_from_grid(TimerGrid *grid, Timer *timer)
{
}

    

int main(int argc, char *argv[]) 
{
    struct nk_glfw glfw = {0};
    static GLFWwindow *win;
    int widht = 0, height = 0;
    struct nk_colorf bg;



    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stdout, "[GLFW] failed to init\n");
        exit(1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tool", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &widht, &height);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }


    struct nk_context *ctx = nk_glfw3_init(&glfw, win, NK_GLFW3_INSTALL_CALLBACKS);

    // create a font atlas struct and 
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "fonts/Roboto-Regular.ttf", 22, 0);
    nk_glfw3_font_stash_end(&glfw);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &roboto->handle);
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;


    TimerGrid *timer_grid;
    timer_grid = init_timer_grid();

    char *timer_status_text;
    // Timer *timer = timer_create('a', 0, 0, 0);

    // the is the main loop for the 
    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        struct nk_rect calc_rect = nk_rect(0, 0, widht, height);

        // NK_API nk_bool nk_begin(struct nk_context *ctx, const char *title, struct nk_rect bounds, nk_flags flags);
        if (nk_begin(ctx, "Study Timer", calc_rect, NK_WINDOW_TITLE)) {

            nk_layout_row_static(ctx, 0, 150, 3);

            if (nk_button_label(ctx, "Create New Timer")) {
                // TODO handle this case
                // if (timer_grid->active_timers >= MAX_TIMERS) {
                //     fprintf(stdout, "Error cannot add more timers\n");
                // }

                timer_grid->timers[timer_grid->active_timers] = timer_create(timer_grid->active_timers, 'a', 0, 0, 0);
                timer_grid->active_timers += 1;
            }
            // if (nk_button_label(ctx, "Add hour")) {
            //     // fprintf(stdout, "button has been pressed\n");
            //     switch (timer->status) {
            //         case 'b': {
            //             timer->hours += 1;
            //             fprintf(stdout, "hours in the timer: %d\n", timer->hours);
            //         } break;
            //         case 'a': {
            //             fprintf(stdout, "there is no timer started\n");
            //         } break;
            //         default: {
            //             exit(1);
            //         }
            //     }
            // }
            // if (nk_button_label(ctx, "Add minute")) {
            //     switch (timer->status) {
            //         case 'b': {
            //             timer->minutes += 1;
            //             fprintf(stdout, "minutes in the timer: %d\n", timer->minutes);
            //         } break;
            //         case 'a': {
            //             fprintf(stdout, "there is no timer started\n");
            //         } break;
            //         default: {
            //             exit(1);
            //         }
            //     }
            // }
        
            // int len; char buffer[256];
            // NK_API nk_flags nk_edit_string(struct nk_context*, nk_flags, char *buffer, int *len, int max, nk_plugin_filter);
            // nk_edit_string(ctx, NK_EDIT_READ_ONLY, buffer, &len, 255, NULL);

            // char screen_display[255];
            // if (timer->status == 'a') {
            //     sprintf(screen_display, "No timer started");
            // } else {

            //     sprintf(screen_display, "Hours: %d;  Minutes: %d;   Seconds: %d", 
            //         timer->hours, timer->minutes, timer->seconds);
            // }

            // nk_layout_row_static(ctx, 0, 500, 1);
            // nk_label_colored(ctx, screen_display, NK_TEXT_CENTERED, nk_rgb(255,255,0));

        }
        nk_end(ctx);

        // always keep track of the width and height
        glfwGetWindowSize(win, &widht, &height);
        glViewport(0, 0, widht, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);
    }

    nk_glfw3_shutdown(&glfw);
    glfwTerminate();

    return 0;
}
