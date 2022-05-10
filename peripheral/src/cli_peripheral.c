#include "cli_peripheral.h"
#include "os_peripheral.h"
#include "os_registry.h"
#include <string.h>

int cli_peripheral_handle_get(registry_get *get_info, const struct shell *shell, size_t argc, char **argv) {
    char *opt = *argv;
    argv++;

    printk("opt: %s \n", opt);

    if (!strcmp(opt, "system")) {
        get_info->request_type = SUBSYSTEM_HISTORY;
    } else if (!strcmp(opt, "device")) {
        get_info->request_type = PERIPHERAL_HISTORY;
        get_info->peripheral_id = atoi(*argv);
    } else if (!strcmp(opt, "recent")) {
        get_info->request_type = SINGLE_VALUE;
        get_info->peripheral_id = atoi(*argv);
    } else {
        shell_print(shell, "Bad command \n");
        return -1;
    }

    get_registry(get_info);
    return 0;
}
int cli_peripheral_handle_set(int (*hal_driver)(int, int), const struct shell *shell, size_t argc, char **argv) {
    char *opt = *argv;
    int mode;
    int set_value;

    argc--;
    argv++;

    if (!strcmp(opt, "system")) {
        mode = -1;
    } else if (!strcmp(opt, "device")) {
        mode = atoi(*argv);
        argc--;
        argv++;
    } else {
        shell_print(shell, "incorrect args \n");
        return -1;
    }

    set_value = atoi(*argv);

    if (hal_driver(mode, set_value)) {
        shell_print(shell, "HAL Function failed \n");
        return -1;
    }
    return 0;
}