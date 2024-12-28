#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EVENTS 100
#define MAX_LEN 100
#define FILENAME "events.tsv"

typedef struct {
  char name[MAX_LEN];
  char airline[MAX_LEN];
  char flight_number[MAX_LEN];
  struct tm date_time;
  int is_arrival;
} Event;

// Function prototypes
int getChoice();
void addEvent(Event events[], int* numEvents);
void generateHTML(Event events[], int numEvents);
void saveEvents(Event events[], int numEvents);
int loadEvents(Event events[]);

int main() {
  Event events[MAX_EVENTS];
  int numEvents = loadEvents(events);

  while (1) {
    int choice = getChoice();
    switch (choice) {
    case 1:
      addEvent(events, &numEvents);
      break;
    case 2:
      generateHTML(events, numEvents);
      break;
    case 3:
      saveEvents(events, numEvents);
      return 0;
    default:
      printf("Invalid choice!\n");
    }
  }
}

int getChoice() {
  int choice;
  printf("\nMenu:\n1. Add Event\n2. Generate HTML\n3. Exit\nChoice: ");
  scanf("%d", &choice);
  while (getchar() != '\n')
    ; // Clear input buffer
  return choice;
}

void addEvent(Event events[], int* numEvents) {
  if (*numEvents >= MAX_EVENTS) {
    printf("Max events reached!\n");
    return;
  }

  Event newEvent;
  printf("Enter name: ");
  fgets(newEvent.name, MAX_LEN, stdin);
  newEvent.name[strcspn(newEvent.name, "\n")] = 0;

  printf("Enter airline: ");
  fgets(newEvent.airline, MAX_LEN, stdin);
  newEvent.airline[strcspn(newEvent.airline, "\n")] = 0;

  printf("Enter flight number: ");
  fgets(newEvent.flight_number, MAX_LEN, stdin);
  newEvent.flight_number[strcspn(newEvent.flight_number, "\n")] = 0;

  printf("Enter date (YYYY-MM-DD): ");
  if (scanf("%d-%d-%d", &newEvent.date_time.tm_year, &newEvent.date_time.tm_mon,
            &newEvent.date_time.tm_mday)
      != 3) {
    printf("Invalid date format!\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }
  newEvent.date_time.tm_year -= 1900;
  newEvent.date_time.tm_mon -= 1;

  printf("Enter time (HH:MM): ");
  if (scanf("%d:%d", &newEvent.date_time.tm_hour, &newEvent.date_time.tm_min)
      != 2) {
    printf("Invalid time format!\n");
    while (getchar() != '\n')
      ; // Clear input buffer
    return;
  }

  printf("Is this an arrival (y/n): ");
  char arrivalInput;
  scanf(" %c", &arrivalInput);
  while (getchar() != '\n')
    ; // Clear input buffer
  newEvent.is_arrival = toupper(arrivalInput) == 'Y';

  events[(*numEvents)++] = newEvent;
}

void generateHTML(Event events[], int numEvents) {
  FILE* fp = fopen("calendar.html", "w");
  if (!fp) {
    printf("Error creating HTML file!\n");
    return;
  }

  fprintf(fp, "<html><body><table>\n");
  // ... (code to generate table rows for each event)
  fprintf(fp, "</table></body></html>\n");
  fclose(fp);
  printf("HTML calendar generated!\n");
}

int loadEvents(Event events[]) {
  // (Loads events from a file. Very similar to saving them.)
}

void saveEvents(Event events[], int numEvents) {
  // ... (Saves events to FILENAME in TSV format)
}
