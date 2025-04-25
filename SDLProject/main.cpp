/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f
#define MAX_RGB 255

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "MainMenu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Effects.h"

// ––––– CONSTANTS ––––– //
constexpr int WINDOW_WIDTH  = 1280 * 0.8,
          WINDOW_HEIGHT = 960 * 0.8;

constexpr float BG_RED = 213.0f / MAX_RGB,
            BG_BLUE    = 92.0f / MAX_RGB,
            BG_GREEN   = 159.0f / MAX_RGB,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_curr_scene;
MainMenu *g_main_menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;

Effects *g_effects;
Scene   *g_levels[3];

SDL_Window* g_display_window = nullptr;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

bool g_is_colliding_bottom = false;
bool space_pressed = false;

AppStatus g_app_status = RUNNING;

void swtich_to_scene(Scene *scene);
void initialise();
void process_input();
void update();
void render();
void shutdown();

void switch_to_scene(Scene *scene)
{    
    g_curr_scene = scene;
    g_curr_scene->initialise();
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Final Project",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-3.75f, 3.75f, -2.8125f, 2.8125f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    g_main_menu = new MainMenu();

    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    g_levels[0] = g_levelA;
    g_levels[1] = g_levelB;
    g_levels[2] = g_levelC;
    
    switch_to_scene(g_levelB);
    
    g_effects = new Effects(g_projection_matrix, g_view_matrix);
  //  g_effects->start(FADEIN, 1.0f);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_curr_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
            
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q: g_app_status = TERMINATED;     break;
                        
                        
                    case SDLK_SPACE:
                        space_pressed = true; 
                    default:
                        break;
                }
                
            case SDLK_RETURN:
                if (g_curr_scene == g_main_menu){
                    switch_to_scene(g_levelA);
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(nullptr);
    
    if (key_state[SDL_SCANCODE_LEFT]) g_curr_scene->get_state().player->move_left();
    
    else if (key_state[SDL_SCANCODE_RIGHT]) g_curr_scene->get_state().player->move_right();
    
    else if (key_state[SDL_SCANCODE_UP]) g_curr_scene->get_state().player->move_up();
    
    else if (key_state[SDL_SCANCODE_DOWN]) g_curr_scene->get_state().player->move_down();
    
    
    if (glm::length(g_curr_scene->get_state().player->get_movement()) > 1.0f)
        g_curr_scene->get_state().player->normalise_movement();
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_curr_scene->update(FIXED_TIMESTEP);
        if (g_effects != nullptr){
            g_effects->update(FIXED_TIMESTEP);

        }
        
        g_is_colliding_bottom = g_curr_scene->get_state().player->get_collided_bottom();
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    if (g_curr_scene == g_main_menu){
        return;
    }
    
    g_view_matrix = glm::mat4(1.0f);
    
    Map* map = g_curr_scene->get_state().map;
    glm::vec3 player_pos = g_curr_scene->get_state().player->get_position();
    
    float camera_half_width = 3.75f;
    float camera_half_height = 2.8125f;
    
    float minx = map->get_left_bound() + camera_half_width;
    float maxx = map->get_right_bound() - camera_half_width;
    float miny = map->get_bottom_bound() + camera_half_height;
    float maxy = map->get_top_bound() - camera_half_height;
    
    float camx = glm::clamp(player_pos.x, minx, maxx);
    float camy = glm::clamp(player_pos.y, miny, maxy);
    
    g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-camx, -camy, 0.0f));
    
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
       
    glClear(GL_COLOR_BUFFER_BIT);
       
    g_curr_scene->render(&g_shader_program);
    if (g_effects != nullptr){
        g_effects->render();
    }
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete g_main_menu;
    delete g_effects;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        
        if (g_curr_scene != g_main_menu && g_curr_scene->m_game_state.next_scene_id > 0) {
            g_effects->start(FADEOUT, 1.0f); // Fade out exiting level
            switch_to_scene(g_levels[g_curr_scene->m_game_state.next_scene_id - 1]);
            g_curr_scene->m_game_state.next_scene_id = -1;
            g_effects->start(FADEIN, 1.0f); // Fade in entering next level
        }
        else if (g_curr_scene != g_main_menu && g_curr_scene->m_game_state.next_scene_id == 0) {
            g_effects->start(FADEOUT, 1.0f); // Fade out exiting level
            switch_to_scene(g_main_menu);
          
        }
        else if (g_curr_scene == g_main_menu && g_curr_scene->m_game_state.next_scene_id > 0) {
      
            switch_to_scene(g_levels[g_curr_scene->m_game_state.next_scene_id - 1]);
        }

        
        render();
    }
    
    shutdown();
    return 0;
}
