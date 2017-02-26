//Scoreboard.h
#include <string>
#include <utility>
#include <vector>
 
class Scoreboard {
    public:
        struct Entry {
            char name[256];
            int  score;
 
            void setName (std::string str);
        };
        typedef std::vector<Entry> Entries;
        typedef std::pair<Entries::iterator, int> EntryLocation;
        Scoreboard ();
       
        bool load (char const* data_file);
        bool save (char const* data_file) const;
 
        int  addEntry (Entry const& entry);
        Entry getEntry (int index) const;
       
        void prune (int max_amt);
        int  size  () const;
    private:
        //Not copy-constructable
        Scoreboard (Scoreboard const&);
 
        Entries entries;
        static int const SizeOfEntry = sizeof(Entry);
 
        EntryLocation entryWorseThan (Entry const& entry);
};