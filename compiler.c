#include "compiler.h"

int compile_file(const char* filename, const char* out_filename, int flags){

    struct compile_process* process = compile_process_create(filename, out_filename, flags);
    if (!process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    // AQUI ENTRA ANALISE LÉXICA
    struct lex_process* lex_process = lex_process_create(process,&compiler_lex_functions,NULL);

    if (!lex_process) return COMPILER_FAILED_WITH_ERRORS;

    if (lex(lex_process) != LEXICAL_ANALYSIS_ALL_OK) return COMPILER_FAILED_WITH_ERRORS;

    // AQUI ENTRA O PARSING DO CÓDIGO

    // AQUI ENTRA A GERAÇÃO DE CÓDIGO

    return COMPILER_FILE_COMPILED_OK;
}