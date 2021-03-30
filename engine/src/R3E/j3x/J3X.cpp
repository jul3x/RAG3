//
// Created by jul3x on 21.04.20.
//

#include <algorithm>

#include <R3E/system/Logger.h>

#include <R3E/j3x/Parser.h>
#include <R3E/j3x/J3XVisitor.h>
#include <R3E/j3x/J3X.h>


namespace r3e::j3x {

    std::shared_ptr<Parameters> parse(const std::string& filename, const std::string& ns)
    {
        J3XVisitor visitor(ns);
        FILE* input = fopen(filename.c_str(), "r");

        if (input)
        {
            LOG.info("[J3X] Started parsing of " + filename + " file.");

            try
            {
                Script* parse_tree = pScript(input);
                if (!parse_tree)
                {
                    throw std::logic_error("parse error");
                }
                parse_tree->accept(&visitor);
                delete parse_tree;
            }
            catch (const std::exception& e)
            {
                throw std::logic_error(
                        "[J3X] Error while reading " + filename + " file.\nError message: " + e.what() + "!\n");
            }

            fclose(input);
        }
        else
        {
            throw std::logic_error("[J3X] J3X " + filename + " file not found!\n");
        }

        return std::make_shared<Parameters>(visitor.getParams());
    }

    std::shared_ptr<Parameters> parse(const std::string& filename)
    {
        return parse(filename, "");
    }

    std::shared_ptr<Parameters> parseContent(const std::string& content)
    {
        J3XVisitor visitor;
        try
        {
            Script* parse_tree = pScript(content.c_str());
            if (!parse_tree)
            {
                throw std::logic_error("parse error");
            }
            parse_tree->accept(&visitor);
            delete parse_tree;
        }
        catch (const std::exception& e)
        {
            throw std::logic_error("[J3X] Error while parsing content from string.\nError message: " + std::string(e.what()) + "!\n");
        }

        return std::make_shared<Parameters>(visitor.getParams());
    }

    void mergeParams(Parameters& params, const Parameters& new_params)
    {
        params.insert(new_params.begin(), new_params.end());
    }

    void serialize(const j3x::List& data, std::string& out)
    {
        out += "[";

        for (auto& elem : data)
        {
            serialize(elem, out);
            out += j3x::DELIMITER_;
        }

        if (!data.empty())
            out.pop_back();

        out += "]";
    }

    void serialize(const j3x::Obj& data, std::string& out)
    {
        if (data.type() == typeid(j3x::List))
        {
            serialize(getObj<j3x::List>(data), out);
        }
        else if (data.type() == typeid(int))
        {
            auto obj = j3x::getObj<int>(data);
            out += std::to_string(obj);
        }
        else if (data.type() == typeid(float))
        {
            auto obj = j3x::getObj<float>(data);
            std::ostringstream out_stream;
            out_stream.precision(2);
            out_stream << std::fixed << obj;
            out += out_stream.str();
        }
        else if (data.type() == typeid(bool))
        {
            auto obj = j3x::getObj<bool>(data);
            out += (obj ? "true" : "false");
        }
        else if (data.type() == typeid(sf::Vector2f))
        {
            auto& vec = j3x::getObj<sf::Vector2f>(data);
            std::ostringstream out_stream;
            out_stream.precision(2);
            out_stream << std::fixed << "(" << vec.x << ", " << vec.y << ")";
            out += out_stream.str();
        }
        else if (data.type() == typeid(std::string))
        {
            auto str = j3x::getObj<std::string>(data);

            while (true)
            {
                auto pos = str.find('\n');
                if (pos == std::string::npos)
                    break;

                str.replace(pos, 1, "\\n");
            }

            out += "\"" + str + "\"";
        }
        else
        {
            throw std::logic_error("[j3x::serialize] Not handled data type: " + std::string(data.type().name()));
        }
    }

    void serializeAssign(const std::string& variable, const j3x::Obj& obj, std::string& out)
    {
        if (obj.type() == typeid(j3x::List))
        {
            out += "list ";
        }
        else if (obj.type() == typeid(int))
        {
            out += "int ";
        }
        else if (obj.type() == typeid(float))
        {
            out += "float";
        }
        else if (obj.type() == typeid(bool))
        {
            out += "bool ";
        }
        else if (obj.type() == typeid(sf::Vector2f))
        {
            out += "vector ";
        }
        else if (obj.type() == typeid(std::string))
        {
            out += "string ";
        }
        else
        {
            throw std::logic_error("[j3x::serialize] Not handled data type: " + std::string(obj.type().name()));
        }

        out += variable + " = ";
        serialize(obj, out);
        out += "\n";
    }

} // namespace r3e::j3x
