#!/usr/bin/env lua

-- Read file path from command line argument
local var_name = arg[1]
local file_path = arg[2]
if not file_path then
    print("Usage: lua <var-name> <file_path>")
    os.exit(1)
end

local function file_to_c_code()
    local file = io.open(file_path, "rb")
    if not file then
        print("Error: Could not open file.")
        return
    end

    local bytes = {}
    for byte in file:lines(1) do
        table.insert(bytes, string.format("0x%02X", string.byte(byte)))
    end
    file:close()

    -- Generate C code
    local num_bytes = #bytes
    local c_code = string.format("uin64_t %s_LENGTH = %d;\n", var_name, num_bytes)
    c_code = c_code .. string.format("uin8_t %s[] = {\n", var_name)

    for i = 1, num_bytes do
        c_code = c_code .. bytes[i]
        if i % 8 == 0 or i == num_bytes then
            c_code = c_code .. ",\n"
        else
            c_code = c_code .. ", "
        end
    end

    c_code = c_code .. "};\n"
    
    return c_code
end

local c_code = file_to_c_code()
if c_code then
    print(c_code)
end
