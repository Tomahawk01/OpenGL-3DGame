#include "config.h"

#include "Core/File.h"
#include "Utilities/Error.h"

#include <ranges>
#include <string_view>

using namespace std::literals;

namespace {

    std::string FindValue(std::string_view key)
    {
        const auto file = Game::File{ ".config" };
        const auto config = file.AsString();

        auto lines = config | std::views::split("\r\n"sv);

        for (const auto& line : lines)
        {
            const auto lineView = std::string_view{ line };
            const auto delim = lineView.find_first_of('=');
            Game::Ensure(delim != std::string::npos, "Invalid config line: {}", lineView);

            const auto currentKey = std::string_view(lineView.data(), delim);
            const auto currentValue = std::string_view(lineView.data() + delim + 1, lineView.length() - delim - 1);

            if (currentKey == key)
            {
                return std::string(currentValue.data(), currentValue.data() + currentValue.size());
            }
        }

        throw Game::Exception("Missing {} from config", key);
    }

}

namespace Game::Config {

    bool LoggingEnabled()
    {
        return FindValue("LoggingEnabled"sv) == "true"sv;
    }

    bool OpenGLDebugEnabled()
    {
        return FindValue("OpenGLDebugEnabled"sv) == "true"sv;
    }

    bool IsFullscreen()
    {
        return FindValue("IsFullscreen"sv) == "true"sv;
    }

}
