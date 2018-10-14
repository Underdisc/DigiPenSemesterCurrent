#ifndef COMMANDLINEMENU_H
#define COMMANDLINEMENU_H

#include <vector>
#include <string>

class CommandLineMenu
{
public:
    CommandLineMenu();
    void Display();
    void Select();
    void AddOption(const std::string & usage, 
                   void (*function)(const std::vector<std::string> &));
    void GetArgs(std::vector<std::string> * args, const std::string & command);
    
    struct MenuOption
    {
        std::vector<std::string> usage;
        void (*function)(const std::vector<std::string> &);
    };
    std::vector<MenuOption> _menu_options;
    unsigned _menu_length;
};

#endif // !COMMANDLINEMENU_H
