void setup() {
}

void loop() {
    if (!client.connected()) {
        Serial.println("Connecting to server...");
        if (client.connect(memory.serverIP, memory.serverPort)) {
            Serial.println("Connected to server");
        } else {
            Serial.println("Connection failed");
            delay(5000);
            return;
        }
    }

    String Event = CheckForEvents();

    // gives id, when server is asking for it.
    if(Event[0] == '2'){
      Serial.println("Server is asking for id");
      client.println("2: -" + memory.id + "-");
    }
    // sets new id, that server gave.
    else if(Event[0] == '3'){
      int first = Event.indexOf('-');
      int second = Event.indexOf('-', first + 1);
      memory.id = Event.substring(first + 1, second);
      Serial.println("new id: " + memory.id);
      client.println("1"); // Done successfully
       
    }

    // gives name, when server is asking for it.
    else if(Event[0] == '4'){
      Serial.println("Server is asking for name");
      client.println("3: -" + memory.name + "-");
    }

    // sets new name, that server gave.
    else if(Event[0] == '5'){
      int first = Event.indexOf('-');
      int second = Event.indexOf('-', first + 1);
      memory.name = Event.substring(first + 1, second);
      Serial.println("new name: " + memory.name);
      client.println("1");
       
    }

    delay(100);
    
}

void UpdateInformation(){
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB
  }
  
  if(Serial){
    Serial.println("Update data Y/n");

    while(Serial.available() == 0){
      
    }
    
    String c = Serial.readStringUntil('\n');
    if(c != "Y"){
        return;
    }

    Serial.println("You can leave field empty, if you don't want to update it");

    Serial.println("id: " + memory.id + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    String data = Serial.readStringUntil('\n');

    if(data != ""){
      memory.id = data;
    }

    Serial.println("name: " + memory.name + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    data = Serial.readStringUntil('\n');
    Serial.println("Data:" + data);

    if(data != ""){
      memory.name = data;
    }

    Serial.println("ssid: " + memory.ssid + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    data = Serial.readStringUntil('\n');
    

    if(data != ""){
      memory.ssid = data;
    }

    Serial.println("Data:" + memory.ssid);

    Serial.println("password: " + memory.password + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    data = Serial.readStringUntil('\n');
    Serial.println("Data:" + data);

    if(data != ""){
      memory.password = data;
    }

    Serial.println("serverIP: " + memory.serverIP + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    data = Serial.readStringUntil('\n');
    Serial.println("Data:" + data);

    if(data != ""){
      memory.serverIP = data;
    }

    Serial.println("serverPort: " + String(memory.serverPort) + " |Waiting for you...|");
    while(Serial.available() == 0){
      
    }
    
    data = Serial.readStringUntil('\n');
    Serial.println("Data:" + data);

    if(data != ""){
      memory.serverPort = data.toInt();
    }
    
    saveToEEPROM();
  }else{
    Serial.println("no connection");
  }
}

/*
 * EEPROM MANAGING:
 */
void saveToEEPROM() {
  EEPROM.put(0, memory);
  boolean ok = EEPROM.commitReset();
  Serial.println((ok) ? "Commit OK" : "Commit failed");   
}

void loadFromEEPROM() {
    EEPROM.get(0, memory);
}
