#include "stages.h"
#include "string_replace.h"
#include "vector.h"

#include <string.h>
#include <assert.h>

void add_stage(stage_t *self, command_t cmd) {
    command_t *malloced = malloc(sizeof(command_t));
    *malloced = cmd;

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

// Default stages
stage_t build_stage(char *default) {
    stage_t stage;
    if (char == NULL) {
        stage = init_stage("Build");
    } else {
        stage = init_stage(default);
    }

    return stage;
}

stage_t run_stage() {
    stage_t stage = build_stage("Run");

// Very fun and not annoying platform specific build instructions
#ifdef __WIN32
    stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {srcdir} && mkdir {srcdir}\\{projectname} && mkdir {srcdir}\\{projectname}\\obj && mkdir {srcdir}\\{projectname}\\bin"));
#else
    stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {srcdir} && mkdir {srcdir}/{projectname} && mkdir {srcdir}/{projectname}/obj && mkdir {srcdir}/{projectname}/bin"));
#endif

    return stage;
}

stage_t clean_stage() {
    stage_t stage = init_stage("Clean");
    
    return stage;
}


command_t new_command(char *name, char *cmd) {
    assert(name != NULL);
    assert(cmd != NULL);
    
    command_t command;
    command.name = strdup(name);
    command.command = strdup(cmd);

    return command;
}

void free_command(command_t cmd) {
    free(cmd.name);
    free(cmd.command);
}