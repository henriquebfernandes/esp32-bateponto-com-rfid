#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

struct Person { uint32_t id; String name; String uid; };
bool initDatabase();
bool findPersonByUid(const String& uid, Person& person);
bool addPerson(const String& name, const String& uid, Person& person, String& error);
std::vector<Person> getPeople();
size_t getPeopleCount();
bool removePerson(uint32_t id, String& error);
bool resetDatabase(String& error);
#endif
