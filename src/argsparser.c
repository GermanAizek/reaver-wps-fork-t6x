/*
 * Reaver - Command line processing functions
 * Copyright (c) 2011, Tactical Network Solutions, Craig Heffner <cheffner@tacnetsol.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of portions of this program with the
 *  OpenSSL library under certain conditions as described in each
 *  individual source file, and distribute linked combinations
 *  including the two.
 *  You must obey the GNU General Public License in all respects
 *  for all of the code used other than OpenSSL. *  If you modify
 *  file(s) with this exception, you may extend this exception to your
 *  version of the file(s), but you are not obligated to do so. *  If you
 *  do not wish to do so, delete this exception statement from your
 *  version. *  If you delete this exception statement from all source
 *  files in the program, then also delete it here.
 */

#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include <fcntl.h>
#include "globule.h"
#include "defs.h"
#include "iface.h"
#include "argsparser.h"
#include "pixie.h"
#include "misc.h"

/* Processes Reaver command line options */
int process_arguments(int argc, char **argv)
{
	int ret_val = EXIT_SUCCESS;
	int c = 0, channel = 0;
	int long_opt_index = 0;
	char bssid[MAC_ADDR_LEN] = { 0 };
	char mac[MAC_ADDR_LEN] = { 0 };
	char *short_options = "b:e:m:i:t:d:c:T:x:r:g:l:p:s:C:O:KZA5ELfnqvDShwN6JFuM";
	struct option long_options[] = {
		{ "pixie-dust", no_argument, NULL, 'K' },
		{ "interface", required_argument, NULL, 'i' },
		{ "bssid", required_argument, NULL, 'b' },
		{ "essid", required_argument, NULL, 'e' },
		{ "mac", required_argument, NULL, 'm' },
		{ "timeout", required_argument, NULL, 't' },
		{ "m57-timeout", required_argument, NULL, 'T' },
		{ "delay", required_argument, NULL, 'd' },
		{ "lock-delay", required_argument, NULL, 'l' },
		{ "fail-wait", required_argument, NULL, 'x' },
		{ "channel", required_argument, NULL, 'c' },
		{ "session", required_argument, NULL, 's' },
		{ "recurring-delay", required_argument, NULL, 'r' },
		{ "max-attempts", required_argument, NULL, 'g' },
		{ "pin", required_argument, NULL, 'p' },
		{ "exec", required_argument, NULL, 'C' },
		{ "no-associate", no_argument, NULL, 'A' },
		{ "ignore-locks", no_argument, NULL, 'L' },
		{ "no-nacks", no_argument, NULL, 'N' },
		{ "eap-terminate", no_argument, NULL, 'E' },
		{ "dh-small", no_argument, NULL, 'S' },
		{ "fixed", no_argument, NULL, 'f' },
		{ "5ghz", no_argument, NULL, '5' },
		{ "repeat-m6", no_argument, NULL, '6' },
		{ "nack", no_argument, NULL, 'n' },
		{ "quiet", no_argument, NULL, 'q' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "win7", no_argument, NULL, 'w' },
		{ "help", no_argument, NULL, 'h' },
		{ "timeout-is-nack", no_argument, NULL, 'J' },
		{ "ignore-fcs", no_argument, NULL, 'F' },
		{ "output-file", required_argument, NULL, 'O'},
		{ "mac-changer", no_argument, NULL, 'M' },
		{ 0, 0, 0, 0 }
	};

	set_output_fd(-1);

	/* Since this function may be called multiple times, be sure to set opt index to 0 each time */
	optind = 0;

	while((c = getopt_long(argc, argv, short_options, long_options, &long_opt_index)) != -1)
        {
                switch(c)
                {
			case 'O':
				{
					int ofd = open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0660);
					set_output_fd(ofd);
					if(ofd == -1) perror("open outputfile failed: ");
				}
				break;
                        case 'Z':
                        case 'K':
				pixie.do_pixie = 1;
				set_max_pin_attempts(1);
				break;
			case 'u':
				pixie.use_uptime = 1;
				break;
                        case 'i':
                                set_iface(optarg);
                                break;
                        case 'b':
                                str2mac(optarg, (unsigned char *) &bssid);
                                set_bssid((unsigned char *) &bssid);
                                break;
                        case 'e':
                                set_ssid(optarg);
                                break;
                        case 'm':
                                str2mac(optarg, (unsigned char *) &mac);
                                set_mac((unsigned char *) &mac);
                                break;
                        case 't':
                                set_rx_timeout(atoi(optarg));
                                break;
                        case 'T':
                                set_m57_timeout(strtof(optarg, NULL) * SEC_TO_US);
                                break;
                        case 'c':
				channel = strtod(optarg, NULL);
                                set_fixed_channel(1);
                                break;
                        case '5':
                                set_wifi_band(AN_BAND);
                                break;
                        case '6':
                                set_repeat_m6(1);
                                break;
                        case 'd':
                                set_delay(atoi(optarg));
                                break;
                        case 'l':
                                set_lock_delay(atoi(optarg));
                                break;
			case 'p':
				parse_static_pin(optarg);
				set_max_pin_attempts(1);
				break;
			case 's':       
				set_session(optarg);   
				break;
			case 'C':
				set_exec_string(optarg);
				break;
			case 'A':
				set_external_association(1);
				break;
                        case 'L':
                                set_ignore_locks(1);
                                break;
                        case 'x':
                                set_fail_delay(atoi(optarg));
                                break;
                        case 'r':
                                parse_recurring_delay(optarg);
                                break;
                        case 'g':
                                set_max_pin_attempts(atoi(optarg));
                                break;
			case 'E':
                                set_eap_terminate(1);
                                break;
			case 'S':
				set_dh_small(1);
				break;
                        case 'n':
				cprintf(INFO, "[+] ignoring obsolete -n switch\n");
				break;
			case 'J':
                                set_timeout_is_nack(1);
                                break;
                        case 'f':
                                set_fixed_channel(1);
                                break;
                        case 'v':
                                set_debug(get_debug() + 1);
                                break;
                        case 'q':
                                set_debug(CRITICAL);
                                break;
			case 'w':
				set_win7_compat(1);
				break;
			case 'N':
				set_oo_send_nack(0);
				break;
			case 'F':
				set_validate_fcs(0);
				break;
			case 'M':
				set_mac_changer(1);
				break;
                        default:
                                ret_val = EXIT_FAILURE;
                }
        }

	if(channel)
	{
		change_channel(channel);
	}

	return ret_val;
}

