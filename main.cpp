#include <iostream>
#include <filesystem>
#include <map>
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
#include "easy_rand.h"
#include "land_plot.h"

Game Game::main;

float eyeChange(float eyeZ) { return 10.0f * Game::main.zoomFactor; }

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void advanceMonthCallback(Button *button);
void advanceYearCallback(Button *button);
void advanceMonth();
void syncPopTable(Table &popTable);

HistoricalFigure *createSettler(LandPlot *plot);
void moveToPart(HistoricalFigure *figure, Family *target);
void fMoveToPart(Family *family, LandPlot *target);
void deleteFamily(Family *family);

template<class T> void removeItem(std::vector<T> vec, T item);

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
    quadRenderer.textureIDs.push_back(container.ID);
    quadRenderer.textureIDs.push_back(logh.ID);
    quadRenderer.textureIDs.push_back(buttonNormal.ID);
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
    
    // Set up initial population
    // -------------------------------
    
    // Create land grid
    Grid landGrid{&textRen, 10, 10};
    Game::main.landGrid = &landGrid;
    
    for (int i = 0; i < 10; i++)
    {
        Family *family = new Family();
        HistoricalFigure *father = new HistoricalFigure(18);
        HistoricalFigure *mother = new HistoricalFigure(18);
        
        family->head = father;

        moveToPart(father, family);
        moveToPart(mother, family);
        
        father->sex = Sex::Male;
        mother->sex = Sex::Female;
        father->spouse = mother;
        mother->spouse = father;
        
        Game::main.livingFigures.push_back(father);
        Game::main.livingFigures.push_back(mother);
        
        Game::main.landGrid->plot(0, 0).families.push_back(family);
        family->plot = &Game::main.landGrid->plot(0, 0);
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
    spreadTable.setItem(0, 4, "Family Head");
    
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
        // Handle user input
        // -----------------
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
        
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                spreadTable.isActive = false;
                spreadTable.scrollButton->isActive = false;
            }
            else
            {
                spreadTable.isActive = true;
                spreadTable.scrollButton->isActive = true;
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
        
        landGrid.draw();
        
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
        for (Family *family : plot.families)
        {
            delete family;
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

    for (int i = 0; i < Game::main.livingFigures.size(); i++)
    {
        HistoricalFigure *figure = Game::main.livingFigures[i];
        // std::cout << figure.name << "'s birth day: year " <<
        // figure.birthDay.year << ", month " << figure.birthDay.month <<
        // std::endl;

        // Advance birthday if necessary
        if (figure->birthDay.month == Game::main.date.month &&
            figure->birthDay.year != Game::main.date.year)
        {
            figure->age++;
            // std::cout << figure->name << " celebrates their birthday, turning
            // "
            //           << figure.age << "." << std::endl;
        }

        if (figure->spouse != nullptr)
        {
            HistoricalFigure *spouse = figure->spouse;

            // DEBUG: Check that marriages are perfectly 1-to-1
            if (figure != figure->spouse->spouse)
            {
                HistoricalFigure *thirdSpouse = figure->spouse->spouse;
                std::cout << "ERROR: " << figure->name << " is married to "
                          << spouse->name << ", who is married to "
                          << thirdSpouse->name << std::endl;
            }

            // Have kids
            if (figure->kids.size() < figure->desiredKids)
            {
                int dieRoll = randInRange(1, 48);
                if (dieRoll == 1)
                {
                    HistoricalFigure *newKid = new HistoricalFigure(0);
                    newKid->parent1          = figure;
                    newKid->parent2          = spouse;
                    moveToPart(newKid, figure->family);

                    figure->kids.push_back(newKid);
                    spouse->kids.push_back(newKid);
                    Game::main.livingFigures.push_back(newKid);
                    
                    std::cout << "newKid's name: " << newKid->name << std::endl;
                }
            }
        }

        // Possibly die
        if (figure->age >= 80)
        {
            int dieRoll = randInRange(1, 48);
            
            // Definitely die
            // TODO: Check if the family effects are correct
            // TODO: Branch families out upon the death of the head
            if (dieRoll == 1)
            {
                std::cout << figure->name
                          << " has died of old age. May they rest in peace."
                          << std::endl;
            
                auto figureIt = Game::main.livingFigures.begin() + i;
                Game::main.livingFigures.erase(figureIt);
                Game::main.deadFigures.push_back(figure);
                figure->isAlive = false;
                
                if (figure->spouse != nullptr)
                {
                    figure->spouse->spouse = nullptr;
                }
                
                // Select new family head if necessary
                if (figure == figure->family->head)
                {
                    std::cout << "\tThey were the head of their family. Now succession shall occur." << std::endl;
                    
                    HistoricalFigure *eldestLivingMember = nullptr;
                    int maxAge = -1;
                    for (HistoricalFigure *member : figure->family->members)
                    {
                        if (member->isAlive && member->age > maxAge && member != figure && member != figure->spouse)
                        {
                            eldestLivingMember = member;
                            maxAge = member->age;
                        }
                    }
                    
                    if (eldestLivingMember == nullptr)
                    {
                        HistoricalFigure *eldestHeir = nullptr;
                        maxAge = -1;
                        for (HistoricalFigure *member : figure->family->members)
                        {
                            if (member != figure && member->isAlive && member->age > maxAge)
                            {
                                eldestHeir = member;
                                maxAge = member->age;
                            }
                        }
                        
                        // There are no eligible heirs, so the family is destroyed
                        if (eldestHeir == nullptr)
                        {
                            auto &families = figure->family->plot->families;
                            auto familyIt = std::find(std::begin(families), std::end(families), figure->family);
                            families.erase(familyIt);
                            // delete figure->family;
                            deleteFamily(figure->family);
                            
                            std::cout << "\tWith no viable heirs, the family crumbles!" << std::endl;
                        }
                        else
                        {
                            figure->family->head = eldestHeir;
                            
                            std::cout << "\tThe eldest member of the family, " << eldestHeir->name << ", inherits!" << std::endl;
                        }
                    }
                    else if (eldestLivingMember->age >= 18)
                    {
                        // TODO: Verify that the family double-references check out
                        
                        // Create the family for the eldest living member
                        Family *firstFamily = new Family();
                        firstFamily->head = eldestLivingMember;
                        fMoveToPart(firstFamily, figure->family->plot);
                        
                        moveToPart(eldestLivingMember, firstFamily);
                        if (eldestLivingMember->spouse != nullptr)
                        {
                            moveToPart(eldestLivingMember->spouse, firstFamily);
                        }
                        
                        // Create the families for the other living kids
                        // TODO: Consider using a more tree-like structure for families?
                        std::cout << "\tMoved family members: " << std::endl;
                        for (HistoricalFigure *member : figure->family->members)
                        {
                            if (member == figure || !member->isAlive) // No need to move the family head himself, since he's dead
                            {
                                continue;
                            }
                            std::cout << "\t\tConsidered: " << member->name;
                            
                            if (member == figure->spouse)
                            {
                                std::cout << ", spouse moves to new family of the eldest";
                                moveToPart(member, firstFamily);
                            }
                            else if (member == eldestLivingMember)
                            {
                                std::cout << ", split into new family as the eldest";
                            }
                            else if (member->age >= 18)
                            {
                                Family *youngFamily = new Family();
                                youngFamily->head = member;
                                fMoveToPart(youngFamily, figure->family->plot);
                                
                                if (member->spouse == eldestLivingMember)
                                {
                                    std::cout << ", spouse of eldest so moves to their new family";
                                    
                                    moveToPart(member, firstFamily);
                                }
                                else if (member->sex == Sex::Male)
                                {
                                    std::cout << ", split into new family";
                                    
                                    moveToPart(member, youngFamily);
                                    
                                    if (member->spouse != nullptr)
                                    {
                                        std::cout << ", brought their spouse (" << member->spouse->name << ")";
                                        
                                        moveToPart(member->spouse, youngFamily);
                                    }
                                }
                                else if (member->sex == Sex::Female)
                                {
                                    if (member->spouse == nullptr)
                                    {
                                        std::cout << ", unmarried, so entered family of eldest member." << std::endl;
                                        
                                        moveToPart(member, firstFamily);
                                    }
                                    else
                                    {
                                        std::cout << ", married female so probably moved with husband";
                                    }
                                }
                                
                                // TODO: Check that family references check out
                            }
                            else
                            {
                                std::cout << ", moved into eldest living member's new family b/c minor (age " << member->age << ")";
                                
                                moveToPart(member, firstFamily);
                            }
                            
                            std::cout << std::endl;
                            
#ifndef NDEBUG
                            if (member->family == figure->family && member != figure)
                            {
                                std::cout << "\tERROR::FAMILY_SUCCESSION: " << member->name << " still has a pointer to their original family!" << std::endl;
                            }
#endif
                        } 
                        
                        auto &families = figure->family->plot->families;
                        auto familyIt = std::find(std::begin(families), std::end(families), figure->family);
                        families.erase(familyIt);
                        // delete figure->family;
                        deleteFamily(figure->family);
                        
                        std::cout << "\tWith their family head gone, the family splits off into new, smaller families, with "
                            << eldestLivingMember->name << " retaining the best claim to the family name as its eldest child." << std::endl;
                    }
                    else if (figure->spouse != nullptr)
                    {
                        figure->family->head = figure->spouse;
                        
                        std::cout << "\tWith no adult children, the spouse, " << figure->spouse->name
                                  << ", becomes the new head of the family." << std::endl;
                    }
                    else
                    {
                        figure->family->head = eldestLivingMember; // Do this even if the kid is not an adult
                        
                        std::cout << "\tEven though there are no adult children, the oldest of the kids, "
                                  << eldestLivingMember->name << ", steps up to the plate." << std::endl;
                    }
                }
            }
        }
    }

    // Resize the population UI table's internal vector as needed
    if (Game::main.livingFigures.size() >= Game::main.spreadTable->data.size())
    {
        Game::main.spreadTable->data.resize(
            Game::main.spreadTable->data.size() * 2);
    }

    // Find everyone eligible for marriage up-front
    std::vector<HistoricalFigure *> marriageEligible;
    for (HistoricalFigure *figure : Game::main.livingFigures)
    {
        if (figure->age >= 18 && figure->spouse == nullptr)
        {
            marriageEligible.push_back(figure);
        }
    }
    // std::cout << "marriageEligible.size(): " << marriageEligible.size() <<
    // std::endl;
    Game::main.marriageEligible = marriageEligible.size();
    for (int i = 0; i < marriageEligible.size(); i++)
    {
        HistoricalFigure *figure = marriageEligible[i];
        HistoricalFigure *spouse = nullptr;
        // auto figureIterator = marriageEligible.begin() + i;
        auto spouseIterator =
            marriageEligible.begin(); // Used for removing from vector

        for (auto it = marriageEligible.begin() + i;
             it != marriageEligible.end(); it++)
        {
            HistoricalFigure *possibleSpouse = *it;
            if (possibleSpouse->sex != figure->sex)
            {
                spouse         = possibleSpouse;
                spouseIterator = it;
                break;
            }
        }

        // Marry the happy couple
        if (spouse != nullptr)
        {
            std::cout << figure->name << " and " << spouse->name
                      << " become happily married!" << std::endl;
            
            figure->spouse = spouse;
            spouse->spouse = figure;
            
            HistoricalFigure *husband = nullptr;
            HistoricalFigure *wife = nullptr;
            if (figure->sex == Sex::Male)
            {
                husband = figure;
                wife = spouse;
            }
            else
            {
                husband = spouse;
                wife = figure;
            }
            
            // Handle how the marriage affects the couple's families
            auto extraFamily = wife->family;
            auto extraHead   = wife->family->head;
            if (wife->family->head == wife)
            {
                // Merge the wife's family into the husband's family
                for (HistoricalFigure *member : wife->family->members)
                {
                    moveToPart(member, husband->family);
                }
                
                auto &families = wife->family->plot->families;
                auto familyIt = std::find(std::begin(families), std::end(families), wife->family);
                families.erase(familyIt);
                // delete wife->family;
                deleteFamily(wife->family);
                
                std::cout << "\tThe newlywed wife, once the head of her family, moves in with her husband, resulting in a merger." << std::endl;
            }
            else
            {
                auto wifeIt = std::find(wife->family->members.begin(), wife->family->members.end(), wife);
                if (wifeIt == wife->family->members.end())
                {
                    std::cout << "ERROR::wife: " << wife->name << " is not in her family, which is strange." << std::endl;
                }
                wife->family->members.erase(wifeIt);
                husband->family->members.push_back(wife);
                
                std::cout << "\tThe newlywed wife moves to her husband's family." << std::endl;
            }
            
            marriageEligible.erase(spouseIterator);
        }
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
        auto extraFamily = figure->family;
        auto extraHead = figure->family->head;
        if (extraHead == nullptr)
        {
            std::cout << "ERROR::syncPopTable: extraHead is nullptr!" << std::endl;
            std::cout << "\tfigure->name: " << figure->name << std::endl;
        }
        auto extraName = figure->family->head->name;
        popTable.setItem(i + 1, 4, figure->family->head->name);
    }
}

void moveToPart(HistoricalFigure *figure, Family *target)
{
    target->members.push_back(figure);
    figure->family = target;
}


void fMoveToPart(Family *family, LandPlot* target)
{
    target->families.push_back(family);
    family->plot = target;
}

void deleteFamily(Family *family)
{
    std::cout << "\tDeleting family headed by " << family->head->name << "." << std::endl;
    
    delete family;
}

template<class T> void removeItem(std::vector<T> vec, T item)
{
    auto it = std::find(std::begin(vec), std::end(vec), item);
    vec.erase(it);
}
