#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "subdivobjects.h"
#include "loop.h"
#include "catmull.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/glm.hpp>

using namespace std;

Fl_Window* window;
Fl_Menu_Bar* menuBar;
Fl_Choice* dropdown;
Fl_Button* processButton;
Fl_Button* showLimitSurface;
Fl_Text_Display* resultDisplay;
Fl_Value_Slider* slider;

// Callback function for the "About" menu item
void aboutCallback(Fl_Widget* widget, void* data) {
    fl_open_uri("https://github.com/aditiawas/354H-FinalProj");
}

//reading the trimesh face from a file
void performReadTrimesh(const std::string& filename, vector<glm::vec3>& vertices, vector<TrimeshFace>& trimeshFaces) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v") {
            float x, y, z;
            if (iss >> x >> y >> z) {
                vertices.push_back(glm::vec3(x, y, z));
            } else {
                std::cerr << "Invalid vertex format in line: " << line << std::endl;
            }
        } else if (token == "f") {
            int a, b, c;
            if (iss >> a >> b >> c) {
                TrimeshFace tempTrimeshFace(a - 1, b - 1, c - 1);
                trimeshFaces.push_back(tempTrimeshFace);
            } else {
                std::cerr << "Invalid face format in line: " << line << std::endl;
            }
        } else {
            std::cout << "Ignored line: " << line << std::endl;
        }
    }

    file.close();
}

void performReadQuad(const std::string& filename, vector<glm::vec3> &vertices, vector<QuadFace*> &quadfaces )
{   
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v") 
        {
            float x, y, z;
            if (iss >> x >> y >> z) 
            {
                //printf("Vertex: %5.2f %5.2f %5.2f \n",x,y,z);
                vertices.push_back(glm::vec3(x,y,z));
            } 
            else 
            {
                std::cerr << "Invalid vertex format in line: " << line << std::endl;
            }
        } 
        else if (token == "f") 
        {
            int a, b, c, d;
            if (iss >> a >> b >> c >>d) 
            {
                //std::cout << "Face: " << a << ", " << b << ", " << c << ", " <<d << std::endl;
                QuadFace* tempQuadFace = new QuadFace(a-1,b-1,c-1,d-1);
                quadfaces.push_back(tempQuadFace);

            } 
            else 
            {
                std::cerr << "Invalid face format in line: " << line << std::endl;
            }
        } 
        else 
        {
            std::cerr << "Unknown token: " << token << " in line: " << line << std::endl;
        }
    }
    printf("Done with readQuad...");
    file.close();

}

void performReadQuadSharp(const std::string& filename, vector<glm::vec3> &vertices, vector<QuadFace*> &quadfaces, vector<int>& sharpness )
{   
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    int num_vertices = 0;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;
        if (token == "v") 
        {
            float x, y, z;
            char sharp;
            if (iss >> x >> y >> z >> sharp) 
            {
                //printf("Vertex: %5.2f %5.2f %5.2f \n",x,y,z);
                vertices.push_back(glm::vec3(x,y,z));
                num_vertices +=1;
                if(sharp == 's')
                {
                    sharpness.push_back(1);
                    //printf("Sharpness of vertex %d is %d", num_vertices, 1);
                }
                else if(sharp=='n')
                {
                    sharpness.push_back(0);
                    //printf("Sharpness of vertex %d is %d", num_vertices, 0);
                }
                else
                {
                    std::cerr << "Invalid sharp vertex format in line: " << line << std::endl;
                }
                
            } 
            else 
            {
                std::cerr << "Invalid vertex format in line: " << line << std::endl;
            }
        } 
        else if (token == "f") 
        {
            int a, b, c, d;
            if (iss >> a >> b >> c >>d) 
            {
                //std::cout << "Face: " << a << ", " << b << ", " << c << ", " <<d << std::endl;
                QuadFace* tempQuadFace = new QuadFace(a-1,b-1,c-1,d-1);
                quadfaces.push_back(tempQuadFace);

            } 
            else 
            {
                std::cerr << "Invalid face format in line: " << line << std::endl;
            }
        } 
        else 
        {
            std::cerr << "Unknown token: " << token << " in line: " << line << std::endl;
        }
    }
    printf("Done with readQuad...");
    file.close();

}

