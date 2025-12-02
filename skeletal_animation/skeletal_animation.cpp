#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>

#include <stb_image.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <SFML/Audio.hpp>

// ==================== SOUND SYSTEM ====================
class SoundManager {
private:
    sf::Music backgroundMusic;
    sf::Music gamebackMusic;

    sf::SoundBuffer chooseButtonBuffer;
    sf::SoundBuffer gameBackBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::SoundBuffer gunShotBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer menuBackBuffer;
    sf::SoundBuffer openingBuffer;
    sf::SoundBuffer startGameBuffer;


    sf::Sound* chooseButtonSound;
    sf::Sound* gameBackSound;
    sf::Sound* gameOverSound;
    sf::Sound* gunShotSound;
    sf::Sound* healSound;
    sf::Sound* menuBackSound;
    sf::Sound* openingSound;
    sf::Sound* startGameSound;

    bool initialized;

public:
    SoundManager() : initialized(false) {
        // Load all sound buffers
        std::string testPath = FileSystem::getPath("resources/audio/_choosebutton.wav");
        std::cout << "Attempting to load sound from: " << testPath << std::endl;
        std::cout << "Path length: " << testPath.length() << std::endl;
        std::cout << "Path is empty: " << (testPath.empty() ? "YES" : "NO") << std::endl;

        if (!chooseButtonBuffer.loadFromFile("C:/Users/ASUS/Desktop/_1/LearnOpenGL-master/resources/audio/choosebutton.wav")) {
            std::cout << "Warning: Failed to load choosebutton sound\n";
        }

        if (!gameBackBuffer.loadFromFile(FileSystem::getPath("resources/audio/_gameback.wav"))) {
            std::cout << "Warning: Failed to load gameback sound\n";
        }

        if (!gameOverBuffer.loadFromFile(FileSystem::getPath("resources/audio/_gameover.wav"))) {
            std::cout << "Warning: Failed to load gameover sound\n";
        }

        if (!gunShotBuffer.loadFromFile(FileSystem::getPath("resources/audio/_gunshot.wav"))) {
            std::cout << "Warning: Failed to load gunshot sound\n";
        }

        if (!healBuffer.loadFromFile(FileSystem::getPath("resources/audio/_heal.wav"))) {
            std::cout << "Warning: Failed to load heal sound\n";
        }

        if (!menuBackBuffer.loadFromFile(FileSystem::getPath("resources/audio/_menuback.wav"))) {
            std::cout << "Warning: Failed to load menuback sound\n";
        }

        if (!openingBuffer.loadFromFile(FileSystem::getPath("resources/audio/_opening.wav"))) {
            std::cout << "Warning: Failed to load opening sound\n";
        }

        if (!startGameBuffer.loadFromFile(FileSystem::getPath("resources/audio/_startgame.wav"))) {
            std::cout << "Warning: Failed to load startgame sound\n";
        }


        chooseButtonSound = new sf::Sound(chooseButtonBuffer);
        gameBackSound = new sf::Sound(gameBackBuffer);
        gameOverSound = new sf::Sound(gameOverBuffer);
        gunShotSound = new sf::Sound(gunShotBuffer);
        healSound = new sf::Sound(healBuffer);
        menuBackSound = new sf::Sound(menuBackBuffer);
        openingSound = new sf::Sound(openingBuffer);
        startGameSound = new sf::Sound(startGameBuffer);

        // Set volumes
        gunShotSound->setVolume(100);
        chooseButtonSound->setVolume(80);
        healSound->setVolume(60);

        initialized = true;
        std::cout << "Sound system initialized!\n";
    }


    ~SoundManager() {
        delete chooseButtonSound;
        delete gameBackSound;
        delete gameOverSound;
        delete gunShotSound;
        delete healSound;
        delete menuBackSound;
        delete openingSound;
        delete startGameSound;
    }

    // ==================== BACKGROUND MUSIC ====================
    void playMenuMusic(bool loop = true) {
        if (backgroundMusic.openFromFile(FileSystem::getPath("resources/audio/_menuback.wav"))) {
            backgroundMusic.setLooping(loop);
            backgroundMusic.setVolume(30);
            backgroundMusic.play();
            std::cout << "Playing menu music\n";
        }
    }

    void playGameMusic(bool loop = true) {
        stopBackgroundMusic();
        if (gamebackMusic.openFromFile(FileSystem::getPath("resources/audio/_gameback.wav"))) {
            gamebackMusic.setLooping(loop);
            gamebackMusic.setVolume(25);
            gamebackMusic.play();
            std::cout << "Playing game music\n";
        }
    }

    void stopBackgroundMusic() {
        backgroundMusic.stop();
        gamebackMusic.stop();
    }

    void pauseBackgroundMusic() {
        backgroundMusic.pause();
        gamebackMusic.pause();
    }

    void resumeBackgroundMusic() {
        backgroundMusic.play();
        gamebackMusic.play();
    }

    void setMusicVolume(float volume) {
        backgroundMusic.setVolume(volume);
        gamebackMusic.setVolume(volume);
    }

    // ==================== SOUND EFFECTS ====================
    void playChooseButton() {
        if (initialized) {
            chooseButtonSound->play();
        }
    }

