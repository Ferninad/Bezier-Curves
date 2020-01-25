#include "common.h"
#include "cmath"
#include "vector"
#include "string"

bool Init();
void CleanUp();
void Run();
void Draw();
vector<double> Bezier(vector<vector<double>> points);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int numPoints = 3;
double t = 0;
double limit = 1;
int mx;
int my;

vector<vector<double>> basePoints;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    for(int i = 0; i < numPoints; i++){
        basePoints.push_back({static_cast<double>(rand() % screenWidth), static_cast<double>(rand() % screenHeight)});
    }

    while (gameLoop)
    {   
        SDL_GetMouseState(&mx, &my);
        limit = 1 * static_cast<double>(mx) / screenWidth;
        Draw();
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_UP:
                        numPoints++;
                        basePoints.clear();
                        for(int i = 0; i < numPoints; i++){
                            basePoints.push_back({static_cast<double>(rand() % screenWidth), static_cast<double>(rand() % screenHeight)});
                        }
                        break;
                    case SDLK_DOWN:
                        numPoints--;
                        if(numPoints < 2)
                            numPoints = 2;
                        basePoints.clear();
                        for(int i = 0; i < numPoints; i++){
                            basePoints.push_back({static_cast<double>(rand() % screenWidth), static_cast<double>(rand() % screenHeight)});
                        }
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

vector<double> Bezier(vector<vector<double>> points){
    if(points.size() == 1)
        return {points[0][0], points[0][1]};
    else{
        vector<vector<double>> newPoints;
        for(int i = 0; i < points.size() - 1; i++){
            double changex = points[i+1][0] - points[i][0];
            double changey = points[i+1][1] - points[i][1];
            double ang = atan2(changey, changex);
            double distance = sqrt(pow(changex, 2) + pow(changey, 2));

            distance = distance * t;
            double x = points[i][0] + distance * cos(ang);
            double y = points[i][1] + distance * sin(ang);
            newPoints.push_back({x, y});
        }
        return Bezier(newPoints);
    }
}

void Draw(){
    for(int i = 0; i < basePoints.size(); i++){
        pos.x = basePoints[i][0] - 1;
        pos.y = basePoints[i][1] - 1;
        pos.w = 3;
        pos.h = 3;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    while(t < limit){
        t += .001;
        vector<double> point = Bezier(basePoints);
        pos.x = round(point[0]);
        pos.y = round(point[1]);
        pos.w = 1;
        pos.h = 1;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
    t = 0;
}