# IoT_Project
 
## Systems
### TableNode
TableNode is the individual ESP32 node on each table in the library. These nodes need to be able to scan RFID cards, show output on their LEDs and LED screen, and both send and recieve info via ESPNOW.

### Hub_Reciever
Hub_Reciever is the reciever half of the hub system. It needs to send and recieve data via ESPNow and send data physically over a link to the sender half.

### Hub_Sender
Hub_Sender is the sender half of the hub system. It needs to be able to recieve data physically over a link to the reciever half and send data over the internet to our online portal.