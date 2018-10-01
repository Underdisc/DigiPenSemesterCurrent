#include "CommandLineMenu.h"

#include <iostream>

bool is_whitespace(char c)
{
    return (c == '\r' || c == '\n' || c == ' ' || c == '\t');
}

CommandLineMenu::CommandLineMenu()
{
    _menu_length = 0;
}

void CommandLineMenu::Display()
{
    // Create the header and footer string.
    std::string header_footer;
    header_footer.push_back('+');
    for(unsigned i = 0; i < (_menu_length - 2); ++i)
    {
        header_footer.push_back('-');
    }
    header_footer.push_back('+');

    // Print out the command menu.
    std::cout << header_footer << std::endl;
    for(int i = 0; i < (int)_menu_options.size(); ++i)
    {
        const std::vector<std::string> & usage = _menu_options[i].usage;
        for(const std::string & arg : usage)
        {
            std::cout << arg << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << header_footer << std::endl;
}

void CommandLineMenu::Select()
{
    std::string complete_command;
    std::getline(std::cin, complete_command);
    std::vector<std::string> args;
    GetArgs(&args, complete_command);

    if(args.size() == 0)
    {
        return;
    }

    for(const MenuOption & option : _menu_options)
    {
        if(args[0] == option.usage[0])
        {
            std::cout << "+-----< output >-----+" << std::endl;
            option.function(args);
            return;
        }
    }
}

void CommandLineMenu::AddOption(
    const std::string & usage, 
    void (*function)(const std::vector<std::string> &))
{

    MenuOption new_option;
    GetArgs(&new_option.usage, usage);
    new_option.function = function;
    _menu_options.push_back(new_option);

    // Update the menu length if this usage label happens to be the largest.
    unsigned usage_len = 0;
    for(const std::string & arg : new_option.usage)
    {
        usage_len += arg.size();
    }
    usage_len += new_option.usage.size() - 1;
    if(_menu_length < usage_len)
    {
        _menu_length = usage_len;
    }
}

void CommandLineMenu::GetArgs(std::vector<std::string> * args,
                              const std::string & command)
{
    bool in_whitespace = true;
    bool encountered_arg = false;
    unsigned arg_start = 0;
    for(unsigned i = 0; i < command.size(); ++i)
    {
        char c = command[i];
        if(in_whitespace && !is_whitespace(c))
        {
            arg_start = i;
            in_whitespace = false;
            encountered_arg = true;
        }
        else if(!in_whitespace && is_whitespace(c))
        {
            args->push_back(command.substr(arg_start, i - arg_start));
            in_whitespace = true;
        }
    }

    if(!in_whitespace && encountered_arg)
    {
        args->push_back(command.substr(arg_start));
    }
}

