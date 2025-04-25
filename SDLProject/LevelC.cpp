/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 20
#define PLATFORM_COUNT 11

unsigned int LEVELC_DATA[] =
{
    81, 81, 81, 81, 81, 81, 81, 81, 233, 81, 81, 81, 81, 81, 81, 81,
    0, 1, 1, 1, 1, 1, 1, 1, 253, 1, 1, 1, 1, 1, 1, 2,
    20, 120, 120, 120, 120, 120, 120, 120, 163, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 163, 120, 120, 120, 163, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 163, 120, 120, 120, 120, 120, 163, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 163, 120, 120, 120, 163, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 163, 163, 163, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 22,
    20, 120, 120, 120, 120, 120, 163, 163, 163, 163, 163, 120, 120, 120, 120, 22,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42
};

LevelC::~LevelC(){
    delete m_game_state.player;
    delete m_game_state.map;
    delete[] m_game_state.platforms;
    
    // add music stuff
}

void LevelC::initialise(){
    
    m_game_state.next_scene_id = -1;
    m_game_state.timer = 60.0f;
    m_game_state.game_lost = false;
    m_game_state.lose_timer = 0.0f;
    
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id,
                               1.0f, 20, 30);
    
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for me to move to the left,
        { 3, 7, 11, 15 }, // for me to move to the right,
        { 2, 6, 10, 14 }, // for me to move upwards,
        { 0, 4, 8, 12 }   // for me to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    GLuint texture_ids = Utility::load_texture("assets/isaSprite2.jpg");

    m_game_state.player = new Entity(
        texture_ids,               // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        0.7f,                      // width
        0.7f,                      // height
        PLAYER                    // entity type
    );
        
    m_game_state.player->set_position(glm::vec3(8.0f, -2.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(0.75f, 0.75f, 0.0f));
    
    // platforms!
    
    m_game_state.platforms = new Entity[PLATFORM_COUNT];
    glm::vec3 platform_positions[PLATFORM_COUNT] = {
        glm::vec3(11.0f, -3.0f, 0.0f),
        glm::vec3(4.0f, -4.0f, 0.0f),
        glm::vec3(8.0f, -6.0f, 0.0f),
        glm::vec3(6.0f, -7.0f, 0.0f),
        glm::vec3(7.0f, -9.0f, 0.0f),
        glm::vec3(12.0f, -10.0f, 0.0f),
        glm::vec3(2.0f, -12.0f, 0.0f),
        glm::vec3(9.0f, -13.0f, 0.0f),
        glm::vec3(6.0f, -15.0f, 0.0f),
        glm::vec3(1.0f, -16.0f, 0.0f),
        glm::vec3(10.0f, -17.0f, 0.0f)
    };
    
    GLuint platform_texture_id = Utility::load_texture("assets/movingPlatform.png");
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
       // Entity &platform = m_game_state.platforms[i];
        m_game_state.platforms[i] = Entity(platform_texture_id, 1.5f, 1.0f, 1.0f, PLATFORM, MovingPLATFORM, IDLE);
        m_game_state.platforms[i].set_position(platform_positions[i]);
        m_game_state.platforms[i].set_scale(glm::vec3(1.0f));

        m_game_state.platforms[i].left_bound = 2.0f;
        m_game_state.platforms[i].right_bound = 14.0f;
        m_game_state.platforms[i].platform_direction = 1; // ✅ FIXED
        m_game_state.platforms[i].is_moving = true;
        m_game_state.platforms[i].m_is_active = true;
        m_game_state.platforms[i].set_speed(2.0f);
        
        m_game_state.platforms[i].update(0.0f, &m_game_state.platforms[i], NULL, 0, m_game_state.map);
        
    }
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/clock-ticking-83775.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(60);
    
    m_game_state.tileSfx = Mix_LoadWAV("assets/Menu Soundpack 3.wav");
    Mix_VolumeChunk(m_game_state.tileSfx, 40);
    
    
    
    
}

void LevelC::update(float delta_time){

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

        // Update each platform
        for (int i = 0; i < PLATFORM_COUNT; i++) {
            Entity& platform = m_game_state.platforms[i];

            // 1. Move platform (if still moving)
            platform.ai_activate(m_game_state.player, delta_time);
            platform.update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

            // 2. Check if the player is near enough to interact
            if (glm::distance(m_game_state.player->get_position(), platform.get_position()) < 1.1f) {
                if (keys[SDL_SCANCODE_S] && !platform.is_hit) {
                    platform.is_moving = !platform.is_moving;
                    platform.is_hit = true;
                    Mix_PlayChannel(-1, m_game_state.tileSfx, 0);


                    // 3. If freezing, update the map tile to be solid
                    if (!platform.is_moving) {
                        int grid_x = static_cast<int>(round(platform.get_position().x));
                        int grid_y = static_cast<int>(round(-platform.get_position().y)); // flip Y because OpenGL

                        // Set the tile to a solid ground tile (example: 1 = ground in your tileset)
                        m_game_state.map->set_tile(grid_x, grid_y, 293);
                    }
                }
            } else {
                platform.is_hit = false; // Reset debounce if player moves away
            }
        }

        // 4. Update the player normally
        m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

    float player_x = m_game_state.player->get_position().x;
    float player_y = m_game_state.player->get_position().y;

    // Win if player is near the 4 bottom tiles
    if (player_y <= -18.0f && player_x >= 5.5f && player_x <= 10.5f) {
        m_game_state.player->m_wins = true;
    }
    
    if (m_game_state.player->m_wins) {
        m_game_state.player->set_movement(glm::vec3(0.0f)); // Stop input movement
        m_game_state.player->set_speed(0.0f);               // Stop actual velocity
    }
    
    if (!m_game_state.game_lost && !m_game_state.player->m_wins) {
        m_game_state.timer -= delta_time;

        if (m_game_state.timer <= 0.0f) {
            m_game_state.game_lost = true;
            m_game_state.timer = 0.0f;
            m_game_state.lose_timer = 0.0f;

        }
    }
    


    
}

void LevelC::render(ShaderProgram *program){
    m_game_state.map->render(program);

    // 🔥 Draw all frozen platforms first
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        if (!m_game_state.platforms[i].is_moving) {
            m_game_state.platforms[i].render(program); // Frozen = background
        }
    }

    // 🧍 Draw player on top
    m_game_state.player->render(program);

    // 🌀 Draw still-moving platforms on top of player
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        if (m_game_state.platforms[i].is_moving) {
            m_game_state.platforms[i].render(program);
        }
    }
    
    if (m_game_state.player->m_wins) {
        Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                           "You Win!", 0.5f, 0.01f, glm::vec3(m_game_state.player->get_position().x - 1.5f,
                                                              m_game_state.player->get_position().y + 1.0f, 0));
    }
    
    std::string time_text = "Time: " + std::to_string(static_cast<int>(m_game_state.timer));
    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                       time_text, 0.4f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));

    if (m_game_state.game_lost) {
        Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                           "You Lose!", 0.7f, 0.01f, glm::vec3(4.0f, -5.0f, 0.0f));
    }


    
}



