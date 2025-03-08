#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <cmath>
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SIZE = 32;
const int ZOMBIE_SIZE = 32;
const int BULLET_SIZE = 5;
const float PLAYER_SPEED = 1.0;
const float BULLET_SPEED = 0.5;
const float ZOMBIE_SPEED = 0.05;

// ============================ Định nghĩa Cấu Trúc ============================
// Định nghĩa đạn (Bullet)
struct Bullet {
    float x, y, dx, dy;
    bool active = true;

    Bullet(float x, float y, float dx, float dy) : x(x), y(y), dx(dx), dy(dy) {}

    void update() {
        x += dx * BULLET_SPEED;
        y += dy * BULLET_SPEED;
    }
};

// Định nghĩa zombie
struct Zombie {
    float x, y;
    bool alive = true;

    Zombie(float _x, float _y) {
        x = _x;
        y = _y;
    }

    void update(float px, float py) {
        float dx = px - x, dy = py - y;
        float length = sqrt(dx * dx + dy * dy);
        if (length > 0) {
            x += (dx / length) * ZOMBIE_SPEED;
            y += (dy / length) * ZOMBIE_SPEED;
        }
    }
};
bool checkCollision(float x1, float y1, int w1, int h1,
                    float x2, float y2, int w2, int h2) {
    return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

// ============================ Hàm Di Chuyển Nhân Vật ============================
void movePlayer(float &player_x, float &player_y, const Uint8* keys) {
    if (keys[SDL_SCANCODE_W]) player_y -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_S]) player_y += PLAYER_SPEED;
    if (keys[SDL_SCANCODE_A]) player_x -= PLAYER_SPEED;
    if (keys[SDL_SCANCODE_D]) player_x += PLAYER_SPEED;

    // Giữ nhân vật trong màn hình
    if (player_x < 0) player_x = 0;
    if (player_x > SCREEN_WIDTH - PLAYER_SIZE) player_x = SCREEN_WIDTH - PLAYER_SIZE;
    if (player_y < 0) player_y = 0;
    if (player_y > SCREEN_HEIGHT - PLAYER_SIZE) player_y = SCREEN_HEIGHT - PLAYER_SIZE;
}
    void checkBulletZombieCollision(std::vector<Bullet> &bullets, std::vector<Zombie> &zombies) {
        for (size_t i = 0; i < bullets.size(); i++) {
        for (size_t j = 0; j < zombies.size(); j++) {
            if (bullets[i].active && zombies[j].alive &&
                checkCollision(bullets[i].x, bullets[i].y, BULLET_SIZE, BULLET_SIZE,
                               zombies[j].x, zombies[j].y, ZOMBIE_SIZE, ZOMBIE_SIZE)) {
                bullets[i].active = false;  // Xóa đạn
                zombies[j].alive = false;   // Xóa zombie

            }
        }
    }
}
// ============================ Hàm Chính (Main) ============================
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Zombie Survival", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    float player_x = SCREEN_WIDTH / 2, player_y = SCREEN_HEIGHT / 2;
    std::vector<Bullet> bullets;
    std::vector<Zombie> zombies = { Zombie(100, 100), Zombie(700, 500) ,Zombie(200 , 200) , Zombie(300 , 400) };

    bool running = true;
    SDL_Event event;

    while (running) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // Bắn đạn khi nhấn chuột
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                float dx = event.button.x - player_x, dy = event.button.y - player_y;
                float length = sqrt(dx * dx + dy * dy);
                if (length > 0) {
                    bullets.push_back(Bullet(player_x, player_y, dx / length, dy / length));
                }
            }
        }

        // Nhận input từ bàn phím
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        movePlayer(player_x, player_y, keys);

        // Cập nhật đạn
        for (auto &bullet : bullets) bullet.update();

        // Cập nhật zombie
        for (auto &zombie : zombies) zombie.update(player_x, player_y);
        checkBulletZombieCollision(bullets, zombies);
        // Xóa đạn đã bắn trúng
        for(int i = bullets.size() -1; i >= 0 ; i--){
            if(!bullets[i].active) {
                bullets.erase(bullets.begin() + i);
            }
        }
        for(int i = zombies.size() -1; i >= 0 ; i--){
            if(!zombies[i].alive){
                zombies.erase(zombies.begin() + i);
            }
        }
        // ============================ Vẽ Game ============================
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Vẽ nhân vật (màu xanh)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect playerRect = { (int)player_x, (int)player_y, PLAYER_SIZE, PLAYER_SIZE };
        SDL_RenderFillRect(renderer, &playerRect);

        // Vẽ zombie (màu đỏ)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto &zombie : zombies) {
            SDL_Rect zombieRect = { (int)zombie.x, (int)zombie.y, ZOMBIE_SIZE, ZOMBIE_SIZE };
            SDL_RenderFillRect(renderer, &zombieRect);
        }

        // Vẽ đạn (màu vàng)
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (auto &bullet : bullets) {
            SDL_Rect bulletRect = { (int)bullet.x, (int)bullet.y, BULLET_SIZE, BULLET_SIZE };
            SDL_RenderFillRect(renderer, &bulletRect);
        }

        SDL_RenderPresent(renderer);
    }

    // Dọn dẹp SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
