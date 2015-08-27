begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
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
file|"cache-tree.h"
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
file|"resolve-undo.h"
end_include
begin_decl_stmt
DECL|variable|nr_trees
specifier|static
name|int
name|nr_trees
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|read_empty
specifier|static
name|int
name|read_empty
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|trees
specifier|static
name|struct
name|tree
modifier|*
name|trees
index|[
name|MAX_UNPACK_TREES
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|list_tree
specifier|static
name|int
name|list_tree
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
if|if
condition|(
name|nr_trees
operator|>=
name|MAX_UNPACK_TREES
condition|)
name|die
argument_list|(
literal|"I cannot read more than %d trees"
argument_list|,
name|MAX_UNPACK_TREES
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return
operator|-
literal|1
return|;
name|trees
index|[
name|nr_trees
operator|++
index|]
operator|=
name|tree
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|read_tree_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|read_tree_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git read-tree [(-m [--trivial] [--aggressive] | --reset | --prefix=<prefix>) [-u [--exclude-per-directory=<gitignore>] | -i]] [--no-sparse-checkout] [--index-output=<file>] (--empty |<tree-ish1> [<tree-ish2> [<tree-ish3>]])"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|index_output_cb
specifier|static
name|int
name|index_output_cb
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|set_alternate_index_output
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|exclude_per_directory_cb
specifier|static
name|int
name|exclude_per_directory_cb
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|dir_struct
modifier|*
name|dir
decl_stmt|;
name|struct
name|unpack_trees_options
modifier|*
name|opts
decl_stmt|;
name|opts
operator|=
operator|(
expr|struct
name|unpack_trees_options
operator|*
operator|)
name|opt
operator|->
name|value
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|dir
condition|)
name|die
argument_list|(
literal|"more than one --exclude-per-directory given."
argument_list|)
expr_stmt|;
name|dir
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|opts
operator|->
name|dir
argument_list|)
argument_list|)
expr_stmt|;
name|dir
operator|->
name|flags
operator||=
name|DIR_SHOW_IGNORED
expr_stmt|;
name|dir
operator|->
name|exclude_per_dir
operator|=
name|arg
expr_stmt|;
name|opts
operator|->
name|dir
operator|=
name|dir
expr_stmt|;
comment|/* We do not need to nor want to do read-directory 	 * here; we are merely interested in reusing the 	 * per directory ignore stack mechanism. 	 */
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|debug_stage
specifier|static
name|void
name|debug_stage
parameter_list|(
specifier|const
name|char
modifier|*
name|label
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|struct
name|unpack_trees_options
modifier|*
name|o
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s "
argument_list|,
name|label
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ce
condition|)
name|printf
argument_list|(
literal|"(missing)\n"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ce
operator|==
name|o
operator|->
name|df_conflict_entry
condition|)
name|printf
argument_list|(
literal|"(conflict)\n"
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%06o #%d %s %.8s\n"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|debug_merge
specifier|static
name|int
name|debug_merge
parameter_list|(
name|struct
name|cache_entry
modifier|*
modifier|*
name|stages
parameter_list|,
name|struct
name|unpack_trees_options
modifier|*
name|o
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|printf
argument_list|(
literal|"* %d-way merge\n"
argument_list|,
name|o
operator|->
name|merge_size
argument_list|)
expr_stmt|;
name|debug_stage
argument_list|(
literal|"index"
argument_list|,
name|stages
index|[
literal|0
index|]
argument_list|,
name|o
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<=
name|o
operator|->
name|merge_size
condition|;
name|i
operator|++
control|)
block|{
name|char
name|buf
index|[
literal|24
index|]
decl_stmt|;
name|sprintf
argument_list|(
name|buf
argument_list|,
literal|"ent#%d"
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|debug_stage
argument_list|(
name|buf
argument_list|,
name|stages
index|[
name|i
index|]
argument_list|,
name|o
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|lock_file
specifier|static
name|struct
name|lock_file
name|lock_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_read_tree
name|int
name|cmd_read_tree
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
name|unused_prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|newfd
decl_stmt|,
name|stage
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|tree_desc
name|t
index|[
name|MAX_UNPACK_TREES
index|]
decl_stmt|;
name|struct
name|unpack_trees_options
name|opts
decl_stmt|;
name|int
name|prefix_set
init|=
literal|0
decl_stmt|;
specifier|const
name|struct
name|option
name|read_tree_options
index|[]
init|=
block|{
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"index-output"
block|,
name|NULL
block|,
name|N_
argument_list|(
literal|"file"
argument_list|)
block|,
name|N_
argument_list|(
literal|"write resulting index to<file>"
argument_list|)
block|,
name|PARSE_OPT_NONEG
block|,
name|index_output_cb
block|}
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"empty"
argument_list|,
operator|&
name|read_empty
argument_list|,
name|N_
argument_list|(
literal|"only empty the index"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT__VERBOSE
argument_list|(
operator|&
name|opts
operator|.
name|verbose_update
argument_list|,
name|N_
argument_list|(
literal|"be verbose"
argument_list|)
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
name|N_
argument_list|(
literal|"Merging"
argument_list|)
argument_list|)
block|,
name|OPT_SET_INT
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
name|N_
argument_list|(
literal|"perform a merge in addition to a read"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"trivial"
argument_list|,
operator|&
name|opts
operator|.
name|trivial_merges_only
argument_list|,
name|N_
argument_list|(
literal|"3-way merge if no file level merging required"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"aggressive"
argument_list|,
operator|&
name|opts
operator|.
name|aggressive
argument_list|,
name|N_
argument_list|(
literal|"3-way merge in presence of adds and removes"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"reset"
argument_list|,
operator|&
name|opts
operator|.
name|reset
argument_list|,
name|N_
argument_list|(
literal|"same as -m, but discard unmerged entries"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
block|{
name|OPTION_STRING
block|,
literal|0
block|,
literal|"prefix"
block|,
operator|&
name|opts
operator|.
name|prefix
block|,
name|N_
argument_list|(
literal|"<subdirectory>/"
argument_list|)
block|,
name|N_
argument_list|(
literal|"read the tree into the index under<subdirectory>/"
argument_list|)
block|,
name|PARSE_OPT_NONEG
operator||
name|PARSE_OPT_LITERAL_ARGHELP
block|}
block|,
name|OPT_SET_INT
argument_list|(
literal|'u'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|update
argument_list|,
name|N_
argument_list|(
literal|"update working tree with merge result"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|0
block|,
literal|"exclude-per-directory"
block|,
operator|&
name|opts
block|,
name|N_
argument_list|(
literal|"gitignore"
argument_list|)
block|,
name|N_
argument_list|(
literal|"allow explicitly ignored files to be overwritten"
argument_list|)
block|,
name|PARSE_OPT_NONEG
block|,
name|exclude_per_directory_cb
block|}
block|,
name|OPT_SET_INT
argument_list|(
literal|'i'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|index_only
argument_list|,
name|N_
argument_list|(
literal|"don't check the working tree after merging"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT__DRY_RUN
argument_list|(
operator|&
name|opts
operator|.
name|dry_run
argument_list|,
name|N_
argument_list|(
literal|"don't update the index or the work tree"
argument_list|)
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"no-sparse-checkout"
argument_list|,
operator|&
name|opts
operator|.
name|skip_sparse_checkout
argument_list|,
name|N_
argument_list|(
literal|"skip applying sparse checkout filter"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"debug-unpack"
argument_list|,
operator|&
name|opts
operator|.
name|debug_unpack
argument_list|,
name|N_
argument_list|(
literal|"debug unpack-trees"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
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
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|unused_prefix
argument_list|,
name|read_tree_options
argument_list|,
name|read_tree_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|newfd
operator|=
name|hold_locked_index
argument_list|(
operator|&
name|lock_file
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|prefix_set
operator|=
name|opts
operator|.
name|prefix
condition|?
literal|1
else|:
literal|0
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|opts
operator|.
name|merge
operator|+
name|opts
operator|.
name|reset
operator|+
name|prefix_set
condition|)
name|die
argument_list|(
literal|"Which one? -m, --reset, or --prefix?"
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|reset
operator|||
name|opts
operator|.
name|merge
operator|||
name|opts
operator|.
name|prefix
condition|)
block|{
if|if
condition|(
name|read_cache_unmerged
argument_list|()
operator|&&
operator|(
name|opts
operator|.
name|prefix
operator|||
name|opts
operator|.
name|merge
operator|)
condition|)
name|die
argument_list|(
literal|"You need to resolve your current index first"
argument_list|)
expr_stmt|;
name|stage
operator|=
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
block|}
name|resolve_undo_clear
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|list_tree
argument_list|(
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to unpack tree object %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|stage
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|nr_trees
operator|==
literal|0
operator|&&
operator|!
name|read_empty
condition|)
name|warning
argument_list|(
literal|"read-tree: emptying the index with no arguments is deprecated; use --empty"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|nr_trees
operator|>
literal|0
operator|&&
name|read_empty
condition|)
name|die
argument_list|(
literal|"passing trees as arguments contradicts --empty"
argument_list|)
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|opts
operator|.
name|index_only
operator|+
name|opts
operator|.
name|update
condition|)
name|die
argument_list|(
literal|"-u and -i at the same time makes no sense"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|opts
operator|.
name|update
operator|||
name|opts
operator|.
name|index_only
operator|)
operator|&&
operator|!
name|opts
operator|.
name|merge
condition|)
name|die
argument_list|(
literal|"%s is meaningless without -m, --reset, or --prefix"
argument_list|,
name|opts
operator|.
name|update
condition|?
literal|"-u"
else|:
literal|"-i"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|opts
operator|.
name|dir
operator|&&
operator|!
name|opts
operator|.
name|update
operator|)
condition|)
name|die
argument_list|(
literal|"--exclude-per-directory is meaningless unless -u"
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|merge
operator|&&
operator|!
name|opts
operator|.
name|index_only
condition|)
name|setup_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|merge
condition|)
block|{
if|if
condition|(
name|stage
operator|<
literal|2
condition|)
name|die
argument_list|(
literal|"just how do you expect me to merge %d trees?"
argument_list|,
name|stage
operator|-
literal|1
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|stage
operator|-
literal|1
condition|)
block|{
case|case
literal|1
case|:
name|opts
operator|.
name|fn
operator|=
name|opts
operator|.
name|prefix
condition|?
name|bind_merge
else|:
name|oneway_merge
expr_stmt|;
break|break;
case|case
literal|2
case|:
name|opts
operator|.
name|fn
operator|=
name|twoway_merge
expr_stmt|;
name|opts
operator|.
name|initial_checkout
operator|=
name|is_cache_unborn
argument_list|()
expr_stmt|;
break|break;
case|case
literal|3
case|:
default|default:
name|opts
operator|.
name|fn
operator|=
name|threeway_merge
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|stage
operator|-
literal|1
operator|>=
literal|3
condition|)
name|opts
operator|.
name|head_idx
operator|=
name|stage
operator|-
literal|2
expr_stmt|;
else|else
name|opts
operator|.
name|head_idx
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|.
name|debug_unpack
condition|)
name|opts
operator|.
name|fn
operator|=
name|debug_merge
expr_stmt|;
name|cache_tree_free
argument_list|(
operator|&
name|active_cache_tree
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_trees
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|tree
modifier|*
name|tree
init|=
name|trees
index|[
name|i
index|]
decl_stmt|;
name|parse_tree
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
name|t
operator|+
name|i
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
block|}
if|if
condition|(
name|unpack_trees
argument_list|(
name|nr_trees
argument_list|,
name|t
argument_list|,
operator|&
name|opts
argument_list|)
condition|)
return|return
literal|128
return|;
if|if
condition|(
name|opts
operator|.
name|debug_unpack
operator|||
name|opts
operator|.
name|dry_run
condition|)
return|return
literal|0
return|;
comment|/* do not write the index out */
comment|/* 	 * When reading only one tree (either the most basic form, 	 * "-m ent" or "--reset ent" form), we can obtain a fully 	 * valid cache-tree because the index must match exactly 	 * what came from the tree. 	 */
if|if
condition|(
name|nr_trees
operator|==
literal|1
operator|&&
operator|!
name|opts
operator|.
name|prefix
condition|)
name|prime_cache_tree
argument_list|(
operator|&
name|active_cache_tree
argument_list|,
name|trees
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
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
operator|&
name|lock_file
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to write new index file"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
