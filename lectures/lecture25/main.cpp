
#include "console.h"
#include "simpio.h"
#include "hashtable.h"
using namespace std;


int main()
{
    HashTable table;
    HashTable::hashCode("abs");
    cout<< table.hashCode("abacus") << endl;
    table.put("abacus");
    table.put("dwindle");
    table.put("fox");
    table.put("goose");
    table.put("bag");
    table.put("beans");
    table.put("ferocious");
    table.put("delicate");
    table.put("Jason");
    table.put("Shreya");
    table.put("Ila");
    table.put("Xander");
    table.put("Anjali");
    table.put("Rushank");
    table.put("cat");
    cout << table;
    return 0;
}
