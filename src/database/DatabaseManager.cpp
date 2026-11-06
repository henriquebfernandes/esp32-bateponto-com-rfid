#include "DatabaseManager.h"
#include <LittleFS.h>
#include "config.h"

static std::vector<Person> people;
static bool savePeople(const std::vector<Person>& list) {
  JsonDocument doc; JsonArray arr = doc["people"].to<JsonArray>();
  for (const auto& p : list) { JsonObject o=arr.add<JsonObject>(); o["id"]=p.id; o["name"]=p.name; o["uid"]=p.uid; }
  File f=LittleFS.open(PEOPLE_TEMP_FILE,"w"); if(!f) return false;
  bool ok=serializeJson(doc,f)>0; f.flush(); f.close(); if(!ok) { LittleFS.remove(PEOPLE_TEMP_FILE); return false; }
  LittleFS.remove(PEOPLE_BACKUP_FILE);
  bool had=LittleFS.exists(PEOPLE_FILE);
  if(had && !LittleFS.rename(PEOPLE_FILE,PEOPLE_BACKUP_FILE)) { LittleFS.remove(PEOPLE_TEMP_FILE); return false; }
  if(!LittleFS.rename(PEOPLE_TEMP_FILE,PEOPLE_FILE)) { if(had) LittleFS.rename(PEOPLE_BACKUP_FILE,PEOPLE_FILE); return false; }
  LittleFS.remove(PEOPLE_BACKUP_FILE); return true;
}
bool initDatabase() {
  if(!LittleFS.exists(PEOPLE_FILE) && LittleFS.exists(PEOPLE_BACKUP_FILE)) LittleFS.rename(PEOPLE_BACKUP_FILE,PEOPLE_FILE);
  if(!LittleFS.exists(PEOPLE_FILE)) return savePeople(people);
  File f=LittleFS.open(PEOPLE_FILE,"r"); if(!f) return false;
  JsonDocument doc; auto err=deserializeJson(doc,f); f.close(); if(err || !doc["people"].is<JsonArray>()) return false;
  std::vector<Person> loaded;
  for(JsonObjectConst o:doc["people"].as<JsonArrayConst>()) { Person p{ o["id"]|uint32_t(0), String(o["name"]|""), String(o["uid"]|"") }; if(p.id && p.name.length() && p.uid.length()) loaded.push_back(p); }
  people=loaded; return true;
}
bool findPersonByUid(const String& uid, Person& person) { for(const auto& p:people) if(p.uid==uid){person=p;return true;} return false; }
bool addPerson(const String& name,const String& uid,Person& person,String& error) {
  if(!name.length() || name.length()>MAX_PERSON_NAME_LENGTH){error="Nome inválido.";return false;}
  if(!uid.length()){error="UID vazio.";return false;} if(people.size()>=MAX_PEOPLE){error="Limite de pessoas atingido.";return false;}
  Person found; if(findPersonByUid(uid,found)){error="Este cartão já está vinculado a outra pessoa.";return false;}
  uint32_t id=1; for(const auto&p:people) if(p.id>=id) id=p.id+1;
  Person added{id,name,uid}; auto next=people; next.push_back(added); if(!savePeople(next)){error="Falha ao salvar os dados.";return false;}
  people=next; person=added; return true;
}
std::vector<Person> getPeople(){return people;}
