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
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

void Entity::ai_activate(Entity *player, float delta_time)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_ambush(player);
            break;
            
            
        case FREEKey:
            ai_key(delta_time);
            break;
        
        case AttackingKNIGHT:
            ai_knight_1();
            break;
            
        case RunningKNIGHT:
            ai_knight_2(delta_time);
            break;
            
        case MovingPLATFORM:
            ai_platform(delta_time);
            break;
        
            
        default:
            break;
    }
}

void Entity::ai_platform(float delta_time){
    
    if (!is_moving){
        m_velocity = glm::vec3(0.0f);
        m_velocity = glm::vec3(0.0f);
        return;
    }
    
    m_position.x += platform_direction * m_speed * delta_time;
    m_movement = glm::vec3(platform_direction*1.0f, 0.0f, 0.0f);

    
    if (m_position.x <= left_bound){
        platform_direction = 1;
    }
    
    else if (m_position.x >= right_bound){
        platform_direction = -1;
    }
        
        
}

void Entity::ai_ambush(Entity *player)
{
    switch (m_ai_state){
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 2.0f){
                m_ai_state = ATTACKING;
            }
            break;
            
        case ATTACKING:
            if (player->get_position().x < m_position.x) {
                m_movement = glm::vec3(-2.0f, 0.0f, 0.0f);
            } else {
            m_movement = glm::vec3(4.0f, 0.0f, 0.0f);  // Move fast towards the player
            }
            
            m_ai_state = IDLE;
            
            break;
            
        case WALKING:
            break;
        default:
            break;
    }
}

void Entity::ai_key(float delta_time)
{

    switch (m_ai_state) {
        case IDLE:
            if (m_velocity == glm::vec3(0.0f)){
                float vx = ((rand() % 2000) / 100.0f) - 10.0f;
                float vy = ((rand() % 2000) / 100.0f) - 10.0f;
                m_velocity = glm::vec3(vx, vy, 0.0f);
            }
            
            if (m_position.x < 1.0f || m_position.x > 14.0f) m_velocity.x = -m_velocity.x;
            if (m_position.y < -9.0f || m_position.y > -1.5f) m_velocity.y = -m_velocity.y;
            
            m_position += m_velocity * delta_time;
            break;
            
        case WALKING:
            // key caught 
            break;
            
        case ATTACKING:
            break;
            
        default:
            break;
    }
}
// attacking
void Entity::ai_knight_1(){
    switch (m_ai_state){
        case IDLE:
            break;
        case ATTACKING:
            break;
            
            
        case WALKING:
            break;
        default:
            break;
    }

    
}
// running
void Entity::ai_knight_2(float delta_time){
    
    if (!m_is_active) return;

    m_movement = glm::vec3(platform_direction * 1.0f, 0.0f, 0.0f); // Move left/right

    // Reverse direction when reaching wall bounds (fake it using position limits)
    if (m_position.x < left_bound) {
        platform_direction = 1;
    } else if (m_position.x > right_bound) {
        platform_direction = -1;
    }
    
}

void Entity::lose_life(){
    m_lives --;
    if (m_lives <= 0){
        m_is_active = false;
    }
    else {
        m_position = glm::vec3(1.0f, 0.0f, 0.0f);   // could change this
    }
}
// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed),m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    face_right();
    set_walking(walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed,  float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState): m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
// Initialize m_walking with zeros or any default value
for (int i = 0; i < SECONDS_PER_FRAME; ++i)
    for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}


Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity **collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float player_bottom = m_position.y - (m_height / 2.0f);
              float platform_top = collidable_entity->m_position.y + (collidable_entity->m_height / 2.0f);

              if (m_velocity.y < 0 && player_bottom > platform_top - 0.05f) // 👈 Only if falling onto it
              {
                  float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
                  float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));

                  m_position.y += y_overlap;
                  m_velocity.y = 0;
                  m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity **collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x     -= x_overlap;
                m_velocity.x      = 0;

                // Collision!
                m_collided_right  = true;
                
            } else if (m_velocity.x < 0)
            {
                m_position.x    += x_overlap;
                m_velocity.x     = 0;
 
                // Collision!
                m_collided_left  = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }
}

void const Entity::check_collision_x(Map *map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

bool Entity::check_collision_simple(Entity* other) const {
    if (!m_is_active || !other->m_is_active) return false;

    float x_dist = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_dist = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return (x_dist < 0.0f && y_dist < 0.0f);
}

void Entity::update(float delta_time, Entity *player, Entity **collidable_entities, int collidable_entity_count, Map *map)
{
    if (!m_is_active) return;
    
    if (m_entity_type == PLATFORM && !is_moving) {
        m_velocity = glm::vec3(0.0f);
        m_movement = glm::vec3(0.0f);
    }
 
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
   // if (m_entity_type == ENEMY) ai_activate(player, delta_time);
    if (m_entity_type == KEY){ ai_key(delta_time);}
    
    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float) 1 / SECONDS_PER_FRAME;
            
            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;
                
                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }
    
    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y = m_movement.y * m_speed;
    m_velocity += m_acceleration * delta_time;
    
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }
    
    m_position.y += m_velocity.y * delta_time;
 //   check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
  //  check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    // Manually handle frozen platform collisions for top-down
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collider = collidable_entities[i];
        if (check_collision_simple(collider)) {
            // Push the player back slightly to prevent overlap
            glm::vec3 diff = m_position - collider->get_position();

            if (fabs(diff.x) > fabs(diff.y)) {
                m_position.x += (diff.x > 0 ? 0.1f : -0.1f);
            } else {
                m_position.y += (diff.y > 0 ? 0.1f : -0.1f);
            }

            m_velocity = glm::vec3(0.0f);
            m_movement = glm::vec3(0.0f);
        }
    }


    
    if (m_entity_type == KEY && player!= nullptr && player->m_is_active){
        if (check_collision(player)){
            // you win
            player->m_wins = true;
        }
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}



void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
