//
// Created by jul3x on 21.04.20.
//

#include <algorithm>

#include <R3E/system/Logger.h>

#include <R3E/j3x/Parser.h>
#include <R3E/j3x/J3XVisitor.h>
#include <R3E/j3x/J3X.h>
#include <R3E/utils/Numeric.h>


namespace r3e::j3x {
    std::shared_ptr<J3XVisitor> parseWithVisitor(const std::string& filename, const std::string& ns)
    {
        auto visitor = std::make_shared<J3XVisitor>(ns);
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
                parse_tree->accept(visitor.get());
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

        return visitor;
    }

    std::shared_ptr<Parameters> parse(const std::string& filename, const std::string& ns)
    {
        return std::make_shared<Parameters>(parseWithVisitor(filename, ns)->getParams());
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
            if (content.empty())
                return std::make_shared<Parameters>();

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
            throw std::logic_error("[J3X] Error while parsing content from string.\n"
                                   "Error message: " + std::string(e.what()) + "!\n"
                                                                               "Parsed content: " + content);
        }

        return std::make_shared<Parameters>(visitor.getParams());
    }

    void mergeParams(Parameters& params, const Parameters& new_params, bool force_update)
    {
        if (!force_update)
            params.insert(new_params.begin(), new_params.end());
        else
        {
            for (const auto& param : new_params)
            {
                params[param.first] = param.second;
            }
        }
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
        out += getType(obj) + " " + variable + " = ";
        serialize(obj, out);
        out += "\n";
    }

    std::string getType(const Obj& obj)
    {
        if (obj.type() == typeid(j3x::List))
        {
            return "list";
        }
        else if (obj.type() == typeid(int))
        {
            return "int";
        }
        else if (obj.type() == typeid(float))
        {
            return "float";
        }
        else if (obj.type() == typeid(bool))
        {
            return "bool";
        }
        else if (obj.type() == typeid(sf::Vector2f))
        {
            return "vector";
        }
        else if (obj.type() == typeid(std::string))
        {
            return "string";
        }
        else
        {
            throw std::logic_error("[j3x::getType] Not handled data type: " + std::string(obj.type().name()));
        }
    }

    bool compare(const j3x::Obj& a, const j3x::Obj& b)
    {
        if (a.type() != b.type())
            return false;

        if (a.type() == typeid(j3x::List))
        {
            throw std::logic_error("[j3x::compare] List comparison not handled yet");
        }
        else if (a.type() == typeid(int))
        {
            return j3x::getObj<int>(a) == j3x::getObj<int>(b);
        }
        else if (a.type() == typeid(float))
        {
            return utils::num::isNearlyEqual(j3x::getObj<float>(a), j3x::getObj<float>(b));
        }
        else if (a.type() == typeid(bool))
        {
            return j3x::getObj<bool>(a) == j3x::getObj<bool>(b);
        }
        else if (a.type() == typeid(sf::Vector2f))
        {
            return utils::num::isNearlyEqual(j3x::getObj<sf::Vector2f>(a), j3x::getObj<sf::Vector2f>(b));
        }
        else if (a.type() == typeid(std::string))
        {
            return j3x::getObj<std::string>(a) == j3x::getObj<std::string>(b);
        }
        else
        {
            throw std::logic_error("[j3x::compare] Not handled data type: " + std::string(a.type().name()));
        }
    }

} // namespace r3e::j3x
