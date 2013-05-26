/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*-
 * SILT: A Memory-Efficient, High-Performance Key-Value Store
 *
 * Copyright © 2011 Carnegie Mellon University
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
 * EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. THIS SOFTWARE IS
 * PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * GOVERNMENT USE: If you are acquiring this software on behalf of the
 * U.S. government, the Government shall have only "Restricted Rights" in the
 * software and related documentation as defined in the Federal Acquisition
 * Regulations (FARs) in Clause 52.227.19 (c) (2). If you are acquiring the
 * software on behalf of the Department of Defense, the software shall be
 * classified as "Commercial Computer Software" and the Government shall have
 * only "Restricted Rights" as defined in Clause 252.227-7013 (c) (1) of
 * DFARs. Notwithstanding the foregoing, the authors grant the U.S. Government
 * and others acting in its behalf permission to use and distribute the
 * software in accordance with the terms specified in this license.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//for time
#include <time.h>
#include <sys/time.h>

#include "debug.h"


unsigned int debug = 0;

/* We could autogenerate this if we felt like it */

struct debug_def {
    int debug_val;
    char *debug_def;
};

static
struct debug_def debugs[] = {
#include "debug-text.h"
    { 0, NULL } /* End of list marker */
};

int set_debug(char *arg)
{
    int i;
    if (!arg || arg[0] == '\0') {
	return -1;
    }

    if (arg[0] == '?' || !strcmp(arg, "list")) {
	fprintf(stderr,
		"Debug values and definitions\n"
		"----------------------------\n");
	for (i = 0;  debugs[i].debug_def != NULL; i++) {
	    fprintf(stderr, "%5d  %s\n", debugs[i].debug_val,
		    debugs[i].debug_def);
	}
        fprintf(stderr, "\n\'all\' will enable all debug flags.\n");
	return -1;
    }
    if (!strcmp(arg, "all")) {
        debug = 0xffffffff;
        return 0;
    }

    if (isdigit(arg[0])) {
	debug |= atoi(arg);
    }
    return 0;
}
