#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//Put your WiFi Access point SSID and password
const char* ssid = "Sonic-8092"; 
const char* password = "59hks8cib9e4";  

//Put ESP32 SSID and password
const char* newssid = "A-Team"; 
const char* newpassword = NULL;

WebServer server(80);

void handleRoot() 
{
  //Enter the URL you want to test with
  server.send(200, "text/plain", "http://esqsoft.com/examples/troubleshooting-http-using-telnet.htm");
}

//handle default GET request for some browsers
void handleFavicon() {
  server.send(404, "text/html; charset=iso-8859-1","<html><head> <title>404 Not Found</title></head><body><h1>Not Found</h1></body></html>");
}

void handleNotFound()
{ const char* host;
  WiFiClient client;

  Serial.print("Requesting uri");
  String requestUri = server.uri();
  Serial.println(requestUri);
  
  requestUri.remove(0,1);
  int i = requestUri.indexOf('/');
  String domain = requestUri.substring(0,i);
  if(domain.startsWith("www")){
    domain.remove(0,4);
  }
  host = domain.c_str();
  requestUri.remove(0,i);

  Serial.print("Host: ");
  Serial.println(host);
  Serial.println(requestUri);

  if (!client.connect(host, 80)) 
  {
    Serial.println("connection failed!");
    Serial.println(client.connect(host, 80));
    return;
  }
  
  client.print(String("GET ") + requestUri);
  
  client.print(String(" HTTP/1.1\r\n") +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
               
  while(!client.available())
  {
    yield();
  }

  String line;
  while(client.available())
  {
    line = client.readStringUntil('\r');
    line.replace("HTTP", "WHAT-IS-THIS");
    line.replace("http", "WHAT-IS-THIS");
  }
  server.send(200, "text/html", line);

  client.stop();
}

void setup(void)
{
  Serial.begin(115200);
  // set ESP32 as both access point and station
  WiFi.mode(WIFI_AP_STA);
    
  WiFi.softAP(newssid, newpassword);
  Serial.print(newssid);
  Serial.print(" server ip: ");
  Serial.println(WiFi.softAPIP());

  server.on("/favicon.ico", handleFavicon);
  server.on("/", handleRoot);
  
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP Server Started");
  if (strcmp (WiFi.SSID().c_str(),ssid) != 0) {
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    yield();
  }

  Serial.print("Connected to: ");
  Serial.print(WiFi.SSID());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(void){
  server.handleClient();
}
