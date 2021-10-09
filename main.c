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
#include <stdbool.h>

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

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define MAX_TIMERS 10

static void error_callback(int e, const char *d)
{
    printf("Error: %d: %s\n", e, d);
}

typedef struct Timer {
    bool is_running; // is the timer going?
    int seconds; 
    int minutes;
    int hours;
    int toggled;
    time_t time_base;
    time_t time_x; 
    time_t time_y;
    double time_delta;
    int secs_to_end;
} Timer;

typedef struct TimerGrid {
    int active_timers;
    Timer *timers[MAX_TIMERS];
} TimerGrid;

Timer *timer_create(int seconds, int minutes, int hours) 
{
    Timer *timer = malloc(sizeof(Timer));
    assert(timer != NULL);
    
    timer->toggled = 0; 
    timer->is_running = false; // a: off b: init c: running
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
    grid->timers[grid->active_timers] = timer;
    grid->active_timers += 1; 
}


// TODO(emanuel) 
void timer_destroy(Timer *timer) 
{
    assert(timer != NULL);

    free(timer);
}

int hms_to_seconds(int hrs, int mins, int secs) 
{
    return (hrs * 60 * 60) + (mins * 60) + secs;
}

/* this function should take the time remaining in seconds
    and update the Timer structs hours, mins, seconds.
    The purpose for this is to update the display on the 
    app.
*/
void seconds_to_hms(int seconds)
{

}

// struct nk_style_button {
//                 /* background */
//                 struct nk_style_item normal;
//                 struct nk_style_item hover;
//                 struct nk_style_item active;
//                 struct nk_color border_color;

//                 /* text */
//                 struct nk_color text_background;
//                 struct nk_color text_normal;
//                 struct nk_color text_hover;
//                 struct nk_color text_active;
//                 nk_flags text_alignment;

//                 /* properties */
//                 float border;
//                 float rounding;
//                 struct nk_vec2 padding;
//                 struct nk_vec2 image_padding;
//                 struct nk_vec2 touch_padding;

//                 /* optional user callbacks */
//                 nk_handle userdata;
//                 void(*draw_begin)(struct nk_command_buffer*, nk_handle userdata);
//                 void(*draw_end)(struct nk_command_buffer*, nk_handle userdata);
//             };



struct nk_style_button create_button_style(
    struct nk_context* ctx, 
    struct nk_color background_color, 
    struct nk_color text_color, 
    struct nk_color border_color)
{
    struct nk_style_button button_style;
    button_style = ctx->style.button; // get defaults from current context
    button_style.normal = nk_style_item_color(background_color);
    button_style.hover = nk_style_item_color(background_color);
    button_style.active = nk_style_item_color(background_color);
    button_style.border_color = background_color;

    button_style.text_background = text_color;
    button_style.text_normal = text_color;
    button_style.text_hover = text_color;
    button_style.text_active = text_color;
    button_style.text_alignment = NK_TEXT_CENTERED;

    button_style.border = 1;

    return button_style;
}

void button_style_update_background(struct nk_style_button *bttn, struct nk_color color)
{
    bttn->normal = nk_style_item_color(color);
    bttn->hover = nk_style_item_color(color);
    bttn->active = nk_style_item_color(color);
}

