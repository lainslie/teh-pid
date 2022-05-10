#ifndef _CLI_PERIPHERAL_H
#define _CLI_PERIPHERAL_H

#include <stdlib.h>
#include <string.h>
#include <zephyr.h>
#include <shell/shell.h>

#include "os_log.h"
#include "os_registry.h"

int cli_peripheral_handle_get(registry_get *get_info, const struct shell *shell, size_t argc, char **argv);
int cli_peripheral_handle_set(int (*hal_driver)(int, int), const struct shell *shell, size_t argc, char **argv);

#endif