/**   
 ***************************************************************
 * @file    mylib/log/include/cli_log.c    
 * @author  Lewis Ainslie - s4485827  
 * @date    08/03/2021   
 * @brief   Log cli src file    
 ***************************************************************
 * EXTERNAL FUNCTIONS
***************************************************************
* log_cli_init () - initialise cli for logging
***************************************************************
 */
#include "cli_fan.h"
#include "hal_fan.h"
#include "os_registry.h"
#include "cli_peripheral.h"



static int cmd_handle_fan(const struct shell *shell, size_t argc, char **argv)
{
	int ret;

    argv++;
    argc--;

    if (!strcmp(*argv, "set")) {
		argv++;
        ret += cli_peripheral_handle_set(hal_set_fans, shell, argc, argv);
    } else if (!strcmp(*argv, "get")) {
		registry_get comms = {.peripheral_type = FAN};

		ret += cli_peripheral_handle_get(&comms, shell, argc, argv);
	}

    return 0;
}


SHELL_CMD_ARG_REGISTER(fan, NULL, "Fan functionality", cmd_handle_fan, 3, 2);