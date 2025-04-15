// 3D_World_Traversal.cpp
/////////////////////////////////////////////////////////////////////////////////////
//
// This code is used to teach the course "Advanced Graphics" in Centennial College
// Developed by Alireza Moghaddam on Feb. 2025
// Modified for Final Project by adding Enemy Spawning and Movement
//
/////////////////////////////////////////////////////////////////////////////////////

#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "..\SOIL\src\SOIL.h"
#include <vector>
#include <iostream>
#include <algorithm>

void renderBitmapString(float x, float y, void* font, const char* string);

using namespace std;

// === Game Object ===
enum GameObject_Type {
    PLAYER,
    ENEMY,
    BULLET,
    OBSTACLE,
    FIREBALL,
};

struct GameObject {
    glm::vec3 location;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 moving_direction;
    GLfloat velocity;
    GLfloat collider_dimension;
    int living_time;
    int life_span;
    int type;
    bool isAlive;
    bool isCollided;
    GLuint textureID;
    int lastShotTime; // For enemy shooting cooldown
    int owner; // 0 = player, 1 = enemy


};

// === Globals ===
std::vector<GameObject> sceneGraph;
std::vector<GameObject> enemyList;
GLuint enemyTextureID;
float spawnTimer = 0.0f;
float spawnInterval = 3000.0f;
const int enemyShootCooldown = 2000; // milliseconds between shots
int playerHealth = 100;
bool gameWon = false;
bool gameOver = false;
int playerScore = 0;


const int Num_Obstacles = 20;
float obstacle_data[Num_Obstacles][3];

GLuint location;
GLuint cam_mat_location;
GLuint proj_mat_location;
GLuint texture[10];
GLuint Buffers[10];

glm::mat4 model_view;
glm::vec3 unit_z_vector = glm::vec3(0, 0, 1);
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 0.8f);
glm::vec3 forward_vector = glm::vec3(1, 1, 0);
glm::vec3 looking_dir_vector = glm::vec3(1, 1, 0);
glm::vec3 up_vector = unit_z_vector;
glm::vec3 side_vector = glm::cross(up_vector, forward_vector);

int x0 = 0, y_0 = 0;
int oldTimeSinceStart = 0;
int deltaTime;
float travel_speed = 300.0f;
float mouse_sensitivity = 0.01f;

const GLuint NumVertices = 28;



float randomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    return a + random * (b - a);
}

void drawCube(glm::vec3 scale, GLuint tex) {
    model_view = glm::scale(model_view, scale);
    glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_QUADS, 4, 24);
}

void drawPyramid(glm::vec3 scale, GLuint tex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    //glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Full brightness
    glColor4ub(255, 255, 255, 255);  // Max out white/alpha


    glPushMatrix();
    glScalef(scale.x, scale.y, scale.z);

    glBegin(GL_TRIANGLES);
    // Front face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);    // Top
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, 0.5f);   // Left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.5f);    // Right

    // Right face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, -0.5f);

    // Back face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.0f, -0.5f);

    // Left face
    glTexCoord2f(0.5f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, 0.0f, 0.5f);
    glEnd();

    // Bottom face (optional — texture won't be visible often)
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.0f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.0f, -0.5f);
    glEnd();

    glPopMatrix();
}


void spawnEnemy(std::vector<GameObject>& enemies, GLuint enemyTexture) {

    float x = (rand() % 40 - 20);
    float z = (rand() % 40);

    float distanceToPlayer = glm::length(glm::vec2(x - cam_pos.x, z - cam_pos.z));
    if (distanceToPlayer < 5.0f) return;

    GameObject enemy;
    enemy.scale = glm::vec3(1.0f);
    enemy.location = glm::vec3(x, 1.0f, z);
	enemy.rotation = glm::vec3(-135.0f);
    enemy.type = ENEMY;
    enemy.velocity = 0.001f + (rand() % 5) * 0.001f;
    enemy.isAlive = true;
    enemy.collider_dimension = 0.9f;
    enemy.isCollided = false;
    enemy.living_time = 0;
    enemy.life_span = -1;
    enemy.textureID = texture[4];
    enemy.moving_direction = glm::vec3(0.0f);
    enemy.lastShotTime = glutGet(GLUT_ELAPSED_TIME);
    enemies.push_back(enemy);
}


