#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <fstream>
#include <vector>
#include <string>
#include <memory> 
#include "NoteEngine.hpp"

// --- GRAPHICAL INTERFACE LOGIC ---
void sync_callback(void* data) {
    Fl_Browser* b = (Fl_Browser*)data;
    b->clear();
    std::ifstream file("notes.txt");
    std::string line;
    while (std::getline(file, line)) b->add(line.c_str());
    Fl::repeat_timeout(1.0, sync_callback, data);
}

// --- DATA PERSISTENCE ---
void add_note_cb(Fl_Widget*, void* data) {
    auto inputs = (Fl_Input**)data;
    std::ofstream file("notes.txt", std::ios::app);
    file << inputs[0]->value() << " : " << inputs[1]->value() << "\n";
    inputs[0]->value(""); inputs[1]->value("");
}

// --- MAIN ENTRY & CONTROLLER ---
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./main <start|stats|import|export|delete> [args]" << std::endl;
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "start") {
        // --- GUI INSTANTIATION WITH SMART POINTERS ---
        auto win = std::unique_ptr<Fl_Window>(new Fl_Window(400, 400, "SyncShare: Shared Notes"));
    
        Fl_Input* inT = new Fl_Input(100, 20, 280, 30, "Title:");
        Fl_Input* inC = new Fl_Input(100, 60, 280, 30, "Content:");
        Fl_Button* btn = new Fl_Button(100, 100, 280, 30, "Add & Sync");
        Fl_Browser* list = new Fl_Browser(20, 150, 360, 220);
        
        Fl_Input* inputs[] = {inT, inC};
        btn->callback(add_note_cb, inputs);
        Fl::add_timeout(1.0, sync_callback, list);
        
        win->end(); win->show();
        return Fl::run();
    } 
    
    // --- CLI COMMAND MANAGEMENT ---
    else if (cmd == "stats") {
        std::ifstream f("notes.txt");
        int count = 0; std::string l;
        while (std::getline(f, l)) count++;
        std::cout << "Database contains " << count << " notes." << std::endl;
    }
    else if (cmd == "import") {
        if (argc < 3) { std::cerr << "Error: Provide filename." << std::endl; return 1; }
        std::ifstream in(argv[2]); std::ofstream out("notes.txt", std::ios::app);
        std::string l; while (std::getline(in, l)) out << l << "\n";
        std::cout << "Import complete." << std::endl;
    }
    else if (cmd == "export") {
        std::ifstream f("notes.txt"); std::string l;
        while (std::getline(f, l)) std::cout << l << std::endl;
    }
    else if (cmd == "delete") {
        // --- FILE MANIPULATION LOGIC ---
        if (argc < 3) { std::cerr << "Usage: ./main delete <title>" << std::endl; return 1; }
        
        std::ifstream f("notes.txt"); 
        std::vector<std::string> lines; 
        std::string l;
        
        while (std::getline(f, l)) {
            if (l.find(argv[2]) != 0) lines.push_back(l);
        }
        f.close();
        
        std::ofstream out("notes.txt", std::ios::trunc);
        for (const auto& line : lines) out << line << "\n";
        out.close();
        
        std::cout << "Note deleted." << std::endl;
    }
    return 0;
}