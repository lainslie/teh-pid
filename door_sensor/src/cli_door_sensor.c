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
#include "cli_door_sensor.h"
#include "hal_door_sensor.h"
#include "os_registry.h"
#include "cli_peripheral.h"



static int cmd_handle_door_sensor(const struct shell *shell, size_t argc, char **argv)
{
	int ret;

    argv++;
    argc--;

    if (!strcmp(*argv, "set")) {
        shell_print(shell, "This peripheral does not support setting \n");
        return -1;
    } else if (!strcmp(*argv, "get")) {
		registry_get comms = {.peripheral_type = DOOR};

		ret += cli_peripheral_handle_get(&comms, shell, argc, argv);
	}

    return 0;
}


SHELL_CMD_ARG_REGISTER(door, NULL, "Door functionality", cmd_handle_door_sensor, 2, 1);