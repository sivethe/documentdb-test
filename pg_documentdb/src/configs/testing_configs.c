/*-------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 * src/configs/feature_flag_configs.c
 *
 * Initialization of GUCs that change behavior that will only be used
 * in tests.
 *-------------------------------------------------------------------------
 */

#include <postgres.h>
#include <miscadmin.h>
#include <utils/guc.h>
#include <limits.h>

#include "metadata/metadata_guc.h"
#include "configs/config_initialization.h"

#define BSON_MAX_ALLOWED_SIZE (16 * 1024 * 1024)

#define DEFAULT_NEXT_COLLECTION_ID NEXT_COLLECTION_ID_UNSET
int NextCollectionId = DEFAULT_NEXT_COLLECTION_ID;

#define DEFAULT_NEXT_COLLECTION_INDEX_ID NEXT_COLLECTION_INDEX_ID_UNSET
int NextCollectionIndexId = DEFAULT_NEXT_COLLECTION_INDEX_ID;

#define DEFAULT_SIMULATE_RECOVERY_STATE false
bool SimulateRecoveryState = DEFAULT_SIMULATE_RECOVERY_STATE;

#define DEFAULT_ENABLE_GENERATE_NON_EXISTS_TERM true
bool EnableGenerateNonExistsTerm = DEFAULT_ENABLE_GENERATE_NON_EXISTS_TERM;

#define DEFAULT_INDEX_TRUNCATION_LIMIT_OVERRIDE INT_MAX
int IndexTruncationLimitOverride = DEFAULT_INDEX_TRUNCATION_LIMIT_OVERRIDE;

#define FORCE_ENABLE_NEW_UNIQUE_OPCLASS false
bool ForceEnableNewUniqueOpClass = FORCE_ENABLE_NEW_UNIQUE_OPCLASS;

#define DEFAULT_ENABLE_CURSORS_ON_AGGREGATION_QUERY_REWRITE false
bool EnableCursorsOnAggregationQueryRewrite =
	DEFAULT_ENABLE_CURSORS_ON_AGGREGATION_QUERY_REWRITE;

#define DEFAULT_UNIQUE_INDEX_KEYHASH_OVERIDE 0
int DefaultUniqueIndexKeyhashOverride = DEFAULT_UNIQUE_INDEX_KEYHASH_OVERIDE;

#define DEFAULT_USE_LOCAL_EXECUTION_SHARD_QUERIES true
bool UseLocalExecutionShardQueries = DEFAULT_USE_LOCAL_EXECUTION_SHARD_QUERIES;

#define DEFAULT_FORCE_INDEX_TERM_TRUNCATION false
bool ForceIndexTermTruncation = DEFAULT_FORCE_INDEX_TERM_TRUNCATION;

#define DEFAULT_MAX_WORKER_CURSOR_SIZE BSON_MAX_ALLOWED_SIZE
int32_t MaxWorkerCursorSize = DEFAULT_MAX_WORKER_CURSOR_SIZE;

