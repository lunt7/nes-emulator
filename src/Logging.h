#ifndef _LOGGING_H
#define _LOGGING_H

#include <fstream>

#ifdef LOGGING_ENABLED
    #define LOG_INIT(filename) Logging::Instance().Init(filename)
    #define LOG_DEBUG(expr) Logging::Instance().Log(expr)
#else
    #define LOG_INIT(filename)
    #define LOG_DEBUG(expr)
#endif

class Logging {
public:
    static Logging& Instance(void) {
        static Logging instance_;
        return instance_;
    }

    ~Logging() = default;

    void Init(const char* filename) {
        if (stream_.is_open()) {
            stream_.close();
        }
        stream_.open(filename);
        filename_ = filename;
    }

    template<typename T>
    void Log(T const& v) {
        stream_ << v << std::endl;
    }

private:
    // singleton
    Logging() = default;
    Logging(const Logging&) = delete;
    Logging(Logging&&) = delete;
    Logging& operator=(const Logging&) = delete;
    Logging& operator=(Logging&&) = delete;

    std::ofstream stream_;
    std::string filename_;
};

#endif
