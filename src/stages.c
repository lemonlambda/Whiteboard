#include "stages.h"
#include "string_replace.h"
#include "vector.h"
#include "rust_types.h"
#include "toml_format.h"
#include "platform_specific.h"
#include "color_codes.h"

#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <err.h>

#define streq(str1, str2) strcmp(str1, str2) == 0

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
        printf("%sStage %s%s%s:%s %s\n", BHMAG, BHGRN, cmd->name, BHMAG, CRESET, replaced);
        int error = system(replaced);
        if (error != 0)
            errx(error, "Recieved error in command, exiting.");
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
    stage_t stage;
    if (def == NULL) {
        stage = init_stage("Build");
    } else {
        stage = init_stage(def);
    }
    fflush(stdout);

    // Very fun and not annoying platform specific build instructions
    #ifdef WIN32
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {targetdir} && mkdir {targetdir}\\{projectname} && mkdir {targetdir}\\{projectname}\\obj && mkdir {targetdir}\\{projectname}\\bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc {ccargs} -c {srcfiles} -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}\\{projectname}\\obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc {ldargs} {targetdir}\\{projectname}\\obj\\* -o {targetdir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir -p {targetdir}/{projectname} && mkdir -p {targetdir}/{projectname}/obj {targetdir}/{projectname}/bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc {ccargs} -c {srcfiles} -I {includedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}/{projectname}/obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc {ldargs} {targetdir}/{projectname}/obj/* -o {targetdir}/{projectname}/bin/{binname}-{projectversion}"));
    #endif
    
    fflush(stdout);

    return stage;
}

stage_t run_stage() {
    stage_t stage = build_stage("Run");
    
    #ifdef WIN32
        stage.callbacks.add_stage(&stage, new_command("Run", ".\\{targetdir}\\{projectname}\\bin\\{binname}-{projectversion}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Run", "./{targetdir}/{projectname}/bin/{binname}-{projectversion}"));
    #endif

    return stage;
}

stage_t clean_stage() {
    stage_t stage = init_stage("Clean");

    #ifdef WIN32
        stage.callbacks.add_stage(&stage, new_command("Remove Target", "rmdir .\\{targetdir}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Remove Target", "rm -rf ./{targetdir}"));
    #endif
    
    return stage;
}

stage_t test_stage() {
    stage_t stage = init_stage("Run");

    #ifdef WIN32
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir {targetdir} && mkdir {targetdir}\\{projectname} && mkdir {targetdir}\\{projectname}\\obj && mkdir {targetdir}\\{projectname}\\bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc {ccargs} -c {srcfiles} -I {includedir} -I {programincludedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}\\{projectname}\\obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc {ldargs} {targetdir}\\{projectname}\\obj\\* -o {targetdir}\\{projectname}\\bin\\{binname}"));
        stage.callbacks.add_stage(&stage, new_command("Test", ".\\{targetdir}\\{projectname}\\bin\\{binname}"));
    #else
        stage.callbacks.add_stage(&stage, new_command("Make Dirs", "mkdir -p {targetdir}/{projectname} && mkdir -p {targetdir}/{projectname}/obj {targetdir}/{projectname}/bin"));
        stage.callbacks.add_stage(&stage, new_command("Compilation", "gcc {ccargs} -c {srcfiles} -I {includedir} -I {programincludedir}"));
        stage.callbacks.add_stage(&stage, new_command("Moving Objects", "mv *.o {targetdir}/{projectname}/obj"));
        stage.callbacks.add_stage(&stage, new_command("Linking", "gcc {ldargs} {targetdir}/{projectname}/obj/* -o {targetdir}/{projectname}/bin/{binname}"));
        stage.callbacks.add_stage(&stage, new_command("Test", "./{targetdir}/{projectname}/bin/{binname}"));
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

char *get_source_files(bin_t* bin);

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
    format = strrepall(format, "{programincludedir}", bin->programincludedir);
    format = strrepall(format, "{ccargs}", bin->ccargs);
    format = strrepall(format, "{ldargs}", bin->ldargs);
    format = strrepall(format, "{projectname}", project->name);
    format = strrepall(format, "{projectversion}", project->version);

    // Let's get all of the flise in a dir and read the extensions too
    char *src_files = get_source_files(bin);
    format = strrepall(format, "{srcfiles}", src_files);
    free(src_files);

    return format;
}

// Should free the result after done using
char *get_source_files(bin_t *bin) {
    usize current_size = 64;
    char *src_dir = calloc(1, sizeof(char) * current_size);
    DIR *dir;
    bool first_run = true;
    struct dirent *en;
    dir = opendir(bin->srcdir);
    assert(dir != NULL);
    while ((en = readdir(dir)) != NULL) {
        #ifdef EXPLICIT_DEBUG
            printf("File: %s\n", en->d_name);
        #endif
        // Reallocate the string if it needs to be bigger
        while (strlen(en->d_name) + strlen(src_dir) + 1 > current_size) {
            current_size *= 2;
            src_dir = realloc(src_dir, current_size);
        }
        // Skip all the useless shit
        if (streq(en->d_name, ".") || streq(en->d_name, "..")) {
            continue;
        }
        const usize length = strlen(en->d_name);
        if (length <= 2)
            continue;
        else if (!(en->d_name[length - 1] == 'c' && en->d_name[length - 2] == '.'))
            continue;
        #ifdef WIN32
            if (!first_run) {
                char *copied = strdup(src_dir);
                sprintf(src_dir, "%s %s\\%s", copied, bin->srcdir, en->d_name);
                free(copied);
            } else
                sprintf(src_dir, "%s\\%s", bin->srcdir, en->d_name);
        #else
            if (!first_run) {
                char *copied = strdup(src_dir);
                sprintf(src_dir, "%s %s/%s", copied, bin->srcdir, en->d_name);
                free(copied);
            } else
                sprintf(src_dir, "%s/%s", bin->srcdir, en->d_name);
        #endif
        first_run = false;
    }
    #ifdef DEBUG
        printf("SRC_FILES: %s\n", src_dir);
    #endif
    return src_dir;
}

// Finds the size of the resulting args
// NOTE: DOES NOT FIND THE LENGTH, IT FINDS THE SIZE
// Another NOTE: does not include null terminator
usize find_size(char *cmd, package_t *project, bin_t *bin) {
    usize count_targetdir = count_string(cmd, "{targetdir}");
    usize count_binname = count_string(cmd, "{binname}");
    usize count_srcdir = count_string(cmd, "{srcdir}");
    usize count_includedir = count_string(cmd, "{includedir}");
    usize count_programincludedir = count_string(cmd, "{programincludedir}");
    usize count_ccargs = count_string(cmd, "{ccargs}");
    usize count_ldargs = count_string(cmd, "{ldargs}");
    usize count_projectname = count_string(cmd, "{projectversion}");
    usize count_projectversion = count_string(cmd, "{projectname}");

    usize length = 0;

    length += count_targetdir * strlen(bin->targetdir);
    length += count_binname * strlen(bin->name);
    length += count_srcdir * strlen(bin->srcdir);
    length += count_includedir * strlen(bin->includedir);
    length += count_programincludedir * strlen(bin->programincludedir);
    length += count_ccargs * strlen(bin->ccargs);
    length += count_ldargs * strlen(bin->ldargs);
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
    while ((tmp = strstr(tmp, str2))) {
        count++;
        tmp++;
    }
    return count;
}
