// Working
// CalendarEvent class stores date, title, type, and a union holding either:
// meeting location
// reminder minutes_before
// birthday person_name
// Three overloaded constructors initialize the event based on type:
// Meeting → fills location
// Reminder → fills minutes_before
// Birthday → fills person_name
// Union ensures only one detail field uses memory depending on event type.
// print_event() checks the event type using a switch and prints the correct detail.
// main() creates one Birthday, one Meeting, and one Reminder event, then prints them.
#include <iostream>
#include <cstring>

using namespace std;

enum EventType { MEETING, REMINDER, BIRTHDAY };

class CalendarEvent {
public:
    int day, month, year;
    char title[32];
    EventType type;

    union EventDetails {
        char location[32];     // for MEETING
        int minutes_before;    // for REMINDER
        char person_name[32];  // for BIRTHDAY

        EventDetails() { memset(this, 0, sizeof(EventDetails)); }
    } details;

    // MEETING
    CalendarEvent(int d, int m, int y, const char* t, const char* loc) {
        day = d; month = m; year = y;
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title)-1] = '\0';
        type = MEETING;
        strncpy(details.location, loc, sizeof(details.location) - 1);
        details.location[sizeof(details.location)-1] = '\0';
    }

    // REMINDER
    CalendarEvent(int d, int m, int y, const char* t, int min_before) {
        day = d; month = m; year = y;
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title)-1] = '\0';
        type = REMINDER;
        details.minutes_before = min_before;
    }

    // BIRTHDAY
    CalendarEvent(int d, int m, int y, const char* t, const char* pname, bool isBirthday) {
        day = d; month = m; year = y;
        strncpy(title, t, sizeof(title) - 1);
        title[sizeof(title)-1] = '\0';
        type = BIRTHDAY;
        strncpy(details.person_name, pname, sizeof(details.person_name) - 1);
        details.person_name[sizeof(details.person_name)-1] = '\0';
    }

    void print_event() const {
        switch (type) {
            case BIRTHDAY:
                cout << "[BIRTHDAY] " << day << "/" << month << "/" << year << " \"" << title << "\" Person: " << details.person_name << "\n";
                break;
            case MEETING:
                cout << "[MEETING] " << day << "/" << month << "/" << year << " \"" << title << "\" Location: " << details.location << "\n";
                break;
            case REMINDER:
                cout << "[REMINDER] " << day << "/" << month << "/" << year << " \"" << title << "\" Reminder: " << details.minutes_before << " min before\n";
                break;
        }
    }
};

// Simple demonstration
int main() {
    CalendarEvent e1(15, 8, 2025, "Dad's Birthday", "Dad", true);
    CalendarEvent e2(16, 8, 2025, "Team Sync", "Conference Room");
    CalendarEvent e3(17, 8, 2025, "Pay Bills", 30);

    e1.print_event();
    e2.print_event();
    e3.print_event();

    return 0;
}
