//
//  main.cpp
//  DrawLines
//
//  Created by Štěpán Pazderka on 04.09.2023.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include <functional>

struct point{
    int x = 0;
    int y = 0;
};

struct Color {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct gameObject {
    int id = 0;
    int x = 0;
    int y = 0;
    Color color = Color{255, 255, 255};
    std::function<void(gameObject&, const std::vector<SDL_Event>&)> updateClosure;
    
    void executeUpdate(const std::vector<SDL_Event>& events) {
        if (updateClosure) {  // Check if the closure is assigned
            updateClosure(*this, events);  // Execute the closure
        } else {
            std::cout << "Closure not set!\n";
        }
    }
};

class AppSettings {
public:
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 800;
    const int BORDER_WIDTH = 1;
    static const int ROWS = 100, COLUMNS = 100;
};

class Scene {
public:
    int objectCounter = 0;
    
    std::vector<gameObject*> objects;
    
    void add(gameObject* object) {
        object->id = this->objectCounter;
        ++this->objectCounter;
        objects.push_back(object);
    }
};

int main(int argc, char* argv[])
{
    std::vector<SDL_Event> events;
    
    AppSettings appSettings;
    Scene* gameScene = new Scene();
    gameObject* randomObject = new gameObject();
    gameScene->add(randomObject);
    
    gameObject* playerObject = new gameObject();
    playerObject->color = Color{0, 255, 0};
    playerObject->x = appSettings.SCREEN_WIDTH / 2;
    playerObject->y = appSettings.SCREEN_HEIGHT / 2;
    playerObject->updateClosure = [](gameObject& gameObject, std::vector<SDL_Event> events) {
        for(const SDL_Event& event: events) {
            switch (event.key.keysym.sym) {
                case SDLK_a:
                    gameObject.x -= 20;
                    break;
                case SDLK_d:
                    gameObject.x += 20;
                    break;
                case SDLK_w:
                    gameObject.y -= 20;
                    break;
                case SDLK_s:
                    gameObject.y += 20;
                    break;
                default:
                    break;
            }
        }
    };
    gameScene->add(playerObject);
    
    int block_width = (appSettings.SCREEN_WIDTH / appSettings.COLUMNS) - (appSettings.BORDER_WIDTH/2);
    int block_height = (appSettings.SCREEN_HEIGHT / appSettings.ROWS) - (appSettings.BORDER_WIDTH/2);
    
    point player = point();
    player.x = appSettings.SCREEN_WIDTH / 2;
    player.y = appSettings.SCREEN_HEIGHT / 2;
    
    point cursor = point();
    cursor.x = player.x;
    cursor.y = player.y - (block_height * 5);
    
    point reticle = point();
    reticle.x = 0;
    reticle.y = 0;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { // Initialize SDL video subsystem
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow(
                                          "Line Drawing",           // Window title
                                          SDL_WINDOWPOS_CENTERED,  // Initial x position
                                          SDL_WINDOWPOS_CENTERED,  // Initial y position
                                          appSettings.SCREEN_WIDTH,                     // Width
                                          appSettings.SCREEN_HEIGHT,                     // Height
                                          SDL_WINDOW_SHOWN         // Flags (you can use SDL_WINDOW_FULLSCREEN, etc.)
                                          );
    
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 2;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 3;
    }
    
    // Simple main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        const int FPS = 60;
        const int FRAME_DELAY = 1000 / FPS;
        
        Uint32 frameStart = SDL_GetTicks();
        events.clear();
        while (SDL_PollEvent(&event)) {
            events.push_back(event);
            
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 125, 255, 255, 255);  // Set draw color (white)
        SDL_RenderClear(renderer);  // Clear the renderer
        
        int mouseXposition;
        int mouseYposition;
        
        bool drawnCursor = false;
        bool drawnPlayer = false;
        bool drawnReticle = false;
        
        
        // MARK: - MATH: Position for reticle
        float posX = sin(float(player.x - cursor.x)/(appSettings.SCREEN_WIDTH/2));
        reticle.x = player.x + (posX * 100);
        
        float posY = sin(float(player.y - cursor.y)/(appSettings.SCREEN_HEIGHT/2));
        reticle.y = player.y + (posY * 100);
        
        // MARK: - Rendering from scene objects
        for(int object = 0; object < gameScene->objects.size(); object++) {
            gameObject* selectedObject = gameScene->objects[object];
            
            selectedObject->executeUpdate(events);
            
        }
        
        for (int row = 0; row < appSettings.ROWS; ++row) {
            for(int column = 0; column < appSettings.COLUMNS; ++column) {
                SDL_Rect squareRect;
                
                SDL_GetMouseState(&mouseXposition, &mouseYposition);
                cursor.x = mouseXposition;
                cursor.y = mouseYposition;
                
                squareRect.x = (block_width + (appSettings.BORDER_WIDTH)) * column;  // X position
                squareRect.y = (block_height + appSettings.BORDER_WIDTH) * row + (appSettings.BORDER_WIDTH/2);  // Y position
                
                
                //                if ((abs(squareRect.x - player.x) < block_width) && (abs(squareRect.y - player.y) < block_height) && !drawnPlayer) {
                //                    SDL_SetRenderDrawColor(renderer, 15, 25, 125, 255);
                //                    drawnPlayer = true;
                //                } else
                if ((abs(squareRect.x - cursor.x) < block_width) && (abs(squareRect.y - cursor.y) < block_height)&& !drawnCursor) { // Drawing mouse cursor
                    SDL_SetRenderDrawColor(renderer, 255, 25, 125, 255);
                    drawnCursor = true;
                }  else if ((abs(squareRect.x - reticle.x) < block_width) && (abs(squareRect.y - reticle.y) < block_height)&& !drawnReticle) { // Drawing reticle cursor
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                    drawnReticle = true;
                } else {
                    SDL_SetRenderDrawColor(renderer, 75, 125, 125, 255);
                }
                
                // MARK: - Rendering from scene objects
                for(int object = 0; object < gameScene->objects.size(); object++) {
                    gameObject* selectedObject = gameScene->objects[object];
                    
                    if ((abs(squareRect.x - selectedObject->x) < block_width) && (abs(squareRect.y - selectedObject->y) < block_height) && !drawnPlayer) {
                        SDL_SetRenderDrawColor(renderer, selectedObject->color.red, selectedObject->color.green, selectedObject->color.blue, 255);
                    }
                }
                
                squareRect.w = block_width;  // Width
                squareRect.h = block_height;  // Height
                SDL_RenderFillRect(renderer, &squareRect);
            }
            std::cout << std::endl;
        }
        
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        
        if (FRAME_DELAY > frameTime) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
        
        SDL_RenderPresent(renderer);  // Present the renderer
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
