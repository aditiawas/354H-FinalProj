#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <string>

Fl_Window* window;
Fl_Menu_Bar* menuBar;
Fl_Choice* dropdown;
Fl_Button* processButton;
Fl_Text_Display* resultDisplay;
Fl_Value_Slider* slider;

// Callback function for the "About" menu item
void aboutCallback(Fl_Widget* widget, void* data) {
    fl_open_uri("https://github.com/aditiawas/354H-FinalProj");
}

// Function to perform the operation on the selected file
std::string performOperation(const std::string& filename, int option, int sliderValue) {
    // Placeholder for operation logic
    return filename + " - Option: " + std::to_string(option) + " - Slider Value: " + std::to_string(sliderValue);
}

// Callback function for the "Open File" menu item
void openFileCallback(Fl_Widget* widget, void* data) {
    // Specify the file types you want to allow
    const char* fileTypes = "*.txt"; // Change to your desired file extension
    Fl_File_Chooser chooser(".", fileTypes, Fl_File_Chooser::SINGLE, "Open File");
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
        std::string result = performOperation(filename, option, sliderValue);
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

    // Create the process button
    processButton = new Fl_Button(350, 60, 100, 25, "Process");

    // Create the text display to show the result
    resultDisplay = new Fl_Text_Display(10, 100, 780, 480);

    // Create the slider
    slider = new Fl_Value_Slider(10, 590, 780, 30, "Slider Value");
    slider->type(FL_HOR_NICE_SLIDER);
    slider->range(1, 10);
    slider->step(1);
    slider->value(5);

    // Set the callbacks for the buttons and slider
    processButton->callback(processButtonCallback, window);

    window->resizable(resultDisplay);
    window->end();
    window->show();
    return Fl::run();
}