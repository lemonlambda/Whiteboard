#pragma once

#include "rust_types.h"
#include "vector.h"
#include "toml_format.h"

// @desc		Structure holding information about a command
// @field(name)		Human-friendly name of the command
// @field(command)	Command to run, with replacement strings
typedef struct command {
    char *name;
    char *command;
} command_t;

// @desc		Create a new command from `name` and `cmd`. Both will be run through `strdup`!
// @arg(name)		Human-friendly name of the command
// @arg(command)	Command to run, with replacement strings
// @returns		New `command_t` with those components
command_t new_command(char *name, char *cmd);
// @desc	Frees up allocations caused by `strdup()` of `name` and `command`
// @arg(cmd)	`command_t` to clean-up after
void free_command(command_t cmd);

// @desc		Struct with staged commands
// @field(stage_name)	Name of stage
// @field(commands)	Vector of commands to be run in order for this stage
// @field(callbacks)	Container for callback functions
// @field(callbacks.add_stage)	Function that adds a command to the stage
// @field(callbacks.run_stages)	Function that runs a stage, with package and bin information.
typedef struct stage {
    char *stage_name;
    // Vector of command_t *
    vector_t commands;

    struct {
        void (*add_stage)(struct stage *, command_t );
        void (*run_stages)(struct stage *, package_t *, bin_t *);
    } callbacks;
} stage_t;

// @desc	Create a new stage
// @arg(name)	Name for this stage, must be non-null
// @returns	A default `stage_t`
stage_t init_stage(char *name);
// @desc	Creates a stage for running
// @returns	`stage_t` set-up for run mode
stage_t run_stage();
// @desc	Creates a stage for building
// @arg(def)	Custom name of stage if non-null
// @returns	`stage_t` set-up for build mode
stage_t build_stage(char *def);
// @desc	Creates a stage for cleaning
// @returns	`stage_t` set-up for clean mode
stage_t clean_stage();
// @desc	Creates a stage for testing
// @returns	`stage_t` set-up for test mode
stage_t test_stage();

// @desc	Adds a `command_t` to a `stage_t`
// @arg(self)	Stage to affect
// @arg(cmd)	Command to add
void add_stage(stage_t *self, command_t cmd);
// @desc		Runs a stage with project/package and bin info
// @arg(self)		Stage to run
// @arg(project)	Project/package info
// @arg(bin)		Bin info for compilation
void run_stages(stage_t *self, package_t *project, bin_t *bin);

// @desc	Cleans up allocations from a `stage_t`
// @arg(self)	`stage_t` to clean-up after.
void free_stage(stage_t self);


// @desc	Gets all the source files in one big string using the bin info.
// @arg(bin)	`bin_t` to read
// @returns	Space-delimited list of all source files
char *get_source_files(bin_t* bin);
// @desc		Replaces all the substitution strings in a command with package/bin fields
// @arg(cmd)		Command to read from
// @arg(package)	Package/project config
// @arg(bin)		Binary config
// @returns		Command with substitution strings filled in
char *replace_args(command_t *cmd, package_t *package, bin_t *bin);
// @desc		Finds the size (not length) of `cmd` post-substitution, without doing any substituting.
// @arg(cmd)		Command to search for substitution strings in
// @arg(project)	Project config
// @arg(bin)		Bin config
// @returns		Size of `cmd` had all the substitution strings been filled in
// @note		Does not include null-terminator
// @note		Reads size, not length
usize find_size(char *cmd, package_t *project, bin_t *bin);
// @desc		Counts instances of `needle` in `haystack`
// @arg(haystack)	String to search in
// @arg(needle)		String to search for
// @returns		Count of instances of `needle` in `haystack`
usize count_string(const char *haystack, const char *needle);
