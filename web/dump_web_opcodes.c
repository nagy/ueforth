// Copyright 2021 Bradley D. Nelson
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>

#include "common/opcodes.h"
#include "common/floats.h"
#include "common/bits.h"

#define VOCABULARY_LIST V(forth) V(internals)

#define PLATFORM_OPCODE_LIST \
  X("CALL", CALL, sp = Call(sp|0, tos|0) | 0; DROP) \
  FLOATING_POINT_LIST

enum {
#define XV(flags, name, op, code) OP_ ## op,
  PLATFORM_OPCODE_LIST
  OPCODE_LIST
#undef XV
};

enum {
#define V(name) VOC_ ## name,
  VOCABULARY_LIST
#undef V
};

enum {
#define V(name) VOC_ ## name ## _immediate = VOC_ ## name + (IMMEDIATE << 8),
  VOCABULARY_LIST
#undef V
};


int main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "cases") == 0) {
#define XV(flags, name, op, code) \
    printf("          case %d:  // %s\n            %s; break;\n", OP_ ## op, name, #code);
    PLATFORM_OPCODE_LIST
    OPCODE_LIST
#undef XV
  } else if (argc == 2 && strcmp(argv[1], "dict") == 0) {
#define V(name) \
    printf("  create(\"" #name "-builtins\", %d);\n", BUILTIN_FORK, OP_DOCREATE); \
    printf("  comma(%d);\n", VOC_ ## name);
    VOCABULARY_LIST
#undef V
#define XV(flags, name, op, code) \
    printf("  builtin(" #name ", %d, %d, %d);\n", \
          ((VOC_ ## flags >> 8) & 0xff) | BUILTIN_MARK, \
          (VOC_ ## flags & 0xff), OP_ ## op);
    PLATFORM_OPCODE_LIST
    OPCODE_LIST
#undef XV
  } else if (argc == 2 && strcmp(argv[1], "sys") == 0) {
    G_SYS *g_sys = 0;
    #define G_SYS 256
    printf("  const g_sys = %d;\n", G_SYS);
    #define EMITSYS(name) \
    printf("  const g_sys_%s = %d;\n", #name, 256 + 4 * (((cell_t*) g_sys) - ((cell_t *) &g_sys->name)));
    EMITSYS(heap);
    EMITSYS(current);
    EMITSYS(context);
    EMITSYS(latestxt);
    EMITSYS(notfound);
    EMITSYS(heap_start);
    EMITSYS(heap_size);
    EMITSYS(stack_cells);
    EMITSYS(boot);
    EMITSYS(boot_size);
    EMITSYS(tib);
    EMITSYS(ntib);
    EMITSYS(tin);
    EMITSYS(state);
    EMITSYS(base);
    EMITSYS(argc);
    EMITSYS(argv);
    EMITSYS(runner);
    EMITSYS(rp);
    EMITSYS(DOLIT_XT);
    EMITSYS(DOFLIT_XT);
    EMITSYS(DOEXIT_XT);
    EMITSYS(YIELD_XT);
    EMITSYS(DOCREATE_OP);
    EMITSYS(builtins);
  } else {
    fprintf(stderr, "USAGE: %s cases/dict/sys\n", argv[1]);
    return 1;
  }
  return 0;
}