void checkCollisions() {
    // First: Check sceneGraph vs sceneGraph (e.g., bullets hitting obstacles, etc.)
    for (int i = 0; i < sceneGraph.size(); i++) {
        for (int j = 0; j < sceneGraph.size(); j++) {
            if (i != j && sceneGraph[i].isAlive && sceneGraph[j].isAlive &&
                !(sceneGraph[i].type == OBSTACLE && sceneGraph[j].type == OBSTACLE)) {
                GameObject& one = sceneGraph[i];
                GameObject& two = sceneGraph[j];

                bool collided = glm::abs(one.location.x - two.location.x) <= (one.collider_dimension / 2 + two.collider_dimension / 2) &&
                    glm::abs(one.location.y - two.location.y) <= (one.collider_dimension / 2 + two.collider_dimension / 2) &&
                    glm::abs(one.location.z - two.location.z) <= (one.collider_dimension / 2 + two.collider_dimension / 2);

                if (collided) {
                    one.isCollided = true;
                    two.isCollided = true;
                }
            }
        }
    }
    
    for (int i = 0; i < sceneGraph.size(); i++) {
        GameObject& bullet = sceneGraph[i];
        if (!bullet.isAlive || bullet.type != BULLET || bullet.owner == 1) continue;

        for (int j = 0; j < enemyList.size(); j++) {
            GameObject& enemy = enemyList[j];
            if (!enemy.isAlive) continue;

            bool collided = glm::abs(bullet.location.x - enemy.location.x) <= (bullet.collider_dimension / 2 + enemy.collider_dimension / 2) &&
                glm::abs(bullet.location.y - enemy.location.y) <= (bullet.collider_dimension / 2 + enemy.collider_dimension / 2) &&
                glm::abs(bullet.location.z - enemy.location.z) <= (bullet.collider_dimension / 2 + enemy.collider_dimension / 2);

            if (collided) {
                bullet.isAlive = false;
                enemy.isAlive = false;
                bullet.isCollided = true;
                enemy.isCollided = true;
                playerScore += 20;
                std::cout << "Bullet hit enemy!" << std::endl;
            }
        }

       
    }
    // === Check if enemy bullets hit the player ===
    for (GameObject& fireball : sceneGraph) {
        if (!fireball.isAlive || fireball.type != FIREBALL || fireball.owner != 1) continue;

        float distToPlayer = glm::length(fireball.location - cam_pos);
        if (distToPlayer < fireball.collider_dimension / 2.0f) {
            fireball.isAlive = false;

            if (!gameOver && !gameWon) {
                playerHealth -= 10;
                playerHealth = std::max(0, playerHealth);  // clamp to 0
                std::cout << "Hit by enemy fireball! Health: " << playerHealth << std::endl;

                if (playerHealth <= 0) {
                    gameOver = true;
                    std::cout << "Game Over! You lost!" << std::endl;
                }
            }
        }

    }
}


