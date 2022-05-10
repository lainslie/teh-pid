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
#include "cli_light_sensor.h"
#include "os_light_sensor.h"
#include "os_registry.h"
#include "cli_peripheral.h"


static int cmd_handle_light_sensor(const struct shell *shell, size_t argc, char **argv)
{
    int ret;
    char *opt;

    argv++;
    argc--;

    opt = *argv;
    argv++;

    if (!strcmp(opt, "set")) {
		shell_print(shell, "This peripheral does not support setting \n");
        return -1;
    } else if (!strcmp(opt, "get")) {
		registry_get get_info = {.peripheral_type = LIGHT_SENSOR};
		ret += cli_peripheral_handle_get(&get_info, shell, argc, argv);
	} else {
        shell_print(shell, "Invalid command \n");
        return -1;
    }

    return ret;
}


SHELL_CMD_ARG_REGISTER(light_sensor, NULL, "Light sensor functionality", cmd_handle_light_sensor, 3, 2);