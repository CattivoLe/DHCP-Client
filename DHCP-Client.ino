#include <EtherCard.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

static byte mymac[] = { 0x72, 0x65, 0x64, 0x2D, 0x90, 0x31 };

byte Ethernet::buffer[700];

static uint32_t timer;

int i = 1;
int buttonPin = 2;
int buzz = 8;
int screen = 0;



void setup () {

pinMode(buttonPin, INPUT_PULLUP);
pinMode(buzz, OUTPUT);
  
Serial.begin(57600);
Serial.println("Ethernet Initialization");

  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.clear();
  lcd.home();
     
  lcd.print("Ethernet");
  lcd.setCursor(0, 1);
  lcd.print("Initialization");
  lcd.setCursor(15, 1);
  lcd.blink();
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
      printEror("Ether error");
  } 

  if (!ether.dhcpSetup()) {
      printEror("DHCP failed");
  }

  ether.printIp("My IP Adress: ", ether.myip);
  ether.printIp("My Netmask: ", ether.netmask);
  ether.printIp("My Gateway IP: ", ether.gwip);
  ether.printIp("My DNS IP: ", ether.dnsip);

#if 1

  if (!ether.dnsLookup(PSTR("www.google.com"))) {    
      printEror("DNS failed");
  }

#endif

  ether.printIp("Ping Server: ", ether.hisip);

timer = -9999999;
  
Serial.println();

tone (buzz, 1400, 300);
  
}



void loop () {

int buttonVal = digitalRead(2);
  
word len = ether.packetReceive();
word pos = ether.packetLoop(len);


  if (buttonVal == LOW) {
    tone (buzz, 800, 100);
      ++i;
    if (i>6) i=1;
      delay(400);
  }

  switch(i) {
      
  case 1: 
       printlcd("My IP Adress", ether.myip);
       screen = 1;
       break;
           
  case 2: 
       printlcd("My Netmask", ether.netmask);
       screen = 2;
       break;
           
  case 3: 
       printlcd("My Gateway IP", ether.gwip);
       screen = 3;
       break;
           
  case 4: 
       printlcd("My DNS IP", ether.dnsip);
       screen = 4;
       break;

  case 5: 
       printlcd("Ping Server", ether.hisip);
       screen = 5;
       break;

  case 6:
  
  if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {

    Serial.print("  ");
    Serial.print((micros() - timer) * 0.001, 3);
    Serial.println(" ms");  
    lcd.print((micros() - timer) * 0.001, 3); 
    lcd.print(" ms");
  } 

  if (micros() - timer >= 1000000) {
    
    ether.printIp("Pinging: ", ether.hisip);
    
    lcd.clear();
    lcd.noBlink();
    lcd.setCursor(0, 0);
    lcd.print("www.google.com");
  
    lcd.setCursor(0, 1);
    lcd.print("Ping: ");
    timer = micros();
    ether.clientIcmpRequest(ether.hisip);
    }
  screen = 0;
  break;
  } 
}



void printlcd(String msg, byte data[4]) {

  if (screen < i) {
      
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print(msg);
  lcd.setCursor(0, 1);

  for (byte i = 0; i < 4; ++i) {
    lcd.print(data[i], DEC);
    if (i < 3)
      lcd.print('.');
    }
  } 
}

void printEror(String msg) {

  if (micros() - timer >= 2000000) {

     Serial.println(msg);
     
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print(msg);  
     
  timer = micros();
      
  }
}


