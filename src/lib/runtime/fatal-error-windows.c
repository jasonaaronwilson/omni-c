char* get_command_line(void) {
  return "command line not available under windows right now";
}

char* get_program_path(void) {
  return "<program-path-unknown>";
}

void print_backtrace(void) {
    HANDLE process = GetCurrentProcess();
    bool sym_initialized = SymInitialize(process, NULL, TRUE);

    const ULONG max_frames = 64;
    void* stack[64] = {0};

    // Capture up to 64 frames, skipping the print_backtrace frame itself (frame 0)
    USHORT frames = CaptureStackBackTrace(1, max_frames, stack, NULL);

    // SYMBOL_INFO requires trailing space for the symbol name buffer
    size_t symbol_buffer_size = sizeof(SYMBOL_INFO) + (MAX_SYM_NAME * sizeof(char));
    SYMBOL_INFO* symbol = cast(SYMBOL_INFO*, malloc_bytes(symbol_buffer_size));
    symbol->MaxNameLen = MAX_SYM_NAME;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (USHORT i = 0; i < frames; i = i + 1) {
        DWORD64 address = cast(DWORD64, stack[i]);
        DWORD64 displacement = 0;

        if (SymFromAddr(process, address, &displacement, symbol)) {
            printf("#%u 0x%016llX %s + 0x%llX\n",
                   cast(unsigned int, i),
                   address,
                   symbol->Name,
                   displacement);
        } else {
            printf("#%u 0x%016llX [Unknown Symbol]\n",
                   cast(unsigned int, i),
                   address);
        }
    }

    if (sym_initialized) {
        SymCleanup(process);
    }
}
