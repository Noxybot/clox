#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"

static void repl()
{
    clox::vm    vm{{}};
    std::string line;
    for (;;)
    {
        std::cout << "> ";

        if (!std::getline(std::cin, line))
        {
            std::cout << std::endl;
            break;
        }

        // interpret(line);
    }
}
static int run_file(const std::filesystem::path& path)
{
    std::string   source;
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << std::format("Could not open file \"{}\"", path.c_str())
                  << std::endl;
        return 74;
    }
    std::getline(file, source, '\0');
    /*    clox::InterpretResult result = interpret(source);

       if (result == clox::InterpretResult::INTERPRET_COMPILE_ERROR)
           return 65;
       if (result == clox::InterpretResult::INTERPRET_RUNTIME_ERROR)
           return 70; */
    return -1;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        repl();
    }
    else if (argc == 2)
    {
        run_file(argv[1]);
    }
    else
    {
        std::cerr << "Usage: clox [path]" << std::endl;
        return 64;
    }
    return 0;
}