void
InitializeTestConfigurations(const char *prefix, const char *newGucPrefix)
{
	DefineCustomIntVariable(
		psprintf("%s.next_collection_id", newGucPrefix),
		gettext_noop("Set the next collection id to use when creationing a collection."),
		gettext_noop("Collection ids are normally generated using a sequence. If "
					 "next_collection_id is set to a value different than "
					 "DEFAULT_NEXT_COLLECTION_ID, then collection will instead be "
					 "generated by incrementing from the value of this GUC and this "
					 "will be reflected in the GUC. This is mainly useful to ensure "
					 "consistent collection ids when running tests in parallel."),
		&NextCollectionId,
		DEFAULT_NEXT_COLLECTION_ID, DEFAULT_NEXT_COLLECTION_ID, INT_MAX,
		PGC_USERSET,
		GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
		NULL, NULL, NULL);

	DefineCustomIntVariable(
		psprintf("%s.next_collection_index_id", newGucPrefix),
		gettext_noop("Set the next collection index id to use when creating a "
					 "collection index."),
		gettext_noop("Collection index ids are normally generated using a sequence. "
					 "If next_collection_index_id is set to a value different than "
					 "DEFAULT_NEXT_COLLECTION_INDEX_ID, then collection index ids "
					 "will instead be generated by incrementing from the value of "
					 "this GUC and this will be reflected in the GUC. This is mainly "
					 "useful to ensure consistent collection index ids when running "
					 "tests in parallel."),
		&NextCollectionIndexId,
		DEFAULT_NEXT_COLLECTION_INDEX_ID, DEFAULT_NEXT_COLLECTION_INDEX_ID, INT_MAX,
		PGC_USERSET,
		GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
		NULL, NULL, NULL);

	DefineCustomBoolVariable(
		psprintf("%s.simulateRecoveryState", prefix),
		gettext_noop(
			"Simulates a database recovery state and throws an error for read-write operations."),
		NULL, &SimulateRecoveryState, DEFAULT_SIMULATE_RECOVERY_STATE,
		PGC_USERSET, 0, NULL, NULL, NULL);

	/* Added variable for testing cursor continuations */
	DefineCustomIntVariable(
		psprintf("%s.maxWorkerCursorSize", prefix),
		gettext_noop(
			"The maximum size a single cursor response page should be in a worker."),
		NULL, &MaxWorkerCursorSize,
		DEFAULT_MAX_WORKER_CURSOR_SIZE, 1, BSON_MAX_ALLOWED_SIZE,
		PGC_USERSET,
		GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
		NULL, NULL, NULL);

	DefineCustomBoolVariable(
		psprintf("%s.enableCursorsOnAggregationQueryRewrite", newGucPrefix),
		gettext_noop(
			"Whether or not to add the cursors on aggregation style queries."),
		NULL,
		&EnableCursorsOnAggregationQueryRewrite,
		DEFAULT_ENABLE_CURSORS_ON_AGGREGATION_QUERY_REWRITE,
		PGC_USERSET,
		0,
		NULL, NULL, NULL);

	DefineCustomBoolVariable(
		psprintf("%s.enableGenerateNonExistsTerm", newGucPrefix),
		gettext_noop(
			"Enables generating the non exists term for new documents in a collection."),
		NULL, &EnableGenerateNonExistsTerm, DEFAULT_ENABLE_GENERATE_NON_EXISTS_TERM,
		PGC_USERSET, 0, NULL, NULL, NULL);

	DefineCustomBoolVariable(
		psprintf("%s.forceIndexTermTruncation", prefix),
		gettext_noop(
			"Whether to force the feature for index term truncation"),
		NULL, &ForceIndexTermTruncation, DEFAULT_FORCE_INDEX_TERM_TRUNCATION,
		PGC_USERSET, 0, NULL, NULL, NULL);

	/* Deprecated, test only. Do not use in prod. */
	DefineCustomBoolVariable(
		psprintf("%s.force_enable_new_unique_opclass", newGucPrefix),
		gettext_noop(
			"Testing GUC on Whether or not to enable the new opclass for large index keys on unique indexes."),
		NULL, &ForceEnableNewUniqueOpClass, FORCE_ENABLE_NEW_UNIQUE_OPCLASS,
		PGC_USERSET, 0, NULL, NULL, NULL);

	DefineCustomIntVariable(
		psprintf("%s.indexTermLimitOverride", prefix),
		gettext_noop(
			"Override for the index term truncation limit (primarily for tests)."),
		NULL, &IndexTruncationLimitOverride,
		DEFAULT_INDEX_TRUNCATION_LIMIT_OVERRIDE, 1, INT_MAX,
		PGC_USERSET,
		GUC_NO_SHOW_ALL | GUC_NOT_IN_SAMPLE,
		NULL, NULL, NULL);

	DefineCustomBoolVariable(
		psprintf("%s.useLocalExecutionShardQueries", newGucPrefix),
		gettext_noop(
			"Determines whether or not to push local shard queries to the shard directly."),
		NULL, &UseLocalExecutionShardQueries, DEFAULT_USE_LOCAL_EXECUTION_SHARD_QUERIES,
		PGC_USERSET, 0, NULL, NULL, NULL);

	DefineCustomIntVariable(
		psprintf("%s.defaultUniqueIndexKeyhashOverride", newGucPrefix),
		gettext_noop(
			"Do not set this in production. GUC used to force a single keyhash result value for testing hash conflicts on unique indexes that require a runtime recheck."),
		NULL, &DefaultUniqueIndexKeyhashOverride,
		DEFAULT_UNIQUE_INDEX_KEYHASH_OVERIDE, 0, INT_MAX,
		PGC_USERSET,
		0,
		NULL, NULL, NULL);
}
