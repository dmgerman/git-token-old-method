begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"progress.h"
end_include
begin_decl_stmt
DECL|variable|prune_packed_usage
specifier|static
specifier|const
name|char
name|prune_packed_usage
index|[]
init|=
literal|"git-prune-packed [-n] [-q]"
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|DRY_RUN
define|#
directive|define
name|DRY_RUN
value|01
end_define
begin_define
DECL|macro|VERBOSE
define|#
directive|define
name|VERBOSE
value|02
end_define
begin_decl_stmt
DECL|variable|progress
specifier|static
name|struct
name|progress
modifier|*
name|progress
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|prune_dir
specifier|static
name|void
name|prune_dir
parameter_list|(
name|int
name|i
parameter_list|,
name|DIR
modifier|*
name|dir
parameter_list|,
name|char
modifier|*
name|pathname
parameter_list|,
name|int
name|len
parameter_list|,
name|int
name|opts
parameter_list|)
block|{
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|char
name|hex
index|[
literal|40
index|]
decl_stmt|;
if|if
condition|(
name|opts
operator|==
name|VERBOSE
condition|)
name|display_progress
argument_list|(
name|progress
argument_list|,
name|i
operator|+
literal|1
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|hex
argument_list|,
literal|"%02x"
argument_list|,
name|i
argument_list|)
expr_stmt|;
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
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
operator|!=
literal|38
condition|)
continue|continue;
name|memcpy
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|has_sha1_pack
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
condition|)
continue|continue;
name|memcpy
argument_list|(
name|pathname
operator|+
name|len
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|&
name|DRY_RUN
condition|)
name|printf
argument_list|(
literal|"rm -f %s\n"
argument_list|,
name|pathname
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|unlink
argument_list|(
name|pathname
argument_list|)
operator|<
literal|0
condition|)
name|error
argument_list|(
literal|"unable to unlink %s"
argument_list|,
name|pathname
argument_list|)
expr_stmt|;
block|}
name|pathname
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|rmdir
argument_list|(
name|pathname
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|prune_packed_objects
name|void
name|prune_packed_objects
parameter_list|(
name|int
name|opts
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
specifier|static
name|char
name|pathname
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|dir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|dir
argument_list|)
decl_stmt|;
if|if
condition|(
name|opts
operator|==
name|VERBOSE
condition|)
name|progress
operator|=
name|start_progress_delay
argument_list|(
literal|"Removing duplicate objects"
argument_list|,
literal|256
argument_list|,
literal|95
argument_list|,
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
name|PATH_MAX
operator|-
literal|42
condition|)
name|die
argument_list|(
literal|"impossible object directory"
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|pathname
argument_list|,
name|dir
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|pathname
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
name|pathname
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
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
name|DIR
modifier|*
name|d
decl_stmt|;
name|sprintf
argument_list|(
name|pathname
operator|+
name|len
argument_list|,
literal|"%02x/"
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|d
operator|=
name|opendir
argument_list|(
name|pathname
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|d
condition|)
continue|continue;
name|prune_dir
argument_list|(
name|i
argument_list|,
name|d
argument_list|,
name|pathname
argument_list|,
name|len
operator|+
literal|3
argument_list|,
name|opts
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|d
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|==
name|VERBOSE
condition|)
name|stop_progress
argument_list|(
operator|&
name|progress
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_prune_packed
name|int
name|cmd_prune_packed
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
name|int
name|i
decl_stmt|;
name|int
name|opts
init|=
name|VERBOSE
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
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
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-n"
argument_list|)
condition|)
name|opts
operator||=
name|DRY_RUN
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-q"
argument_list|)
condition|)
name|opts
operator|&=
operator|~
name|VERBOSE
expr_stmt|;
else|else
name|usage
argument_list|(
name|prune_packed_usage
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* Handle arguments here .. */
name|usage
argument_list|(
name|prune_packed_usage
argument_list|)
expr_stmt|;
block|}
name|sync
argument_list|()
expr_stmt|;
name|prune_packed_objects
argument_list|(
name|opts
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
