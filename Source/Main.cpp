#include "Program.h"

int main()
{
    Program* program = Program::GetInstance();
    program->Init();
    program->Run();

    delete program;

    return 0;
}