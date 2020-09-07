//
// Created by jul3x on 05.09.20.
//

#include <cstdio>

#include <R3E/j3x/Parser.h>
#include <R3E/j3x/J3XVisitor.h>
#include <R3E/j3x/Absyn.h>

using namespace r3e::j3x;

int main(int argc, char ** argv)
{
    FILE *input;
    if (argc > 1) 
    {
        input = fopen(argv[1], "r");
        if (!input)
        {
            fprintf(stderr, "Error opening input file.\n");
            exit(1);
        }
    }
    else input = stdin;

    Script *parse_tree = pScript(input);
    if (parse_tree)
    {
        printf("\nParse Succesful!\n");

        try {
            J3XVisitor *visitor = new J3XVisitor();
            parse_tree->accept(visitor);
        }
        catch (const std::exception& e) {
            fprintf(stderr, "semantic error: %s\n", e.what());
            return 1;
        }

        printf("Semantic Analysis Succesful!\n");

        return 0;
    }
    return 1;
}

