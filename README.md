# Rohit_Jena_ENLOG-Firmware-Engineer-Test-Assignment_submission
Assignment Submission
Question 1:
MQTT Buffered Publisher (queue + broker)
step 1 - Init — setup() initializes Serial, sets brokerConnected = false, seeds RNG.
step 2 - Periodic publish — every 2s loop() generates a random temperature string "Temp:XX" and calls publishMessage().
step 3 - Buffering — if broker is disconnected the message is pushed to messageBuffer; if buffer full (MAX_BUFFER_SIZE) the oldest message is discarded and logged.
step 4 - Broker toggle — every 10s the code flips brokerConnected; on reconnect onBrokerReconnect() pops and publishes all buffered messages; on disconnect it logs a warning.
step 5 - Reliable delivery behavior — while disconnected messages accumulate (bounded); when reconnected buffered messages are flushed in FIFO order and published.

Question 2:
Ad-hoc JSON Register Parser (string parsing):

step 1 - Helpers — getJsonValue() finds "<key>", locates :, skips whitespace, and extracts either a quoted string or a raw token; trimWspace() removes leading/trailing spaces.
step 2 - Locate array — parseRegistersFromJson() finds the "registers" key, then the [ ... ] block and extracts its content.
step 3 - Extract objects — it iterates arrayContent, locating { ... } blocks and for each object calls getJsonValue() to extract addr, name, desc, unit.
step 4 - Sanitize & store — trims fields, builds RegisterInfo and pushes into the registers vector.

Output & warnings — main() prints device name, then iterates registers: if addr or name missing it logs a warning and skips; otherwise prints

Question 3:
TcpHeartbeatClient (Winsock TCP PING/PONG) — 5 steps

step 1 - Startup — run() calls initWinsock() which runs WSAStartup; on failure the client stops with error.
step 2 - Connect loop — main loop checks isConnected(); if not connected it calls connectToServer() which creates a socket, validates IP, and calls connect(). On error it closes socket and retries after 5s.
step 3 - Send heartbeat — when connected sendHeartbeat() sends "PING" via send(); on send error it returns failure.
step 4 - Receive response — receiveResponse() sets a 5s recv timeout, calls recv(), handles timeout or closed socket, strips whitespace and checks payload: "PONG" → success; otherwise failure.
step 5 - Recovery & cleanup — on any send/recv failure disconnect() closes socket and loop will retry; on success the client sleeps 10s before sending next heartbeat; destructor calls disconnect() and WSACleanup().

Question 4:
CalendarEvent (union-based event class) — 5 steps

step 1 - Data model — CalendarEvent stores date, title[32], EventType and a union EventDetails which holds only one of location[32], minutes_before, or person_name[32].
step2 - Constructors — three overloaded constructors initialize the common fields (date, title), set type (MEETING/REMINDER/BIRTHDAY) and populate the relevant union member.
step 3 - Union init — EventDetails() zeroes memory (safe default) so unused union bytes are cleared.
step 4 - Printing — print_event() switches on type and prints a single formatted line using the correct union field.
step 5 - Demo — main() creates a Birthday, Meeting, and Reminder and prints.