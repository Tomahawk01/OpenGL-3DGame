#include "config.h"

#include "Core/File.h"
#include "Utilities/Error.h"

#include <ranges>
#include <string_view>

using namespace std::literals;

namespace Game::Config {

    bool LoggingEnabled()
    {
        const auto file = File{ ".config" };
        const auto config = file.AsString();

        auto lines = config | std::views::split("\n"sv);
        
        for (const auto& line : lines)
        {
            const auto lineView = std::string_view{ line };
            const auto delim = lineView.find_first_of('=');
            Ensure(delim != std::string::npos, "Invalid config line: {}", lineView);

            const auto key = std::string_view(lineView.data(), delim);
            const auto value = std::string_view(lineView.data() + delim + 1, lineView.length() - delim - 1);

            if (key == "LoggingEnabled"sv)
            {
                return value == "true"sv;
            }
        }

        throw Exception("Missing LoggingEnabled from config");
    }

}
