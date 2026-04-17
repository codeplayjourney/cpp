#include <string>
#include <iostream>

class Log {
    public:
        enum class Level {
            Trace, Debug, Info, Warning, Error, Critical
        };

        static void log(Level level, const std::string& message) {
            // Implementation of logging based on the level
        }
    private:
        Level m_level = Level::Info; // Default log level
    public:
        void Info(const std::string& message) {
            if (m_level <= Level::Info) {
                std::cout << "[INFO]: " << message << std::endl;
            }
        }
};

int main() {
    Log logger;
    logger.Info("This is an info message.");
    return 0;
}