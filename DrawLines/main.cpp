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

char matrix[rows][columns] = { };

int main(int argc, char* argv[])
{
    int block_width = (SCREEN_WIDTH / columns) - (borderWidth/2);
    int block_height = (SCREEN_HEIGHT / rows) - (borderWidth/2);
    
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
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 125, 255, 255, 255);  // Set draw color (white)
        SDL_RenderClear(renderer);  // Clear the renderer
        
        // Set color for the square (for example, red)
        SDL_SetRenderDrawColor(renderer, 75, 125, 125, 255);
        
        for (int row = 0; row < rows; ++row) {
            for(int column = 0; column < columns; ++column) {
                SDL_Rect squareRect;
                squareRect.x = (block_width + (borderWidth)) * column;  // X position
                squareRect.y = (block_height + borderWidth) * row + (borderWidth/2);  // Y position
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
