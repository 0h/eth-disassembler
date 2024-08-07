#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"

void disassemble(const char* bytecode, FILE* output) {
    if (bytecode == NULL) {
        fprintf(stderr, "Error: bytecode is NULL\n");
        return;
    }

    size_t length = strlen(bytecode) / 2;
    int block_number = 0;
    int block_started = 0;

    for (size_t i = 0; i < length; ) {
        unsigned char byte;
        if (sscanf(bytecode + 2 * i, "%2hhx", &byte) != 1) {
            fprintf(stderr, "Error: failed to read byte at position %zu\n", i);
            return;
        }
        const Opcode* opcode = get_opcode(byte);
        
        if (i == 0 || (opcode && opcode->code == 0x5b)) { // 0x5b is JUMPDEST, indicating a new block
            if (block_started) {
                fprintf(output, "\n");
            }
            fprintf(output, "Block %d:\n", block_number++);
            block_started = 1;
        }

        if (opcode) {
            fprintf(output, "%s", opcode->name);
            if (opcode->variable_length > 0) {
                fprintf(output, " ");
                for (int j = 0; j < opcode->variable_length; j++) {
                    if (sscanf(bytecode + 2 * (i + 1 + j), "%2hhx", &byte) != 1) {
                        fprintf(stderr, "Error: failed to read byte at position %zu\n", i + 1 + j);
                        return;
                    }
                    fprintf(output, "%02x", byte);
                }
                i += opcode->variable_length;
            }
            fprintf(output, "\n");
        } else {
            fprintf(output, "UNKNOWN_%02x\n", byte);
        }

        i++;

        if (opcode && (opcode->code == 0x56 || opcode->code == 0x57)) { // End block after JUMP or JUMPI
            fprintf(output, "\nBlock %d:\n", block_number++);
            block_started = 0;
        }
    }
}

void read_bytecode_from_file(const char* filename, char** bytecode) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open bytecode file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    *bytecode = malloc(length + 1);
    if (!*bytecode) {
        perror("Failed to allocate memory for bytecode");
        exit(EXIT_FAILURE);
    }

    fread(*bytecode, 1, length, file);
    (*bytecode)[length] = '\0';

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc != 3 || strcmp(argv[1], "-t") != 0) {
        fprintf(stderr, "Usage: %s -t <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* bytecode;
    read_bytecode_from_file(argv[2], &bytecode);

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "result-%s", argv[2]);

    FILE* output = fopen(output_filename, "w");
    if (!output) {
        perror("Failed to open output file");
        free(bytecode);
        return EXIT_FAILURE;
    }

    disassemble(bytecode, output);

    fclose(output);
    free(bytecode);

    printf("Disassemble completed. Check %s for the results.\n", output_filename);

    return EXIT_SUCCESS;
}

