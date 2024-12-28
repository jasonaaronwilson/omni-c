/**
 * @file travel.c
 *
 * This program is meant to produce HTML to help organize the arrival
 * and departure of guests that are visiting. Since old school menu
 * driven interfaces are still OK in my mind as the input let's go
 * with that but at least in 2024 we need to output HTML so the output
 * can be a bit pretty for printing.
 *
 * The first version is going to be mostly AI generated via ChatGPT 4o
 * around the time of this commit. Then we can "c-armyknife-lib"
 * enhance it so that it has fewer fixed limitations, security holes,
 * etc.
 *
 * This is now the second version. I prompted to use TSV and removed
 * the old events.txt file that was created.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTS 100
#define MAX_STR_LEN 100

typedef struct {
  char name[MAX_STR_LEN];
  char airline[MAX_STR_LEN];
  char flight_number[MAX_STR_LEN];
  char date[MAX_STR_LEN];
  char time[MAX_STR_LEN];
  int is_arrival;
} Event;

Event events[MAX_EVENTS];
int event_count = 0;

const char* months[]
    = {"January", "February", "March",     "April",   "May",      "June",
       "July",    "August",   "September", "October", "November", "December"};

int days_in_month(int month, int year) {
  switch (month) {
  case 1:
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)
               ? 29
               : 28; // February
  case 3:
  case 5:
  case 8:
  case 10:
    return 30; // April, June, September, November
  default:
    return 31; // January, March, May, July, August, October, December
  }
}

int day_of_week(int day, int month, int year) {
  struct tm time_in = {0, 0, 0, day, month, year - 1900};
  time_t time_temp = mktime(&time_in);

  // Extract day of the week (0=Sunday, 1=Monday, ..., 6=Saturday)
  struct tm* time_out = localtime(&time_temp);
  return time_out->tm_wday;
}

void print_calendar(int month, int year) {
  printf("     %s %d\n", months[month], year);
  printf("Su Mo Tu We Th Fr Sa\n");

  int days = days_in_month(month, year);
  int start_day = day_of_week(1, month, year);

  // Print leading spaces
  for (int i = 0; i < start_day; i++) {
    printf("   ");
  }

  // Print days of the month
  for (int day = 1; day <= days; day++) {
    printf("%2d ", day);
    if ((start_day + day) % 7 == 0 || day == days) {
      printf("\n");
    }
  }
  printf("\n");
}

void print_two_months_calendar(int month1, int year1, int month2, int year2) {
  print_calendar(month1, year1);
  printf("\n");
  print_calendar(month2, year2);
}

/* ====================================================================== */

