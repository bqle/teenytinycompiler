#include "emit.h"

Emitter::Emitter(const string& fullPath) : fullPath(fullPath), lvl(0)
{
    header.reserve(1000);
    code.reserve(1000);
}

inline void Emitter::increase_level() {
    lvl++;
}

inline void Emitter::decrease_lvl() {
    lvl--;
    lvl = max(0, lvl);
}

void Emitter::emit(const string& code, bool push) {
    if (push) {
        for (int i = 0; i < lvl; ++i) {
            this->code+= "\t";
        }
    }
    this->code += code;
}

void Emitter::emit_line(const string&code, bool push) {
    if (push) {
        for (int i = 0; i < lvl; ++i) {
            this->code+= "\t";
        }
    }
    this->code += code;
    this->code += "\n";
}

void Emitter::header_line(const string& code, bool push) {
    if (push) {
        this->header += "\t";
    }
    this->header += code;
    this->header += "\n";
}

void Emitter::write_file() {
    ofstream output (fullPath);
    if (!output.is_open()) {
        cerr << "Failed to open file\n";
        return;
    }

    output << "Hello, world!\n";
    output.close();
}