void updateSceneGraph() {
    checkCollisions();

    for (int i = 0; i < sceneGraph.size(); i++) {
        GameObject& go = sceneGraph[i];
        if (go.life_span > 0 && go.isAlive && go.living_time >= go.life_span)
            go.isAlive = false;
        if (go.life_span > 0 && go.isAlive && go.living_time < go.life_span) {
            go.location += ((GLfloat)deltaTime) * go.velocity * glm::normalize(go.moving_direction);
            go.living_time += deltaTime;
        }
    }

    for (int i = 0; i < enemyList.size(); i++) {
        GameObject& enemy = enemyList[i];
        if (!enemy.isAlive) continue;

        // Move toward player
        enemy.moving_direction = glm::normalize(cam_pos - enemy.location);
        enemy.location += enemy.moving_direction * enemy.velocity * (GLfloat)deltaTime;

        // === ENEMY SHOOTING ===
        int now = glutGet(GLUT_ELAPSED_TIME);
        if (now - enemy.lastShotTime > enemyShootCooldown) {
            GameObject fireball;
            fireball.owner = 1;  // Enemy
            fireball.location = enemy.location;
            fireball.rotation = glm::vec3(0);
            fireball.scale = glm::vec3(0.03f);
            fireball.collider_dimension = fireball.scale.x;
            fireball.isAlive = true;
            fireball.living_time = 0;
            fireball.isCollided = false;
            fireball.velocity = 0.02f;
            fireball.type = FIREBALL;
            fireball.moving_direction = glm::normalize(cam_pos - enemy.location);
            fireball.life_span = 4000;
            fireball.textureID = texture[3];
            sceneGraph.push_back(fireball);
 
            enemy.lastShotTime = now;
        }

        // Check collision with player
        float dist = glm::length(cam_pos - enemy.location);
        if (dist < enemy.collider_dimension / 2.0f) {
            if (!gameOver && !gameWon && playerHealth > 0) {
                playerHealth -= 10;
                playerHealth = std::max(0, playerHealth);  // clamp to 0
                enemy.isAlive = false;
                std::cout << "Player hit! Health: " << playerHealth << std::endl;

                if (playerHealth <= 0) {
                    gameOver = true;
                    std::cout << "Game Over! You lost!" << std::endl;
                }
            }
        }

       

    }
}

void draw_bullet()
{
    updateSceneGraph();
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (GameObject& go : sceneGraph) {
        if (go.isAlive && !go.isCollided) {
            model_view = glm::translate(glm::mat4(1.0), go.location);
            glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
            drawCube(go.scale, texture[3]);
            model_view = glm::mat4(1.0);
        }
    }

}

void draw_level() {
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glDrawArrays(GL_QUADS, 0, 4);
    updateSceneGraph();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    for (GameObject& go : sceneGraph) {
        if (go.isAlive && !go.isCollided) {
            model_view = glm::translate(glm::mat4(1.0), go.location);
            glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
            if (go.type == OBSTACLE) {
                drawCube(go.scale, texture[1]);
            }
            else if (go.type == BULLET) {

                drawCube(go.scale, texture[2]);
            }
           
            model_view = glm::mat4(1.0);
        }
    }


   

    for (GameObject& enemy : enemyList) {
        if (!enemy.isAlive || enemy.isCollided) continue;
        model_view = glm::mat4(1.0f);
        model_view = glm::translate(glm::mat4(1.0), enemy.location);
        glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);
        drawPyramid(glm::vec3(0.5f), enemy.textureID);
        model_view = glm::mat4(1.0f);
    }
}

