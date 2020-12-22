#ifndef STDE_LOG_LOG_HPP_
#define STDE_LOG_LOG_HPP_

#include <map>
#include <string>
#include <ctime>

#include "../streams/tee.hpp"

namespace stde {
    namespace log {

        enum class level : int8_t {
            debug = 0, info = 1, warning = 2, error = 3, fatal = 4
        };

        class log {
        public:

            virtual ~log();

            static log& get(const std::string& name);
            static void clean();

            std::ostream& operator<<(const level l);

        private:
            log(const std::string& name);

            static inline const std::string level_name(level l) {
                switch (l) {
                    case level::debug:
                        return "DBUG";
                    case level::info:
                        return "INFO";
                    case level::warning:
                        return "WARN";
                    case level::error:
                        return "ERR!";
                    case level::fatal:
                        return "FTL!";
                    default:
                        return "NULL";
                }
            }

            std::string m_name;

            static std::string logfile();
            static std::string logtime() {
                time_t rawtime;
                struct tm *info;
                char buffer[32];

                time(&rawtime);

                info = localtime(&rawtime);

                strftime(buffer, 32, "%d/%m/%Y %H:%M:%S", info);

                return std::string(buffer);
            }

            static std::ofstream m_logfile;
            static stde::streams::tee_ostream m_outstream;
            static std::map<std::string, log*> m_loggers;
            static bool m_init;

        };

    }
}

#endif
