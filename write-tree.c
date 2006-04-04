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
file|"tree.h"
end_include
begin_decl_stmt
DECL|variable|missing_ok
specifier|static
name|int
name|missing_ok
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_valid_sha1
specifier|static
name|int
name|check_valid_sha1
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
comment|/* If we were anal, we'd check that the sha1 of the contents actually matches */
name|ret
operator|=
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|==
literal|0
condition|)
name|perror
argument_list|(
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|ret
condition|?
literal|0
else|:
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|write_tree
specifier|static
name|int
name|write_tree
parameter_list|(
name|struct
name|cache_entry
modifier|*
modifier|*
name|cachep
parameter_list|,
name|int
name|maxentries
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|unsigned
name|char
modifier|*
name|returnsha1
parameter_list|)
block|{
name|unsigned
name|char
name|subdir_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|;
name|int
name|nr
decl_stmt|;
comment|/* Guess at some random initial size */
name|size
operator|=
literal|8192
expr_stmt|;
name|buffer
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|offset
operator|=
literal|0
expr_stmt|;
name|nr
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|nr
operator|<
name|maxentries
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|cachep
index|[
name|nr
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|pathname
init|=
name|ce
operator|->
name|name
decl_stmt|,
modifier|*
name|filename
decl_stmt|,
modifier|*
name|dirname
decl_stmt|;
name|int
name|pathlen
init|=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
decl_stmt|,
name|entrylen
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
comment|/* Did we hit the end of the directory? Return how many we wrote */
if|if
condition|(
name|baselen
operator|>=
name|pathlen
operator|||
name|memcmp
argument_list|(
name|base
argument_list|,
name|pathname
argument_list|,
name|baselen
argument_list|)
condition|)
break|break;
name|sha1
operator|=
name|ce
operator|->
name|sha1
expr_stmt|;
name|mode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
comment|/* Do we have _further_ subdirectories? */
name|filename
operator|=
name|pathname
operator|+
name|baselen
expr_stmt|;
name|dirname
operator|=
name|strchr
argument_list|(
name|filename
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirname
condition|)
block|{
name|int
name|subdir_written
decl_stmt|;
name|subdir_written
operator|=
name|write_tree
argument_list|(
name|cachep
operator|+
name|nr
argument_list|,
name|maxentries
operator|-
name|nr
argument_list|,
name|pathname
argument_list|,
name|dirname
operator|-
name|pathname
operator|+
literal|1
argument_list|,
name|subdir_sha1
argument_list|)
expr_stmt|;
name|nr
operator|+=
name|subdir_written
expr_stmt|;
comment|/* Now we need to write out the directory entry into this tree.. */
name|mode
operator|=
name|S_IFDIR
expr_stmt|;
name|pathlen
operator|=
name|dirname
operator|-
name|pathname
expr_stmt|;
comment|/* ..but the directory entry doesn't count towards the total count */
name|nr
operator|--
expr_stmt|;
name|sha1
operator|=
name|subdir_sha1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|missing_ok
operator|&&
name|check_valid_sha1
argument_list|(
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|entrylen
operator|=
name|pathlen
operator|-
name|baselen
expr_stmt|;
if|if
condition|(
name|offset
operator|+
name|entrylen
operator|+
literal|100
operator|>
name|size
condition|)
block|{
name|size
operator|=
name|alloc_nr
argument_list|(
name|offset
operator|+
name|entrylen
operator|+
literal|100
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|offset
operator|+=
name|sprintf
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
literal|"%o %.*s"
argument_list|,
name|mode
argument_list|,
name|entrylen
argument_list|,
name|filename
argument_list|)
expr_stmt|;
name|buffer
index|[
name|offset
operator|++
index|]
operator|=
literal|0
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|offset
operator|+=
literal|20
expr_stmt|;
name|nr
operator|++
expr_stmt|;
block|}
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|offset
argument_list|,
name|tree_type
argument_list|,
name|returnsha1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|nr
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|write_tree_usage
specifier|static
specifier|const
name|char
name|write_tree_usage
index|[]
init|=
literal|"git-write-tree [--missing-ok]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|funny
decl_stmt|;
name|int
name|entries
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
name|entries
operator|=
name|read_cache
argument_list|()
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|2
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--missing-ok"
argument_list|)
condition|)
name|missing_ok
operator|=
literal|1
expr_stmt|;
else|else
name|die
argument_list|(
name|write_tree_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|>
literal|2
condition|)
name|die
argument_list|(
literal|"too many options"
argument_list|)
expr_stmt|;
if|if
condition|(
name|entries
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"git-write-tree: error reading cache"
argument_list|)
expr_stmt|;
comment|/* Verify that the tree is merged */
name|funny
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|entries
condition|;
name|i
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
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
if|if
condition|(
literal|10
operator|<
operator|++
name|funny
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"...\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: unmerged (%s)\n"
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
block|}
if|if
condition|(
name|funny
condition|)
name|die
argument_list|(
literal|"git-write-tree: not able to write tree"
argument_list|)
expr_stmt|;
comment|/* Also verify that the cache does not have path and path/file 	 * at the same time.  At this point we know the cache has only 	 * stage 0 entries. 	 */
name|funny
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|entries
operator|-
literal|1
condition|;
name|i
operator|++
control|)
block|{
comment|/* path/file always comes after path because of the way 		 * the cache is sorted.  Also path can appear only once, 		 * which means conflicting one would immediately follow. 		 */
specifier|const
name|char
modifier|*
name|this_name
init|=
name|active_cache
index|[
name|i
index|]
operator|->
name|name
decl_stmt|;
specifier|const
name|char
modifier|*
name|next_name
init|=
name|active_cache
index|[
name|i
operator|+
literal|1
index|]
operator|->
name|name
decl_stmt|;
name|int
name|this_len
init|=
name|strlen
argument_list|(
name|this_name
argument_list|)
decl_stmt|;
if|if
condition|(
name|this_len
operator|<
name|strlen
argument_list|(
name|next_name
argument_list|)
operator|&&
name|strncmp
argument_list|(
name|this_name
argument_list|,
name|next_name
argument_list|,
name|this_len
argument_list|)
operator|==
literal|0
operator|&&
name|next_name
index|[
name|this_len
index|]
operator|==
literal|'/'
condition|)
block|{
if|if
condition|(
literal|10
operator|<
operator|++
name|funny
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"...\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"You have both %s and %s\n"
argument_list|,
name|this_name
argument_list|,
name|next_name
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|funny
condition|)
name|die
argument_list|(
literal|"git-write-tree: not able to write tree"
argument_list|)
expr_stmt|;
comment|/* Ok, write it out */
if|if
condition|(
name|write_tree
argument_list|(
name|active_cache
argument_list|,
name|entries
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|sha1
argument_list|)
operator|!=
name|entries
condition|)
name|die
argument_list|(
literal|"git-write-tree: internal error"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
