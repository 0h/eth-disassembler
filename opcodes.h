#ifndef OPCODES_H
#define OPCODES_H

#define MAX_OPCODE_LENGTH 15

typedef struct {
    unsigned char code;
    char name[MAX_OPCODE_LENGTH];
    int variable_length;
} Opcode;

extern Opcode opcodes[];

const Opcode* get_opcode(unsigned char code);

#endif // OPCODES_H