void add_event() {
  if (event_count >= MAX_EVENTS) {
    printf("Event list is full!\n");
    return;
  }

  Event e;

  printf("Enter name: ");
  fgets(e.name, sizeof(e.name), stdin);
  e.name[strcspn(e.name, "\n")] = 0; // Remove newline

  printf("Enter airline: ");
  fgets(e.airline, sizeof(e.airline), stdin);
  e.airline[strcspn(e.airline, "\n")] = 0; // Remove newline

  printf("Enter flight number: ");
  fgets(e.flight_number, sizeof(e.flight_number), stdin);
  e.flight_number[strcspn(e.flight_number, "\n")] = 0; // Remove newline

  int c; // Add this to clear the input buffer after each fgets
  while ((c = getchar()) != '\n' && c != EOF)
    ; // Clear input buffer after fgets

  printf("Enter date (YYYY-MM-DD): ");
  if (scanf("%d-%d-%d", &e.date_time.tm_year, &e.date_time.tm_mon,
            &e.date_time.tm_mday)
      != 3) {
    fprintf(stderr, "Invalid date format! Use YYYY-MM-DD\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  e.date_time.tm_year -= 1900;
  e.date_time.tm_mon -= 1;

  printf("Enter time (HH:MM): ");
  if (scanf("%d:%d", &e.date_time.tm_hour, &e.date_time.tm_min) != 2) {
    fprintf(stderr, "Invalid time format!\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  while (getchar() != '\n')
    ; // Clear input buffer on success

  printf("Is this an arrival (y/n): ");
  char arrivalInput;
  scanf(" %c", &arrivalInput);
  while (getchar() != '\n')
    ; // Clear input buffer on success
  e.is_arrival = toupper(arrivalInput) == 'Y';

  events[event_count++] = e;
}

#ifdef GEMNI_V3_BS
/* Version 3 */

void add_event() {
  // ... (Input validation for max events)
  Event e;

  printf("Enter name: ");
  fgets(e.name, MAX_STR_LEN, stdin);
  e.name[strcspn(e.name, "\n")] = 0; // Remove newline

  // ... (same as above for airline and flight number using fgets) ...

  // Modified input for date and time:
  int year, month, day, hour, minute;

  printf("Enter date (YYYY-MM-DD): ");
  if (scanf("%d-%d-%d", &year, &month, &day) != 3) {
    fprintf(stderr, "Invalid date format! Use YYYY-MM-DD\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }

  printf("Enter time (HH:MM): ");
  if (scanf("%d:%d", &hour, &minute) != 2) {
    fprintf(stderr, "Invalid time format!\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  while (getchar() != '\n')
    ; // Discard remaining characters in the input buffer

  // Store date and time in separate fields:
  snprintf(e.date, MAX_STR_LEN, "%04d-%02d-%02d", year, month, day);
  snprintf(e.time, MAX_STR_LEN, "%02d:%02d", hour, minute);

  // Input Validation for Arrival/Departure:
  while (1) { // Keep looping until valid input is received
    printf("Is this an arrival (y/n): ");
    char arrivalInput;
    if (scanf(" %c", &arrivalInput) != 1) {
      while (getchar() != '\n')
        ; // Clear input buffer on failure
    } else {
      arrivalInput = toupper(arrivalInput);
      if (arrivalInput == 'Y' || arrivalInput == 'N') {
        e.is_arrival = arrivalInput == 'Y';
        break; // Exit loop on valid input
      }
    }
    printf("Invalid input. Please enter 'y' or 'n'.\n");
  }

  events[event_count++] = e;
}

#endif /* GEMNI_V3_BS  */

/* Version 2 */

#ifdef GEMINI_CODE_V2

void add_event() {
  // ... (Input validation for max events)
  Event e;

  printf("Enter name: ");
  fgets(e.name, MAX_STR_LEN, stdin);
  e.name[strcspn(e.name, "\n")] = 0;

  // ... (same as above for airline and flight number using fgets) ...

  printf("Enter date (YYYY-MM-DD): ");
  if (scanf("%d-%d-%d", &e.date_time.tm_year, &e.date_time.tm_mon,
            &e.date_time.tm_mday)
      != 3) {
    fprintf(stderr, "Invalid date format! Use YYYY-MM-DD\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  e.date_time.tm_year -= 1900;
  e.date_time.tm_mon -= 1;

  printf("Enter time (HH:MM): ");
  if (scanf("%d:%d", &e.date_time.tm_hour, &e.date_time.tm_min) != 2) {
    fprintf(stderr, "Invalid time format!\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  while (getchar() != '\n')
    ; // <-- Add this line to clear the buffer

  printf("Is this an arrival (y/n): ");
  char arrivalInput;
  scanf(" %c", &arrivalInput);
  while (getchar() != '\n')
    ; // Clear input buffer on success
  e.is_arrival = toupper(arrivalInput) == 'Y';

  events[event_count++] = e;
}

#endif /* GEMINI_CODE_V2 */

/* Version 1 after a "super heavy" form of FU LLM realignment (I have
   no idea if this is a good technique of now. */

#ifdef GEMINI_MIXED_V1

void add_event() {
  if (event_count >= MAX_EVENTS) {
    printf("Event list is full!\n");
    return;
  }

  Event e;

  printf("Enter name: ");
  fgets(e.name, MAX_STR_LEN, stdin);
  e.name[strcspn(e.name, "\n")] = 0; // Remove newline

  // ... (same as above for airline and flight number using fgets) ...

  printf("Enter date (YYYY-MM-DD): ");
  fgets(e.date, MAX_STR_LEN, stdin);
  e.date[strcspn(e.date, "\n")] = 0;

  printf("Enter time (HH:MM): ");
  fgets(e.time, MAX_STR_LEN, stdin);
  e.time[strcspn(e.time, "\n")] = 0;

  // Input Validation for Arrival/Departure:
  while (1) { // Keep looping until valid input is received
    printf("Is this an arrival (y/n): ");
    char arrivalInput;
    if (scanf(" %c", &arrivalInput) != 1) {
      while (getchar() != '\n')
        ; // Clear input buffer on failure
    } else {
      arrivalInput = toupper(arrivalInput);
      if (arrivalInput == 'Y' || arrivalInput == 'N') {
        e.is_arrival = arrivalInput == 'Y';
        break; // Exit loop on valid input
      }
    }
    printf("Invalid input. Please enter 'y' or 'n'.\n");
  }

  events[event_count++] = e;
}

#endif /* GEMINI_MIXED_V1 */

/* ====================================================================== */

// void add_event() {
//     if (event_count >= MAX_EVENTS) {
//         printf("Event list is full!\n");
//         return;
//     }
//
//     Event e;
//
//     printf("Enter name: ");
//     scanf("%99[^\n]%*c", e.name); // Read up to 99 characters until newline,
//     then discard newline printf("Enter airline: "); scanf("%99[^\n]%*c",
//     e.airline); printf("Enter flight number: "); scanf("%99[^\n]%*c",
//     e.flight_number); printf("Enter date (YYYY-MM-DD): ");
//     scanf("%99[^\n]%*c", e.date);
//     printf("Enter time (HH:MM): ");
//     scanf("%99[^\n]%*c", e.time);
//     printf("Is this an arrival (1 for yes, 0 for no): ");
//     scanf("%d%*c", &e.is_arrival);
//
//     events[event_count++] = e;
// }

/* ====================================================================== */

void display_events() {
  for (int i = 0; i < event_count; i++) {
    Event e = events[i];
    printf("Event %d:\n", i + 1);
    printf("Name: %s\n", e.name);
    printf("Airline: %s\n", e.airline);
    printf("Flight Number: %s\n", e.flight_number);
    printf("Date: %s\n", e.date);
    printf("Time: %s\n", e.time);
    printf("Type: %s\n", e.is_arrival ? "Arrival" : "Departure");
    printf("\n");
  }
}

void save_events_to_file(const char* filename) {
  FILE* file = fopen(filename, "w");
  if (!file) {
    perror("Could not open file");
    return;
  }

  for (int i = 0; i < event_count; i++) {
    Event e = events[i];
    fprintf(file, "%s\t%s\t%s\t%s\t%s\t%d\n", e.name, e.airline,
            e.flight_number, e.date, e.time, e.is_arrival);
  }

  fclose(file);
}

void load_events_from_file(const char* filename) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    perror("Could not open file");
    return;
  }

  event_count = 0;
  while (fscanf(file, "%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%d\n",
                events[event_count].name, events[event_count].airline,
                events[event_count].flight_number, events[event_count].date,
                events[event_count].time, &events[event_count].is_arrival)
         != EOF) {
    event_count++;
  }

  fclose(file);
}

void generate_html(const char* filename) {
  FILE* file = fopen(filename, "w");
  if (!file) {
    perror("Could not open file");
    return;
  }

  fprintf(file,
          "<!DOCTYPE html>\n<html>\n<head>\n<title>Event "
          "Calendar</title>\n<style>\n");
  fprintf(file, "table { width: 100%%; border-collapse: collapse; }\n");
  fprintf(
      file,
      "th, td { border: 1px solid black; padding: 10px; text-align: left; }\n");
  fprintf(file, ".important { font-weight: bold; }\n");
  fprintf(file, "sub { font-size: smaller; }\n");
  fprintf(file, "</style>\n</head>\n<body>\n");
  fprintf(
      file,
      "<h1>Event "
      "Calendar</h1>\n<table>\n<tr>\n<th>Date</th>\n<th>Event</th>\n</tr>\n");

  for (int i = 0; i < event_count; i++) {
    Event e = events[i];
    fprintf(file,
            "<tr>\n<td>%s</td>\n<td class=\"important\">%s<sub>%s, %s, %s, "
            "%s</sub></td>\n</tr>\n",
            e.date, e.name, e.airline, e.flight_number, e.time,
            e.is_arrival ? "Arrival" : "Departure");
  }

  fprintf(file, "</table>\n</body>\n</html>\n");

  fclose(file);
}

int main() {
  int choice;
  const char* filename = "events.tsv";
  const char* html_filename = "calendar.html";

  load_events_from_file(filename);

  while (1) {
    printf("1. Add Event\n");
    printf("2. Display Events\n");
    printf("3. Save Events\n");
    printf("4. Generate HTML\n");
    printf("5. Print Two Months Calendar\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      add_event();
      break;
    case 2:
      display_events();
      break;
    case 3:
      save_events_to_file(filename);
      break;
    case 4:
      generate_html(html_filename);
      printf("HTML file generated: %s\n", html_filename);
      break;
    case 5: {
      int month1, year1, month2, year2;
      printf("Enter first month and year (MM YYYY): ");
      scanf("%d %d", &month1, &year1);
      printf("Enter second month and year (MM YYYY): ");
      scanf("%d %d", &month2, &year2);
      print_two_months_calendar(month1 - 1, year1, month2 - 1, year2);
      break;
    }
    case 6:
      save_events_to_file(filename);
      exit(0);
    default:
      printf("Invalid choice\n");
    }
  }

  return 0;
}
