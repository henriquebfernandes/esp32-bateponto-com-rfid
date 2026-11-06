#include "DatabaseManager.h"
#include "../filesystem/FileSystemManager.h"
#include "config.h"

static std::vector<Person> people;

static bool loadPeople(const char* path, std::vector<Person>& loaded) {
  File f=openFile(path,"r");
  if(!f) return false;
  JsonDocument doc;
  DeserializationError err=deserializeJson(doc,f);
  f.close();
  if(err || !doc["people"].is<JsonArray>()) return false;

  std::vector<Person> parsed;
  for(JsonObjectConst o:doc["people"].as<JsonArrayConst>()) {
    Person p{ o["id"]|uint32_t(0), String(o["name"]|""), String(o["uid"]|"") };
    if(!p.id || !p.name.length() || p.name.length()>MAX_PERSON_NAME_LENGTH || !p.uid.length() || parsed.size()>=MAX_PEOPLE) return false;
    for(const auto& existing:parsed) if(existing.id==p.id || existing.uid==p.uid) return false;
    parsed.push_back(p);
  }
  loaded=parsed;
  return true;
}

static bool savePeople(const std::vector<Person>& list) {
  JsonDocument doc; JsonArray arr = doc["people"].to<JsonArray>();
  for (const auto& p : list) { JsonObject o=arr.add<JsonObject>(); o["id"]=p.id; o["name"]=p.name; o["uid"]=p.uid; }
  File f=openFile(PEOPLE_TEMP_FILE,"w"); if(!f) return false;
  bool ok=serializeJson(doc,f)==measureJson(doc); f.flush(); f.close(); if(!ok) { removeFile(PEOPLE_TEMP_FILE); return false; }
  removeFile(PEOPLE_BACKUP_FILE);
  bool had=fileExists(PEOPLE_FILE);
  if(had && !renameFile(PEOPLE_FILE,PEOPLE_BACKUP_FILE)) { removeFile(PEOPLE_TEMP_FILE); return false; }
  if(!renameFile(PEOPLE_TEMP_FILE,PEOPLE_FILE)) { if(had) renameFile(PEOPLE_BACKUP_FILE,PEOPLE_FILE); return false; }
  removeFile(PEOPLE_BACKUP_FILE); return true;
}
bool initDatabase() {
  std::vector<Person> loaded;
  bool hasMain=fileExists(PEOPLE_FILE);
  bool hasBackup=fileExists(PEOPLE_BACKUP_FILE);

  if(hasMain && loadPeople(PEOPLE_FILE,loaded)) {
    people=loaded;
    if(hasBackup) removeFile(PEOPLE_BACKUP_FILE);
    return true;
  }

  if(hasBackup && loadPeople(PEOPLE_BACKUP_FILE,loaded)) {
    // Restore a valid backup even when the main file exists but is corrupted.
    if(hasMain && !removeFile(PEOPLE_FILE)) return false;
    if(!renameFile(PEOPLE_BACKUP_FILE,PEOPLE_FILE)) return false;
    people=loaded;
    return true;
  }

  if(hasMain || hasBackup) return false;
  people.clear();
  return savePeople(people);
}
bool findPersonByUid(const String& uid, Person& person) { for(const auto& p:people) if(p.uid==uid){person=p;return true;} return false; }
bool addPerson(const String& name,const String& uid,Person& person,String& error) {
  if(!name.length() || name.length()>MAX_PERSON_NAME_LENGTH){error="Nome inválido.";return false;}
  if(!uid.length()){error="UID vazio.";return false;} if(people.size()>=MAX_PEOPLE){error="Limite de pessoas atingido.";return false;}
  Person found; if(findPersonByUid(uid,found)){error="Este cartão já está vinculado a outra pessoa.";return false;}
  uint32_t id=1; for(const auto&p:people) if(p.id>=id) { if(p.id==UINT32_MAX){error="Não há mais identificadores disponíveis.";return false;} id=p.id+1; }
  Person added{id,name,uid}; auto next=people; next.push_back(added); if(!savePeople(next)){error="Falha ao salvar os dados.";return false;}
  people=next; person=added; return true;
}
std::vector<Person> getPeople(){return people;}
size_t getPeopleCount(){return people.size();}
bool removePerson(uint32_t id, String& error) {
  std::vector<Person> next;
  next.reserve(people.size());
  bool found=false;
  for(const auto& person:people) {
    if(person.id==id) found=true;
    else next.push_back(person);
  }
  if(!found) { error="Cadastro não encontrado."; return false; }
  if(!savePeople(next)) { error="Falha ao salvar a remoção."; return false; }
  people=next;
  return true;
}
bool resetDatabase(String& error) {
  std::vector<Person> empty;
  if(!savePeople(empty)) { error="Falha ao apagar os cadastros salvos."; return false; }
  people.clear();
  return true;
}