    void playGunShot() {
        if (initialized) {
            gunShotSound->play();
        }
    }

    void playHeal() {
        if (initialized) {
            healSound->play();
        }
    }

    void playGameOver() {
        if (initialized) {
            stopBackgroundMusic();
            gameOverSound->play();
        }
    }

    void playOpening() {
        if (initialized) {
            openingSound->play();
        }
    }

    void playStartGame() {
        if (initialized) {
            startGameSound->play();
        }
    }

    void playGameBack() {
        if (initialized) {
            gameBackSound->play();
        }
    }

    void stopAllSounds() {
        chooseButtonSound->stop();
        gameBackSound->stop();
        gameOverSound->stop();
        gunShotSound->stop();
        healSound->stop();
        menuBackSound->stop();
        openingSound->stop();
        startGameSound->stop();
    }
};

// Global sound manager
SoundManager* soundManager = nullptr;



enum class GameState {
    MENU,
    PLAYING,
    PAUSED
};

GameState gameState = GameState::MENU;

int selectedIndex = 0;   // 0 = Start, 1 = Quit
int pausedSelectedIndex = 0;  // 0 = Resume, 1 = Return to Menu

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

std::map<char, Character> Characters;
unsigned int textVAO, textVBO;


unsigned int quadVAO = 0, quadVBO = 0;

float quadVertices[] = {
    // pos        // uv
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f,

    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f
};

unsigned int triangleVAO = 0, triangleVBO = 0;

float triangleVertices[] = {
    // pos        // uv
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.5f, 1.0f
};

