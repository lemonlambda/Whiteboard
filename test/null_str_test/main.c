#include <assert.h>
#include <string.h>

char *null_str(const char **value) {
    return (value ? (*value ? (char *)*value : "NULL") : "NULL");
}

int main() {
    char *should_be_null = null_str((const char **)NULL);
    assert(strcmp(should_be_null, "NULL") == 0);
    const char *reffed = "NOT NULL";
    char *shouldnt_be_null = null_str(&reffed);
    assert(strcmp(should_be_null, "NOT NULL") == 0);

    return 0;
}
