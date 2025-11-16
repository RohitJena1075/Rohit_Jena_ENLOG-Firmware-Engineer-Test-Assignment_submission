# Rohit Jena — ENLOG Firmware Engineer Test Assignment  
### Assignment Submission (All Questions Explained Step-By-Step)

---

## **Question 1: MQTT Buffered Publisher**

### **MQTT Working Steps**

**Step 1 — Initialization**  
- `setup()` initializes Serial communication, seeds the random generator, and sets `brokerConnected = false`.

**Step 2 — Periodic Publish**  
- Every 2 seconds, `loop()` generates a random temperature message `"Temp:XX"` and calls `publishMessage()`.

**Step 3 — Buffering When Broker Is Down**  
- If broker is disconnected:  
  - Message is pushed into `messageBuffer`.  
  - If buffer is full (`MAX_BUFFER_SIZE`), the oldest message is removed and logged.

**Step 4 — Broker Reconnect/Disconnect Toggle**  
- Every 10 seconds, connection state toggles.  
- On reconnect: `onBrokerReconnect()` publishes all buffered messages in FIFO order.  
- On disconnect: logs a warning.

**Step 5 — Reliable Delivery Behavior**  
- Messages accumulate safely while disconnected.  
- As soon as broker reconnects, all buffered messages are flushed and delivered in correct order.

---

## **Question 2: JSON Register Parser (Manual String Parsing)**

### **Parsing Steps**

**Step 1 — Helper Functions**  
- `getJsonValue()` locates a key, finds the colon, skips whitespace, and extracts values (quoted or raw).  
- `trimWspace()` removes leading/trailing spaces.

**Step 2 — Find Register Array**  
- `parseRegistersFromJson()` searches for `"registers"` then extracts the `[ ... ]` content.

**Step 3 — Extract JSON Objects**  
- Iterates the array to find `{ ... }` blocks.  
- For each block, extracts `addr`, `name`, `desc`, and `unit`.

**Step 4 — Sanitize & Store**  
- Trims values and stores them as `RegisterInfo` objects inside a vector.

**Output Behavior**  
- `main()` prints device name.  
- For each register:  
  - Logs warnings for missing `addr` or `name`.  
  - Otherwise prints valid register info.

---

## **Question 3: TCP Heartbeat Client (Winsock PING/PONG)**

### **Operational Steps**

**Step 1 — Startup**  
- `run()` calls `initWinsock()` which performs `WSAStartup`.  
- If initialization fails, execution stops.

**Step 2 — Auto-Reconnect Loop**  
- If not connected, `connectToServer()` creates a socket, validates IP, and attempts a TCP connect.  
- On failure, socket is closed and retry happens after 5 seconds.

**Step 3 — Send Heartbeat**  
- When connected, `"PING"` is sent via `sendHeartbeat()`.

**Step 4 — Receive Response**  
- `receiveResponse()` uses a 5-second timeout.  
- If `"PONG"` is received → server is alive.  
- Any failure (timeout, no data, unexpected response) triggers a disconnect.

**Step 5 — Recovery & Cleanup**  
- On any error → socket is closed and loop retries.  
- On success → waits 10 seconds before next heartbeat.  
- Destructor closes the socket and calls `WSACleanup()`.

---

## **Question 4: CalendarEvent with `union` (C++ OOP + Memory Sharing)**

### **Working Steps**

**Step 1 — Data Model**  
- Event stores:
  - date (`day, month, year`)  
  - `title[32]`  
  - `EventType`  
  - `union EventDetails` → stores **one** of:
    - `location[32]` (MEETING)  
    - `minutes_before` (REMINDER)  
    - `person_name[32]` (BIRTHDAY)

**Step 2 — Constructors**  
- Three overloaded constructors set the shared fields and assign the union field depending on event type.

**Step 3 — Union Initialization**  
- `EventDetails()` zeroes memory to prevent leftover bytes from previous data.

**Step 4 — Printing**  
- `print_event()` checks event type and prints the correct union member.

**Step 5 — Demo Output**  
Creates:  
- Birthday event  
- Meeting event  
- Reminder event  



---
