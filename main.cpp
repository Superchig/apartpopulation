#include <iostream>
#include <filesystem>
#include <map>
#include <stack>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"
#include "text_renderer.h"
#include "table.h"
#include "game.h"
#include "check_error.h"
#include "texture_2d.h"
#include "quad_renderer.h"
#include "button.h"
#include "historical_figure.h"
#include "util.h"
#include "land_plot.h"
#include "world_grid.h"

constexpr int MIGRATE_FAMILIES = 150;
constexpr int HALF_FAMILIES = 50;

Game Game::main;

float eyeChange(float eyeZ) { return 10.0f * Game::main.zoomFactor; }

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void advanceMonthCallback(Button *button);
void advanceYearCallback(Button *button);
void cursorCallback(GLFWwindow *window, double xPos, double yPos);
void advanceMonth();
void syncPopTable(Table &popTable);

void transferOrbiters(FamilyNode *source, FamilyNode *destination);
void advanceFigureBirths(HistoricalFigure *figure);
void advanceFamilyBirths(FamilyNode *familyNode);
void deleteFamily(FamilyNode *family);
void deleteRecursively(FamilyNode *family);

template<class T> void transferContents(std::vector<T> source, std::vector<T> destination);
void pushIfValid(std::array<LandPlot *, 8> &candidates, int &size, int x, int y);

int main()
{
    std::cout << "Hello, world!" << std::endl;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);


#endif

    glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window =
        glfwCreateWindow(Game::main.windowWidth, Game::main.windowHeight, "Apartpopulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << '\n';
        return -1;
    }

#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        std::cout << "Initializing debug!" << std::endl;
        
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