/* Initialize some basic config settings */
void init_default_settings(void)
{
	set_log_file(stdout);
	set_max_pin_attempts(P1_SIZE + P2_SIZE);
        set_delay(DEFAULT_DELAY);
        set_lock_delay(DEFAULT_LOCK_DELAY);
        set_debug(INFO);
        set_auto_channel_select(1);
        set_timeout_is_nack(0);
	set_oo_send_nack(1);
        set_wifi_band(BG_BAND);
	set_validate_fcs(1);
	pixie.do_pixie = 0;
	set_pin_string_mode(0);
	set_mac_changer(0);
}

/* Parses the recurring delay optarg */
void parse_recurring_delay(char *arg)
{
        char *x = NULL, *y = NULL;

        x = strdup(arg);
        y = strchr(x, ':');

        if(y)
        {
                memset(y, 0, 1);
                y++;

                set_recurring_delay_count(atoi(x));
                set_recurring_delay(atoi(y));
        }

        free(x);
}

int is_valid_pin(char *pin)
{
    if(!pin)
        return 0;

    int i;
	size_t len = strlen(pin);
    for (i = 0; i < len; i++)
    {
         if(!isdigit(pin[i]))
             return 0;
    }
    if(strlen(pin) == 8)
    {
        char pin7[8] = { 0 };
        char pin8[9] = { 0 };
        memcpy((void *) &pin7, pin, sizeof(pin7)-1);
        snprintf(pin8, 9, "%s%d", pin7, wps_pin_checksum(atoi(pin7)));
        if (strcmp(pin, pin8) != 0)
            return 0;
    }
    return 1;
}

/* Parse the WPS pin to use into p1 and p2 */
void parse_static_pin(char *pin)
{
	int len = 0;
	char p1[5] = { 0 };
	char p2[4] = { 0 };

	len = strlen(pin);

	if((len == 4 || len == 7 || len == 8) && is_valid_pin(pin) != 0)
	{
		memcpy((void *) &p1, pin, sizeof(p1)-1);
		set_static_p1((char *) &p1);

		if(len > 4)
		{
			memcpy((void *) &p2, pin+sizeof(p1)-1, sizeof(p2)-1);
			set_static_p2((char *) &p2);
		}
	}
	else
	{
		set_pin_string_mode(1);
		set_static_p1(pin);
	}
}

