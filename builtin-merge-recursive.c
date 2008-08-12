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
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"merge-recursive.h"
end_include
begin_function
DECL|function|better_branch_name
specifier|static
specifier|const
name|char
modifier|*
name|better_branch_name
parameter_list|(
specifier|const
name|char
modifier|*
name|branch
parameter_list|)
block|{
specifier|static
name|char
name|githead_env
index|[
literal|8
operator|+
literal|40
operator|+
literal|1
index|]
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|branch
argument_list|)
operator|!=
literal|40
condition|)
return|return
name|branch
return|;
name|sprintf
argument_list|(
name|githead_env
argument_list|,
literal|"GITHEAD_%s"
argument_list|,
name|branch
argument_list|)
expr_stmt|;
name|name
operator|=
name|getenv
argument_list|(
name|githead_env
argument_list|)
expr_stmt|;
return|return
name|name
condition|?
name|name
else|:
name|branch
return|;
block|}
end_function
begin_function
DECL|function|get_ref
specifier|static
name|struct
name|commit
modifier|*
name|get_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|object
modifier|*
name|object
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|ref
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not resolve ref '%s'"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
name|object
operator|=
name|deref_tag
argument_list|(
name|parse_object
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|ref
argument_list|,
name|strlen
argument_list|(
name|ref
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|object
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|object
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
return|return
name|make_virtual_commit
argument_list|(
operator|(
expr|struct
name|tree
operator|*
operator|)
name|object
argument_list|,
name|better_branch_name
argument_list|(
name|ref
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|object
operator|->
name|type
operator|!=
name|OBJ_COMMIT
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|parse_commit
argument_list|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|object
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not parse commit '%s'"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|(
expr|struct
name|commit
operator|*
operator|)
name|object
return|;
block|}
end_function
begin_function
DECL|function|cmd_merge_recursive
name|int
name|cmd_merge_recursive
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
specifier|static
specifier|const
name|char
modifier|*
name|bases
index|[
literal|20
index|]
decl_stmt|;
specifier|static
name|unsigned
name|bases_count
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|,
name|clean
decl_stmt|;
specifier|const
name|char
modifier|*
name|branch1
decl_stmt|,
modifier|*
name|branch2
decl_stmt|;
name|struct
name|commit
modifier|*
name|result
decl_stmt|,
modifier|*
name|h1
decl_stmt|,
modifier|*
name|h2
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|ca
init|=
name|NULL
decl_stmt|;
name|struct
name|lock_file
modifier|*
name|lock
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
name|index_fd
decl_stmt|;
name|int
name|subtree_merge
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|argv
index|[
literal|0
index|]
condition|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
literal|8
operator|<
name|namelen
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
operator|+
name|namelen
operator|-
literal|8
argument_list|,
literal|"-subtree"
argument_list|)
condition|)
name|subtree_merge
operator|=
literal|1
expr_stmt|;
block|}
name|git_config
argument_list|(
name|merge_recursive_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|merge_recursive_setup
argument_list|(
name|subtree_merge
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|4
condition|)
name|die
argument_list|(
literal|"Usage: %s<base>... --<head><remote> ...\n"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
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
operator|++
name|i
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
break|break;
if|if
condition|(
name|bases_count
operator|<
sizeof|sizeof
argument_list|(
name|bases
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
operator|*
name|bases
argument_list|)
condition|)
name|bases
index|[
name|bases_count
operator|++
index|]
operator|=
name|argv
index|[
name|i
index|]
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|-
name|i
operator|!=
literal|3
condition|)
comment|/* "--" "<head>" "<remote>" */
name|die
argument_list|(
literal|"Not handling anything other than two heads merge."
argument_list|)
expr_stmt|;
name|branch1
operator|=
name|argv
index|[
operator|++
name|i
index|]
expr_stmt|;
name|branch2
operator|=
name|argv
index|[
operator|++
name|i
index|]
expr_stmt|;
name|h1
operator|=
name|get_ref
argument_list|(
name|branch1
argument_list|)
expr_stmt|;
name|h2
operator|=
name|get_ref
argument_list|(
name|branch2
argument_list|)
expr_stmt|;
name|branch1
operator|=
name|better_branch_name
argument_list|(
name|branch1
argument_list|)
expr_stmt|;
name|branch2
operator|=
name|better_branch_name
argument_list|(
name|branch2
argument_list|)
expr_stmt|;
if|if
condition|(
name|merge_recursive_verbosity
operator|>=
literal|3
condition|)
name|printf
argument_list|(
literal|"Merging %s with %s\n"
argument_list|,
name|branch1
argument_list|,
name|branch2
argument_list|)
expr_stmt|;
name|index_fd
operator|=
name|hold_locked_index
argument_list|(
name|lock
argument_list|,
literal|1
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
name|bases_count
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|commit
modifier|*
name|ancestor
init|=
name|get_ref
argument_list|(
name|bases
index|[
name|i
index|]
argument_list|)
decl_stmt|;
name|ca
operator|=
name|commit_list_insert
argument_list|(
name|ancestor
argument_list|,
operator|&
name|ca
argument_list|)
expr_stmt|;
block|}
name|clean
operator|=
name|merge_recursive
argument_list|(
name|h1
argument_list|,
name|h2
argument_list|,
name|branch1
argument_list|,
name|branch2
argument_list|,
name|ca
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
if|if
condition|(
name|active_cache_changed
operator|&&
operator|(
name|write_cache
argument_list|(
name|index_fd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
name|lock
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"unable to write %s"
argument_list|,
name|get_index_file
argument_list|()
argument_list|)
expr_stmt|;
return|return
name|clean
condition|?
literal|0
else|:
literal|1
return|;
block|}
end_function
end_unit
