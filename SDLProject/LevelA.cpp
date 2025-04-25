/**
* Author: [Isabelle Larson]
* Assignment: [Puzzle Escape]
* Date due: [April 25, 2025], 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"
#include <vector>

#define LEVEL_WIDTH 15
#define LEVEL_HEIGHT 11

int tile_values[3] = {0, 0, 0};
extern bool space_pressed;
int caught_keys = 0;

unsigned int LEVELA_DATA[] =
{
    81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 233, 81, 81, 81, 81,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 253, 1, 1, 1, 2,
    20, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 22,
    20, 81, 285, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 22,
    20, 81, 305, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 22,
    20, 81, 320, 321, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 22,
    20, 81, 340, 341, 81, 81, 81, 81, 81, 81, 81, 81, 81, 501, 22,
    20, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 500, 510, 22,
    20, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 500, 530, 22,
    20, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 502, 22,
    40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42
};

LevelA::~LevelA()
{
    delete m_game_state.player;
    delete m_game_state.map;
}

void LevelA::initialise()
{
    m_game_state.timer = 60.0f;     
    m_game_state.game_lost = false;
    m_game_state.lose_timer = 0.0f;
    
    m_game_state.next_scene_id = -1;
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id,
                               1.0f, 20, 30);
    
    int player_walking_animation[4][4] =
    {
        { 1, 5, 9, 13 },  // for George to move to the left,
        { 3, 7, 11, 15 }, // for George to move to the right,
        { 2, 6, 10, 14 }, // for George to move upwards,
        { 0, 4, 8, 12 }   // for George to move downwards
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
        
    m_game_state.player->set_position(glm::vec3(5.5f, -4.0f, 0.0f));
    m_game_state.player->set_scale(glm::vec3(0.75f, 0.75f, 0.0f));
    
    GLuint key_texture_id = Utility::load_texture("assets/key.png");
    m_game_state.keys = new Entity[3];
    for (int i = 0; i<3; i++){
        m_game_state.keys[i] = Entity(key_texture_id, 1.0f, 1.0f, 1.0f, KEY, FREEKey, IDLE);
        m_game_state.keys[i].set_scale(glm::vec3(0.5f, 0.5f, 0.0f));
        m_game_state.keys[i].deactivate();
    }
    

    
    // puzzle logic
    puzzle_tile_count = 3;
    puzzleTiles = new Entity[puzzle_tile_count];
    GLuint tile_texture_id = Utility::load_texture("assets/puzzleTiles.png");

    
    for (int i=0; i< puzzle_tile_count; i++){
        puzzleTiles[i] = Entity(tile_texture_id, 0.0f, 1.0f, 1.0f, PLATFORM);
        puzzleTiles[i].set_position(glm::vec3(4.0f + 1.25*i, -5.0f, 0.0f));
        puzzleTiles[i].set_scale(glm::vec3(1.0f, 1.0f, 0.0f));
        puzzleTiles[i].set_width(1.0f);
        puzzleTiles[i].set_height(1.0f);
        puzzleTiles[i].update(0, m_game_state.player, NULL, 0, m_game_state.map);
    }
    
    puzzle_solved = false;  // puzzle starts unsolved
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/clock-ticking-83775.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(60);
    
    m_game_state.doorSfx = Mix_LoadWAV("assets/door_open.mp3");
    Mix_VolumeChunk(m_game_state.doorSfx, 10);

    

    
    
    
    
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0,
                                m_game_state.map);
    
    
    // tile collision logic
    for (int i=0; i< puzzle_tile_count; i++){
        if (m_game_state.player->check_collision(&puzzleTiles[i])){
  
            if (space_pressed){
                tile_values[i] = (tile_values[i] + 1) % 8;
                space_pressed = false;
            }
        }
    }
    
    // puzzle solved
    if (!puzzle_solved &&
        tile_values[0] == 2 &&
        tile_values[1] == 4 &&
        tile_values[2] == 6){
        puzzle_solved = true;
        
        m_game_state.keys[0].set_position(glm::vec3(10.0f, -5.0f, 0.0f));
        m_game_state.keys[1].set_position(glm::vec3(3.0f, -8.0f, 0.0f));
        m_game_state.keys[2].set_position(glm::vec3(5.0f, -6.0f, 0.0f));
        
        for (int i=0; i<3; i++){
            m_game_state.keys[i].activate();
            m_game_state.keys[i].ai_activate(m_game_state.player, delta_time);
            m_game_state.keys[i].update(0, &m_game_state.keys[i], NULL, 0, m_game_state.map);

        }
        printf("Puzzle solved! Keys released!\n");

        
         
    }
    
    // key movement
    if (puzzle_solved){
        for (int i = 0; i < 3; i++){
            
            // collision
            if (m_game_state.player->check_collision(&m_game_state.keys[i])){
                //m_game_state.keys[i].deactivate();
                m_game_state.keys[i].set_ai_state(WALKING);
                m_game_state.keys[i].set_position(glm::vec3(7.25f + 0.75*i, -1.0f, 0.0f));
                caught_keys ++;
            }
            m_game_state.keys[i].update(delta_time, &m_game_state.keys[i], NULL, 0, m_game_state.map);
        }
    }
    
    // all keys caught
    if (caught_keys == 3){
        Mix_PlayChannel(-1, m_game_state.doorSfx, 0);  // all keys are caught, the door opens

        m_game_state.keys[0].deactivate();
        m_game_state.keys[1].deactivate();
        m_game_state.keys[2].deactivate();

        glm::vec3 player_pos = m_game_state.player->get_position();
        
        glm::vec3 door_pos = glm::vec3(10.5f, -1.0f, 0.0f);
        float door_w = 1.0f;
        float door_h = 1.0f;
        
        bool touching_door =
                fabs(player_pos.x - door_pos.x) < (door_w / 2.0f + 0.7f / 2.0f) &&
                fabs(player_pos.y - door_pos.y) < (door_h / 2.0f + 0.7f / 2.0f);

        if (touching_door){
            printf("Door entered, onto level B!");
            m_game_state.next_scene_id = 2;
        }
    }
    
    if (!m_game_state.game_lost && !m_game_state.player->m_wins) {
        m_game_state.timer -= delta_time;

        if (m_game_state.timer <= 0.0f) {
            m_game_state.game_lost = true;
            m_game_state.timer = 0.0f;
            m_game_state.lose_timer = 0.0f;

        }
    }
    if (m_game_state.game_lost) {
        m_game_state.player->set_movement(glm::vec3(0.0f));
        m_game_state.player->set_speed(0.0f);

        m_game_state.lose_timer += delta_time; // start counting seconds

        if (m_game_state.lose_timer >= 5.0f) {
            m_game_state.next_scene_id = 0; // Go back to main menu (scene 0)
        }
        
        return; // Stop updating game normally
    }
    
    
    
}

void LevelA::render(ShaderProgram *program)
{
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for(int i=0; i<3; i++){
        if (m_game_state.keys[i].get_is_active()){
            m_game_state.keys[i].render(program);

        }
    }
    if (puzzle_solved){
        for(int i=0; i < 3; i++){
            m_game_state.keys[i].render(program);
        }
    }
   
    if (!puzzle_solved){
        for(int i=0; i< 3; i++){
            puzzleTiles[i].render(program);
        }
        for(int i=0; i<3; i++){
            glm::vec3 pos = puzzleTiles[i].get_position();
            std::string num = std::to_string(tile_values[i]);
            Utility::draw_text(program, Utility::load_texture("assets/font1.png"), num, 0.5f, 0.1f, pos);
        }
    }
    
    std::string time_text = "Time: " + std::to_string(static_cast<int>(m_game_state.timer));
    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                       time_text, 0.4f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));

    if (m_game_state.game_lost) {
        Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
                           "You Lose!", 0.7f, 0.01f, glm::vec3(4.0f, -5.0f, 0.0f));
    }

    
    

}