void renderBitmapString(float x, float y, void* font, const char* string) {
    glWindowPos2f(x, y); 
    for (const char* c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void drawTextBackgroundBox(float x, float y, float width, float height) {
    glColor3f(0.4f, 0.2f, 0.1f); 

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}


void display() {
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset model matrix
    model_view = glm::mat4(1.0);
    glUniformMatrix4fv(location, 1, GL_FALSE, &model_view[0][0]);

    // Camera matrix
    glm::vec3 look_at = cam_pos + looking_dir_vector;
    glm::mat4 camera_matrix = glm::lookAt(cam_pos, look_at, up_vector);
    glUniformMatrix4fv(cam_mat_location, 1, GL_FALSE, &camera_matrix[0][0]);

    // Projection matrix
    glm::mat4 proj_matrix = glm::frustum(-0.01f, 0.01f, -0.01f, 0.01f, 0.01f, 100.0f);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, &proj_matrix[0][0]);

    // Draw level and game objects
    draw_level();
    draw_bullet();

    // === Overlay: Win or Loss Message ===
    if (gameWon || gameOver) {
        // Disable depth and texture so 2D overlay isn't affected
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        // Enable blending so we can draw solid color quads over screen
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Switch to 2D orthographic projection for screen-space drawing
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 1024, 0, 1024);  // Match window size

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Draw a brown rectangle as background behind the text
        glColor3f(0.4f, 0.2f, 0.1f);  // Dark brown
        glBegin(GL_QUADS);
        glVertex2f(390.0f, 495.0f);  // slightly bigger for padding
        glVertex2f(670.0f, 495.0f);
        glVertex2f(670.0f, 545.0f);
        glVertex2f(390.0f, 545.0f);
        glEnd();

        // Now draw the text on top of the background box
        glColor3f(1.0f, 1.0f, 1.0f);  // White
        if (gameOver) {
            renderBitmapString(420.0f, 520.0f, GLUT_BITMAP_HELVETICA_18, "Game Over! You lost!");
        }
        else if (gameWon) {
            renderBitmapString(460.0f, 520.0f, GLUT_BITMAP_HELVETICA_18, "You Win!");
        }

        // Display Final Score
        char scoreText[64];
        sprintf(scoreText, "Final Score: %d", playerScore);
        renderBitmapString(440.0f, 500.0f, GLUT_BITMAP_HELVETICA_18, scoreText);

        // Restore projection and modelview matrices
        glPopMatrix(); // MODELVIEW
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        // Restore OpenGL settings
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }




    glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
    if (gameWon || gameOver) return;  // freeze controls

	if (key == 'a')
	{
		//Moving camera along opposit direction of side vector
		cam_pos += side_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 'd')
	{
		//Moving camera along side vector
		cam_pos -= side_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 'w')
	{
		//Moving camera along forward vector. To be more realistic, we use X=V.T equation in physics
		cam_pos += forward_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}
	if (key == 's')
	{
		//Moving camera along backward (negative forward) vector. To be more realistic, we use X=V.T equation in physics
		cam_pos -= forward_vector * travel_speed * ((float)deltaTime) / 1000.0f;
	}

	//Added on Nov. 21 2021 by: Alireza Moghaddam
	if (key == 'f')
	{

        GameObject go;
        go.location = cam_pos;	//The bullet will spawn with an offset from the location of the player
        go.rotation = glm::vec3(0, 0, 0);
        go.scale = glm::vec3(0.07, 0.07, 0.07);
        go.collider_dimension = go.scale.x;
        go.isAlive = true;
        go.living_time = 0;
        go.isCollided = false;
        go.velocity = 0.01;
        go.type = BULLET;
        go.moving_direction = looking_dir_vector;
        go.life_span = 4000;	//Each bullet lives for 2 seconds
        sceneGraph.push_back(go);

    }
}

//Controlling Pitch with vertical mouse movement
void mouse(int x, int y) {
    int delta_x = x - x0;
    forward_vector = glm::rotate(forward_vector, -delta_x * mouse_sensitivity, unit_z_vector);
    looking_dir_vector = glm::rotate(looking_dir_vector, -delta_x * mouse_sensitivity, unit_z_vector);
    side_vector = glm::rotate(side_vector, -delta_x * mouse_sensitivity, unit_z_vector);
    up_vector = glm::rotate(up_vector, -delta_x * mouse_sensitivity, unit_z_vector);
    x0 = x;

    int delta_y = y - y_0;
    glm::vec3 tmp_look = glm::rotate(looking_dir_vector, delta_y * mouse_sensitivity, side_vector);
    if (glm::dot(tmp_look, forward_vector) > 0) {
        looking_dir_vector = tmp_look;
        up_vector = glm::rotate(up_vector, delta_y * mouse_sensitivity, side_vector);
    }
    y_0 = y;
}

void idle() {
    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

    if (!gameOver && !gameWon && timeSinceStart >= 30000) {
        gameWon = true;
        std::cout << "You Win!" << std::endl;
    }

    if (!gameOver && !gameWon) {
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0;
            spawnEnemy(enemyList, enemyTextureID);
            spawnInterval = std::max(500.0f, spawnInterval - 50.0f);
        }
    }

    glutPostRedisplay();
}

