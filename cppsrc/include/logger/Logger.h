#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <fstream>
#include <string>
#include <chrono>

// Use the namespace you want
namespace logger {


    class FileLogger {

    public:

        // If you can´t/dont-want-to use C++11, remove the "class" word after enum
        enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_MESSAGE };

        // ctor (remove parameters if you don´t need them)
        explicit FileLogger(const std::string& info, const char* fname = "logger.txt", unsigned max_lines_count = 2)
            : numWarnings(0U)
            , numErrors(0U)
            , max_lines(max_lines_count)
        {
            time(&_now);
            myFile.open(fname, std::ofstream::out | std::ofstream::trunc);

            // Write the first lines
            if (myFile.is_open()) {
                myFile << "Log file info: " << info << std::endl;
                myFile << "Log file created" << std::endl << std::endl;
            } // if
            _start = myFile.tellp();
        }

        // dtor
        ~FileLogger() {

            if (myFile.is_open()) {
                myFile << std::endl << std::endl;

                // Report number of errors and warnings
                myFile << numWarnings << " warnings" << std::endl;
                myFile << numErrors << " errors" << std::endl;

                myFile.close();
            } // if

        }

        std::string PrepTime() {
            time(&_now);
            struct tm* t;
            t = localtime(&_now);
            std::string s, m, h, D, M, Y;
            s = std::to_string(t->tm_sec);
            m = std::to_string(t->tm_min);
            h = std::to_string(t->tm_hour);
            D = std::to_string(t->tm_mday);
            M = std::to_string(t->tm_mon + 1);
            Y = std::to_string(t->tm_year + 1900);
            if (t->tm_sec < 10) s = "0" + s;
            if (t->tm_min < 10) m = "0" + m;
            if (t->tm_hour < 10) h = "0" + h;
            if (t->tm_mday < 10) D = "0" + D;
            if (t->tm_mon + 1 < 10) M = "0" + M;

            std::string res =
                "[" + Y + "-" + M + "-" + D + "T" + h + ":" + m + ":" + s + "] ";

            return res;
        }

        // Overload << operator using log type
        friend FileLogger& operator << (FileLogger& logger, const e_logType l_type) {
            ++logger.lines;
            switch (l_type) {
            case logger::FileLogger::e_logType::LOG_ERROR:
                logger.myFile << logger.PrepTime() << "[ERROR] :\t";
                ++logger.numErrors;
                break;

            case logger::FileLogger::e_logType::LOG_WARNING:
                logger.myFile << logger.PrepTime() << "[WARNING] :\t";
                ++logger.numWarnings;
                break;

            case logger::FileLogger::e_logType::LOG_MESSAGE:
                logger.myFile << logger.PrepTime() << "[MESSAGE] :\t";
                break;

            default:
                logger.myFile << logger.PrepTime() << "[INFO] :\t";
                break;
            } // sw
            if (logger.lines > logger.max_lines) {
                //logger.myFile.seekp(logger._start, std::ios_base::beg);
                //std::string toDelete;
                //std::getline(logger.myFile, toDelete);
                /*logger.myFile << "kek";
                logger.myFile.seekp(0, std::ios_base::end);*/
                --logger.lines;
            }
            return logger;
        }

        // Overload << operator using C style strings
        // No need for std::string objects here
        friend FileLogger& operator << (FileLogger& logger, const std::string& text) {

            logger.myFile << text << std::endl;
            return logger;
        }

        // Make it Non Copyable (or you can inherit from sf::NonCopyable if you want)
        FileLogger(const FileLogger&) = delete;
        FileLogger& operator= (const FileLogger&) = delete;

    private:
        const unsigned max_lines;
        unsigned lines = 0;
        unsigned _start = 0;
        std::fstream myFile;
        time_t _now;

        unsigned int numWarnings;
        unsigned int numErrors;

    }; // class end

}  // namespace


#endif // FILELOGGER_HPP