// mkdir ./build-dir/tmp
// ./build-dir/bin/omni-c build tests/term-events.c ./build-dir/bin/lib.oar --c-output-file ./build-dir/tmp/term-events.xpiled.c --binary-output-file ./build-dir/bin/term-events

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static /* volatile */ boolean_t running = true;

void handle_sigint(int sig) {
    running = false;
}

int main(void) {
    signal(SIGINT, handle_sigint);

    if (term_init() != 0) {
        perror("Failed to init terminal");
        return 1;
    }

    printf("Terminal input initialized.\r\n");
    printf("Press keys, move/click mouse, hold modifiers. Press Ctrl+C or Escape to exit.\r\n\r\n");

    int64_t last_event_time = 0;
    boolean_t printed_separator = true;

    while (running) {
      term_input_event_t ev = {0};
      if (term_poll_event(&ev)) {
	last_event_time = current_time_millis();
	printed_separator = false;
	if (ev.type == TERM_EVENT_KEY) {
	  printf("[KEY] action=%-7s keycode=%-16s codepoint=%3u ('%c') mods=0x%02x loc=%s (%d)\r\n",
		 term_key_action_to_string(ev.key.action),
		 term_keycode_to_string(ev.key.keycode),
		 ev.key.codepoint,
		 (ev.key.codepoint >= 32 && ev.key.codepoint < 127) ? cast(char, ev.key.codepoint) : ' ',
		 ev.key.modifiers,
		 term_key_location_to_string(ev.key.location),
		 ev.key.location);

	  if (ev.key.keycode == TERM_KEY_ESCAPE && ev.key.action == TERM_KEY_ACTION_PRESS) {
	    break;
	  }
	  if (ev.key.codepoint == 'c' && (ev.key.modifiers & TERM_MOD_CTRL)) {
	    break;
	  }
	} else if (ev.type == TERM_EVENT_MOUSE) {
	  printf("[MOUSE] btn=%d action=%d x=%-3d y=%-3d mods=0x%02x\r\n",
		 ev.mouse.button,
		 ev.mouse.action,
		 ev.mouse.x,
		 ev.mouse.y,
		 ev.mouse.modifiers);
	}
	fflush(stdout);
      } else {
	uint64_t now = current_time_millis();
        if (!printed_separator && (now - last_event_time > 1000)) {
            printf("--- [ idle ] -------------------------------------\r\n");
            fflush(stdout);
            printed_separator = true;
        }	
      }

      // Sleep poll loop for now (1ms)
      usleep(1000);
    }

    term_restore();
    printf("\r\nTerminal restored. Exiting.\r\n");
    return 0;
}
