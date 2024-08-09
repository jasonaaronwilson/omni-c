#!/usr/bin/env lua

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

local TestType = {
    -- The script handles it's own logic
    SCRIPT = 1,
    -- The file will be extracted as both a header file and a source
    -- file and compiled with gcc
    PARSE_TEST = 2,
    -- The same as PARSE_TEST except additionally the compiled program
    -- with be run
    EXECUTE_TEST = 3
}

local function get_test_type(filename)
   if ends_with(filename, ".sh") then
      return TestType.SCRIPT
   else
      local contents = read_file(filename)
      if contains(contents, "main%(") then
          return TestType.EXECUTE_TEST
      else 
          return TestType.PARSE_TEST
      end
   end
end

local success = 0
local failure = 0
local failed_tests = {}

local function get_timestamp()
  return os.clock() * 1000 -- Millisecond precision
end

local start_time = get_timestamp()

for _, arg in ipairs(arg) do
  print("------------------------------------------------------------------------")
  print("Running: " .. arg)
  print("------------------------------------------------------------------------")

  local exit_status = false
  local test_type = get_test_type(arg)

  if test_type == TestType.SCRIPT then
     exit_status = os.execute(arg)
  elseif test_type == TestType.PARSE_TEST then
     exit_status = os.execute("./tools/generate-header-file-test.sh " .. arg)
  elseif test_type == TestType.EXECUTE_TEST then
     exit_status = os.execute("./tools/compile.sh " .. arg)
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

print("Total test run time: " .. delta_ms .. " seconds")

if success > 0 then
  print("Passed " .. success .. " tests.")
end

if failure == 0 then
  os.exit(0)
else
  print("Failed " .. failure .. " tests:")
  for _, test in ipairs(failed_tests) do
    print(test)
  end
  os.exit(1) 
end

