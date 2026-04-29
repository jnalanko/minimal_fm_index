#include "FM_index.hh"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    // Index three DNA strings
    vector<string> sequences = {
        "ACGTACGTACGT",
        "GATTACAGATTACA",
        "ACGTTTTT",
    };

    FM_index fmi;
    fmi.construct(sequences);

    cout << "Indexed " << sequences.size() << " sequences, "
         << "total BWT length: " << fmi.size() << "\n\n";

    // Search for a few patterns and report all occurrences
    vector<string> queries = {"ACGT", "GATTA", "TTTT", "ZZZZ"};

    for (const string& q : queries) {
        FM_index::Interval I = fmi.search(q);
        if (I == fmi.get_empty_interval()) {
            cout << "\"" << q << "\"  not found\n";
            continue;
        }
        cout << "\"" << q << "\"  found " << I.size() << " time(s):\n";
    }

    return 0;
}
