/* This file was automatically generated;
 * changes made here will be lost.
 */
#ifndef ALL_TESTS_H
#define ALL_TESTS_H

void alloc_counts_allocations (void);
void alloc_fails_when_told_to_fail (void);
void null_ptr_set_einval (void);
void invalid_argument_sets_errno (void);
void invalid_session_argument_sets_errno (void);
void null_table_sets_einval (void);
void new_table_is_empty (void);
void adding_zero_length_is_ok (void);
void column_info_can_be_retrieved (void);
void invalid_column_id_sets_einval (void);
void deleting_beyond_end_of_table_set_einval (void);
int new_array_has_specified_width (void);
int new_array_is_empty (void);
int resized_array_is_set_to_zero (void);
int resized_array_retains_data (void);
int copied_array_keeps_original_data (void);
int invalid_arguments_set_einval (void);
int truncated_and_shrunk_array_returns_null (void);
int array_can_shrink_safely (void);
int new_rlist_is_empty (void);
int null_rlist_sets_einval (void);
int inverted_range_is_invalid (void);
int rlist_general_tests (void);
int session_supports_deletion (void);
int session_supports_insertion (void);
int session_general_test (void);
int insertion_handles_out_of_memory (void);
int table_deletion_edge_cases (void);
int table_general_test (void);

static struct
{
	char const * name;
	void (* function) (void);
} all_void_tests[] =
{
	{ .name = "alloc_counts_allocations", .function = alloc_counts_allocations },
	{ .name = "alloc_fails_when_told_to_fail", .function = alloc_fails_when_told_to_fail },
	{ .name = "null_ptr_set_einval", .function = null_ptr_set_einval },
	{ .name = "invalid_argument_sets_errno", .function = invalid_argument_sets_errno },
	{ .name = "invalid_session_argument_sets_errno", .function = invalid_session_argument_sets_errno },
	{ .name = "null_table_sets_einval", .function = null_table_sets_einval },
	{ .name = "new_table_is_empty", .function = new_table_is_empty },
	{ .name = "adding_zero_length_is_ok", .function = adding_zero_length_is_ok },
	{ .name = "column_info_can_be_retrieved", .function = column_info_can_be_retrieved },
	{ .name = "invalid_column_id_sets_einval", .function = invalid_column_id_sets_einval },
	{ .name = "deleting_beyond_end_of_table_set_einval", .function = deleting_beyond_end_of_table_set_einval },
};

static struct
{
	char const * name;
	int (* function) (void);
} all_mem_tests[] =
{
	{ .name = "new_array_has_specified_width", .function = new_array_has_specified_width },
	{ .name = "new_array_is_empty", .function = new_array_is_empty },
	{ .name = "resized_array_is_set_to_zero", .function = resized_array_is_set_to_zero },
	{ .name = "resized_array_retains_data", .function = resized_array_retains_data },
	{ .name = "copied_array_keeps_original_data", .function = copied_array_keeps_original_data },
	{ .name = "invalid_arguments_set_einval", .function = invalid_arguments_set_einval },
	{ .name = "truncated_and_shrunk_array_returns_null", .function = truncated_and_shrunk_array_returns_null },
	{ .name = "array_can_shrink_safely", .function = array_can_shrink_safely },
	{ .name = "new_rlist_is_empty", .function = new_rlist_is_empty },
	{ .name = "null_rlist_sets_einval", .function = null_rlist_sets_einval },
	{ .name = "inverted_range_is_invalid", .function = inverted_range_is_invalid },
	{ .name = "rlist_general_tests", .function = rlist_general_tests },
	{ .name = "session_supports_deletion", .function = session_supports_deletion },
	{ .name = "session_supports_insertion", .function = session_supports_insertion },
	{ .name = "session_general_test", .function = session_general_test },
	{ .name = "insertion_handles_out_of_memory", .function = insertion_handles_out_of_memory },
	{ .name = "table_deletion_edge_cases", .function = table_deletion_edge_cases },
	{ .name = "table_general_test", .function = table_general_test },
};

#endif