void init()
{
    //Normalizing all vectors
    up_vector = glm::normalize(up_vector);
    forward_vector = glm::normalize(forward_vector);
    looking_dir_vector = glm::normalize(looking_dir_vector);
    side_vector = glm::normalize(side_vector);

    //Randomizing obstacles and adding them to the GameScene
    for (int i = 0; i < Num_Obstacles; i++)
    {
        obstacle_data[i][0] = randomFloat(-50, 50); //X
        obstacle_data[i][1] = randomFloat(-50, 50); //Y
        obstacle_data[i][2] = randomFloat(0.1f, 10.0f); //Scale

        GameObject go;
        go.location = glm::vec3(obstacle_data[i][0], obstacle_data[i][1], 0);
        go.rotation = glm::vec3(0, 0, 0);
        go.scale = glm::vec3(obstacle_data[i][2], obstacle_data[i][2], obstacle_data[i][2]);
        go.collider_dimension = go.scale.x;
        go.isAlive = true;
        go.living_time = 0;
        go.isCollided = false;
        go.velocity = 0;
        go.type = OBSTACLE;
        go.moving_direction = glm::vec3(0, 0, 0);
        go.life_span = -1;
        sceneGraph.push_back(go);
    }

    /*enemyTextureID = SOIL_load_OGL_texture("ship.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (enemyTextureID == 0) {
        std::cout << "Failed to load enemy texture!" << std::endl;
    }
    else {
        std::cout << "Enemy texture loaded: " << enemyTextureID << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, enemyTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/


    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" },
        { GL_NONE, NULL }
    };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);

    GLfloat vertices[NumVertices][3] = {
        { -100.0, -100.0, 0.0 }, { 100.0, -100.0, 0.0 }, { 100.0, 100.0, 0.0 }, { -100.0, 100.0, 0.0 },
        { -0.5, -0.5 ,0.01 }, { 0.5, -0.5 ,0.01 }, { 0.5, 0.5 ,0.01 }, { -0.5, 0.5 ,0.01 },
        { -0.5, -0.5, 1.01 }, { 0.5, -0.5, 1.01 }, { 0.5, 0.5, 1.01 }, { -0.5, 0.5, 1.01 },
        { 0.5, -0.5 , 0.01 }, { 0.5, 0.5 , 0.01 }, { 0.5, 0.5 ,1.01 }, { 0.5, -0.5 ,1.01 },
        { -0.5, -0.5, 0.01 }, { -0.5, 0.5 , 0.01 }, { -0.5, 0.5 ,1.01 }, { -0.5, -0.5 ,1.01 },
        { -0.5, 0.5 , 0.01 }, { 0.5, 0.5 , 0.01 }, { 0.5, 0.5 ,1.01 }, { -0.5, 0.5 ,1.01 },
        { -0.5, -0.5 , 0.01 }, { 0.5, -0.5 , 0.01 }, { 0.5, -0.5 ,1.01 }, { -0.5, -0.5 ,1.01 },
    };

    GLfloat textureCoordinates[28][2] = {
        0.0f, 0.0f, 200.0f, 0.0f, 200.0f, 200.0f, 0.0f, 200.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    // t0
    GLint width1, height1;
    unsigned char* textureData1 = SOIL_load_image("floor.png", &width1, &height1, 0, SOIL_LOAD_RGB);
    //t1
    GLint width2, height2;
    unsigned char* textureData2 = SOIL_load_image("box.png", &width2, &height2, 0, SOIL_LOAD_RGB);
    //t2
    GLint width3, height3;
    unsigned char* textureData3 = SOIL_load_image("bullet.png", &width3, &height3, 0, SOIL_LOAD_RGB);
    //t3
    GLint width4, height4;
    unsigned char* textureData4 = SOIL_load_image("fire.png", &width4, &height4, 0, SOIL_LOAD_RGB);
    //t4
    GLint width5, height5;
    unsigned char* textureData5 = SOIL_load_image("ammo.png", &width5, &height5, 0, SOIL_LOAD_RGB);

    glGenBuffers(3, Buffers);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindAttribLocation(program, 0, "vPosition");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    glBindAttribLocation(program, 1, "vTexCoord");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
    glBindAttribLocation(program, 1, "vTexCoord");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);


    location = glGetUniformLocation(program, "model_matrix");
    cam_mat_location = glGetUniformLocation(program, "camera_matrix");
    proj_mat_location = glGetUniformLocation(program, "projection_matrix");

    glGenTextures(10, texture);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData2);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData3);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData4);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData5);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    // Set a bright sky blue background
    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);   // R, G, B, A

}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("Camera and Projection");

    glewInit();
    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(mouse);
    glutMainLoop();
    return 0;
}
