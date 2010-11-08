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
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"reachable.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_decl_stmt
DECL|variable|prune_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|prune_usage
index|[]
init|=
block|{
literal|"git prune [-n] [-v] [--expire<time>] [--] [<head>...]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_only
specifier|static
name|int
name|show_only
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
specifier|static
name|int
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|expire
specifier|static
name|unsigned
name|long
name|expire
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|prune_tmp_object
specifier|static
name|int
name|prune_tmp_object
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|fullpath
init|=
name|mkpath
argument_list|(
literal|"%s/%s"
argument_list|,
name|path
argument_list|,
name|filename
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|fullpath
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not stat '%s'"
argument_list|,
name|fullpath
argument_list|)
return|;
if|if
condition|(
name|st
operator|.
name|st_mtime
operator|>
name|expire
condition|)
return|return
literal|0
return|;
name|printf
argument_list|(
literal|"Removing stale temporary file %s\n"
argument_list|,
name|fullpath
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
condition|)
name|unlink_or_warn
argument_list|(
name|fullpath
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|prune_object
specifier|static
name|int
name|prune_object
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|fullpath
init|=
name|mkpath
argument_list|(
literal|"%s/%s"
argument_list|,
name|path
argument_list|,
name|filename
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|fullpath
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not stat '%s'"
argument_list|,
name|fullpath
argument_list|)
return|;
if|if
condition|(
name|st
operator|.
name|st_mtime
operator|>
name|expire
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|show_only
operator|||
name|verbose
condition|)
block|{
name|enum
name|object_type
name|type
init|=
name|sha1_object_info
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
decl_stmt|;
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
operator|(
name|type
operator|>
literal|0
operator|)
condition|?
name|typename
argument_list|(
name|type
argument_list|)
else|:
literal|"unknown"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|show_only
condition|)
name|unlink_or_warn
argument_list|(
name|fullpath
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|prune_dir
specifier|static
name|int
name|prune_dir
parameter_list|(
name|int
name|i
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|char
name|name
index|[
literal|100
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|is_dot_or_dotdot
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
operator|==
literal|38
condition|)
block|{
name|sprintf
argument_list|(
name|name
argument_list|,
literal|"%02x"
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|name
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|39
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
break|break;
comment|/* 			 * Do we know about this object? 			 * It must have been reachable 			 */
if|if
condition|(
name|lookup_object
argument_list|(
name|sha1
argument_list|)
condition|)
continue|continue;
name|prune_object
argument_list|(
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
literal|"tmp_obj_"
argument_list|)
condition|)
block|{
name|prune_tmp_object
argument_list|(
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"bad sha1 file: %s/%s\n"
argument_list|,
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|show_only
condition|)
name|rmdir
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|prune_object_dir
specifier|static
name|void
name|prune_object_dir
parameter_list|(
specifier|const
name|char
modifier|*
name|path
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
literal|256
condition|;
name|i
operator|++
control|)
block|{
specifier|static
name|char
name|dir
index|[
literal|4096
index|]
decl_stmt|;
name|sprintf
argument_list|(
name|dir
argument_list|,
literal|"%s/%02x"
argument_list|,
name|path
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|prune_dir
argument_list|(
name|i
argument_list|,
name|dir
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Write errors (particularly out of space) can result in  * failed temporary packs (and more rarely indexes and other  * files beginning with "tmp_") accumulating in the object  * and the pack directories.  */
end_comment
begin_function
DECL|function|remove_temporary_files
specifier|static
name|void
name|remove_temporary_files
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unable to open directory %s\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return;
block|}
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
literal|"tmp_"
argument_list|)
condition|)
name|prune_tmp_object
argument_list|(
name|path
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_prune
name|int
name|cmd_prune
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
name|rev_info
name|revs
decl_stmt|;
specifier|const
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|,
literal|"do not remove, show only"
argument_list|)
block|,
name|OPT__VERBOSE
argument_list|(
operator|&
name|verbose
argument_list|,
literal|"report pruned objects"
argument_list|)
block|,
name|OPT_DATE
argument_list|(
literal|0
argument_list|,
literal|"expire"
argument_list|,
operator|&
name|expire
argument_list|,
literal|"expire objects older than<time>"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|char
modifier|*
name|s
decl_stmt|;
name|expire
operator|=
name|ULONG_MAX
expr_stmt|;
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
name|read_replace_refs
operator|=
literal|0
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|prefix
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
name|prefix
argument_list|,
name|options
argument_list|,
name|prune_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
while|while
condition|(
name|argc
operator|--
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
operator|*
name|argv
operator|++
decl_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|object
condition|)
name|die
argument_list|(
literal|"bad object: %s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|object
argument_list|,
literal|""
argument_list|)
expr_stmt|;
block|}
else|else
name|die
argument_list|(
literal|"unrecognized argument: %s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
name|mark_reachable_objects
argument_list|(
operator|&
name|revs
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|prune_object_dir
argument_list|(
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|prune_packed_objects
argument_list|(
name|show_only
argument_list|)
expr_stmt|;
name|remove_temporary_files
argument_list|(
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|s
operator|=
name|xstrdup
argument_list|(
name|mkpath
argument_list|(
literal|"%s/pack"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
argument_list|)
expr_stmt|;
name|remove_temporary_files
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|s
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
