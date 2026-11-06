#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
#include <ArduinoJson.h>
#include <WString.h>
#include <vector>

struct Person { uint32_t id; String name; String uid; };
bool initDatabase();
bool findPersonByUid(const String& uid, Person& person);
bool addPerson(const String& name, const String& uid, Person& person, String& error);
std::vector<Person> getPeople();
#endif
