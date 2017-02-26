//Scoreboard.cpp
#include "Scoreboard.h"
#include <fstream>
#include <algorithm>
using namespace std;
 
Scoreboard::Scoreboard () {}
 
bool Scoreboard::load (char const* data_file) {
    ifstream f;
    f.open(data_file, ios::binary);
    if (!f.is_open()) { return false; }
 
    Entry entry_buffer;
    f.read(reinterpret_cast<char*>(&entry_buffer), SizeOfEntry);
    entries.clear();
 
    while (!f.eof()) {
        entries.push_back(entry_buffer);
        f.read(reinterpret_cast<char*>(&entry_buffer), SizeOfEntry);
    }
    f.close();
    return true;
}
 
bool Scoreboard::save (char const* data_file) const {
    ofstream f;
    f.open(data_file, ios::binary | ios::out);
    if (!f.is_open()) { return false; }
 
    f.write(reinterpret_cast<char const*>(&entries[0]), SizeOfEntry * entries.size());
    f.close();
    return true;
}
 
int Scoreboard::addEntry (Entry const& entry) {
    auto loc = entryWorseThan(entry);
    entries.insert(loc.first, entry);
    return loc.second;
}
 
Scoreboard::Entry Scoreboard::getEntry (int index) const {
    return entries[index];
}
 
void Scoreboard::prune (int max_amt) {
    entries.resize(max_amt);
}
 
int Scoreboard::size () const { return entries.size(); }
 
Scoreboard::EntryLocation Scoreboard::entryWorseThan (Entry const& entry) {
    int i = 0;
    for (auto it=entries.begin(); it!=entries.end(); ++it, ++i) {
        if (it->score <= entry.score) {
            return EntryLocation(it, i);
        }
    }
    return EntryLocation(entries.end(), i);
}
 
void Scoreboard::Entry::setName (std::string str) {
    memcpy(&name[0], &str.c_str()[0], sizeof(char) * (str.length()+1));
}