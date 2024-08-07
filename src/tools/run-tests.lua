#!/usr/bin/env lua

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

  local exit_status = os.execute(arg)

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

print("Total test run time: " .. delta_ms .. " ms")

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

