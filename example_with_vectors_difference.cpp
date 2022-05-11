#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

long long assess_vectors_diff(vector<long long> &v1, vector<long long> &v2) {
    unordered_set<long long> set1;
    unordered_set<long long> set2;
    for (auto el1: v1) {
        if (el1 == -1) {
            cerr << "Error: el1 == -1 in long long assess_vectors_diff\n";
            exit(1);
        }
        set1.insert(el1);
    }
    for (auto el2: v2) {
        if (el2 == -1) {
            cerr << "Error: el2 == -1 in long long assess_vectors_diff\n";
            exit(1);
        }
        set2.insert(el2);
    }
    long long diff = 0;
    for (auto el1: set1) {
        for (auto el2: set2) {
            if (set2.contains(el1) && set1.contains(el2)) {
                continue;
            }
            diff += abs(el1 - el2);
        }
    }
    return diff;
}

int main() {
    vector<long long> v1 = {21, 22, 23, 31, 32, 33};
    vector<long long> v2 = {22, 23, 24, 32, 33, 34};
    cout << "\"Разность\" векторов v1 и v2 равна " << assess_vectors_diff(v1, v2) << '\n';
}
