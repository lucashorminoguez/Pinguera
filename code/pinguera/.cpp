#include notifications.h

/*
Envia un whatsapp mediante la api de callmebot
- recibe string, numero y una api key que nos dan la primera vez que conectamos el nro
  con la api.
 */
void send_wsp(String message, const String& phone, const String& key) {

  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phone + "&apikey=" + key + "&text=" + urlEncode(message);        
  
  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, url)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(url); 
    
    if (httpResponseCode == 200) {
      Serial.println("Wsp enviado correctamente");
    } else {
      Serial.print("Error Codigo: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Error al iniciar conexión HTTP");
  }
}