#include <Arduino.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "WebServerManager.h"
#include "../database/DatabaseManager.h"
#include "../rfid/RFIDManager.h"
#include "../hardware/HardwareManager.h"
#include "config.h"

static WebServer server(80);
static String enrollmentName, lastUid, lastName, lastResult="none";
static bool enrolling=false; static uint32_t enrollmentStarted=0;
static void json(int code, JsonDocument& doc){String out;serializeJson(doc,out);server.send(code,"application/json",out);}
static void error(int code,const String& message){JsonDocument d;d["status"]="error";d["message"]=message;json(code,d);}
static void sendFile(const char* path,const char* type){File f=LittleFS.open(path,"r");if(!f){server.send(404,"text/plain","Arquivo não encontrado");return;}server.streamFile(f,type);f.close();}
void initWebServer(){
  server.on("/",HTTP_GET,[]{sendFile("/index.html","text/html; charset=utf-8");});
  server.on("/style.css",HTTP_GET,[]{sendFile("/style.css","text/css; charset=utf-8");});
  server.on("/script.js",HTTP_GET,[]{sendFile("/script.js","application/javascript; charset=utf-8");});
  server.on("/api/status",HTTP_GET,[]{JsonDocument d;d["status"]="success";d["wifi"]=WiFi.softAPgetStationNum()>0?"connected":"ready";d["ip"]=WiFi.softAPIP().toString();d["peopleCount"]=getPeople().size();d["pollIntervalMs"]=UI_POLL_INTERVAL_MS;json(200,d);});
  server.on("/api/people",HTTP_GET,[]{JsonDocument d;d["status"]="success";JsonArray a=d["people"].to<JsonArray>();for(const auto&p:getPeople()){JsonObject o=a.add<JsonObject>();o["id"]=p.id;o["name"]=p.name;o["uid"]=p.uid;}json(200,d);});
  server.on("/api/enrollment/start",HTTP_POST,[]{if(enrolling){error(409,"Já existe um cadastro em andamento.");return;}JsonDocument d;if(deserializeJson(d,server.arg("plain"))){error(400,"JSON inválido.");return;}String name=d["name"]|"";name.trim();if(!name.length()||name.length()>MAX_PERSON_NAME_LENGTH){error(400,"Informe um nome válido.");return;}enrollmentName=name;enrolling=true;enrollmentStarted=millis();lastResult="waiting";lastName="";JsonDocument r;r["status"]="waiting";r["message"]="Aproxime o cartão RFID.";json(200,r);});
  server.on("/api/enrollment/cancel",HTTP_POST,[]{enrolling=false;enrollmentName="";JsonDocument d;d["status"]="success";json(200,d);});
  server.on("/api/enrollment/status",HTTP_GET,[]{JsonDocument d;d["status"]=enrolling?"waiting":"idle";d["message"]=enrolling?"Aproxime o cartão RFID.":lastName;d["result"]=lastResult;json(200,d);});
  server.on("/api/rfid/last",HTTP_GET,[]{JsonDocument d;d["status"]=lastResult;d["uid"]=lastUid;d["name"]=lastName;json(200,d);});
  server.onNotFound([](){server.send(404,"text/plain","Não encontrado");});server.begin();Serial.println("Servidor HTTP iniciado.");
}
void handleWebServer(){server.handleClient();}
void processRFID(){
  if(enrolling && millis()-enrollmentStarted>=ENROLLMENT_TIMEOUT_MS){enrolling=false;enrollmentName="";lastResult="timeout";lastName="Cadastro expirado";}
  String uid;if(!pollRFID(uid))return;lastUid=uid;lastName="";
  if(enrolling){Person p;String e;if(addPerson(enrollmentName,uid,p,e)){lastResult="enrolled";lastName=p.name;signalSuccess();}else{lastResult="duplicate";lastName=e;signalFailure();}enrolling=false;enrollmentName="";return;}
  Person p;if(findPersonByUid(uid,p)){lastResult="recognized";lastName=p.name;signalSuccess();}else{lastResult="unknown";lastName="Cartão não cadastrado";signalFailure();}
}
