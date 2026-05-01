/**
 * @file
 *
 * The roci compiler works by using the omni-c tokenizer to produce a
 * list of tokens which are then parsed and converted directly into
 * virtual machine instructions eliminating the need for a parse tree.
 *
 * The roci language looks like:
 *
 * Statements
 *
 * ```
 * let v = expr;
 * v = expr;
 * fn_name(exprs...);
 * if (expr) { statements }
 * if (expr) { statements } else { statements }
 * if (expr) { statements } else if (expr) { statements } else { statements }
 * while (expr) { statements }
 * return;
 * return expr;
 * ```
 *
 * Expressions
 *
 * ```
 * v
 * fn_name(exprs...)
 * fn(args) { statements }
 * ```
 */

void roci_compile_buffer(void) {
}
