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
stage_t build_stage(char *def) {
    stage_t stage;
    if (def == NULL) {
        stage = init_stage("Build");
    } else {
        stage = init_stage(def);
    }

    // Very fun and not annoying platform specific build instructions
    #ifdef __WIN32
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {targetdir} && mkdir {targetdir}\\{projectname} && mkdir {targetdir}\\{projectname}\\obj && mkdir {targetdir}\\{projectname}\\bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc -O2 -c {srcdir}\\* -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}\\{projectname}\\obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc -B gcc {targetdir}\\{projectname}\\obj\\* -o {targetdir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir -p {targetdir}/{projectname}/obj {targetdir}/{projectname}/bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc -O2 -c {srcdir}/* -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}/{projectname}/obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc -B gcc {targetdir}/{projectname}/obj/* -o {targetdir}/{projectname}/bin/{binname}-{projectversion}"));
    #endif

    return stage;
}

stage_t run_stage() {
    stage_t stage = build_stage("Run");

    #ifdef __WIN32
        stage.callbacks.add_stage(&stage, new_command("Run", ".\\{target_dir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Run", "./{target_dir}/{projectname}/bin/{binname}-{projectversion}"))
    #endif

    return stage;
}

stage_t clean_stage() {
    stage_t stage = init_stage("Clean");

    #ifdef __WIN32
        stage.callbacks.add_stage(&stage, new_command("Remove Target", "rmdir .\\{targetdir}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Remove Target", "rm -r ./{targetdir}"));
    #endif
    
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