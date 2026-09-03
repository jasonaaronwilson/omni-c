////
//// A symbol table for internal usage (the first use will be for
//// record keys, eventually we may use this for environment keys as
//// well which should make looking up in non root environments
//// probably faster than using a string hashtable)
////

typedef roci_symid_t = uint32_t;

// We should use a lock for multithreading
string_hashtable_t* the_roci_symbol_table = nullptr;
value_array_t* symid_to_interned_string = nullptr;

// Do we need is_interned to avoid interning symbols "by accident"?

char* roci_intern_symbol(char* symbol, roci_symid_t* symid_out) {
  if (the_roci_symbol_table == nullptr) {
    the_roci_symbol_table = make_string_hashtable(512);
    symid_to_interned_string = make_value_array(512);
  }

  value_result_t result = string_ht_find(the_roci_symbol_table, symbol);
  if (is_ok(result)) {
    roci_symid_t symid = result.u64;
    if (symid_out != nullptr) {
      *symid_out = symid;
    }
    return value_array_get(symid_to_interned_string, symid).str;
  }

  roci_symid_t symid = symid_to_interned_string->length;
  value_array_add(symid_to_interned_string, str_to_value(symbol));
  string_ht_insert(the_roci_symbol_table, symbol, u64_to_value(symid));
  if (symid_out != nullptr) {
    *symid_out = symid;
  }
  return symbol;
}

char* roci_symid_to_string(roci_symid_t symid) {
  if ((symid_to_interned_string != nullptr)
      && (symid < symid_to_interned_string->length)) {
    return value_array_get(symid_to_interned_string, symid).str;
  }
  return nullptr;
}
