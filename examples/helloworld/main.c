/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>

#define RTE_LOGTYPE_HELLOAPP RTE_LOGTYPE_USER1

static int
lcore_hello(__attribute__((unused)) void *arg)
{
	unsigned lcore_id;
	lcore_id = rte_lcore_id();
	printf("hello from core %u\n", lcore_id);
	return 0;
}

static void
get_configuration(void)
{
	struct rte_config *c = rte_eal_get_configuration();
	if (!c) {
		RTE_LOG(EMERG, HELLOAPP, "can't get eal configuration\n");
		rte_exit(EXIT_FAILURE, "error with get configuration\n");
	}
	RTE_LOG(INFO, HELLOAPP, "master core: %u\n", c->master_lcore);
	RTE_LOG(INFO, HELLOAPP, "lcore count: %u\n", c->lcore_count);
	RTE_LOG(INFO, HELLOAPP, "service lcore count: %u\n", c->service_lcore_count);
}

static void
get_process_type(void)
{
	enum rte_proc_type_t proc_type = rte_eal_process_type();
	RTE_LOG(INFO, HELLOAPP, "process type: %d\n", proc_type);
}

int
main(int argc, char **argv)
{
	int ret;
	unsigned lcore_id;

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	get_configuration();
	get_process_type();

	/* call lcore_hello() on every slave lcore */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_hello, NULL, lcore_id);
	}

	/* call it on master lcore too */
	lcore_hello(NULL);

	rte_eal_mp_wait_lcore();
	RTE_LOG(INFO, HELLOAPP, "primary proc is alive: %d\n", rte_eal_primary_proc_alive(NULL));
	RTE_LOG(INFO, HELLOAPP, "has huge pages: %d\n", rte_eal_has_hugepages());
	RTE_LOG(INFO, HELLOAPP, "has huge pages: %d\n", rte_eal_has_hugepages());

	return 0;
}