void button_style_update_text_color(struct nk_style_button *bttn, struct nk_color color)
{
    bttn->text_background = color;
    bttn->text_normal = color;
    bttn->text_hover = color;
    bttn->text_active = color;
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
    struct nk_style_button bttn_style;
    bttn_style = create_button_style(ctx, nk_rgb(255, 0, 0), nk_rgb(0, 0, 0), nk_rgb(0, 0, 255));

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

        if (nk_begin(ctx, "Study Timer", calc_rect, NK_WINDOW_TITLE)) {

            for (int i = 0; i < timer_grid->active_timers; i++) {

                char *start_stop_message;
                // clock time for the active timers every iteration
                // TODO(ergz): this branch of the if statements seems very very very bad
                // please fix this!!!! 
                if (timer_grid->timers[i]->is_running) {
                    if (timer_grid->timers[i]->toggled == 1) { // first time run? if so then start the base timer
                        timer_grid->timers[i]->toggled += 1;
                        timer_grid->timers[i]->time_delta = difftime(time(NULL), timer_grid->timers[i]->time_base);
                        if (timer_grid->timers[i]->time_delta >= timer_grid->timers[i]->secs_to_end) {
                            timer_grid->timers[i]->is_running = false;
                            timer_grid->timers[i]->time_delta = 0;
                        }
                        
                    } else {
                        timer_grid->timers[i]->time_delta = difftime(time(NULL), timer_grid->timers[i]->time_base);

                        if (timer_grid->timers[i]->time_delta >= timer_grid->timers[i]->secs_to_end) {
                            fprintf(stdout, "DONE!!!\n");
                            timer_grid->timers[i]->is_running = false;
                            timer_grid->timers[i]->time_delta = 0;

                        } else {
                            fprintf(stdout, "there is %f seconds left\n", timer_grid->timers[i]->time_delta - timer_grid->timers[i]->secs_to_end);
                        }
                    }
                }

                nk_layout_row_static(ctx, 0, 300, 5);
                Timer *this_timer = timer_grid->timers[i];


                if (this_timer->is_running) {
                    start_stop_message = "Stop";
                    button_style_update_background(&bttn_style, nk_rgb(255, 0, 0));
                } else {
                    start_stop_message = "Start";
                    button_style_update_background(&bttn_style, nk_rgb(0, 255, 0));
                }

                // Start Stop Button ----------------------------
                if (nk_button_label_styled(ctx, &bttn_style, start_stop_message)) {
                    if (this_timer->is_running) { // turn off
                        timer_grid->timers[i]->is_running = false;
                        timer_grid->timers[i]->toggled += 1; // 
                        // timer_grid->timers[i]->time_x = time(NULL);
                    } else { // turn on
                        timer_grid->timers[i]->is_running = true;
                        timer_grid->timers[i]->toggled += 1; // 
                        timer_grid->timers[i]->time_base = time(NULL);
                    } 
                }

                if (nk_button_label(ctx, "Add hour")) {
                    timer_grid->timers[i]->hours += 1;
                    timer_grid->timers[i]->secs_to_end = hms_to_seconds(
                        timer_grid->timers[i]->hours, 
                        timer_grid->timers[i]->minutes, 
                        timer_grid->timers[i]->seconds
                        );
                }

                if (nk_button_label(ctx, "Add minute")) {
                    timer_grid->timers[i]->minutes += 1;
                    timer_grid->timers[i]->secs_to_end = hms_to_seconds(
                        timer_grid->timers[i]->hours, 
                        timer_grid->timers[i]->minutes, 
                        timer_grid->timers[i]->seconds
                        );
                }

                if (nk_button_label(ctx, "Add second")) {
                    timer_grid->timers[i]->seconds += 1;
                    timer_grid->timers[i]->secs_to_end = hms_to_seconds(
                        timer_grid->timers[i]->hours, 
                        timer_grid->timers[i]->minutes, 
                        timer_grid->timers[i]->seconds
                        );
                }

                // NOTE(ergz) for I want to start working on changing the color 
                // of the button based on the timer status
                // NK_API nk_bool nk_button_label_styled(struct nk_context*, const struct nk_style_button*, const char *title);

                char display_text[500];
                sprintf(&display_text, "  Hours: %d,  Minutes: %d,  Seconds: %d", 
                    this_timer->hours, this_timer->minutes, this_timer->seconds);

                nk_label_colored(ctx, display_text, NK_TEXT_CENTERED, nk_rgb(255,255,0));
            }

            nk_layout_row_static(ctx, 0, 100, 2);

            if (nk_button_label(ctx, "+")) {
                // TODO handle this case
                if (timer_grid->active_timers >= MAX_TIMERS) {
                    fprintf(stdout, "Error cannot add more timers\n");
                } else {
                    timer_grid->timers[timer_grid->active_timers] = timer_create(0, 0, 0);
                    fprintf(stdout, "+ BUTTON CLICKED .... toggle state: %d\n", timer_grid->timers[timer_grid->active_timers]->toggled);
                    timer_grid->active_timers += 1;
                }
            }

            if (nk_button_label(ctx, "-")) {
                // TODO handle this case
                if (timer_grid->active_timers == 0) {
                    fprintf(stdout, "no timers to remove\n");
                } else {
                    timer_grid->active_timers -= 1;
                    timer_destroy(timer_grid->timers[timer_grid->active_timers]);
                }
            }

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