#endif

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);

    // Set projection matrix once outside of render loop
    // -----------------------------------
    Game::main.updateOrtho();

    // Declare camera manipulation variables
    // -------------------------------------
    const float zChange = 10.0f;

    // Load shaders and other resources
    // --------------------------------
    
    Texture2D *whiteTexture = Texture2D::newWhiteTexture();
    
    // Initialize Game::main.quadRenderer before any text renderers
    QuadRenderer quadRenderer{whiteTexture->ID};
    Texture2D logh{"sprites/logh.png", true};
    Texture2D container{"sprites/container.jpg", false};
    Texture2D buttonNormal{"sprites/adwitr-5th-button0.png", true, GL_NEAREST};
    Texture2D cobblestone{"sprites/cobblestone.png", true, GL_NEAREST};
    Texture2D testPlayerTex{"sprites/test-sprite.png", true, GL_NEAREST};
    quadRenderer.textureIDs.push_back(container.ID);
    quadRenderer.textureIDs.push_back(logh.ID);
    quadRenderer.textureIDs.push_back(buttonNormal.ID);
    quadRenderer.textureIDs.push_back(cobblestone.ID);
    quadRenderer.textureIDs.push_back(testPlayerTex.ID);
    Game::main.quadRenderer = &quadRenderer;
    
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS << std::endl;
    
    // Texture2D buttonPressed{"sprites/adwitr-5th-button1", true};

    TextRenderer textRen("fonts/Cantarell-Regular.otf", 32);

    glCheckError();

    Button button{-1,
                  -0.25f * Game::main.windowHeight,
                  34 * 10,
                  10 * 10,
                  &buttonNormal,
                  &textRen,
                  "Pass Month",
                  34 * 10 * 0.25f};
    button.x = (button.width - Game::main.windowWidth) / 2.0f;
    button.onClickStart = advanceMonthCallback;
    Game::main.buttons.push_back(&button);
    
    Button passYearButton{-1,
                          -1,
                          34 * 10,
                          10 * 10,
                          &buttonNormal,
                          &textRen,
                          "Pass Year",
                          34 * 10 * 0.25f};
    passYearButton.x = (passYearButton.width - Game::main.windowWidth) / 2.0f;
    passYearButton.y = button.y - button.height;
    passYearButton.onClickStart = advanceYearCallback;
    Game::main.buttons.push_back(&passYearButton);
    
    // Set up test player.
    // -------------------------------
    Entity *testPlayer = new Entity();
    testPlayer->isShown = true;
    testPlayer->texture = &testPlayerTex;
    Limb *newLeg     = new Limb();
    newLeg->posX       = 0;
    newLeg->posY       = 0;
    newLeg->posZ       = 0;
    newLeg->isShown    = false;
    testPlayer->Add_Limb(newLeg);

    // Set up world map.
    // -------------------------------

    WorldGrid worldGrid{15, 15};
    Game::main.worldGrid = &worldGrid;
    for (int x = 0; x < worldGrid.rows; x++)
    {
        for (int y = 0; y < worldGrid.cols; y++)
        {
            Node *newNode = new Node();
            newNode->m_x = x;
            newNode->m_y = y;
            worldGrid.nodes.push_back(newNode);

            Thing *newFloor = new Thing();
            newFloor->isShown = true;
            newFloor->posX   = x;
            newFloor->posY   = y;
            newFloor->texture = &cobblestone;
            newNode->occupants.push_back(newFloor);
        }
    }
    worldGrid.nodes[0]->occupants.push_back(testPlayer);

    // Set up initial population
    // -------------------------------
    
    // Create land grid
    Grid landGrid{&textRen, 15, 15};
    Game::main.landGrid = &landGrid;
    
    for (int i = 0; i < 20; i++)
    {
        LandPlot &plot = Game::main.landGrid->plot(0, 0);

        HistoricalFigure *father = new HistoricalFigure(18);
        HistoricalFigure *mother = new HistoricalFigure(18);
        FamilyNode *family = new FamilyNode(nullptr, father, &plot, nullptr);

        family->head = father;

        father->sex = Sex::Male;
        father->spouse = mother;
        father->family = family;

        mother->sex = Sex::Female;
        mother->spouse = father;
        mother->family = family;
        
        Game::main.livingFigures.push_back(father);
        Game::main.livingFigures.push_back(mother);
        
        plot.rootFamilies.push_back(family);
    }
    
    // for (int i = 0; i < 20; i++)
    // {
    //     Game::main.livingFigures.push_back(new HistoricalFigure(18));
    // }

    // Initialize and set up table
    // ---------------------------
    Table spreadTable(-200.0f, 300.0f, Game::main.livingFigures.size() + 1,
                      &textRen);
    Game::main.spreadTable = &spreadTable;
    spreadTable.isActive = false;
    spreadTable.yDownLength = 0.85f * Game::main.windowHeight;
    spreadTable.setColWidth(0, 200);
    spreadTable.setColWidth(1, 100);
    spreadTable.setColWidth(2, 100);
    spreadTable.setColWidth(3, 200);
    spreadTable.setColWidth(4, 200);
    spreadTable.setItem(0, 0, "Name");
    spreadTable.setItem(0, 1, "Age");
    spreadTable.setItem(0, 2, "Sex");
    spreadTable.setItem(0, 3, "Spouse");
    spreadTable.setItem(0, 4, "Plot");

    syncPopTable(spreadTable);

    glCheckError();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glCheckError();

    Button tableScroll{-300.0f, 0.0f, 20, 40, &container, &textRen};
    spreadTable.scrollButton    = &tableScroll;
    tableScroll.isActive = false;
    // testMoveButton.y         = spreadTable.yPos - testMoveButton.height / 2.0f;
    Game::main.buttons.push_back(&tableScroll);
    
    while (!glfwWindowShouldClose(window))
    {
        // Handle user input, when not with callback
        // -----------------------------------------
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
        {
            // Game::main.eyeZ -= zChange;
            // std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
            Game::main.zoomFactor -= 0.01f;

            Game::main.updateOrtho();
        }
        else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
        {
            // Game::main.Game::main.eyeZ += zChange;
            // std::cout << "Game::main.eyeZ: " << Game::main.eyeZ << std::endl;
            Game::main.zoomFactor += 0.01f;

            Game::main.updateOrtho();
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            Game::main.zoomFactor = 1.0f;
            Game::main.eyeX = 0.0f;
            Game::main.eyeY = 0.0f;

            Game::main.updateOrtho();
        }
        else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            Game::main.zoomFactor = 1.0f;
            Game::main.eyeX = 0.0f;
            Game::main.eyeY = -20000.0f;

            Game::main.updateOrtho();
        }

        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            Game::main.eyeY += eyeChange(Game::main.eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        {
            Game::main.eyeY -= eyeChange(Game::main.eyeZ);
        }

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            Game::main.eyeX -= eyeChange(Game::main.eyeZ);
        }
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            Game::main.eyeX += eyeChange(Game::main.eyeZ);
        }

        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
        {
            spreadTable.yOffset -= 10.0f;
            std::cout << "yOffset: " << spreadTable.yOffset << std::endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
        {
            spreadTable.yOffset += 10.0f;
            std::cout << "yOffset: " << spreadTable.yOffset << std::endl;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                spreadTable.isActive = false;
                spreadTable.scrollButton->isActive = false;

                Game::main.landGrid->isActive = false;

                passYearButton.isActive = false;
                button.isActive         = false;

                worldGrid.isActive      = true;
            }
            else
            {
                spreadTable.isActive = true;
                spreadTable.scrollButton->isActive = true;

                Game::main.landGrid->isActive = false;

                passYearButton.isActive = true;
                button.isActive         = true;

                worldGrid.isActive      = false;
            }
        }

        glm::vec3 eye    = glm::vec3(Game::main.eyeX, Game::main.eyeY, Game::main.eyeZ);
        glm::vec3 center = eye + glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
        Game::main.view  = glm::lookAt(eye, center, up);
        
        const float halfWindowHeight = Game::main.windowHeight * Game::main.zoomFactor * 0.5f;
        const float halfWindowWidth = Game::main.windowWidth * Game::main.zoomFactor * 0.5f;
        Game::main.topY = Game::main.eyeY + halfWindowHeight;
        Game::main.bottomY = Game::main.eyeY - halfWindowHeight;
        Game::main.rightX = Game::main.eyeX + halfWindowWidth;
        Game::main.leftX = Game::main.eyeX - halfWindowWidth;

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        double xPos;
        double yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        std::stringstream sstream;
        
        const double xNDC = (xPos / (Game::main.windowWidth / 2.0f)) - 1.0f;
        const double yNDC = 1.0f - (yPos / (Game::main.windowHeight / 2.0f));
        
        // Calculate the world position of the mouse
        // -----------------------------------------
        // NOTE: This math doesn't work with perspective projection matrices, but it does
        // work with an orthographic projection matrix
        glm::mat4 VP = Game::main.projection * Game::main.view;
        glm::mat4 VPinv = glm::inverse(VP);
        glm::vec4 mouseClip = glm::vec4((float)xNDC, (float)yNDC, 1.0f, 1.0f);
        glm::vec4 worldMouse = VPinv * mouseClip;
        // std::cout << "worldMouse: " << worldMouse.x << ", " << worldMouse.y << ", " << worldMouse.z << ", " << worldMouse.w << std::endl;
        sstream.str("");
        sstream << "worldMouse: " << worldMouse.x << ", " << worldMouse.y << ", " << worldMouse.z << ", " << worldMouse.w;
        textRen.renderText(sstream.str(), -640.0f, 0.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        Game::main.deltaMouseX = worldMouse.x - Game::main.mouseX;
        Game::main.deltaMouseY = worldMouse.y - Game::main.mouseY;
        Game::main.mouseX = worldMouse.x;
        Game::main.mouseY = worldMouse.y;
        
        // Display simulation info
        // -----------------------
        // This month has just happened
        const std::string date = "Year: " + std::to_string(Game::main.date.year)
            + ", month: " + std::to_string(Game::main.date.month);
        textRen.renderText(date, -640.0f, Game::main.windowHeight / 2.0f - 32.0f * 3.0f,
                           1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        const std::string popCount = "Living nobles: " + std::to_string(Game::main.livingFigures.size());
        textRen.renderText(popCount, -640.0f, Game::main.windowHeight / 2.0f - 32.0f * 4.0f,
                           1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        const std::string marriageEligible = "Marriage eligible: " + std::to_string(Game::main.marriageEligible);
        textRen.renderText(marriageEligible, -640.0f, Game::main.windowHeight / 2.0f - 32.0f * 5.0f,
                           1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        if (landGrid.isActive)
        {
            landGrid.draw();
        }

        if (worldGrid.isActive)
        {
            worldGrid.draw();
        }
        
        if (spreadTable.isActive)
        {
            spreadTable.sendToRenderer();
        }
        
        for (Button *b : Game::main.buttons)
        {
            if (b->isActive)
            {
                b->sendToRenderer();
            }
        }
        
        // Game::main.quadRenderer->prepareQuad(glm::vec2(-500.0f, 0.0f), 200.0f, 200.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), logh.ID);
        
        Game::main.quadRenderer->sendToGL();
        Game::main.quadRenderer->resetBuffers();
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        glCheckError();
    }
    
    // Deallocate and free
    // -------------------
    for (const HistoricalFigure *figure : Game::main.livingFigures)
    {
        delete figure;
    }
    
    for (const HistoricalFigure *figure : Game::main.deadFigures)
    {
        delete figure;
    }
    
    for (LandPlot &plot : Game::main.landGrid->land)
    {
        for (FamilyNode *family : plot.rootFamilies)
        {
            deleteRecursively(family);
        }
    }
    
    delete whiteTexture;

    glfwTerminate();
    return 0;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        for (Button *button : Game::main.buttons)
        {
            if (button->isActive && button->hasInBounds(Game::main.mouseX, Game::main.mouseY))
            {
                button->isClicked = true;

                // std::cout << "Left mouse pressed a BUTTON at " << Game::main.mouseX
                //           << ", " << Game::main.mouseY << std::endl;
                
                if (button->onClickStart != nullptr)
                {
                    button->onClickStart(button);
                }
                break;
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        for (Button *button : Game::main.buttons)
        {
            button->isClicked = false;

            if (button->hasInBounds(Game::main.mouseX, Game::main.mouseY)) 
            {
                if (button->onClickStop != nullptr)
                {
                    button->onClickStop(button);
                }
                break;
            }
        }
    }
}

void advanceMonthCallback(Button *button) { advanceMonth(); }

void advanceYearCallback(Button *button)
{
    for (int i = 0; i < 12; i++)
    {
        advanceMonth();
    }
}

void advanceMonth()
{
    // Update the actual month
    // -----------------------
    if (Game::main.date.month >= DEC)
    {
        Game::main.date.month = JAN;
        Game::main.date.year++;
    }
    else
    {
        Game::main.date.month = (Month)(Game::main.date.month + 1);
    }

    // Advance the simulation
    // ----------------------

    // Celebrate birthday if necessary
    for (HistoricalFigure *figure : Game::main.livingFigures)
    {
        if (figure->birthDay.month == Game::main.date.month &&
            figure->birthDay.year != Game::main.date.year)
        {
            figure->age++;
        }    
    }

    // Give births when it applies
    // Make a copy of the living figures because main.livingFigures may be modified
    // by the act of giving birth
    std::vector<HistoricalFigure *> maybeBirthsFigures =
        Game::main.livingFigures;
    for (HistoricalFigure *figure : maybeBirthsFigures)
    {
        advanceFigureBirths(figure);
    }

    Game::main.marriageEligible = 0;
    for (LandPlot &plot : Game::main.landGrid->land)
    {
        #ifndef NDEBUG
        for (FamilyNode *rootFamily : plot.rootFamilies)
        {
            if (rootFamily->leader != nullptr) 
            {
                std::cout << "ERROR::ROOT_FAMILY: Its leader is not null!" << std::endl;
            }
        }
        #endif
        
        // std::vector<FamilyNode *> emancipatedFamilies;
        std::vector<HistoricalFigure *> plotFigures;
        plot.gatherPopulationInto(plotFigures);

        // Check if figures should die
        // ---------------------------
        for (HistoricalFigure *figure : plotFigures)
        {
            HistoricalFigure *spouse = figure->spouse;
            FamilyNode *      family = figure->family;

            if (figure->age >= 80)
            {
                std::cout << figure->name
                          << " has died of old age. May they rest in peace."
                          << std::endl;

                removeItem(Game::main.livingFigures, figure);
                Game::main.deadFigures.push_back(figure);
                figure->isAlive = false;

                if (spouse != nullptr)
                {
                    spouse->spouse = nullptr; 
                }

                if (figure == family->head)
                {
                    // Emancipate spouse and orbiters
                    std::cout << "\tSince they were the head of their family, "
                                 "their dependents will now be "
                                 "emancipated."
                              << std::endl;

                    if (spouse != nullptr)
                    {
                        FamilyNode *emancipated =
                            new FamilyNode(nullptr, spouse, family->plot, family->clan);
                        plot.rootFamilies.push_back(emancipated);

                        std::cout
                            << "\t" << spouse->name
                            << ", after the death of their spouse, is now "
                               "emancipated into their own, new family!"
                            << std::endl;
                    }

                    for (FamilyNode *orbiter : family->orbit)
                    {
                        std::cout << "\t" << orbiter->head->name << "'s orbiting family is now emancipated!" << std::endl;

                        orbiter->leader = nullptr;
                        plot.rootFamilies.push_back(orbiter);
                        // emancipatedFamilies.push_back(orbiter);
                    }

                    deleteFamily(family); // Hoo boy
                }
            }
        }
        
        // Obtain new vector of figures, since some may have died
        plotFigures.clear();
        plot.gatherPopulationInto(plotFigures);
        
        std::stack<HistoricalFigure *> eligibleMales;
        std::stack<HistoricalFigure *> eligibleFemales;
        for (HistoricalFigure *figure : plotFigures)
        {
            if (figure->age < 18 || figure->spouse != nullptr)
            {
                continue;
            }
            
            switch (figure->sex)
            {
                case Sex::Male:
                    eligibleMales.push(figure);
                    break;
                case Sex::Female:
                    eligibleFemales.push(figure);
                    break;
            }
        }

        // Marry all possible couples
        while (!eligibleMales.empty() && !eligibleFemales.empty())
        {
            HistoricalFigure *husband = eligibleMales.top();
            HistoricalFigure *wife    = eligibleFemales.top();
            eligibleMales.pop();
            eligibleFemales.pop();

            FamilyNode *wFamily = wife->family;

            std::cout << husband->name << " and " << wife->name
                      << " are happily married!" << std::endl;

            husband->spouse = wife;
            wife->spouse    = husband;
            wife->family    = husband->family;

            transferOrbiters(wFamily, husband->family);
            if (wFamily->leader != nullptr)
            {
                std::cout << "\tThe wife leaves her previous family node, upper-leader was: " << wFamily->leader->head->name << "." << std::endl;
                removeItem(wFamily->leader->orbit, wFamily);
            }

            deleteFamily(wFamily); // Hoo boy
        }

        Game::main.marriageEligible +=
            eligibleMales.size() + eligibleFemales.size();
            
        // Migrate to another plot, if necessary
        // -------------------------------------
        plotFigures.clear();
        plot.gatherPopulationInto(plotFigures);
        
        if (plotFigures.size() > 150)
        {
            LandPlot *nextPlot = nullptr;
            const int dieRoll = randInRange(1, 10);
            if (dieRoll == 1)
            {
                std::vector<LandPlot *> candidates;

                constexpr int reach = 5;
                int leftBound = std::max(0, plot.mapX - reach);
                int rightBound = std::min(Game::main.landGrid->cols - 1, plot.mapX + reach);
                int topBound = std::min(Game::main.landGrid->rows - 1, plot.mapY + reach);
                int bottomBound = std::max(0, plot.mapY - reach);
                for (int row = bottomBound; row <= topBound; row++)
                {
                    for (int col = leftBound; col <= rightBound; col++)
                    {
                        LandPlot &candidate = Game::main.landGrid->plot(col, row);
                        if (candidate.calcPopSize() < MIGRATE_FAMILIES)
                        {
                            candidates.push_back(&Game::main.landGrid->plot(col, row));
                        }
                    }
                }

                if (!candidates.empty())
                {
                    nextPlot = candidates[randInRange(0, candidates.size() - 1)];
                }
            }
            else
            {
                std::array<LandPlot *, 8> candidates;
                int size = 0;
                pushIfValid(candidates, size, plot.mapX - 1, plot.mapY + 1);
                pushIfValid(candidates, size, plot.mapX,     plot.mapY + 1);
                pushIfValid(candidates, size, plot.mapX + 1, plot.mapY + 1);
                pushIfValid(candidates, size, plot.mapX + 1, plot.mapY    );
                pushIfValid(candidates, size, plot.mapX + 1, plot.mapY - 1);
                pushIfValid(candidates, size, plot.mapX,     plot.mapY - 1);
                pushIfValid(candidates, size, plot.mapX - 1, plot.mapY - 1);
                pushIfValid(candidates, size, plot.mapX - 1, plot.mapY    );

                if (size > 0)
                {
                    const int randIndex = randInRange(0, size - 1);
                    nextPlot = candidates[randIndex];
                }
            }

            if (nextPlot == nullptr)
            {
                std::cout << "Families in " << plot.getCoords() << " have nowhere to go, but death does not exist." << std::endl;
            }
            else
            {
                for (int i = 0; i < plot.rootFamilies.size() / 3; i++)
                {
                    const int index = randInRange(0, plot.rootFamilies.size() - 1);
                    FamilyNode *removed = plot.rootFamilies[index];
                    plot.rootFamilies.erase(std::begin(plot.rootFamilies) + index);

                    removed->propagateRelocation(nextPlot);
                    nextPlot->rootFamilies.push_back(removed);
                }

                std::cout << "In search of more elite jobs, some nobles from plot "
                              << plot.getCoords() << " move to plot " << nextPlot->getCoords() << "." << std::endl;
            }
        }
    }

    // Resize the population UI table's internal vector as needed
    if (Game::main.livingFigures.size() >= Game::main.spreadTable->data.size())
    {
        Game::main.spreadTable->data.resize(
            Game::main.spreadTable->data.size() * 2);
    }
    
    // Update table UI
    // ---------------
    syncPopTable(*Game::main.spreadTable);
}

void syncPopTable(Table &popTable)
{
    // Empty the table's current info first
    for (int i = 1; i < popTable.data.size(); i++)
    {
        for (int j = 0; j < popTable.data[0].size(); j++)
        {
            popTable.setItem(i, j, "");
        }
    }

    for (int i = 0; i < Game::main.livingFigures.size(); i++)
    {
        const HistoricalFigure *figure = Game::main.livingFigures[i];

        popTable.setItem(i + 1, 0, figure->name);
        popTable.setItem(i + 1, 1, std::to_string(figure->age));
        popTable.setItem(i + 1, 2, sexStrings[figure->sex]);
        popTable.setItem(i + 1, 3, figure->getSpouseName());
        popTable.setItem(i + 1, 4, figure->family->plot->getCoords());
    }
}

void deleteFamily(FamilyNode *family)
{
    std::cout << "\tDeleting family headed by " << family->head->name << "." << std::endl;
    
    if (family->leader == nullptr) // Root family
    {
        removeItem(family->plot->rootFamilies, family);
    }
    delete family;
}

void deleteRecursively(FamilyNode *family)
{
    for (FamilyNode *orbiter : family->orbit)
    {
        deleteRecursively(orbiter);
    }
    delete family;
}

template<class T> void transferContents(std::vector<T> source, std::vector<T> destination)
{
    while (!source.empty())
    {
        destination.push_back(source.pop_back());
    }
}

void transferOrbiters(FamilyNode *source, FamilyNode *destination)
{
    while (!source->orbit.empty())
    {
        FamilyNode *orbiter = source->orbit[source->orbit.size() - 1];
        source->orbit.pop_back();

        orbiter->leader = destination;
        destination->orbit.push_back(orbiter);
    }
}

void advanceFigureBirths(HistoricalFigure *figure)
{
    HistoricalFigure *spouse = figure->spouse;
    FamilyNode *      family = figure->family;

    if (figure->age >= 18)
    {
        if (figure->spouse != nullptr && figure->kids.size() < figure->desiredKids)
        {
            HistoricalFigure *newKid = new HistoricalFigure(0);
            Game::main.livingFigures.push_back(newKid);

            std::cout << figure->name << " and " << spouse->name
                      << " have a new child: " << newKid->name << "."
                      << std::endl;

            figure->kids.push_back(newKid);
            spouse->kids.push_back(newKid);

            family->orbit.push_back(new FamilyNode(family, newKid, family->plot, family->clan));
        }
    }
}

void advanceFamilyBirths(FamilyNode *familyNode)
{
    HistoricalFigure *head   = familyNode->head;
    HistoricalFigure *spouse = familyNode->head->spouse;

    advanceFigureBirths(familyNode->head);
    if (spouse != nullptr)
    {
        advanceFigureBirths(spouse);
    }
    for (FamilyNode *orbiter : familyNode->orbit)
    {
        advanceFamilyBirths(orbiter);
    }
}

void getAllFigures(std::vector<HistoricalFigure *> &figures, FamilyNode *familyNode)
{
    figures.push_back(familyNode->head);
    if (familyNode->head->spouse != nullptr)
    {
        figures.push_back(familyNode->head->spouse);
    }
    for (FamilyNode *orbiter : familyNode->orbit)
    {
        getAllFigures(figures, orbiter); 
    }
}

void pushIfValid(std::array<LandPlot *, 8> &candidates, int &size, int x, int y)
{
    const bool coordsAreValid = 0 <= x && x < Game::main.landGrid->cols
                             && 0 <= y && y < Game::main.landGrid->rows;
    if (coordsAreValid)
    {
        LandPlot *candidate = &Game::main.landGrid->plot(x, y);

        if (candidate->calcPopSize() < HALF_FAMILIES)
        {
            candidates[size] = candidate;
            size++;
        }
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        Game::main.spreadTable->isActive = !Game::main.spreadTable->isActive;
        Game::main.spreadTable->scrollButton->isActive = !Game::main.spreadTable->scrollButton->isActive;
        Game::main.landGrid->isActive = !Game::main.landGrid->isActive;
    }
}

void cursorCallback(GLFWwindow *window, double xPos, double yPos)
{
    // TODO: Calculate when mouse enters UI plot of land

    if (Game::main.buttons[0]->hasInBounds(Game::main.mouseX, Game::main.mouseY))
    {
        std::cout << "In bounds of a button!" << std::endl;
    }
}
