function configure_file(input, output, replacements)
    local file = io.open(input, "r")
    assert(file, "Failed to open input file: " .. input)
    local content = file:read("*all")
    file:close()

    for key, value in pairs(replacements) do
        content = content:gsub("@" .. key .. "@", value)
    end

    local out = io.open(output, "w")
    assert(out, "Failed to open output file: " .. output)
    out:write(content)
    out:close()
end
