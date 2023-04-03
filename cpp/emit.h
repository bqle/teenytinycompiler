#pragma once
#include <bits/stdc++.h>

using namespace std;

struct Emitter {
    string fullPath;
    string header;
    string code;
    int lvl;

    Emitter(const string& fullPath);

    inline void increase_level();
    
    inline void decrease_lvl();

    void emit(const string& code, bool push=false);

    void emit_line(const string&code, bool push=false);

    void header_line(const string& code, bool push=false);

    void write_file();
};