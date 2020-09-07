//
// Created by jul3x on 21.04.20.
//

#include <utility>

#include <R3E/j3x/Parser.h>
#include <R3E/j3x/J3XVisitor.h>
#include <R3E/j3x/J3X.h>


namespace r3e::j3x {

    Parameters parse(const std::string &filename, const std::string &ns)
    {
        J3XVisitor visitor(ns);
        FILE *input = fopen(filename.c_str(), "r");

        if (input)
        {
            std::cout << "[J3X] Started parsing of " + filename + " file." << std::endl;

            try
            {
                Script *parse_tree = pScript(input);
                parse_tree->accept(&visitor);
                delete parse_tree;
            }
            catch (const std::exception& e)
            {
                throw std::logic_error("[J3X] Error while reading " + filename + " file.\nError message: " + e.what() + "!\n");
            }

            fclose(input);
        }
        else
        {
            throw std::logic_error("[J3X] J3X " + filename + " file not found!\n");
        }

        return visitor.getParams();
    }

    Parameters parse(const std::string &filename)
    {
        return parse(filename, "");
    }

    void mergeParams(Parameters& params, const Parameters& new_params)
    {
        params.insert(new_params.begin(), new_params.end());
    }

} // namespace r3e::j3x
