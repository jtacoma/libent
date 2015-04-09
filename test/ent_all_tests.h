/* This file was automatically generated;
 * changes made here will be lost.
 */
#ifndef ALL_TESTS_H
#define ALL_TESTS_H

int alloc_counts_allocations (void);
int alloc_fails_when_told_to_fail (void);
int alloc_with_null_pptr_sets_einval (void);
int new_array_has_specified_width (void);
int new_array_is_empty (void);
int resized_array_is_set_to_zero (void);
int resized_array_retains_data (void);
int copied_array_keeps_original_data (void);
int null_array_sets_einval (void);
int truncated_and_shrunk_array_returns_null (void);
int array_can_shrink_safely (void);
int processor_invalid_argument_sets_einval (void);
int new_rlist_is_empty (void);
int null_rlist_sets_einval (void);
int rlist_cannot_hold_inverted_ranges (void);
int rlist_general_tests (void);
int session_supports_deletion (void);
int session_supports_insertion (void);
int session_invalid_argument_sets_einval (void);
int session_general_test (void);
int table_null_sets_einval (void);
int new_table_is_empty (void);
int table_can_grow_by_zero (void);
int table_retrieves_column_info (void);
int table_sets_einval_for_invalid_column_id (void);
int table_sets_einval_when_deleting_beyond_end (void);
int table_insertion_handles_out_of_memory (void);
int table_deletion_edge_cases (void);
int table_general_test (void);

static struct
{
	char const * name;
	int (* function) (void);
} all_mem_tests[] =
{
	{ .name = "alloc_counts_allocations", .function = alloc_counts_allocations },
	{ .name = "alloc_fails_when_told_to_fail", .function = alloc_fails_when_told_to_fail },
	{ .name = "alloc_with_null_pptr_sets_einval", .function = alloc_with_null_pptr_sets_einval },
	{ .name = "new_array_has_specified_width", .function = new_array_has_specified_width },
	{ .name = "new_array_is_empty", .function = new_array_is_empty },
	{ .name = "resized_array_is_set_to_zero", .function = resized_array_is_set_to_zero },
	{ .name = "resized_array_retains_data", .function = resized_array_retains_data },
	{ .name = "copied_array_keeps_original_data", .function = copied_array_keeps_original_data },
	{ .name = "null_array_sets_einval", .function = null_array_sets_einval },
	{ .name = "truncated_and_shrunk_array_returns_null", .function = truncated_and_shrunk_array_returns_null },
	{ .name = "array_can_shrink_safely", .function = array_can_shrink_safely },
	{ .name = "processor_invalid_argument_sets_einval", .function = processor_invalid_argument_sets_einval },
	{ .name = "new_rlist_is_empty", .function = new_rlist_is_empty },
	{ .name = "null_rlist_sets_einval", .function = null_rlist_sets_einval },
	{ .name = "rlist_cannot_hold_inverted_ranges", .function = rlist_cannot_hold_inverted_ranges },
	{ .name = "rlist_general_tests", .function = rlist_general_tests },
	{ .name = "session_supports_deletion", .function = session_supports_deletion },
	{ .name = "session_supports_insertion", .function = session_supports_insertion },
	{ .name = "session_invalid_argument_sets_einval", .function = session_invalid_argument_sets_einval },
	{ .name = "session_general_test", .function = session_general_test },
	{ .name = "table_null_sets_einval", .function = table_null_sets_einval },
	{ .name = "new_table_is_empty", .function = new_table_is_empty },
	{ .name = "table_can_grow_by_zero", .function = table_can_grow_by_zero },
	{ .name = "table_retrieves_column_info", .function = table_retrieves_column_info },
	{ .name = "table_sets_einval_for_invalid_column_id", .function = table_sets_einval_for_invalid_column_id },
	{ .name = "table_sets_einval_when_deleting_beyond_end", .function = table_sets_einval_when_deleting_beyond_end },
	{ .name = "table_insertion_handles_out_of_memory", .function = table_insertion_handles_out_of_memory },
	{ .name = "table_deletion_edge_cases", .function = table_deletion_edge_cases },
	{ .name = "table_general_test", .function = table_general_test },
};

#endif
