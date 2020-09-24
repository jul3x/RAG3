//
// Created by jul3x on 24.09.20.
//

#ifndef RAG3_ENGINE_INCLUDE_R3E_SYSTEM_LOGGER_H
#define RAG3_ENGINE_INCLUDE_R3E_SYSTEM_LOGGER_H

#include <iostream>


namespace r3e {
    class Logger {

    public:
        enum class LogStatus {
            NoLogging = 0,
            ErrorOnly = 1,
            Full = 2
        };

        Logger(const Logger&) = delete;

        Logger& operator=(const Logger&) = delete;

        static Logger& getInstance()
        {
            static Logger instance;
            return instance;
        }

        void setLogStatus(LogStatus status)
        {
            status_ = status;
        }

        void error(const std::string& str)
        {
            if (status_ >= LogStatus::ErrorOnly && str != last_message_)
            {
                std::cerr << "[ERROR] " << str << std::endl;
                last_message_ = str;
            }
        }

        void info(const std::string& str)
        {
            if (status_ >= LogStatus::Full && str != last_message_)
            {
                std::cout << "[INFO] " << str << std::endl;
                last_message_ = str;
            }
        }

    private:
        Logger() = default;

        std::string last_message_;
        LogStatus status_{LogStatus::NoLogging};

    };

#define LOG Logger::getInstance()

} // namespace r3e

#endif //RAG3_ENGINE_INCLUDE_R3E_SYSTEM_LOGGER_H
