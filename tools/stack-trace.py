#!/usr/bin/env python3
import sys
import subprocess
import re

def decode_addresses(addresses):
    if not addresses:
        return
    
    print("\n\033[1;31m--- Decoded Stack Trace ---\033[0m")
    
    # Run addr2line once with all addresses for efficiency
    cmd = ["addr2line", "-e", argv[1], "-f", "-C"] + addresses
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        lines = result.stdout.strip().split('\n')
        
        # addr2line outputs 2 lines per address: Function name, then File:Line
        for i in range(0, len(lines), 2):
            func = lines[i]
            file_line = lines[i+1] if i+1 < len(lines) else "??:0"
            
            # Clean up the output paths for readability
            if "?? " in func: func = "\033[33munknown_function\033[0m"
            
            frame_num = i // 2
            print(f"  [{frame_num}] \033[1m{func}\033[0m\n      at {file_line}")
            
    except subprocess.CalledProcessError as e:
        print(f"Error running addr2line: {e}")
        
    print("\033[1;31m---------------------------\033[0m\n")

def main():
    in_trace = False
    captured_addresses = []

    # Stream stdin line-by-line
    for line in sys.stdin:
        cleaned_line = line.strip()
        
        if "=== STACK TRACE START ===" in cleaned_line:
            in_trace = True
            captured_addresses = []
            continue
        elif "=== STACK TRACE END ===" in cleaned_line:
            in_trace = False
            decode_addresses(captured_addresses)
            continue
        
        if in_trace:
            # Validate it looks like a hex address before saving
            if re.match(r"0x[0-9a-fA-F]+", cleaned_line):
                captured_addresses.append(cleaned_line)
        else:
            # Pass regular application logs straight to the console
            sys.stdout.write(line)
            sys.stdout.flush()

if __name__ == "__main__":
    main()
