begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_include
include|#
directive|include
file|"archive.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"unpack-trees.h"
end_include
begin_decl_stmt
DECL|variable|archive_usage
specifier|static
name|char
specifier|const
modifier|*
specifier|const
name|archive_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git archive [options]<tree-ish> [<path>...]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git archive --list"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git archive --remote<repo> [--exec<cmd>] [options]<tree-ish> [<path>...]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git archive --remote<repo> [--exec<cmd>] --list"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|archivers
specifier|static
specifier|const
name|struct
name|archiver
modifier|*
modifier|*
name|archivers
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_archivers
specifier|static
name|int
name|nr_archivers
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|alloc_archivers
specifier|static
name|int
name|alloc_archivers
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|register_archiver
name|void
name|register_archiver
parameter_list|(
name|struct
name|archiver
modifier|*
name|ar
parameter_list|)
block|{
name|ALLOC_GROW
argument_list|(
name|archivers
argument_list|,
name|nr_archivers
operator|+
literal|1
argument_list|,
name|alloc_archivers
argument_list|)
expr_stmt|;
name|archivers
index|[
name|nr_archivers
operator|++
index|]
operator|=
name|ar
expr_stmt|;
block|}
end_function
begin_function
DECL|function|format_subst
specifier|static
name|void
name|format_subst
parameter_list|(
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|size_t
name|len
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|char
modifier|*
name|to_free
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|fmt
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|pretty_print_context
name|ctx
init|=
block|{
literal|0
block|}
decl_stmt|;
name|ctx
operator|.
name|date_mode
operator|=
name|DATE_NORMAL
expr_stmt|;
name|ctx
operator|.
name|abbrev
operator|=
name|DEFAULT_ABBREV
expr_stmt|;
if|if
condition|(
name|src
operator|==
name|buf
operator|->
name|buf
condition|)
name|to_free
operator|=
name|strbuf_detach
argument_list|(
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|b
decl_stmt|,
modifier|*
name|c
decl_stmt|;
name|b
operator|=
name|memmem
argument_list|(
name|src
argument_list|,
name|len
argument_list|,
literal|"$Format:"
argument_list|,
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|b
condition|)
break|break;
name|c
operator|=
name|memchr
argument_list|(
name|b
operator|+
literal|8
argument_list|,
literal|'$'
argument_list|,
operator|(
name|src
operator|+
name|len
operator|)
operator|-
name|b
operator|-
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
break|break;
name|strbuf_reset
argument_list|(
operator|&
name|fmt
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|fmt
argument_list|,
name|b
operator|+
literal|8
argument_list|,
name|c
operator|-
name|b
operator|-
literal|8
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|buf
argument_list|,
name|src
argument_list|,
name|b
operator|-
name|src
argument_list|)
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
name|fmt
operator|.
name|buf
argument_list|,
name|buf
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|len
operator|-=
name|c
operator|+
literal|1
operator|-
name|src
expr_stmt|;
name|src
operator|=
name|c
operator|+
literal|1
expr_stmt|;
block|}
name|strbuf_add
argument_list|(
name|buf
argument_list|,
name|src
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|fmt
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|to_free
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sha1_file_to_archive
name|void
modifier|*
name|sha1_file_to_archive
parameter_list|(
specifier|const
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
name|enum
name|object_type
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|)
block|{
name|void
modifier|*
name|buffer
decl_stmt|;
specifier|const
name|struct
name|commit
modifier|*
name|commit
init|=
name|args
operator|->
name|convert
condition|?
name|args
operator|->
name|commit
else|:
name|NULL
decl_stmt|;
name|path
operator|+=
name|args
operator|->
name|baselen
expr_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
name|sizep
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
operator|&&
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|size_t
name|size
init|=
literal|0
decl_stmt|;
name|strbuf_attach
argument_list|(
operator|&
name|buf
argument_list|,
name|buffer
argument_list|,
operator|*
name|sizep
argument_list|,
operator|*
name|sizep
operator|+
literal|1
argument_list|)
expr_stmt|;
name|convert_to_working_tree
argument_list|(
name|path
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
condition|)
name|format_subst
argument_list|(
name|commit
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
operator|*
name|sizep
operator|=
name|size
expr_stmt|;
block|}
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|setup_archive_check
specifier|static
name|void
name|setup_archive_check
parameter_list|(
name|struct
name|git_attr_check
modifier|*
name|check
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr
modifier|*
name|attr_export_ignore
decl_stmt|;
specifier|static
name|struct
name|git_attr
modifier|*
name|attr_export_subst
decl_stmt|;
if|if
condition|(
operator|!
name|attr_export_ignore
condition|)
block|{
name|attr_export_ignore
operator|=
name|git_attr
argument_list|(
literal|"export-ignore"
argument_list|)
expr_stmt|;
name|attr_export_subst
operator|=
name|git_attr
argument_list|(
literal|"export-subst"
argument_list|)
expr_stmt|;
block|}
name|check
index|[
literal|0
index|]
operator|.
name|attr
operator|=
name|attr_export_ignore
expr_stmt|;
name|check
index|[
literal|1
index|]
operator|.
name|attr
operator|=
name|attr_export_subst
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|archiver_context
struct|struct
name|archiver_context
block|{
DECL|member|args
name|struct
name|archiver_args
modifier|*
name|args
decl_stmt|;
DECL|member|write_entry
name|write_archive_entry_fn_t
name|write_entry
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|write_archive_entry
specifier|static
name|int
name|write_archive_entry
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
name|filename
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
specifier|static
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|archiver_context
modifier|*
name|c
init|=
name|context
decl_stmt|;
name|struct
name|archiver_args
modifier|*
name|args
init|=
name|c
operator|->
name|args
decl_stmt|;
name|write_archive_entry_fn_t
name|write_entry
init|=
name|c
operator|->
name|write_entry
decl_stmt|;
name|struct
name|git_attr_check
name|check
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|path_without_prefix
decl_stmt|;
name|int
name|err
decl_stmt|;
name|args
operator|->
name|convert
operator|=
literal|0
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_grow
argument_list|(
operator|&
name|path
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|path
argument_list|,
name|args
operator|->
name|base
argument_list|,
name|args
operator|->
name|baselen
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|path
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|path
argument_list|,
name|filename
argument_list|)
expr_stmt|;
name|path_without_prefix
operator|=
name|path
operator|.
name|buf
operator|+
name|args
operator|->
name|baselen
expr_stmt|;
name|setup_archive_check
argument_list|(
name|check
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_check_attr
argument_list|(
name|path_without_prefix
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|check
argument_list|)
argument_list|,
name|check
argument_list|)
condition|)
block|{
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|check
index|[
literal|0
index|]
operator|.
name|value
argument_list|)
condition|)
return|return
literal|0
return|;
name|args
operator|->
name|convert
operator|=
name|ATTR_TRUE
argument_list|(
name|check
index|[
literal|1
index|]
operator|.
name|value
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
operator|||
name|S_ISGITLINK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|path
operator|.
name|len
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|err
operator|=
name|write_entry
argument_list|(
name|args
argument_list|,
name|sha1
argument_list|,
name|path
operator|.
name|buf
argument_list|,
name|path
operator|.
name|len
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
return|return
name|err
return|;
return|return
operator|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|?
name|READ_TREE_RECURSIVE
else|:
literal|0
operator|)
return|;
block|}
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|path
operator|.
name|len
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
return|return
name|write_entry
argument_list|(
name|args
argument_list|,
name|sha1
argument_list|,
name|path
operator|.
name|buf
argument_list|,
name|path
operator|.
name|len
argument_list|,
name|mode
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_archive_entries
name|int
name|write_archive_entries
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
name|write_archive_entry_fn_t
name|write_entry
parameter_list|)
block|{
name|struct
name|archiver_context
name|context
decl_stmt|;
name|struct
name|unpack_trees_options
name|opts
decl_stmt|;
name|struct
name|tree_desc
name|t
decl_stmt|;
name|struct
name|pathspec
name|pathspec
decl_stmt|;
name|int
name|err
decl_stmt|;
if|if
condition|(
name|args
operator|->
name|baselen
operator|>
literal|0
operator|&&
name|args
operator|->
name|base
index|[
name|args
operator|->
name|baselen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
block|{
name|size_t
name|len
init|=
name|args
operator|->
name|baselen
decl_stmt|;
while|while
condition|(
name|len
operator|>
literal|1
operator|&&
name|args
operator|->
name|base
index|[
name|len
operator|-
literal|2
index|]
operator|==
literal|'/'
condition|)
name|len
operator|--
expr_stmt|;
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|len
argument_list|,
name|args
operator|->
name|base
argument_list|)
expr_stmt|;
name|err
operator|=
name|write_entry
argument_list|(
name|args
argument_list|,
name|args
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|args
operator|->
name|base
argument_list|,
name|len
argument_list|,
literal|040777
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
return|return
name|err
return|;
block|}
name|context
operator|.
name|args
operator|=
name|args
expr_stmt|;
name|context
operator|.
name|write_entry
operator|=
name|write_entry
expr_stmt|;
comment|/* 	 * Setup index and instruct attr to read index only 	 */
if|if
condition|(
operator|!
name|args
operator|->
name|worktree_attributes
condition|)
block|{
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
name|index_only
operator|=
literal|1
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
name|opts
operator|.
name|fn
operator|=
name|oneway_merge
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|t
argument_list|,
name|args
operator|->
name|tree
operator|->
name|buffer
argument_list|,
name|args
operator|->
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|unpack_trees
argument_list|(
literal|1
argument_list|,
operator|&
name|t
argument_list|,
operator|&
name|opts
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|git_attr_set_direction
argument_list|(
name|GIT_ATTR_INDEX
argument_list|,
operator|&
name|the_index
argument_list|)
expr_stmt|;
block|}
name|init_pathspec
argument_list|(
operator|&
name|pathspec
argument_list|,
name|args
operator|->
name|pathspec
argument_list|)
expr_stmt|;
name|err
operator|=
name|read_tree_recursive
argument_list|(
name|args
operator|->
name|tree
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|,
operator|&
name|pathspec
argument_list|,
name|write_archive_entry
argument_list|,
operator|&
name|context
argument_list|)
expr_stmt|;
name|free_pathspec
argument_list|(
operator|&
name|pathspec
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|==
name|READ_TREE_RECURSIVE
condition|)
name|err
operator|=
literal|0
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|lookup_archiver
specifier|static
specifier|const
name|struct
name|archiver
modifier|*
name|lookup_archiver
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
return|return
name|NULL
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_archivers
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|name
argument_list|,
name|archivers
index|[
name|i
index|]
operator|->
name|name
argument_list|)
condition|)
return|return
name|archivers
index|[
name|i
index|]
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|reject_entry
specifier|static
name|int
name|reject_entry
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
name|filename
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
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|path_exists
specifier|static
name|int
name|path_exists
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|paths
index|[]
init|=
block|{
name|path
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|pathspec
name|pathspec
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|init_pathspec
argument_list|(
operator|&
name|pathspec
argument_list|,
name|paths
argument_list|)
expr_stmt|;
name|ret
operator|=
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
operator|&
name|pathspec
argument_list|,
name|reject_entry
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|free_pathspec
argument_list|(
operator|&
name|pathspec
argument_list|)
expr_stmt|;
return|return
name|ret
operator|!=
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_pathspec_arg
specifier|static
name|void
name|parse_pathspec_arg
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|ar_args
parameter_list|)
block|{
name|ar_args
operator|->
name|pathspec
operator|=
name|pathspec
operator|=
name|get_pathspec
argument_list|(
literal|""
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathspec
condition|)
block|{
while|while
condition|(
operator|*
name|pathspec
condition|)
block|{
if|if
condition|(
operator|!
name|path_exists
argument_list|(
name|ar_args
operator|->
name|tree
argument_list|,
operator|*
name|pathspec
argument_list|)
condition|)
name|die
argument_list|(
literal|"path not found: %s"
argument_list|,
operator|*
name|pathspec
argument_list|)
expr_stmt|;
name|pathspec
operator|++
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|parse_treeish_arg
specifier|static
name|void
name|parse_treeish_arg
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|ar_args
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|remote
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|name
init|=
name|argv
index|[
literal|0
index|]
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
decl_stmt|;
name|time_t
name|archive_time
decl_stmt|;
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
specifier|const
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
comment|/* Remotes are only allowed to fetch actual refs */
if|if
condition|(
name|remote
condition|)
block|{
name|char
modifier|*
name|ref
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|colon
init|=
name|strchr
argument_list|(
name|name
argument_list|,
literal|':'
argument_list|)
decl_stmt|;
name|int
name|refnamelen
init|=
name|colon
condition|?
name|colon
operator|-
name|name
else|:
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|dwim_ref
argument_list|(
name|name
argument_list|,
name|refnamelen
argument_list|,
name|sha1
argument_list|,
operator|&
name|ref
argument_list|)
condition|)
name|die
argument_list|(
literal|"no such ref: %.*s"
argument_list|,
name|refnamelen
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name"
argument_list|)
expr_stmt|;
name|commit
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|sha1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
condition|)
block|{
name|commit_sha1
operator|=
name|commit
operator|->
name|object
operator|.
name|sha1
expr_stmt|;
name|archive_time
operator|=
name|commit
operator|->
name|date
expr_stmt|;
block|}
else|else
block|{
name|commit_sha1
operator|=
name|NULL
expr_stmt|;
name|archive_time
operator|=
name|time
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
block|}
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|tree
operator|==
name|NULL
condition|)
name|die
argument_list|(
literal|"not a tree object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
block|{
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|int
name|err
decl_stmt|;
name|err
operator|=
name|get_tree_entry
argument_list|(
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|prefix
argument_list|,
name|tree_sha1
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|||
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|die
argument_list|(
literal|"current working directory is untracked"
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|tree_sha1
argument_list|)
expr_stmt|;
block|}
name|ar_args
operator|->
name|tree
operator|=
name|tree
expr_stmt|;
name|ar_args
operator|->
name|commit_sha1
operator|=
name|commit_sha1
expr_stmt|;
name|ar_args
operator|->
name|commit
operator|=
name|commit
expr_stmt|;
name|ar_args
operator|->
name|time
operator|=
name|archive_time
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|OPT__COMPR
define|#
directive|define
name|OPT__COMPR
parameter_list|(
name|s
parameter_list|,
name|v
parameter_list|,
name|h
parameter_list|,
name|p
parameter_list|)
define|\
value|{ OPTION_SET_INT, (s), NULL, (v), NULL, (h), \ 	  PARSE_OPT_NOARG | PARSE_OPT_NONEG, NULL, (p) }
end_define
begin_define
DECL|macro|OPT__COMPR_HIDDEN
define|#
directive|define
name|OPT__COMPR_HIDDEN
parameter_list|(
name|s
parameter_list|,
name|v
parameter_list|,
name|p
parameter_list|)
define|\
value|{ OPTION_SET_INT, (s), NULL, (v), NULL, "", \ 	  PARSE_OPT_NOARG | PARSE_OPT_NONEG | PARSE_OPT_HIDDEN, NULL, (p) }
end_define
begin_function
DECL|function|parse_archive_args
specifier|static
name|int
name|parse_archive_args
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
name|struct
name|archiver
modifier|*
modifier|*
name|ar
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
specifier|const
name|char
modifier|*
name|name_hint
parameter_list|,
name|int
name|is_remote
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|format
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|base
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|output
init|=
name|NULL
decl_stmt|;
name|int
name|compression_level
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|verbose
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|list
init|=
literal|0
decl_stmt|;
name|int
name|worktree_attributes
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|opts
index|[]
init|=
block|{
name|OPT_GROUP
argument_list|(
literal|""
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"format"
argument_list|,
operator|&
name|format
argument_list|,
name|N_
argument_list|(
literal|"fmt"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"archive format"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"prefix"
argument_list|,
operator|&
name|base
argument_list|,
name|N_
argument_list|(
literal|"prefix"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"prepend prefix to each pathname in the archive"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'o'
argument_list|,
literal|"output"
argument_list|,
operator|&
name|output
argument_list|,
name|N_
argument_list|(
literal|"file"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"write the archive to this file"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"worktree-attributes"
argument_list|,
operator|&
name|worktree_attributes
argument_list|,
name|N_
argument_list|(
literal|"read .gitattributes in working directory"
argument_list|)
argument_list|)
block|,
name|OPT__VERBOSE
argument_list|(
operator|&
name|verbose
argument_list|,
name|N_
argument_list|(
literal|"report archived files on stderr"
argument_list|)
argument_list|)
block|,
name|OPT__COMPR
argument_list|(
literal|'0'
argument_list|,
operator|&
name|compression_level
argument_list|,
name|N_
argument_list|(
literal|"store only"
argument_list|)
argument_list|,
literal|0
argument_list|)
block|,
name|OPT__COMPR
argument_list|(
literal|'1'
argument_list|,
operator|&
name|compression_level
argument_list|,
name|N_
argument_list|(
literal|"compress faster"
argument_list|)
argument_list|,
literal|1
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'2'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|2
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'3'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|3
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'4'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|4
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'5'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|5
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'6'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|6
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'7'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|7
argument_list|)
block|,
name|OPT__COMPR_HIDDEN
argument_list|(
literal|'8'
argument_list|,
operator|&
name|compression_level
argument_list|,
literal|8
argument_list|)
block|,
name|OPT__COMPR
argument_list|(
literal|'9'
argument_list|,
operator|&
name|compression_level
argument_list|,
name|N_
argument_list|(
literal|"compress better"
argument_list|)
argument_list|,
literal|9
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|""
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'l'
argument_list|,
literal|"list"
argument_list|,
operator|&
name|list
argument_list|,
name|N_
argument_list|(
literal|"list supported archive formats"
argument_list|)
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|""
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"remote"
argument_list|,
operator|&
name|remote
argument_list|,
name|N_
argument_list|(
literal|"repo"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"retrieve the archive from remote repository<repo>"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"exec"
argument_list|,
operator|&
name|exec
argument_list|,
name|N_
argument_list|(
literal|"command"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"path to the remote git-upload-archive command"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|NULL
argument_list|,
name|opts
argument_list|,
name|archive_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
condition|)
name|die
argument_list|(
literal|"Unexpected option --remote"
argument_list|)
expr_stmt|;
if|if
condition|(
name|exec
condition|)
name|die
argument_list|(
literal|"Option --exec can only be used together with --remote"
argument_list|)
expr_stmt|;
if|if
condition|(
name|output
condition|)
name|die
argument_list|(
literal|"Unexpected option --output"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
name|base
operator|=
literal|""
expr_stmt|;
if|if
condition|(
name|list
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_archivers
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|is_remote
operator|||
name|archivers
index|[
name|i
index|]
operator|->
name|flags
operator|&
name|ARCHIVER_REMOTE
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|archivers
index|[
name|i
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|format
operator|&&
name|name_hint
condition|)
name|format
operator|=
name|archive_format_from_filename
argument_list|(
name|name_hint
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|format
condition|)
name|format
operator|=
literal|"tar"
expr_stmt|;
comment|/* We need at least one parameter -- tree-ish */
if|if
condition|(
name|argc
operator|<
literal|1
condition|)
name|usage_with_options
argument_list|(
name|archive_usage
argument_list|,
name|opts
argument_list|)
expr_stmt|;
operator|*
name|ar
operator|=
name|lookup_archiver
argument_list|(
name|format
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|ar
operator|||
operator|(
name|is_remote
operator|&&
operator|!
operator|(
operator|(
operator|*
name|ar
operator|)
operator|->
name|flags
operator|&
name|ARCHIVER_REMOTE
operator|)
operator|)
condition|)
name|die
argument_list|(
literal|"Unknown archive format '%s'"
argument_list|,
name|format
argument_list|)
expr_stmt|;
name|args
operator|->
name|compression_level
operator|=
name|Z_DEFAULT_COMPRESSION
expr_stmt|;
if|if
condition|(
name|compression_level
operator|!=
operator|-
literal|1
condition|)
block|{
if|if
condition|(
operator|(
operator|*
name|ar
operator|)
operator|->
name|flags
operator|&
name|ARCHIVER_WANT_COMPRESSION_LEVELS
condition|)
name|args
operator|->
name|compression_level
operator|=
name|compression_level
expr_stmt|;
else|else
block|{
name|die
argument_list|(
literal|"Argument not supported for format '%s': -%d"
argument_list|,
name|format
argument_list|,
name|compression_level
argument_list|)
expr_stmt|;
block|}
block|}
name|args
operator|->
name|verbose
operator|=
name|verbose
expr_stmt|;
name|args
operator|->
name|base
operator|=
name|base
expr_stmt|;
name|args
operator|->
name|baselen
operator|=
name|strlen
argument_list|(
name|base
argument_list|)
expr_stmt|;
name|args
operator|->
name|worktree_attributes
operator|=
name|worktree_attributes
expr_stmt|;
return|return
name|argc
return|;
block|}
end_function
begin_function
DECL|function|write_archive
name|int
name|write_archive
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
parameter_list|,
name|int
name|setup_prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|name_hint
parameter_list|,
name|int
name|remote
parameter_list|)
block|{
name|int
name|nongit
init|=
literal|0
decl_stmt|;
specifier|const
name|struct
name|archiver
modifier|*
name|ar
init|=
name|NULL
decl_stmt|;
name|struct
name|archiver_args
name|args
decl_stmt|;
if|if
condition|(
name|setup_prefix
operator|&&
name|prefix
operator|==
name|NULL
condition|)
name|prefix
operator|=
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|init_tar_archiver
argument_list|()
expr_stmt|;
name|init_zip_archiver
argument_list|()
expr_stmt|;
name|argc
operator|=
name|parse_archive_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|ar
argument_list|,
operator|&
name|args
argument_list|,
name|name_hint
argument_list|,
name|remote
argument_list|)
expr_stmt|;
if|if
condition|(
name|nongit
condition|)
block|{
comment|/* 		 * We know this will die() with an error, so we could just 		 * die ourselves; but its error message will be more specific 		 * than what we could write here. 		 */
name|setup_git_directory
argument_list|()
expr_stmt|;
block|}
name|parse_treeish_arg
argument_list|(
name|argv
argument_list|,
operator|&
name|args
argument_list|,
name|prefix
argument_list|,
name|remote
argument_list|)
expr_stmt|;
name|parse_pathspec_arg
argument_list|(
name|argv
operator|+
literal|1
argument_list|,
operator|&
name|args
argument_list|)
expr_stmt|;
return|return
name|ar
operator|->
name|write_archive
argument_list|(
name|ar
argument_list|,
operator|&
name|args
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_extension
specifier|static
name|int
name|match_extension
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
specifier|const
name|char
modifier|*
name|ext
parameter_list|)
block|{
name|int
name|prefixlen
init|=
name|strlen
argument_list|(
name|filename
argument_list|)
operator|-
name|strlen
argument_list|(
name|ext
argument_list|)
decl_stmt|;
comment|/* 	 * We need 1 character for the '.', and 1 character to ensure that the 	 * prefix is non-empty (k.e., we don't match .tar.gz with no actual 	 * filename). 	 */
if|if
condition|(
name|prefixlen
operator|<
literal|2
operator|||
name|filename
index|[
name|prefixlen
operator|-
literal|1
index|]
operator|!=
literal|'.'
condition|)
return|return
literal|0
return|;
return|return
operator|!
name|strcmp
argument_list|(
name|filename
operator|+
name|prefixlen
argument_list|,
name|ext
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|archive_format_from_filename
specifier|const
name|char
modifier|*
name|archive_format_from_filename
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_archivers
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|match_extension
argument_list|(
name|filename
argument_list|,
name|archivers
index|[
name|i
index|]
operator|->
name|name
argument_list|)
condition|)
return|return
name|archivers
index|[
name|i
index|]
operator|->
name|name
return|;
return|return
name|NULL
return|;
block|}
end_function
end_unit
