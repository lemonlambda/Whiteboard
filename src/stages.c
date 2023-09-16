#include "stages.h"
#include "string_replace.h"
#include "vector.h"
#include "rust_types.h"
#include "toml_format.h"
#include "platform_specific.h"

#include <string.h>
#include <assert.h>

char *replace_args(command_t *cmd, package_t *project, bin_t *bin);

void add_stage(stage_t *self, command_t cmd) {
    command_t *malloced = malloc(sizeof(command_t));
    assert(malloced != NULL);
    *malloced = cmd;

    self->commands.callbacks.push(&self->commands, (void *)malloced);
}

void run_stages(stage_t *self, package_t *project, bin_t *bin) {
    vector_t commands = self->commands;

    for (int i = 0; true; i++) {
        command_t *cmd = (command_t *)commands.callbacks.get(&commands, i);
        if (!cmd)
            break;

        char *replaced = replace_args(cmd, project, bin);
    }
}

stage_t init_stage(char *name) {
    assert(name != NULL);
    
    stage_t stage;
    stage.stage_name = strdup(name);
    stage.commands = init_vector();
    stage.callbacks.add_stage = &add_stage;
    stage.callbacks.run_stages = &run_stages;
    return stage;
}

void free_stage(stage_t self) {
    free(self.stage_name);
    free_vector(self.commands);
}

// Default stages
stage_t build_stage(char *def) {
    fflush(stdout);
    stage_t stage;
    if (def == NULL) {
        stage = init_stage("Build");
    } else {
        stage = init_stage(def);
    }
    fflush(stdout);

    // Very fun and not annoying platform specific build instructions
    #ifndef WIN32
        fflush(stdout);
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {targetdir} && mkdir {targetdir}\\{projectname} && mkdir {targetdir}\\{projectname}\\obj && mkdir {targetdir}\\{projectname}\\bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc -O2 -c {srcdir}\\* -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}\\{projectname}\\obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc -B gcc {targetdir}\\{projectname}\\obj\\* -o {targetdir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        fflush(stdout);
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir -p {targetdir}/{projectname}/obj {targetdir}/{projectname}/bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc -O2 -c {srcdir}/* -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}/{projectname}/obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc -B gcc {targetdir}/{projectname}/obj/* -o {targetdir}/{projectname}/bin/{binname}-{projectversion}"));
    #endif
    
    fflush(stdout);

    return stage;
}

stage_t run_stage() {
    fflush(stdout);
    stage_t stage = build_stage("Run");
    fflush(stdout);
    
    #ifndef WIN32
        stage.callbacks.add_stage(&stage, new_command("Run", ".\\{targetdir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Run", "./{targetdir}/{projectname}/bin/{binname}-{projectversion}"));
    #endif

    return stage;
}

stage_t clean_stage() {
    stage_t stage = init_stage("Clean");

    #ifndef WIN32
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

usize find_size(char *cmd, package_t *project, bin_t *bin);
usize count_string(const char *str1, const char *str2);

// Replaces the args like `{binname}` in commands to real things
// WARN: You need to free the returning char *
char *replace_args(command_t *cmd, package_t *project, bin_t *bin) {
    // Need a lot of memory because of the fact that I don't want to find out how much it will be
    char *format = strdup(cmd->command);
    format = realloc(format, find_size(cmd->command, project, bin) + 1);

    format = strrepall(format, "{targetdir}", bin->targetdir);
    format = strrepall(format, "{binname}", bin->name);
    format = strrepall(format, "{srcdir}", bin->srcdir);
    format = strrepall(format, "{includedir}", bin->includedir);
    format = strrepall(format, "{projectname}", project->name);
    format = strrepall(format, "{projectversion}", project->version);

    return format;
}

// Finds the size of the resulting args
// NOTE: DOES NOT FIND THE LENGTH, IT FINDS THE SIZE
// Another NOTE: does not include null terminator
usize find_size(char *cmd, package_t *project, bin_t *bin) {
    usize count_targetdir = count_string(cmd, "{targetdir}");
    usize count_binname = count_string(cmd, "{binname}");
    usize count_srcdir = count_string(cmd, "{srcdir}");
    usize count_includedir = count_string(cmd, "{includedir}");
    usize count_projectname = count_string(cmd, "{projectversion}");
    usize count_projectversion = count_string(cmd, "{projectname}");

    usize length = 0;

    length += count_targetdir * strlen(bin->targetdir);
    length += count_binname * strlen(bin->name);
    length += count_srcdir * strlen(bin->srcdir);
    length += count_includedir * strlen(bin->includedir);
    length += count_projectname * strlen(project->name);
    length += count_projectversion * strlen(project->version);

    return (length * sizeof(char) + strlen(cmd));
}

// Finds the amount of substrings
usize count_string(const char *str1, const char *str2) {
    assert(str1 != NULL);
    assert(str2 != NULL);
    
    usize count = 0;
    const char *tmp = str1;
    while (tmp = strstr(tmp, str2)) {
        count++;
        tmp++;
    }
    return count;
}
