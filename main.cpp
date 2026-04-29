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

    vector<string> queries = {"ACGT", "GATTA", "TTTT", "ZZZZ"};

    for (const string& q : queries) {
        cout << "Suffixes of \"" << q << "\":\n";

        FM_index::Interval I = fmi.get_full_interval();
        for (int64_t i = (int64_t)q.size() - 1; i >= 0; i--) {
            I = fmi.left_extend(I, q[i]);
            int64_t count = (I == fmi.get_empty_interval()) ? 0 : I.size();
            cout << "  \"" << q.substr(i) << "\": " << count << "\n";
            if (I == fmi.get_empty_interval()) break;
        }
        cout << "\n";
    }

    return 0;
}
