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
    local c_code = ""
    c_code = c_code .. "#include <c-armyknife-lib.h>\n\n"
    c_code = c_code .. string.format("buffer_t* get_%s_buffer(void) {\n", var_name);

    c_code = c_code .. string.format("  static uint8_t %s[] = {\n    ", var_name)

    for i = 1, num_bytes do
        c_code = c_code .. bytes[i]
        if i % 8 == 0 or i == num_bytes then
            c_code = c_code .. ",\n    "
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
