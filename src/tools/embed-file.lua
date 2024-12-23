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
        table.insert(bytes, byte)
    end
    file:close()

    -- Generate C code
    local num_bytes = #bytes
    local c_code = ""
    c_code = c_code .. string.format("buffer_t* get_%s_buffer(void) {\n", var_name);
    c_code = c_code .. string.format("  static uint8_t %s[] = {\n    ", var_name)

    local line_comment = ""
    for i = 1, num_bytes do
        local byte_val = string.byte(bytes[i])
        local hex_val = string.format("0x%02X", byte_val)

        c_code = c_code .. hex_val

        -- Build comment string
        if byte_val >= 32 and byte_val <= 126 then
            line_comment = line_comment .. string.char(byte_val)
        else
            line_comment = line_comment .. '.' -- Use '.' for non-printable characters
        end

        if i % 16 == 0 or i == num_bytes then
            c_code = c_code .. ", // " .. line_comment .. "\n    "
            line_comment = "" -- Reset comment
        else
            c_code = c_code .. ", "
        end
    end

    c_code = c_code .. "};\n"
    c_code = c_code .. string.format("  buffer_t* result = make_buffer(%d);\n", num_bytes)
    c_code = c_code .. string.format("  for (int i = 0; i < %d; i++) {\n", num_bytes)
    c_code = c_code .. string.format("    buffer_append_byte(result, %s[i]);\n", var_name)
    c_code = c_code .. string.format("  }\n")
    c_code = c_code .. string.format("  return result;\n")
    c_code = c_code .. string.format("}\n");

    return c_code
end

local c_code = file_to_c_code()
if c_code then
    print(c_code)
end
