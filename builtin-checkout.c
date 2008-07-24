begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"unpack-trees.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"merge-recursive.h"
end_include
begin_include
include|#
directive|include
file|"branch.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_decl_stmt
DECL|variable|checkout_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|checkout_usage
index|[]
init|=
block|{
literal|"git checkout [options]<branch>"
block|,
literal|"git checkout [options] [<branch>] --<file>..."
block|,
name|NULL
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|post_checkout_hook
specifier|static
name|int
name|post_checkout_hook
parameter_list|(
name|struct
name|commit
modifier|*
name|old
parameter_list|,
name|struct
name|commit
modifier|*
name|new
parameter_list|,
name|int
name|changed
parameter_list|)
block|{
name|struct
name|child_process
name|proc
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|git_path
argument_list|(
literal|"hooks/post-checkout"
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[
literal|5
index|]
decl_stmt|;
if|if
condition|(
name|access
argument_list|(
name|name
argument_list|,
name|X_OK
argument_list|)
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|memset
argument_list|(
operator|&
name|proc
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|proc
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|name
expr_stmt|;
name|argv
index|[
literal|1
index|]
operator|=
name|xstrdup
argument_list|(
name|sha1_to_hex
argument_list|(
name|old
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
literal|2
index|]
operator|=
name|xstrdup
argument_list|(
name|sha1_to_hex
argument_list|(
name|new
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
literal|3
index|]
operator|=
name|changed
condition|?
literal|"1"
else|:
literal|"0"
expr_stmt|;
name|argv
index|[
literal|4
index|]
operator|=
name|NULL
expr_stmt|;
name|proc
operator|.
name|argv
operator|=
name|argv
expr_stmt|;
name|proc
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|proc
operator|.
name|stdout_to_stderr
operator|=
literal|1
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|proc
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|update_some
specifier|static
name|int
name|update_some
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|pathname
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
if|if
condition|(
name|S_ISGITLINK
argument_list|(
name|mode
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
return|return
name|READ_TREE_RECURSIVE
return|;
name|len
operator|=
name|baselen
operator|+
name|strlen
argument_list|(
name|pathname
argument_list|)
expr_stmt|;
name|ce
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
name|cache_entry_size
argument_list|(
name|len
argument_list|)
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
operator|+
name|baselen
argument_list|,
name|pathname
argument_list|,
name|len
operator|-
name|baselen
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|create_ce_flags
argument_list|(
name|len
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|ADD_CACHE_OK_TO_ADD
operator||
name|ADD_CACHE_OK_TO_REPLACE
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_tree_some
specifier|static
name|int
name|read_tree_some
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
block|{
name|read_tree_recursive
argument_list|(
name|tree
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|,
name|pathspec
argument_list|,
name|update_some
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* update the index with the given tree's info 	 * for all args, expanding wildcards, and exit 	 * with any non-zero return code. 	 */
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|checkout_paths
specifier|static
name|int
name|checkout_paths
parameter_list|(
name|struct
name|tree
modifier|*
name|source_tree
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
block|{
name|int
name|pos
decl_stmt|;
name|struct
name|checkout
name|state
decl_stmt|;
specifier|static
name|char
modifier|*
name|ps_matched
decl_stmt|;
name|unsigned
name|char
name|rev
index|[
literal|20
index|]
decl_stmt|;
name|int
name|flag
decl_stmt|;
name|struct
name|commit
modifier|*
name|head
decl_stmt|;
name|int
name|errs
init|=
literal|0
decl_stmt|;
name|int
name|newfd
decl_stmt|;
name|struct
name|lock_file
modifier|*
name|lock_file
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|lock_file
argument_list|)
argument_list|)
decl_stmt|;
name|newfd
operator|=
name|hold_locked_index
argument_list|(
name|lock_file
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|source_tree
condition|)
name|read_tree_some
argument_list|(
name|source_tree
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
for|for
control|(
name|pos
operator|=
literal|0
init|;
name|pathspec
index|[
name|pos
index|]
condition|;
name|pos
operator|++
control|)
empty_stmt|;
name|ps_matched
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
name|pos
argument_list|)
expr_stmt|;
for|for
control|(
name|pos
operator|=
literal|0
init|;
name|pos
operator|<
name|active_nr
condition|;
name|pos
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|pos
index|]
decl_stmt|;
name|pathspec_match
argument_list|(
name|pathspec
argument_list|,
name|ps_matched
argument_list|,
name|ce
operator|->
name|name
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|report_path_error
argument_list|(
name|ps_matched
argument_list|,
name|pathspec
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|1
return|;
comment|/* Now we are committed to check them out */
name|memset
argument_list|(
operator|&
name|state
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|state
argument_list|)
argument_list|)
expr_stmt|;
name|state
operator|.
name|force
operator|=
literal|1
expr_stmt|;
name|state
operator|.
name|refresh_cache
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|pos
operator|=
literal|0
init|;
name|pos
operator|<
name|active_nr
condition|;
name|pos
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|pos
index|]
decl_stmt|;
if|if
condition|(
name|pathspec_match
argument_list|(
name|pathspec
argument_list|,
name|NULL
argument_list|,
name|ce
operator|->
name|name
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|errs
operator||=
name|checkout_entry
argument_list|(
name|ce
argument_list|,
operator|&
name|state
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
name|lock_file
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to write new index file"
argument_list|)
expr_stmt|;
name|resolve_ref
argument_list|(
literal|"HEAD"
argument_list|,
name|rev
argument_list|,
literal|0
argument_list|,
operator|&
name|flag
argument_list|)
expr_stmt|;
name|head
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|errs
operator||=
name|post_checkout_hook
argument_list|(
name|head
argument_list|,
name|head
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|errs
return|;
block|}
end_function
begin_function
DECL|function|show_local_changes
specifier|static
name|void
name|show_local_changes
parameter_list|(
name|struct
name|object
modifier|*
name|head
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
comment|/* I think we want full paths, even if we're in a subdirectory. */
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|abbrev
operator|=
literal|0
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator||=
name|DIFF_FORMAT_NAME_STATUS
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|rev
argument_list|,
name|head
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|run_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|describe_detached_head
specifier|static
name|void
name|describe_detached_head
parameter_list|(
name|char
modifier|*
name|msg
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
name|pretty_print_commit
argument_list|(
name|CMIT_FMT_ONELINE
argument_list|,
name|commit
argument_list|,
operator|&
name|sb
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s %s... %s\n"
argument_list|,
name|msg
argument_list|,
name|find_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|checkout_opts
struct|struct
name|checkout_opts
block|{
DECL|member|quiet
name|int
name|quiet
decl_stmt|;
DECL|member|merge
name|int
name|merge
decl_stmt|;
DECL|member|force
name|int
name|force
decl_stmt|;
DECL|member|writeout_error
name|int
name|writeout_error
decl_stmt|;
DECL|member|new_branch
name|char
modifier|*
name|new_branch
decl_stmt|;
DECL|member|new_branch_log
name|int
name|new_branch_log
decl_stmt|;
DECL|member|track
name|enum
name|branch_track
name|track
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|reset_tree
specifier|static
name|int
name|reset_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
name|struct
name|checkout_opts
modifier|*
name|o
parameter_list|,
name|int
name|worktree
parameter_list|)
block|{
name|struct
name|unpack_trees_options
name|opts
decl_stmt|;
name|struct
name|tree_desc
name|tree_desc
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|opts
operator|.
name|head_idx
operator|=
operator|-
literal|1
expr_stmt|;
name|opts
operator|.
name|update
operator|=
name|worktree
expr_stmt|;
name|opts
operator|.
name|skip_unmerged
operator|=
operator|!
name|worktree
expr_stmt|;
name|opts
operator|.
name|reset
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|fn
operator|=
name|oneway_merge
expr_stmt|;
name|opts
operator|.
name|verbose_update
operator|=
operator|!
name|o
operator|->
name|quiet
expr_stmt|;
name|opts
operator|.
name|src_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|opts
operator|.
name|dst_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|parse_tree
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|tree_desc
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|unpack_trees
argument_list|(
literal|1
argument_list|,
operator|&
name|tree_desc
argument_list|,
operator|&
name|opts
argument_list|)
condition|)
block|{
case|case
operator|-
literal|2
case|:
name|o
operator|->
name|writeout_error
operator|=
literal|1
expr_stmt|;
comment|/* 		 * We return 0 nevertheless, as the index is all right 		 * and more importantly we have made best efforts to 		 * update paths in the work tree, and we cannot revert 		 * them. 		 */
case|case
literal|0
case|:
return|return
literal|0
return|;
default|default:
return|return
literal|128
return|;
block|}
block|}
end_function
begin_struct
DECL|struct|branch_info
struct|struct
name|branch_info
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
comment|/* The short name used */
DECL|member|path
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
comment|/* The full name of a real branch */
DECL|member|commit
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
comment|/* The named commit */
block|}
struct|;
end_struct
begin_function
DECL|function|setup_branch_path
specifier|static
name|void
name|setup_branch_path
parameter_list|(
name|struct
name|branch_info
modifier|*
name|branch
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
literal|"refs/heads/"
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|branch
operator|->
name|name
argument_list|)
expr_stmt|;
name|branch
operator|->
name|path
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|merge_working_tree
specifier|static
name|int
name|merge_working_tree
parameter_list|(
name|struct
name|checkout_opts
modifier|*
name|opts
parameter_list|,
name|struct
name|branch_info
modifier|*
name|old
parameter_list|,
name|struct
name|branch_info
modifier|*
name|new
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|struct
name|lock_file
modifier|*
name|lock_file
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|lock_file
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|newfd
init|=
name|hold_locked_index
argument_list|(
name|lock_file
argument_list|,
literal|1
argument_list|)
decl_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|force
condition|)
block|{
name|ret
operator|=
name|reset_tree
argument_list|(
name|new
operator|->
name|commit
operator|->
name|tree
argument_list|,
name|opts
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
block|}
else|else
block|{
name|struct
name|tree_desc
name|trees
index|[
literal|2
index|]
decl_stmt|;
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|struct
name|unpack_trees_options
name|topts
decl_stmt|;
name|memset
argument_list|(
operator|&
name|topts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|topts
argument_list|)
argument_list|)
expr_stmt|;
name|topts
operator|.
name|head_idx
operator|=
operator|-
literal|1
expr_stmt|;
name|topts
operator|.
name|src_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|topts
operator|.
name|dst_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|topts
operator|.
name|msgs
operator|.
name|not_uptodate_file
operator|=
literal|"You have local changes to '%s'; cannot switch branches."
expr_stmt|;
name|refresh_cache
argument_list|(
name|REFRESH_QUIET
argument_list|)
expr_stmt|;
if|if
condition|(
name|unmerged_cache
argument_list|()
condition|)
block|{
name|error
argument_list|(
literal|"you need to resolve your current index first"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
comment|/* 2-way merge to the new branch */
name|topts
operator|.
name|update
operator|=
literal|1
expr_stmt|;
name|topts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
name|topts
operator|.
name|gently
operator|=
name|opts
operator|->
name|merge
expr_stmt|;
name|topts
operator|.
name|verbose_update
operator|=
operator|!
name|opts
operator|->
name|quiet
expr_stmt|;
name|topts
operator|.
name|fn
operator|=
name|twoway_merge
expr_stmt|;
name|topts
operator|.
name|dir
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|topts
operator|.
name|dir
argument_list|)
argument_list|)
expr_stmt|;
name|topts
operator|.
name|dir
operator|->
name|show_ignored
operator|=
literal|1
expr_stmt|;
name|topts
operator|.
name|dir
operator|->
name|exclude_per_dir
operator|=
literal|".gitignore"
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|old
operator|->
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|trees
index|[
literal|0
index|]
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|new
operator|->
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|trees
index|[
literal|1
index|]
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
name|ret
operator|=
name|unpack_trees
argument_list|(
literal|2
argument_list|,
name|trees
argument_list|,
operator|&
name|topts
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|==
operator|-
literal|1
condition|)
block|{
comment|/* 			 * Unpack couldn't do a trivial merge; either 			 * give up or do a real merge, depending on 			 * whether the merge flag was used. 			 */
name|struct
name|tree
modifier|*
name|result
decl_stmt|;
name|struct
name|tree
modifier|*
name|work
decl_stmt|;
if|if
condition|(
operator|!
name|opts
operator|->
name|merge
condition|)
return|return
literal|1
return|;
name|parse_commit
argument_list|(
name|old
operator|->
name|commit
argument_list|)
expr_stmt|;
comment|/* Do more real merge */
comment|/* 			 * We update the index fully, then write the 			 * tree from the index, then merge the new 			 * branch with the current tree, with the old 			 * branch as the base. Then we reset the index 			 * (but not the working tree) to the new 			 * branch, leaving the working tree as the 			 * merged version, but skipping unmerged 			 * entries in the index. 			 */
name|add_files_to_cache
argument_list|(
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|work
operator|=
name|write_tree_from_memory
argument_list|()
expr_stmt|;
name|ret
operator|=
name|reset_tree
argument_list|(
name|new
operator|->
name|commit
operator|->
name|tree
argument_list|,
name|opts
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|merge_trees
argument_list|(
name|new
operator|->
name|commit
operator|->
name|tree
argument_list|,
name|work
argument_list|,
name|old
operator|->
name|commit
operator|->
name|tree
argument_list|,
name|new
operator|->
name|name
argument_list|,
literal|"local"
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
name|ret
operator|=
name|reset_tree
argument_list|(
name|new
operator|->
name|commit
operator|->
name|tree
argument_list|,
name|opts
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
block|}
block|}
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
name|lock_file
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to write new index file"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opts
operator|->
name|force
condition|)
name|show_local_changes
argument_list|(
operator|&
name|new
operator|->
name|commit
operator|->
name|object
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|report_tracking
specifier|static
name|void
name|report_tracking
parameter_list|(
name|struct
name|branch_info
modifier|*
name|new
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|branch
modifier|*
name|branch
init|=
name|branch_get
argument_list|(
name|new
operator|->
name|name
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|format_tracking_info
argument_list|(
name|branch
argument_list|,
operator|&
name|sb
argument_list|)
condition|)
return|return;
name|fputs
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|update_refs_for_switch
specifier|static
name|void
name|update_refs_for_switch
parameter_list|(
name|struct
name|checkout_opts
modifier|*
name|opts
parameter_list|,
name|struct
name|branch_info
modifier|*
name|old
parameter_list|,
name|struct
name|branch_info
modifier|*
name|new
parameter_list|)
block|{
name|struct
name|strbuf
name|msg
decl_stmt|;
specifier|const
name|char
modifier|*
name|old_desc
decl_stmt|;
if|if
condition|(
name|opts
operator|->
name|new_branch
condition|)
block|{
name|create_branch
argument_list|(
name|old
operator|->
name|name
argument_list|,
name|opts
operator|->
name|new_branch
argument_list|,
name|new
operator|->
name|name
argument_list|,
literal|0
argument_list|,
name|opts
operator|->
name|new_branch_log
argument_list|,
name|opts
operator|->
name|track
argument_list|)
expr_stmt|;
name|new
operator|->
name|name
operator|=
name|opts
operator|->
name|new_branch
expr_stmt|;
name|setup_branch_path
argument_list|(
name|new
argument_list|)
expr_stmt|;
block|}
name|strbuf_init
argument_list|(
operator|&
name|msg
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|old_desc
operator|=
name|old
operator|->
name|name
expr_stmt|;
if|if
condition|(
operator|!
name|old_desc
condition|)
name|old_desc
operator|=
name|sha1_to_hex
argument_list|(
name|old
operator|->
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|msg
argument_list|,
literal|"checkout: moving from %s to %s"
argument_list|,
name|old_desc
argument_list|,
name|new
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|new
operator|->
name|path
condition|)
block|{
name|create_symref
argument_list|(
literal|"HEAD"
argument_list|,
name|new
operator|->
name|path
argument_list|,
name|msg
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opts
operator|->
name|quiet
condition|)
block|{
if|if
condition|(
name|old
operator|->
name|path
operator|&&
operator|!
name|strcmp
argument_list|(
name|new
operator|->
name|path
argument_list|,
name|old
operator|->
name|path
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Already on \"%s\"\n"
argument_list|,
name|new
operator|->
name|name
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Switched to%s branch \"%s\"\n"
argument_list|,
name|opts
operator|->
name|new_branch
condition|?
literal|" a new"
else|:
literal|""
argument_list|,
name|new
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|new
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
block|{
name|update_ref
argument_list|(
name|msg
operator|.
name|buf
argument_list|,
literal|"HEAD"
argument_list|,
name|new
operator|->
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|NULL
argument_list|,
name|REF_NODEREF
argument_list|,
name|DIE_ON_ERR
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opts
operator|->
name|quiet
condition|)
block|{
if|if
condition|(
name|old
operator|->
name|path
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Note: moving to \"%s\" which isn't a local branch\nIf you want to create a new branch from this checkout, you may do so\n(now or later) by using -b with the checkout command again. Example:\n  git checkout -b<new_branch_name>\n"
argument_list|,
name|new
operator|->
name|name
argument_list|)
expr_stmt|;
name|describe_detached_head
argument_list|(
literal|"HEAD is now at"
argument_list|,
name|new
operator|->
name|commit
argument_list|)
expr_stmt|;
block|}
block|}
name|remove_branch_state
argument_list|()
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opts
operator|->
name|quiet
operator|&&
operator|(
name|new
operator|->
name|path
operator|||
operator|!
name|strcmp
argument_list|(
name|new
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
operator|)
condition|)
name|report_tracking
argument_list|(
name|new
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|switch_branches
specifier|static
name|int
name|switch_branches
parameter_list|(
name|struct
name|checkout_opts
modifier|*
name|opts
parameter_list|,
name|struct
name|branch_info
modifier|*
name|new
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
decl_stmt|;
name|struct
name|branch_info
name|old
decl_stmt|;
name|unsigned
name|char
name|rev
index|[
literal|20
index|]
decl_stmt|;
name|int
name|flag
decl_stmt|;
name|memset
argument_list|(
operator|&
name|old
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|old
argument_list|)
argument_list|)
expr_stmt|;
name|old
operator|.
name|path
operator|=
name|resolve_ref
argument_list|(
literal|"HEAD"
argument_list|,
name|rev
argument_list|,
literal|0
argument_list|,
operator|&
name|flag
argument_list|)
expr_stmt|;
name|old
operator|.
name|commit
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|rev
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|flag
operator|&
name|REF_ISSYMREF
operator|)
condition|)
name|old
operator|.
name|path
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|old
operator|.
name|path
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|old
operator|.
name|path
argument_list|,
literal|"refs/heads/"
argument_list|)
condition|)
name|old
operator|.
name|name
operator|=
name|old
operator|.
name|path
operator|+
name|strlen
argument_list|(
literal|"refs/heads/"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new
operator|->
name|name
condition|)
block|{
name|new
operator|->
name|name
operator|=
literal|"HEAD"
expr_stmt|;
name|new
operator|->
name|commit
operator|=
name|old
operator|.
name|commit
expr_stmt|;
if|if
condition|(
operator|!
name|new
operator|->
name|commit
condition|)
name|die
argument_list|(
literal|"You are on a branch yet to be born"
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|new
operator|->
name|commit
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * If the new thing isn't a branch and isn't HEAD and we're 	 * not starting a new branch, and we want messages, and we 	 * weren't on a branch, and we're moving to a new commit, 	 * describe the old commit. 	 */
if|if
condition|(
operator|!
name|new
operator|->
name|path
operator|&&
name|strcmp
argument_list|(
name|new
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
operator|&&
operator|!
name|opts
operator|->
name|new_branch
operator|&&
operator|!
name|opts
operator|->
name|quiet
operator|&&
operator|!
name|old
operator|.
name|path
operator|&&
name|new
operator|->
name|commit
operator|!=
name|old
operator|.
name|commit
condition|)
name|describe_detached_head
argument_list|(
literal|"Previous HEAD position was"
argument_list|,
name|old
operator|.
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|old
operator|.
name|commit
condition|)
block|{
if|if
condition|(
operator|!
name|opts
operator|->
name|quiet
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: You appear to be on a branch yet to be born.\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: Forcing checkout of %s.\n"
argument_list|,
name|new
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|opts
operator|->
name|force
operator|=
literal|1
expr_stmt|;
block|}
name|ret
operator|=
name|merge_working_tree
argument_list|(
name|opts
argument_list|,
operator|&
name|old
argument_list|,
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|update_refs_for_switch
argument_list|(
name|opts
argument_list|,
operator|&
name|old
argument_list|,
name|new
argument_list|)
expr_stmt|;
name|ret
operator|=
name|post_checkout_hook
argument_list|(
name|old
operator|.
name|commit
argument_list|,
name|new
operator|->
name|commit
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
name|ret
operator|||
name|opts
operator|->
name|writeout_error
return|;
block|}
end_function
begin_function
DECL|function|cmd_checkout
name|int
name|cmd_checkout
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|checkout_opts
name|opts
decl_stmt|;
name|unsigned
name|char
name|rev
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|arg
decl_stmt|;
name|struct
name|branch_info
name|new
decl_stmt|;
name|struct
name|tree
modifier|*
name|source_tree
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__QUIET
argument_list|(
operator|&
name|opts
operator|.
name|quiet
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'b'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|new_branch
argument_list|,
literal|"new branch"
argument_list|,
literal|"branch"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'l'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|new_branch_log
argument_list|,
literal|"log for new branch"
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|'t'
argument_list|,
literal|"track"
argument_list|,
operator|&
name|opts
operator|.
name|track
argument_list|,
literal|"track"
argument_list|,
name|BRANCH_TRACK_EXPLICIT
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'f'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|force
argument_list|,
literal|"force"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'m'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|merge
argument_list|,
literal|"merge"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|new
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|opts
operator|.
name|track
operator|=
name|git_branch_track
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|options
argument_list|,
name|checkout_usage
argument_list|,
name|PARSE_OPT_KEEP_DASHDASH
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
condition|)
block|{
name|arg
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|rev
argument_list|)
condition|)
empty_stmt|;
elseif|else
if|if
condition|(
operator|(
name|new
operator|.
name|commit
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|rev
argument_list|,
literal|1
argument_list|)
operator|)
condition|)
block|{
name|new
operator|.
name|name
operator|=
name|arg
expr_stmt|;
name|setup_branch_path
argument_list|(
operator|&
name|new
argument_list|)
expr_stmt|;
if|if
condition|(
name|resolve_ref
argument_list|(
name|new
operator|.
name|path
argument_list|,
name|rev
argument_list|,
literal|1
argument_list|,
name|NULL
argument_list|)
condition|)
name|new
operator|.
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|rev
argument_list|)
expr_stmt|;
else|else
name|new
operator|.
name|path
operator|=
name|NULL
expr_stmt|;
name|parse_commit
argument_list|(
name|new
operator|.
name|commit
argument_list|)
expr_stmt|;
name|source_tree
operator|=
name|new
operator|.
name|commit
operator|->
name|tree
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|(
name|source_tree
operator|=
name|parse_tree_indirect
argument_list|(
name|rev
argument_list|)
operator|)
condition|)
block|{
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
block|}
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|opts
operator|.
name|new_branch
operator|&&
operator|(
name|opts
operator|.
name|track
operator|!=
name|git_branch_track
operator|)
condition|)
name|die
argument_list|(
literal|"git checkout: --track and --no-track require -b"
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|force
operator|&&
name|opts
operator|.
name|merge
condition|)
name|die
argument_list|(
literal|"git checkout: -f and -m are incompatible"
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
condition|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
init|=
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|pathspec
condition|)
name|die
argument_list|(
literal|"invalid path specification"
argument_list|)
expr_stmt|;
comment|/* Checkout paths */
if|if
condition|(
name|opts
operator|.
name|new_branch
operator|||
name|opts
operator|.
name|force
operator|||
name|opts
operator|.
name|merge
condition|)
block|{
if|if
condition|(
name|argc
operator|==
literal|1
condition|)
block|{
name|die
argument_list|(
literal|"git checkout: updating paths is incompatible with switching branches/forcing\nDid you intend to checkout '%s' which can not be resolved as commit?"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|die
argument_list|(
literal|"git checkout: updating paths is incompatible with switching branches/forcing"
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|checkout_paths
argument_list|(
name|source_tree
argument_list|,
name|pathspec
argument_list|)
return|;
block|}
if|if
condition|(
name|new
operator|.
name|name
operator|&&
operator|!
name|new
operator|.
name|commit
condition|)
block|{
name|die
argument_list|(
literal|"Cannot switch branch to a non-commit."
argument_list|)
expr_stmt|;
block|}
return|return
name|switch_branches
argument_list|(
operator|&
name|opts
argument_list|,
operator|&
name|new
argument_list|)
return|;
block|}
end_function
end_unit
