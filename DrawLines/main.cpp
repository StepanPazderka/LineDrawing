//
//  main.cpp
//  DrawLines
//
//  Created by Štěpán Pazderka on 04.09.2023.
//

#include <iostream>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int borderWidth = 4;
const int rows = 30, columns = 30;

typedef struct {
    int x = 0;
    int y = 0;
} playerPos;

char matrix[rows][columns] = { };

int main(int argc, char* argv[])
{
    int block_width = (SCREEN_WIDTH / columns) - (borderWidth/2);
    int block_height = (SCREEN_HEIGHT / rows) - (borderWidth/2);
    
    playerPos player = playerPos();
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT / 2;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { // Initialize SDL video subsystem
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow(
                                          "SDL2 Window",           // Window title
                                          SDL_WINDOWPOS_CENTERED,  // Initial x position
                                          SDL_WINDOWPOS_CENTERED,  // Initial y position
                                          SCREEN_WIDTH,                     // Width
                                          SCREEN_HEIGHT,                     // Height
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
        while (SDL_PollEvent(&event)) {
            switch (event.key.keysym.sym) {
                case SDLK_a:
                    player.x -= 20;
                    break;
                case SDLK_d:
                    player.x += 20;
                    break;
                case SDLK_w:
                    player.y -= 20;
                    break;
                case SDLK_s:
                    player.y += 20;
                    break;
                default:
                    break;
            }
            
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 125, 255, 255, 255);  // Set draw color (white)
        SDL_RenderClear(renderer);  // Clear the renderer
        
        int mouseXposition;
        
        for (int row = 0; row < rows; ++row) {
            for(int column = 0; column < columns; ++column) {
                SDL_Rect squareRect;
                
                SDL_GetMouseState(&mouseXposition, nullptr);
                
                squareRect.x = (block_width + (borderWidth)) * column;  // X position
                squareRect.y = (block_height + borderWidth) * row + (borderWidth/2);  // Y position
                
                if (abs(squareRect.x - player.x) < ((block_width / 2) + borderWidth) && abs(squareRect.y - player.y) < ((block_width / 2) + borderWidth)) {
                    SDL_SetRenderDrawColor(renderer, 15, 25, 125, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 75, 125, 125, 255);
                }
                
                squareRect.w = block_width;  // Width
                squareRect.h = block_height;  // Height
                SDL_RenderFillRect(renderer, &squareRect);
            }
            std::cout << std::endl;
        }
        
        SDL_RenderPresent(renderer);  // Present the renderer
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
