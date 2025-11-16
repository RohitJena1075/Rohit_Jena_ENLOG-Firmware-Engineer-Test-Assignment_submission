// Title: JSON parser for embedded devices
// Working
// Step 1: Program Initialization - JSON data setup
// Step 2: Extract Device Name - Detailed getJsonValue trace
// Step 3: Parse All Registers - 5 iterations showing each register parsing
// Step 4: Validate and Display - Shows validation logic for each register
// Step 5: Complete Program Output - Final console output


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Register info
struct RegisterInfo {
    string address;
    string name;
    string description;
    string unit;
};

// Trim whitespace
void trimWspace(string &text) {
    auto notSpace = [](int ch) { return !isspace(ch); };
    text.erase(text.begin(), find_if(text.begin(), text.end(), notSpace));
    text.erase(find_if(text.rbegin(), text.rend(), notSpace).base(), text.end());
}

// Extract JSON value
string getJsonValue(const string& jsonSnippet, const string& key) {
    string searchKey = "\"" + key + "\"";
    size_t keyPos = jsonSnippet.find(searchKey);
    if (keyPos == string::npos) return "";

    size_t colonPos = jsonSnippet.find(':', keyPos);
    if (colonPos == string::npos) return "";

    size_t valueStart = colonPos + 1;
    while (valueStart < jsonSnippet.size() && isspace(jsonSnippet[valueStart]))
        ++valueStart;

    if (valueStart >= jsonSnippet.size()) return "";

    if (jsonSnippet[valueStart] == '"') {
        size_t start = valueStart + 1;
        size_t end = jsonSnippet.find('"', start);
        if (end == string::npos) return "";
        return jsonSnippet.substr(start, end - start);
    } else {
        size_t end = jsonSnippet.find_first_of(",}", valueStart);
        if (end == string::npos) end = jsonSnippet.size();
        return jsonSnippet.substr(valueStart, end - valueStart);
    }
}

// Parse the registers
vector<RegisterInfo> parseRegistersFromJson(const string& fullJson) {
    vector<RegisterInfo> registers;

    const string registersKey = "\"registers\"";
    size_t arrayStart = fullJson.find(registersKey);
    if (arrayStart == string::npos) return registers;

    arrayStart = fullJson.find('[', arrayStart);
    if (arrayStart == string::npos) return registers;

    size_t arrayEnd = fullJson.find(']', arrayStart);
    if (arrayEnd == string::npos) return registers;

    string arrayContent = fullJson.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

    size_t pos = 0;
    while (true) {
        size_t objStart = arrayContent.find('{', pos);
        if (objStart == string::npos) break;

        size_t objEnd = arrayContent.find('}', objStart);
        if (objEnd == string::npos) break;

        string registerJson = arrayContent.substr(objStart + 1, objEnd - objStart - 1);

        RegisterInfo reg;
        reg.address    = getJsonValue(registerJson, "addr");
        reg.name       = getJsonValue(registerJson, "name");
        reg.description= getJsonValue(registerJson, "desc");
        reg.unit       = getJsonValue(registerJson, "unit");

        trimWspace(reg.address);
        trimWspace(reg.name);
        trimWspace(reg.description);
        trimWspace(reg.unit);

        registers.push_back(reg);

        pos = objEnd + 1;
    }

    return registers;
}

int main() {
    const string jsonData = R"json(
    {
        "device": "PH1",
        "registers": [
            { "addr": "0x00", "name": "VOLTAGE", "desc": "Line Voltage", "unit": "V" },
            { "addr": "0x01", "name": "CURRENT", "desc": "Line Current", "unit": "A" },
            { "addr": "0x02", "name": "POWER", "desc": "Active Power", "unit": "W" },
            { "addr": "0x03", "name": "", "desc": "Missing Name", "unit": "X" },
            { "addr": "0x04", "desc": "No Name Key", "unit": "Y" }
        ]
    }
    )json";

    string deviceName = getJsonValue(jsonData, "device");
    cout << "Device Name: " << deviceName << "\n\n";

    auto registers = parseRegistersFromJson(jsonData);

    for (const auto& reg : registers) {
        
        if (reg.address.empty()) {
            cout << "[WARNING] Missing or empty register address. Skipping this entry.\n";
            continue;
        }
        if (reg.name.empty()) {
            cout << "[WARNING] Register at Addr=" << reg.address << " missing 'name'. Skipping this entry.\n";
            continue;
        }

        
        string desc = reg.description.empty() ? "No Description" : reg.description;
        string unit = reg.unit.empty() ? "No Unit" : reg.unit;

        cout << "[REG] Addr=" << reg.address
             << " Name=" << reg.name
             << " Desc=\"" << desc << "\""
             << " Unit=" << unit << "\n";
    }

    return 0;
}



