// Title: MQTT Broker Connection and Message Buffering Simulation
// Working 
// Step 1: Initialization process
// Step 2: Main loop with two concurrent tasks:
// a) Temperature publishing every 2 seconds
// b) Broker connection toggle every 10 seconds
// Step 3: Complete 20-second timeline example showing exact behavior
// Step 4: Buffer overflow scenario demonstration

#include <iostream>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;
using String = string;

#define MAX_BUFFER_SIZE 20

// Serial for logging
struct SerialType
{
    void begin(int) {}
    void println(const String &s) { cout << s << endl; }
} Serial;

queue<String> messageBuffer;
bool brokerConnected = false; // MQTT broker connection

// Log helper
void log(const String &msg)
{
    Serial.println(msg);
}

// Publish directly to broker simulation
void publishDirectly(const String &msg)
{
    log("Published: " + msg);
}

// Publish a message if broker disconnected
void publishMessage(const String &msg)
{
    if (brokerConnected)
    {
        publishDirectly(msg);
    }
    else
    {
        if (messageBuffer.size() >= MAX_BUFFER_SIZE)
        {
            String discarded = messageBuffer.front();
            messageBuffer.pop();
            log("Buffer overflow. Discarded oldest message: " + discarded);
        }
        messageBuffer.push(msg);
        log("Buffered message: " + msg);
    }
}

// Broker reconnect
void onBrokerReconnect()
{
    log("Broker reconnected. Publishing buffered messages...");

    while (!messageBuffer.empty())
    {
        String msg = messageBuffer.front();
        messageBuffer.pop();
        publishDirectly(msg);
    }
}

// Arduino millis()
unsigned long millis()
{
    using namespace chrono;
    static const auto start = steady_clock::now();
    return static_cast<unsigned long>(
        duration_cast<milliseconds>(steady_clock::now() - start).count());
}

// RNG
void randomSeed(unsigned int s)
{
    srand(s);
}

// Analog read stub
int analogRead(int)
{
    return 0;
}

void setup()
{
    Serial.begin(115200);
    brokerConnected = false; // Disconnected
    randomSeed(static_cast<unsigned int>(time(nullptr)));
}

unsigned long lastPublishTime = 0;

void loop()
{
    unsigned long now = millis();

    // Publish temperature for every 2 seconds
    if (now - lastPublishTime >= 2000)
    {
        lastPublishTime = now;

        int temperature = rand() % 100;
        String msg = "Temp:" + to_string(temperature);

        publishMessage(msg);
    }

    // Broker connection toggle in 10 secs
    static unsigned long lastToggle = 0;
    if (now - lastToggle > 10000)
    {
        lastToggle = now;

        brokerConnected = !brokerConnected;
        if (brokerConnected)
        {
            onBrokerReconnect();
        }
        else
        {
            log("Broker disconnected.");
        }
    }
}

int main()
{
    setup();

    while (true)
    {
        loop();
    }

    return 0;
}