void initTriangle()
{
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void drawTriangle(Shader& shader, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotation = 0.0f)
{
    glm::mat4 model = glm::mat4(1.0f);

    // Translate to position
    model = glm::translate(model, glm::vec3(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, 0.0f));

    // Rotate
    if (rotation != 0.0f)
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

    // Scale
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    shader.setMat4("model", model);
    shader.setVec3("color", color);

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void initQuad()
{
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

bool isMouseOver(float mx, float my, float x, float y, float w, float h)
{
    return (mx >= x && mx <= x + w && my >= y && my <= y + h);
}

void drawButton(Shader& shader, glm::vec2 pos, glm::vec2 size, glm::vec3 color)
{
    glm::mat4 model = glm::mat4(1.0f);

    // Translate to center of button in pixel coordinates (ortho projection uses pixels)
    model = glm::translate(model, glm::vec3(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f, 0.0f));
    // Scale: quad's half-size is 0.5 in model, so scaling by 'size' maps to pixel size
    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    shader.setMat4("model", model);
    shader.setVec3("color", color);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawHealthBar(Shader& shader, float health, float maxHealth, float screenHeight)
{
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float barX = 20.0f;
    float barY = screenHeight - 40.0f;

    // background
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(barX + barWidth * 0.5f, barY + barHeight * 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(barWidth, barHeight, 1.0f));
    shader.setMat4("model", model);
    shader.setVec3("color", glm::vec3(0.2f, 0.2f, 0.2f));
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    float healthPercent = health / maxHealth;
    float healthBarWidth = barWidth * healthPercent;

    if (healthBarWidth > 0.0f)
    {
        glm::vec3 healthColor;
        if (healthPercent > 0.6f)
            healthColor = glm::vec3(0.0f, 1.0f, 0.0f);
        else if (healthPercent > 0.3f)
            healthColor = glm::vec3(1.0f, 1.0f, 0.0f);
        else
            healthColor = glm::vec3(1.0f, 0.0f, 0.0f);

        // blood bar
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(barX + healthBarWidth * 0.5f, barY + barHeight * 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(healthBarWidth, barHeight - 4.0f, 1.0f));
        shader.setMat4("model", model);
        shader.setVec3("color", healthColor);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}


void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    shader.use();
    shader.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    for (auto c : text)
    {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Score System
int currentScore = 0;
int highScore = 0;

void drawScore(Shader& textShader, int score, int highScore, float screenWidth, float screenHeight)
{
    textShader.use();

    std::string scoreText = "Score: " + std::to_string(score);
    float scoreX = screenWidth - 200.0f;
    float scoreY = screenHeight - 40.0f;

    RenderText(textShader, scoreText, scoreX, scoreY, 0.8f, glm::vec3(1, 1, 1));

    std::string highScoreText = "High Score: " + std::to_string(highScore);
    float highScoreX = screenWidth - 250.0f;
    float highScoreY = screenHeight - 75.0f;

    RenderText(textShader, highScoreText, highScoreX, highScoreY, 0.6f, glm::vec3(1, 1, 0));
}


struct Bullet {
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float life;
};

std::vector<Bullet> bullets;
const float BULLET_SPEED = 15.0f;
const float BULLET_LIFETIME = 3.0f;

struct Target {
    glm::vec3 position;
    float speed;
    Animator* animator;     // per-target animator (allocated with new at spawn)
    glm::vec3 bboxMin;      // local-space AABB min
    glm::vec3 bboxMax;      // local-space AABB max
    glm::vec3 modelScale;   // model scale used when rendering -> apply to bbox
};

std::vector<Target> targets;
const float TARGET_SPEED = 1.2f;
const float SPAWN_INTERVAL = 3.0f;
float timeSinceLastSpawn = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateCamera();
bool bulletHitsTarget(const Bullet& bullet, const Target& target);
void cleanupTargets(); // cleanup any leftover dynamic animators on exit

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.2f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// player (character)
glm::vec3 characterPosition = glm::vec3(0.0f, 0.09f, 0.0f);
float characterYaw = 0.0f; // rotation of the player model
float cameraYaw = 0.0f;    // horizontal orbit angle around the player
float cameraPitch = 0.0f;
glm::vec3 characterScale = glm::vec3(0.5f);
const float CHARACTER_SPEED = 2.5f; // units/sec
const float MOUSE_SENSITIVITY = 0.1f;
const float CAMERA_DISTANCE = 3.0f; // distance behind character
const float CAMERA_HEIGHT = 1.5f;   // height above character

// Health System
float playerHealth = 100.0f;
const float MAX_HEALTH = 100.0f;
bool playerDead = false;
float respawnTimer = 0.0f;
const float RESPAWN_TIME = 3.0f;
const float ENEMY_DAMAGE = 20.0f;
float lastDamageTime = 0.0f;
const float DAMAGE_COOLDOWN = 1.0f;

// animation state (player)
Animation* idleAnimPtr = nullptr;
Animation* runForwardPtr = nullptr;
Animation* runBackPtr = nullptr;
Animation* runLeftPtr = nullptr;
Animation* runRightPtr = nullptr;
Animation* runForwardLeftPtr = nullptr;
Animation* runForwardRightPtr = nullptr;
Animation* runBackLeftPtr = nullptr;
Animation* runBackRightPtr = nullptr;
Animation* currentAnimPtr = nullptr;
Animator* animatorPtr = nullptr;

// Enemy model + animation pointers (point to objects created in main)
Model* enemyModelPtr = nullptr;
Animation* enemyRunPtr = nullptr;

unsigned int cubeVAO = 0, cubeVBO = 0;

float cubeVertices[] = {
    // A simple 1x1x1 cube (36 vertices)
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f
};

void initCube() {
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

int main()
{
    srand((unsigned int)time(nullptr));

    // glfw init + callbacks
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Granny Please Go Home", NULL, NULL);
    if (!window) { std::cout << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed GLAD\n"; return -1; }
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);


    // Load text shader (after OpenGL context exists)
    Shader textShader("text.vs", "text.fs");

    // ----------------- TEXT RENDERING INIT -----------------
    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    std::string fontPath = FileSystem::getPath("resources/fonts/Antonio-Regular.ttf");
    std::cout << "Loading font from: " << fontPath << std::endl;

    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font: " << fontPath << std::endl;
        FT_Done_FreeType(ft);
        glfwTerminate();
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Setup VAO/VBO for text
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindVertexArray(0);

    initQuad();
    initTriangle();

    // Shaders
    Shader menuShader("menu.vs", "menu.fs");
    Shader skinnedShader("anim_model.vs", "anim_model.fs");
    Shader platformShader("single_color.vs", "single_color.fs");

    // load model + animations (PLAYER)
    Model ourModel(FileSystem::getPath("resources/objects/gun2/rifle.dae"));
    Animation idleAnim(FileSystem::getPath("resources/objects/gun2/rifle_idle.dae"), &ourModel);
    Animation runForwardAnim(FileSystem::getPath("resources/objects/gun2/run_forward.dae"), &ourModel);
    Animation runBackAnim(FileSystem::getPath("resources/objects/gun2/run_back.dae"), &ourModel);
    Animation runLeftAnim(FileSystem::getPath("resources/objects/gun2/run_left.dae"), &ourModel);
    Animation runRightAnim(FileSystem::getPath("resources/objects/gun2/run_right.dae"), &ourModel);
    Animation runForwardLeftAnim(FileSystem::getPath("resources/objects/gun2/run_forward_left.dae"), &ourModel);
    Animation runForwardRightAnim(FileSystem::getPath("resources/objects/gun2/run_forward_right.dae"), &ourModel);
    Animation runBackLeftAnim(FileSystem::getPath("resources/objects/gun2/run_back_left.dae"), &ourModel);
    Animation runBackRightAnim(FileSystem::getPath("resources/objects/gun2/run_back_right.dae"), &ourModel);

    // assign player animation pointers
    idleAnimPtr = &idleAnim;
    runForwardPtr = &runForwardAnim;
    runBackPtr = &runBackAnim;
    runLeftPtr = &runLeftAnim;
    runRightPtr = &runRightAnim;
    runForwardLeftPtr = &runForwardLeftAnim;
    runForwardRightPtr = &runForwardRightAnim;
    runBackLeftPtr = &runBackLeftAnim;
    runBackRightPtr = &runBackRightAnim;

    Animator animator(&idleAnim);
    animatorPtr = &animator;
    animator.PlayAnimation(idleAnimPtr);
    currentAnimPtr = idleAnimPtr;

    // --- ENEMY model + animation load (use your own files here) ---
    Model enemyModel(FileSystem::getPath("resources/objects/kid/running.dae"));
    Animation enemyRunAnim(FileSystem::getPath("resources/objects/kid/running.dae"), &enemyModel);

    enemyModelPtr = &enemyModel;
    enemyRunPtr = &enemyRunAnim;

    initCube();
    soundManager = new SoundManager();
    soundManager->playMenuMusic(true);

    bool escPressedLastFrame = false;
    bool enterPressedLastFrame = false;

    std::string p = FileSystem::getPath("resources/audio/_choosebutton.wav");
    std::cout << "Resolved path: " << p << std::endl;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // ============ GLOBAL ESC HANDLER============
        bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
        bool escJustPressed = escPressed && !escPressedLastFrame;

        if (escJustPressed && gameState == GameState::PLAYING)
        {
            gameState = GameState::PAUSED;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "Game PAUSED" << std::endl; // debug
        }
        escPressedLastFrame = escPressed;
        // ============ END GLOBAL ESC HANDLER ============

        // ============ GLOBAL ENTER EDGE DETECTION ============
        bool enterPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
        bool enterJustPressed = enterPressed && !enterPressedLastFrame;
        enterPressedLastFrame = enterPressed;
        // ============ END GLOBAL ENTER EDGE DETECTION ============

        if (gameState == GameState::MENU)
        {
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH,
                0.0f, (float)SCR_HEIGHT);
            menuShader.use();
            menuShader.setMat4("projection", projection);

            // --- INPUT CONTROL ---
            static double lastInputTime = 0.0;
            double now = glfwGetTime();

            if (now - lastInputTime > 0.15)
            {
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    if (soundManager) soundManager->playChooseButton();
                    selectedIndex--;
                    if (selectedIndex < 0) selectedIndex = 0;
                    lastInputTime = now;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (soundManager) soundManager->playChooseButton();
                    selectedIndex++;
                    if (selectedIndex > 1) selectedIndex = 1;
                    lastInputTime = now;
                }
                if (enterJustPressed)
                {
                    if (selectedIndex == 0)
                    {
                        if (soundManager) soundManager->playStartGame();
                        gameState = GameState::PLAYING;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        if (soundManager) soundManager->playGameMusic(true);
                        std::cout << "Game STARTED" << std::endl;
                    }
                    if (selectedIndex == 1)
                        glfwSetWindowShouldClose(window, true);

                    lastInputTime = now;
                }
            }


            // --- DRAW MENU BUTTONS ---
            glm::vec2 startPos = glm::vec2(300, 350);
            glm::vec2 quitPos = glm::vec2(300, 230);

            drawButton(menuShader, startPos, glm::vec2(250, 80), glm::vec3(0.0f, 0.0f, 0.0f));
            drawButton(menuShader, quitPos, glm::vec2(250, 80), glm::vec3(0.0f, 0.0f, 0.0f));

            // --- DRAW SELECTION ARROW ---
            glm::vec2 arrowPos;
            if (selectedIndex == 0)
                arrowPos = glm::vec2(startPos.x - 40, startPos.y + 30);
            else
                arrowPos = glm::vec2(quitPos.x - 40, quitPos.y + 30);

            drawTriangle(menuShader, arrowPos, glm::vec2(30, 30), glm::vec3(1, 1, 0), -90.0f);


            // ------------------- TEXT RENDERING -------------------
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            textShader.use();

            glm::mat4 textProjection = glm::ortho(
                0.0f,
                (float)SCR_WIDTH,
                0.0f,
                (float)SCR_HEIGHT
            );

            textShader.setMat4("projection", textProjection);
            textShader.setInt("text", 0);


            if (selectedIndex == 0)
            {
                RenderText(textShader, "START GAME", 330.0f, 380.0f, 1.0f, glm::vec3(0, 1, 1));
                RenderText(textShader, "QUIT", 350.0f, 260.0f, 1.0f, glm::vec3(1, 1, 1));
            }
            else
            {
                RenderText(textShader, "START GAME", 330.0f, 380.0f, 1.0f, glm::vec3(1, 1, 1));
                RenderText(textShader, "QUIT", 350.0f, 260.0f, 1.0f, glm::vec3(0, 1, 1));
            }


            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }
        // ============ PAUSED STATE ============
        if (gameState == GameState::PAUSED)
        {
            // Input handling
            static double lastPauseInputTime = 0.0;
            double now = glfwGetTime();

            if (now - lastPauseInputTime > 0.15)
            {

                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    if (soundManager) soundManager->playChooseButton();
                    pausedSelectedIndex--;
                    if (pausedSelectedIndex < 0) pausedSelectedIndex = 0;
                    lastPauseInputTime = now;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (soundManager) soundManager->playChooseButton();
                    pausedSelectedIndex++;
                    if (pausedSelectedIndex > 1) pausedSelectedIndex = 1;
                    lastPauseInputTime = now;
                }
                if (enterJustPressed)
                {
                    if (pausedSelectedIndex == 0)
                    {
                        gameState = GameState::PLAYING;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        std::cout << "Game RESUMED" << std::endl;
                    }
                    if (pausedSelectedIndex == 1)
                    {
                        // Return to main menu
                        if (soundManager) {
                            soundManager->playGameBack();
                            soundManager->playMenuMusic(true);
                        }
                        gameState = GameState::MENU;
                        selectedIndex = 0;
                        pausedSelectedIndex = 0;
                        cleanupTargets();
                        bullets.clear();
                        currentScore = 0;
                        playerHealth = MAX_HEALTH;
                        playerDead = false;
                        characterPosition = glm::vec3(0.0f, 0.09f, 0.0f);
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        std::cout << "Returned to MENU" << std::endl;
                    }
                    lastPauseInputTime = now;
                }

            }

            // Render frozen game scene
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            // Draw player
            if (!playerDead)
            {
                skinnedShader.use();
                skinnedShader.setMat4("projection", projection);
                skinnedShader.setMat4("view", view);

                auto transforms = animator.GetFinalBoneMatrices();
                for (int i = 0; i < (int)transforms.size(); ++i)
                    skinnedShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition);
                model = glm::rotate(model, glm::radians(characterYaw + 180.0f), glm::vec3(0, 1, 0));
                model = glm::scale(model, characterScale);
                skinnedShader.setMat4("model", model);

                ourModel.Draw(skinnedShader);
            }

            // Draw platform
            platformShader.use();
            platformShader.setMat4("projection", projection);
            platformShader.setMat4("view", view);
            platformShader.setVec3("color", glm::vec3(0.4f, 0.4f, 0.4f));
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::scale(m, glm::vec3(10.0f, 0.2f, 10.0f));
            platformShader.setMat4("model", m);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Draw walls (same as in playing state)
            platformShader.setVec3("color", glm::vec3(0.2f, 0.2f, 0.2f));
            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(0.0f, 1.0f, -5.0f));
            m = glm::scale(m, glm::vec3(10.0f, 2.0f, 0.2f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(0.0f, 1.0f, 5.0f));
            m = glm::scale(m, glm::vec3(10.0f, 2.0f, 0.2f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(-5.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(0.2f, 2.0f, 10.0f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(5.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(0.2f, 2.0f, 10.0f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Draw bullets
            platformShader.setVec3("color", glm::vec3(1.0f, 0.8f, 0.2f));
            for (auto& bullet : bullets)
            {
                glm::mat4 bm = glm::mat4(1.0f);
                bm = glm::translate(bm, bullet.position);
                bm = glm::scale(bm, glm::vec3(0.06f));
                platformShader.setMat4("model", bm);
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Draw enemies
            skinnedShader.use();
            skinnedShader.setMat4("projection", projection);
            skinnedShader.setMat4("view", view);

            for (auto& t : targets)
            {
                auto boneTransforms = t.animator->GetFinalBoneMatrices();
                for (int bi = 0; bi < (int)boneTransforms.size(); ++bi)
                    skinnedShader.setMat4("finalBonesMatrices[" + std::to_string(bi) + "]", boneTransforms[bi]);

                glm::mat4 em = glm::mat4(1.0f);
                em = glm::translate(em, t.position);

                glm::vec3 toPlayer = characterPosition - t.position;
                toPlayer.y = 0.0f;
                if (glm::length2(toPlayer) > 1e-6f) {
                    toPlayer = glm::normalize(toPlayer);
                    glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f), toPlayer, glm::vec3(0.0f, 1.0f, 0.0f)));
                    rot = rot * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));
                    em *= rot;
                }

                em = glm::scale(em, t.modelScale);
                skinnedShader.setMat4("model", em);
                enemyModelPtr->Draw(skinnedShader);
            }

            // Draw semi-transparent overlay
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 orthoProjection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
            menuShader.use();
            menuShader.setMat4("projection", orthoProjection);

            // Dark overlay (50% transparent black)
            glm::mat4 overlayModel = glm::mat4(1.0f);
            overlayModel = glm::translate(overlayModel, glm::vec3(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.5f, 0.0f));
            overlayModel = glm::scale(overlayModel, glm::vec3(SCR_WIDTH, SCR_HEIGHT, 1.0f));
            menuShader.setMat4("model", overlayModel);
            menuShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Draw pause menu buttons
            glm::vec2 resumePos = glm::vec2(275, 350);
            glm::vec2 menuPos = glm::vec2(275, 230);

            drawButton(menuShader, resumePos, glm::vec2(250, 80), glm::vec3(0.0f, 0.0f, 0.0f));
            drawButton(menuShader, menuPos, glm::vec2(250, 80), glm::vec3(0.0f, 0.0f, 0.0f));

            // Selection arrow
            glm::vec2 arrowPos;
            if (pausedSelectedIndex == 0)
                arrowPos = glm::vec2(resumePos.x - 40, resumePos.y + 30);
            else
                arrowPos = glm::vec2(menuPos.x - 40, menuPos.y + 30);

            drawTriangle(menuShader, arrowPos, glm::vec2(30, 30), glm::vec3(1, 1, 0), -90.0f);


            // Draw text
            textShader.use();
            glm::mat4 textProjection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
            textShader.setMat4("projection", textProjection);
            textShader.setInt("text", 0);

            RenderText(textShader, "PAUSE", 310.0f, 480.0f, 1.5f, glm::vec3(1, 1, 0));

            if (pausedSelectedIndex == 0)
            {
                RenderText(textShader, "CONTINUE", 320.0f, 380.0f, 1.0f, glm::vec3(0, 1, 1));
                RenderText(textShader, "MAIN MENU", 290.0f, 260.0f, 1.0f, glm::vec3(1, 1, 1));
            }
            else
            {
                RenderText(textShader, "CONTINUE", 320.0f, 380.0f, 1.0f, glm::vec3(1, 1, 1));
                RenderText(textShader, "MAIN MENU", 290.0f, 260.0f, 1.0f, glm::vec3(0, 1, 1));
            }

            glDisable(GL_BLEND);

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // ============ PLAYING STATE ============
        if (gameState == GameState::PLAYING)
        {
            // 1. Update Time
            glEnable(GL_DEPTH_TEST);
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // 2. Handle Player Death and Respawn
            if (playerDead)
            {
                respawnTimer += deltaTime;
                if (respawnTimer >= RESPAWN_TIME)
                {
                    // Respawn
                    playerDead = false;
                    playerHealth = MAX_HEALTH;
                    respawnTimer = 0.0f;
                    characterPosition = glm::vec3(0.0f, 0.09f, 0.0f);
                    currentScore = 0;
                    cleanupTargets();
                    std::cout << "Player Respawned!" << std::endl;
                }
            }

            // 3. Update Game Logic (only if player is alive)
            if (!playerDead)
            {
                processInput(window);
                updateCamera();
                animator.UpdateAnimation(deltaTime);

                // Update existing enemies' animations
                for (auto& t : targets) {
                    if (t.animator) t.animator->UpdateAnimation(deltaTime);
                }

                // Target spawn logic
                timeSinceLastSpawn += deltaTime;
                if (timeSinceLastSpawn >= SPAWN_INTERVAL)
                {
                    timeSinceLastSpawn = 0.0f;

                    float range = 12.0f;
                    glm::vec3 pos;
                    do {
                        pos = glm::vec3(
                            (rand() % 100 / 100.0f - 0.5f) * 2.0f * range,
                            0.1f,
                            (rand() % 100 / 100.0f - 0.5f) * 2.0f * range
                        );
                    } while (glm::length(pos - characterPosition) < 2.5f);

                    Target t;
                    t.position = pos;
                    t.speed = TARGET_SPEED;
                    t.animator = new Animator(enemyRunPtr);
                    t.animator->PlayAnimation(enemyRunPtr);

                    t.modelScale = glm::vec3(0.6f);
                    t.bboxMin = glm::vec3(-0.3f, 0.0f, -0.3f);
                    t.bboxMax = glm::vec3(0.3f, 1.5f, 0.3f);

                    targets.push_back(t);
                }

                // Update bullets
                for (int i = 0; i < (int)bullets.size(); )
                {
                    bullets[i].position += bullets[i].direction * bullets[i].speed * deltaTime;
                    bullets[i].life -= deltaTime;

                    if (bullets[i].life <= 0.0f)
                        bullets.erase(bullets.begin() + i);
                    else
                        ++i;
                }

                // Update targets (move toward player)
                for (auto& t : targets)
                {
                    for (auto& t : targets)
                    {
                        float distanceToPlayer = glm::length(characterPosition - t.position);
                        if (distanceToPlayer > 0.5f)
                        {
                            glm::vec3 dir = glm::normalize(characterPosition - t.position);
                            t.position += dir * t.speed * deltaTime;
                        }
                    }
                }

                // Enemy-player collision (damage)
                if (currentFrame - lastDamageTime >= DAMAGE_COOLDOWN)
                {
                    for (const auto& t : targets)
                    {
                        float distance = glm::length(t.position - characterPosition);
                        if (distance < 0.8f)
                        {
                            playerHealth -= ENEMY_DAMAGE;
                            lastDamageTime = currentFrame;

                            std::cout << "Player Hit! Health: " << playerHealth << std::endl;

                            if (playerHealth <= 0.0f)
                            {
                                playerHealth = 0.0f;
                                playerDead = true;
                                if (soundManager) soundManager->playGameOver();
                                std::cout << "Player Died!" << std::endl;
                            }
                            break;
                        }
                    }
                }

                // Bullet-target collision
                for (int i = 0; i < (int)bullets.size(); )
                {
                    bool bulletRemoved = false;

                    for (int j = 0; j < (int)targets.size(); )
                    {
                        if (bulletHitsTarget(bullets[i], targets[j]))
                        {
                            if (targets[j].animator) {
                                delete targets[j].animator;
                                targets[j].animator = nullptr;
                            }
                            targets.erase(targets.begin() + j);
                            bullets.erase(bullets.begin() + i);
                            bulletRemoved = true;
                            currentScore++;
                            std::cout << "Score: " << currentScore << std::endl;

                            // Update High Score
                            if (currentScore > highScore) {
                                highScore = currentScore;
                                std::cout << "High Score: " << highScore << std::endl;
                            }
                            break;
                        }
                        else
                        {
                            ++j;
                        }
                    }

                    if (!bulletRemoved)
                        ++i;
                }
            }
            else // if player is dead
            {
                updateCamera();
            }

            // 4. Render Everything
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            // Draw player (skinned)
            if (!playerDead)
            {
                skinnedShader.use();
                skinnedShader.setMat4("projection", projection);
                skinnedShader.setMat4("view", view);

                auto transforms = animator.GetFinalBoneMatrices();
                for (int i = 0; i < (int)transforms.size(); ++i)
                    skinnedShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, characterPosition);
                model = glm::rotate(model, glm::radians(characterYaw + 180.0f), glm::vec3(0, 1, 0));
                model = glm::scale(model, characterScale);
                skinnedShader.setMat4("model", model);

                ourModel.Draw(skinnedShader);
            }

            // Draw platform & bullets & non-skinned objects
            platformShader.use();
            platformShader.setMat4("projection", projection);
            platformShader.setMat4("view", view);

            // platform
            platformShader.setVec3("color", glm::vec3(0.4f, 0.4f, 0.4f));
            glm::mat4 m = glm::mat4(1.0f);
            m = glm::scale(m, glm::vec3(30.0f, 0.2f, 30.0f));
            platformShader.setMat4("model", m);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // walls
            platformShader.setVec3("color", glm::vec3(0.2f, 0.2f, 0.2f));

            // back wall
            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(0.0f, 1.0f, -15.0f));
            m = glm::scale(m, glm::vec3(30.0f, 2.0f, 0.2f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // front
            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(0.0f, 1.0f, 15.0f));
            m = glm::scale(m, glm::vec3(30.0f, 2.0f, 0.2f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // left
            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(-15.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(0.2f, 2.0f, 30.0f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // right
            m = glm::mat4(1.0f);
            m = glm::translate(m, glm::vec3(15.0f, 1.0f, 0.0f));
            m = glm::scale(m, glm::vec3(0.2f, 2.0f, 30.0f));
            platformShader.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // bullets
            platformShader.use();
            platformShader.setMat4("projection", projection);
            platformShader.setMat4("view", view);
            platformShader.setVec3("color", glm::vec3(1.0f, 0.8f, 0.2f)); // yellowish

            for (auto& bullet : bullets)
            {
                glm::mat4 bm = glm::mat4(1.0f);
                bm = glm::translate(bm, bullet.position);
                bm = glm::scale(bm, glm::vec3(0.06f)); // small bullet
                platformShader.setMat4("model", bm);
                glBindVertexArray(cubeVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Draw enemies (skinned)
            skinnedShader.use();
            skinnedShader.setMat4("projection", projection);
            skinnedShader.setMat4("view", view);

            for (auto& t : targets)
            {
                // Set bone transforms from this enemy animator
                auto boneTransforms = t.animator->GetFinalBoneMatrices();
                for (int bi = 0; bi < (int)boneTransforms.size(); ++bi)
                    skinnedShader.setMat4("finalBonesMatrices[" + std::to_string(bi) + "]", boneTransforms[bi]);

                // Compute model transform so enemy faces the player
                glm::mat4 em = glm::mat4(1.0f);
                em = glm::translate(em, t.position);

                // robust facing: compute XZ-only direction and use inverse(lookAt)
                glm::vec3 toPlayer = characterPosition - t.position;
                toPlayer.y = 0.0f; // ignore vertical difference so enemy doesn't tilt up/down
                if (glm::length2(toPlayer) > 1e-6f) {
                    toPlayer = glm::normalize(toPlayer);

                    // inverse(view) where view = lookAt(0, toPlayer, up) gives a rotation matrix
                    glm::mat4 rot = glm::inverse(glm::lookAt(glm::vec3(0.0f), toPlayer, glm::vec3(0.0f, 1.0f, 0.0f)));

                    // If your model's forward axis is +Z instead of -Z
                    rot = rot * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 1, 0));

                    em *= rot; // em = T * R
                }

                em = glm::scale(em, t.modelScale); // finally scale: T * R * S
                skinnedShader.setMat4("model", em);

                // draw the enemy model
                enemyModelPtr->Draw(skinnedShader);
            }

            // 5. Draw HUD (health bar, scores, messages)
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glm::mat4 orthoProjection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
            menuShader.use();
            menuShader.setMat4("projection", orthoProjection);

            drawHealthBar(menuShader, playerHealth, MAX_HEALTH, (float)SCR_HEIGHT);

            // Draw scores
            textShader.use();
            textShader.setMat4("projection", orthoProjection);
            drawScore(textShader, currentScore, highScore, (float)SCR_WIDTH, (float)SCR_HEIGHT);

            // show "You Died" message if player is dead
            if (playerDead)
            {
                textShader.use();
                textShader.setMat4("projection", orthoProjection);

                float remainingTime = RESPAWN_TIME - respawnTimer;
                std::string respawnText = "Respawning in " + std::to_string((int)remainingTime + 1) + "...";

                // YOU DIED!
                float deathX = 400.0f - 150.0f;  // = 250
                float deathY = 350.0f;

                // Respawning 
                float respawnX = 400.0f - 200.0f;  // = 200
                float respawnY = 280.0f;

                RenderText(textShader, "YOU DIED!", deathX, deathY, 1.8f, glm::vec3(1, 0, 0));
                RenderText(textShader, respawnText, respawnX, respawnY, 1.0f, glm::vec3(1, 1, 1));

                // Final Score
                std::string finalScoreText = "Final Score: " + std::to_string(currentScore);
                RenderText(textShader, finalScoreText, 250.0f, 220.0f, 0.9f, glm::vec3(1, 1, 0));
            }

            glDisable(GL_BLEND);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }


    // cleanup dynamic animators attached to targets
    cleanupTargets();

    if (soundManager) {
        delete soundManager;
        soundManager = nullptr;
    }

    glfwTerminate();
    return 0;
}

// Update camera position to follow character
void updateCamera()
{
    characterYaw = cameraYaw;

    // Calculate camera position behind character
    float yawRad = glm::radians(cameraYaw);
    float pitchRad = glm::radians(cameraPitch);

    // Offset from character (behind and up)
    glm::vec3 offset;
    offset.x = CAMERA_DISTANCE * sin(yawRad) * cos(pitchRad);
    offset.y = CAMERA_HEIGHT + CAMERA_DISTANCE * sin(pitchRad);
    offset.z = CAMERA_DISTANCE * cos(yawRad) * cos(pitchRad);

    camera.Position = characterPosition + offset;

    // Make camera look at character (slightly above center)
    glm::vec3 lookAtPoint = characterPosition + glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 direction = glm::normalize(lookAtPoint - camera.Position);

    // Update camera's front vector
    camera.Front = direction;
    camera.Right = glm::normalize(glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

void processInput(GLFWwindow* window)
{
    if (playerDead) return;

    float yawRad = glm::radians(cameraYaw);
    glm::vec3 camForward = glm::normalize(glm::vec3(-sin(yawRad), 0.0f, -cos(yawRad)));
    glm::vec3 camRight = glm::normalize(glm::vec3(cos(yawRad), 0.0f, -sin(yawRad)));

    glm::vec3 moveDir(0.0f);

    bool w = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool s = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool d = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

    if (w) moveDir += camForward;
    if (s) moveDir -= camForward;
    if (a) moveDir -= camRight;
    if (d) moveDir += camRight;

    bool moving = glm::length(moveDir) > 0.01f;
    if (moving)
    {
        moveDir = glm::normalize(moveDir);
        characterPosition += moveDir * CHARACTER_SPEED * deltaTime;
    }

    // Keep within area
    float limit = 15.0f;
    characterPosition.x = glm::clamp(characterPosition.x, -limit, limit);
    characterPosition.z = glm::clamp(characterPosition.z, -limit, limit);

    // Pick the right animation
    Animation* newAnim = idleAnimPtr;
    if (moving)
    {
        if (w && a && !s && !d)
            newAnim = runForwardLeftPtr;
        else if (w && d && !s && !a)
            newAnim = runForwardRightPtr;
        else if (s && a && !w && !d)
            newAnim = runBackLeftPtr;
        else if (s && d && !w && !a)
            newAnim = runBackRightPtr;
        else if (w && !a && !s && !d)
            newAnim = runForwardPtr;
        else if (s && !a && !w && !d)
            newAnim = runBackPtr;
        else if (a && !w && !s && !d)
            newAnim = runLeftPtr;
        else if (d && !w && !s && !a)
            newAnim = runRightPtr;
        else
            newAnim = runForwardPtr; // fallback
    }

    // Switch animation only if changed
    if (newAnim != currentAnimPtr)
    {
        currentAnimPtr = newAnim;
        animatorPtr->PlayAnimation(newAnim);
    }

    // Shooting (press J or Left Mouse)
    static bool shootPressedLastFrame = false;
    bool jPressed = glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS;
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool shootPressed = jPressed || mousePressed;

    if (shootPressed && !shootPressedLastFrame)
    {
        // Aim where the camera looks
        glm::vec3 forward = glm::normalize(glm::vec3(camera.Front.x, camera.Front.y, camera.Front.z));

        Bullet bullet;
        bullet.position = characterPosition + glm::vec3(-0.1f, 0.8f, 0.0f);
        bullet.direction = forward;
        bullet.speed = BULLET_SPEED;
        bullet.life = BULLET_LIFETIME;
        bullets.push_back(bullet);

        if (soundManager) soundManager->playGunShot();
    }

    shootPressedLastFrame = shootPressed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    // Rotate camera (not character)
    characterYaw -= xoffset;
    cameraYaw -= xoffset;
    cameraPitch += yoffset;

    if (cameraPitch > 45.0f)
        cameraPitch = 45.0f;
    if (cameraPitch < -45.0f)
        cameraPitch = -45.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// Precise AABB test using world-space bullet position and target's local bbox scaled/translated to world
bool bulletHitsTarget(const Bullet& bullet, const Target& target)
{
    // Compute world AABB for the target
    glm::vec3 minWorld = target.position + target.bboxMin * target.modelScale;
    glm::vec3 maxWorld = target.position + target.bboxMax * target.modelScale;

    // Simple point-in-AABB test for bullet position
    const glm::vec3& p = bullet.position;
    return (p.x >= minWorld.x && p.x <= maxWorld.x) &&
        (p.y >= minWorld.y && p.y <= maxWorld.y) &&
        (p.z >= minWorld.z && p.z <= maxWorld.z);
}

void cleanupTargets()
{
    for (auto& t : targets) {
        if (t.animator) {
            delete t.animator;
            t.animator = nullptr;
        }
    }
    targets.clear();
}
