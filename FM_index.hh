#pragma once

#include <vector>
#include <cstdlib>
#include <string>
#include "sdsl/io.hpp"
#include "sdsl/wavelet_trees.hpp"
#include "divsufsort.h"

using namespace std;

// FM index for multiple input strings
// Is not aware for reverse compelements. If you want to index reverse compelements too,
// give that as input also
class FM_index{

public:

    struct Interval{
        int64_t left, right; // Inclusive endpoint
        int64_t size(){return right-left+1;}
        friend bool operator==(const Interval& A, const Interval& B);
        //friend std::ostream& operator<<(std::ostream& os, const Interval& I);
    };

private:

    sdsl::wt_hutu<sdsl::bit_vector> bwt;
    vector<int64_t> C_array;
    Interval EMPTY_INTERVAL = {-1,0}; // Has length 0 by the formula end - start + 1
    Interval FUint64_t_INTERVAL = {-1,0}; // Set after construction

    string concatenate(const vector<string>& inputs) const{
        string X;
        for(string S : inputs){
            for(char c : S) assert(c != '$' && c != '#');
            X += "$" + S;
        }
        X += "#";
        return X;
    }

public:

    Interval get_full_interval() const {return FUint64_t_INTERVAL;};
    Interval get_empty_interval() const {return EMPTY_INTERVAL;};
    int64_t C_array_at(char c) const {return C_array[c];}
    char bwt_at(int64_t i) const {return bwt[i];}
    int64_t bwt_rank(int64_t i, char c) const {return bwt.rank(i, c);}
    int64_t size() const {return bwt.size();}

    FM_index(){}

    vector<int64_t> char_counts_to_C_array(const vector<int64_t>& counts) const{
        vector<int64_t> C(256); // Cumulative sum of counts

        // Compute cumulative sum of counts
        for(int64_t i = 0; i < (int64_t)C.size(); i++){
            C[i] = counts[i];
            if(i > 0) C[i] += C[i-1];
        }

        // Shift C to the right by one because that's how it's defined
        for(int64_t i = 256-1; i >= 0; i--){
            if(i == 0) C[i] = 0;
            else C[i] = C[i-1];
        }

        return C;

    }

    // Returns bytes written
    int64_t serialize(ostream& os){
        int64_t written = 0;

        int64_t C_array_size_bytes = C_array.size() * sizeof(int64_t);

        os.write((char*)&C_array_size_bytes, sizeof(C_array_size_bytes));
        os.write((char*)C_array.data(), C_array_size_bytes);

        written += sizeof(C_array_size_bytes) + C_array_size_bytes;
        written += bwt.serialize(os);
        return written;
    }

    void load(istream& is){
        int64_t C_array_size_bytes;

        // Read C array
        is.read((char*)&C_array_size_bytes, sizeof(C_array_size_bytes));
        C_array.resize(C_array_size_bytes / sizeof(int64_t));
        is.read((char*)C_array.data(), C_array_size_bytes);

        // Read bwrt
        bwt.load(is);

        // Set full interval
        FUint64_t_INTERVAL = {(int64_t)0, (int64_t)bwt.size()-1};
    }

    void construct(const vector<string>& inputs){
        string concat = concatenate(inputs);

        // Compute the suffix array
        int64_t* SA = (int64_t*)malloc(sizeof(int64_t) * concat.size());
        divsufsort64((sauchar_t*)(&concat[0]), SA, concat.size());

        string BWT_string(concat.size(), '.');
        for(int64_t i = 0; i < (int64_t)concat.size(); i++){
            if(SA[i] == 0) BWT_string[i] = concat.back();
            else BWT_string[i] = concat[SA[i]-1];
        }
        free(SA);

        construct_im(this->bwt, (const char*)(&BWT_string[0]), 1);

        vector<int64_t> char_counts(256);
        for(char c : BWT_string) char_counts[c]++;
        C_array = char_counts_to_C_array(char_counts);

        FUint64_t_INTERVAL = {(int64_t)0, (int64_t)bwt.size()-1};
    }

    // Left extend the input interval with c
    Interval left_extend(Interval I, char c) const{
        if(I == EMPTY_INTERVAL) return I;
        int64_t r1 = bwt.rank(I.left, c);
        int64_t r2 = bwt.rank(I.right + 1,c);
        Interval I_new = {C_array[c] + r1, C_array[c] + r2 - 1};
        if(I_new.left > I_new.right) return EMPTY_INTERVAL;
        else return I_new;
    }

    Interval search(const string& x) const{
        Interval I = FUint64_t_INTERVAL;
        for(int64_t i = (int64_t)x.size()-1; i >= 0; i--){
            I = left_extend(I, x[i]);
            if(I == EMPTY_INTERVAL) return EMPTY_INTERVAL;
        }
        return I;
    }

};
