#include "stages.h"
#include "vector.h"

#include <string.h>
#include <assert.h>

void add_stage(stage_t *self, char *cmd) {
    assert(cmd != NULL);

    char *malloced = strdup(cmd);

    vector_t *commands = &self->commands;
    commands->callbacks.push(commands, (void *)malloced);
}

stage_t init_stage(char *name) {
    assert(name != NULL);
    
    stage_t stage;
    stage.stage_name = name;
    stage.commands = init_vector();
    stage.callbacks.add_stage = &add_stage;
}

void free_stage(stage_t self) {
    free_vector(self.commands);
}