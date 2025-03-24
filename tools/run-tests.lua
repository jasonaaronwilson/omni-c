#!/usr/bin/env lua

local TestType = {
    -- This is a unit test automatically linked with "lib"
    LIB_UNIT_TEST = 1,
    -- The script handles it's own logic
    SCRIPT = 2,
    -- The file will be extracted as both a header file and a source
    -- file and compiled with gcc
    PARSE_TEST = 3,
    -- The same as PARSE_TEST except additionally the compiled program
    -- is run
    EXECUTE_TEST = 4,
    -- The input file is parsed and printed and then compared to a
    -- golden file
    PARSE_PRINT_TEST = 5,
}

local omni_c_root = os.getenv("OMNI_C_ROOT")
local build_dir = os.getenv("BUILD_DIR")

if omni_c_root then
  print("OMNI_C_ROOT:", omni_c_root)
else
  print("Error: OMNI_C_ROOT environment variable not set.")
  -- Handle the error appropriately, e.g., exit the script:
  os.exit(1)
end

if build_dir then
  print("BUILD_DIR:", build_dir)
else
  print("Error: BUILD_DIR environment variable not set.")
  -- Handle the error appropriately, e.g., exit the script:
  os.exit(1)
end

-- sudo apt-get install lua-filesystem
local lfs = require("lfs") 

local function file_exists(filename)
    local file = io.open(filename, "r")
    if file then
        io.close(file)
        return true
    else
        return false
    end
end

local function read_file(filename)
  local file, err = io.open(filename, "r")  -- Open in read mode
  if not file then
    error("Error opening file: " .. tostring(err))
  end
  local content = file:read("*a")   -- Read all content
  file:close()                     -- Close the file
  return content
end

function ends_with(str, suffix)
  return suffix == '' or str:sub(-#suffix) == suffix
end

function contains(str, substr)
  return str:find(substr) ~= nil
end

local function get_test_type(filename)
   if ends_with(filename, ".sh") then
      return TestType.SCRIPT
   elseif contains(filename, "lib/") then
      -- Major Kludge. Not even tests/lib!
      return TestType.LIB_UNIT_TEST
   elseif ends_with(filename, ".expr") or ends_with(filename, ".stmt") then
      return TestType.PARSE_PRINT_TEST
   else
      local contents = read_file(filename)
      if contains(contents, "main%(") then
          return TestType.EXECUTE_TEST
      else 
          return TestType.PARSE_TEST
      end
   end
end

local function escape_and_quote(str)
  return '"' .. str:gsub('"', '\\"') .. '"'
end

-- Function to determine which omni-c executable to use
local function get_file_mtime(filename)
    local attr = lfs.attributes(filename)
    if attr then return attr.modification
    else return 0 end
end

--- local function get_omni_c_executable()
---     local omni_c_path = "./build/bin/omni-c"
---     local self_path = "./build/bin/self"
--- 
---     -- and get_file_mtime(self_path) > get_file_mtime(omni_c_path) then
---     if file_exists(self_path) then
---         return self_path
---     else
---         return omni_c_path
---     end
--- end

local function get_omni_c_executable()
  local exe = os.getenv("OMNI_C_EXECUTABLE")
  if exe then
     return exe
  else
     return build_dir .. "/bin/omni-c"
  end
end

local success = 0
local failure = 0
local failed_tests = {}
local no_golden_tests = {}

-- Return a time-stamp in milliseconds ignoring roll-over issues.
--
-- As you "cargo cult" this code, be aware that both Lua's integer
-- precision (53bits?) and other factors may change when a "roll-over"
-- or lack of precision occurs.
local function get_timestamp()
  --
  -- This should just work cross-platform but resolution is limited to seconds
  -- return os.time() * 1000
  --
  -- This does not appear to work (I got crazy numbers at least on my
  -- modern Linux): os.clock()
  -- 
  -- This should work with GNU date installed but other "date" programs
  -- or older GNU date may not understand it.
  -- local handle = io.popen("date +%s%3N")
  --
  -- This seems to work OK on my modern Linux platform and hopefully
  -- MacOS etc. It may stop working around November 20, 2286.
  --
  local handle = io.popen("date +%s%N | cut -b1-13")
  local result = handle:read("*a")
  handle:close()
  return tonumber(result)
end

local start_time = get_timestamp()

for _, arg in ipairs(arg) do
  print("------------------------------------------------------------------------")
  print("Running: " .. arg)
  print("------------------------------------------------------------------------")

  local exit_status = false
  local test_type = get_test_type(arg)

  local omni_c_exec = get_omni_c_executable()

  if test_type == TestType.LIB_UNIT_TEST then
     print("TestType.LIB_UNIT_TEST:", arg)
     local lib_archive = build_dir .. "/bin/lib.oar"
     exit_status = os.execute(omni_c_exec .. " test " .. lib_archive .. " " .. arg)
  elseif test_type == TestType.SCRIPT then
     print("TestType.SCRIPT:", arg)
     exit_status = os.execute(arg)
  elseif test_type == TestType.PARSE_TEST then
     print("TestType.PARSE_TEST:", arg)
     exit_status = os.execute(omni_c_root .. "/tools/generate-header-file-test.sh " .. arg)
  elseif test_type == TestType.EXECUTE_TEST then
     print("TestType.EXECUTE_TEST:", arg)
     exit_status = os.execute(omni_c_root .. "/tools/compile.sh " .. arg .. ".gen.c" .. " " .. arg)
  elseif test_type == TestType.PARSE_PRINT_TEST then
     print("TestType.PARSE_PRINT_TEST(*):", arg)
     local golden_file = arg .. ".golden"
     local output_file = arg .. ".out"
     exit_status = os.execute(omni_c_exec .. " test " .. arg .. " >" .. output_file)
     if exit_status and file_exists(golden_file) then
          exit_status = os.execute("diff -B -y " .. golden_file .. " " .. output_file)
     else
        table.insert(no_golden_tests, arg)
	print(read_file(output_file))
     end
  else
     print("ERROR - unhandled test type")
  end

  if exit_status then
    success = success + 1
  else
    failure = failure + 1
    table.insert(failed_tests, arg)
    print("FAIL: non zero exit status for " .. arg)
  end

  print("") 
end

print("======================================================================")

local end_time = get_timestamp()
local delta_ms = end_time - start_time

print("Total test run time: " .. delta_ms .. "ms")

for _, test in ipairs(no_golden_tests) do
  print("  -- No golden file for " .. test)
end

if success > 0 then
  print("Passed " .. success .. " tests.")
end

local number_failures_shown = 0

function indent_lines(text, num_spaces)
  local indented_text = ""
  for line in string.gmatch(text, "([^\n]*)\n?") do
    indented_text = indented_text .. string.rep(" ", num_spaces) .. line .. "\n"
  end
  return indented_text
end

if failure == 0 then
  os.exit(0)
else
  print("Failed " .. failure .. " tests:")
  for _, test in ipairs(failed_tests) do
    print("    " .. test)
    number_failures_shown = number_failures_shown + 1
    if number_failures_shown < 5 then
      local contents = read_file(test)
      print(indent_lines(contents, 10))
    end
  end
  os.exit(1) 
end