// Function to perform the operation on the selected file
std::string performOperation(const std::string& filename, int option, int sliderValue, bool limitSurface) {

     string option_str;
     if (option==0) //catmull-clark
     {
        option_str = "Catmull-Clark Algorithm";
     }
     else if(option==1)//loop subdivision
     {
        option_str = "Loop Subdivision Algorithm";
     }
     else if(option==2)
     {
        option_str = "Catmull-Clark Algorithm with Sharp vertices";
     }
    
     string temp = "Option selected was: " + option_str + "\nFilename: " + filename + "\nSlider Value: " + std::to_string(sliderValue) + "\n";

     vector<glm::vec3> vertices;

     if (option==0) //catmull-clark
     {  
        vector <QuadFace*> quadfaces;
        performReadQuad(filename, vertices, quadfaces);
        catmullClark catmullObject = catmullClark(vertices,quadfaces);
        if(limitSurface==1)
        {
            string log = catmullObject.doSubdivisionIteratively(6, filename);
            temp+=log;
        }
        else
        {
            string log = catmullObject.doSubdivisionIteratively(sliderValue, filename);
            temp+=log;
        }
     }
     else if(option==1) //loop subdivision
     {
        vector<TrimeshFace> trimeshfaces;
        performReadTrimesh(filename, vertices, trimeshfaces);
        LoopSubdiv loopObject = LoopSubdiv(vertices, trimeshfaces);
        if(limitSurface==1)
        {
            string log = loopObject.doSubdivision(filename, 6);
            temp += log;
        }
        else
        {
            string log = loopObject.doSubdivision(filename, sliderValue);
            temp += log;
        }
     }
     else if(option==2) //sharp catmull-clark
     {
        vector <QuadFace*> quadfaces;
        vector<int> sharpness;
        performReadQuadSharp(filename, vertices, quadfaces, sharpness);
        catmullClark catmullObject = catmullClark(vertices,quadfaces,sharpness);
        if(limitSurface==1)
        {
            string log = catmullObject.doSubdivisionIteratively(6, filename);
            temp+=log;
        }
        else
        {
            string log = catmullObject.doSubdivisionIteratively(sliderValue, filename);
            temp+=log;
        }
     }
     else
     {
        option_str = "Invalid option";
     }
     //cout << temp;
    return temp;
}

// Callback function for the "Open File" menu item
void openFileCallback(Fl_Widget* widget, void* data) {
    // Specify the file types you want to allow
    const char* fileTypes = ""; // Change to your desired file extension
    Fl_File_Chooser chooser("./assets", fileTypes, Fl_File_Chooser::SINGLE, "Open File");
    chooser.show();

    // If a file is selected, update the window title
    while (chooser.shown()) {
        Fl::wait();
    }

    if (chooser.value()) {
        window->copy_label(chooser.value());
    }
}

// Callback function for the process button
void processButtonCallback(Fl_Widget* widget, void* data) {
    // Get the selected file and option
    const char* filename = window->label();
    int option = dropdown->value();
    int sliderValue = static_cast<int>(slider->value());

    if (strlen(filename) == 0) {
        fl_alert("Please select a file first.");
    } else {
        std::string result = performOperation(filename, option, sliderValue, 0);
        resultDisplay->buffer(new Fl_Text_Buffer());
        resultDisplay->buffer()->text(result.c_str());
    }
}

void displayScene(std::string arg1, int option) {
    // Path to the executable
    std::string executable = "./render";

    // Construct the command string with arguments
    std::string command = executable + " " + arg1 + " 1";

    int result = system(command.c_str());

    // Check the return value
    if (result != 0)
        std::cout << "\nFailed to run the executable." << std::endl;
}

void showLimit(Fl_Widget* widget, void* data) {
    // Get the selected file and option
    /*const char* filename = window->label();
    std::string opfile = filename;
    int option = dropdown->value();

    if (strlen(filename) == 0) {
        fl_alert("Please select a file first.");
    } else {
        opfile = opfile + "_limit.txt";
        displayScene(opfile, option);
    }*/
    const char* filename = window->label();
    int option = dropdown->value();
    int sliderValue = static_cast<int>(slider->value());

    if (strlen(filename) == 0) {
        fl_alert("Please select a file first.");
    } else {
        std::string result = performOperation(filename, option, sliderValue, 1);
        resultDisplay->buffer(new Fl_Text_Buffer());
        resultDisplay->buffer()->text(result.c_str());
    }
}


int main() {
    // Create the main window
    window = new Fl_Window(800, 650, "Graphics Final");

    // Create the menu bar
    menuBar = new Fl_Menu_Bar(0, 0, window->w(), 25);
    Fl_Menu_Item menuItems[] = {
        {"&File", 0, 0, 0, FL_SUBMENU},
        {"&Open File", FL_CTRL + 'o', openFileCallback, window},
        {0},
        {"&Help", 0, 0, 0, FL_SUBMENU},
        {"&About", 0, aboutCallback, nullptr},
        {0},
        {0}
    };
    menuBar->copy(&menuItems[0]);

    // Create the dropdown menu
    dropdown = new Fl_Choice(300, 30, 200, 25, "Options ");
    dropdown->add("Catmull-Clark");
    dropdown->add("Loop Subdivision");
    dropdown->add("Catmull-Clark with Sharp Edges");

    // Create the process button
    processButton = new Fl_Button(270, 60, 100, 25, "Process");

    // Create the limit surface button
    showLimitSurface = new Fl_Button(430, 60, 150, 25, "Show limit surface");

    // Create the text display to show the result
    resultDisplay = new Fl_Text_Display(10, 100, 780, 480);

    // Create the slider
    slider = new Fl_Value_Slider(10, 590, 780, 30, "Number of Iterations");
    slider->type(FL_HOR_NICE_SLIDER);
    slider->range(0, 6);
    slider->step(1);
    slider->value(0);

    // Set the callbacks for the buttons and slider
    processButton->callback(processButtonCallback, window);

    showLimitSurface->callback(showLimit, window);

    window->resizable(resultDisplay);
    window->end();
    window->show();
    return Fl::run();
}