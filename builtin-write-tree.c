begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
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
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_decl_stmt
DECL|variable|write_tree_usage
specifier|static
specifier|const
name|char
name|write_tree_usage
index|[]
init|=
literal|"git-write-tree [--missing-ok] [--prefix=<prefix>/]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|write_tree
name|int
name|write_tree
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|missing_ok
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|entries
decl_stmt|,
name|was_valid
decl_stmt|,
name|newfd
decl_stmt|;
comment|/* We can't free this memory, it becomes part of a linked list parsed atexit() */
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
name|entries
operator|=
name|read_cache
argument_list|()
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
if|if
condition|(
operator|!
name|active_cache_tree
condition|)
name|active_cache_tree
operator|=
name|cache_tree
argument_list|()
expr_stmt|;
name|was_valid
operator|=
name|cache_tree_fully_valid
argument_list|(
name|active_cache_tree
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|was_valid
condition|)
block|{
if|if
condition|(
name|cache_tree_update
argument_list|(
name|active_cache_tree
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|,
name|missing_ok
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"git-write-tree: error building trees"
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|newfd
condition|)
block|{
if|if
condition|(
operator|!
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|&&
operator|!
name|close
argument_list|(
name|newfd
argument_list|)
condition|)
block|{
name|commit_lock_file
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
name|newfd
operator|=
operator|-
literal|1
expr_stmt|;
block|}
block|}
comment|/* Not being able to write is fine -- we are only interested 		 * in updating the cache-tree part, and if the next caller 		 * ends up using the old index with unupdated cache-tree part 		 * it misses the work we did here, but that is just a 		 * performance penalty and not a big deal. 		 */
block|}
if|if
condition|(
name|prefix
condition|)
block|{
name|struct
name|cache_tree
modifier|*
name|subtree
init|=
name|cache_tree_find
argument_list|(
name|active_cache_tree
argument_list|,
name|prefix
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|subtree
condition|)
name|die
argument_list|(
literal|"git-write-tree: prefix %s not found"
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|subtree
operator|->
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|active_cache_tree
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|newfd
condition|)
name|close
argument_list|(
name|newfd
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_write_tree
name|int
name|cmd_write_tree
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
name|missing_ok
init|=
literal|0
decl_stmt|,
name|ret
decl_stmt|;
specifier|const
name|char
modifier|*
name|prefix
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
condition|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--missing-ok"
argument_list|)
condition|)
name|missing_ok
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--prefix="
argument_list|)
condition|)
name|prefix
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
else|else
name|usage
argument_list|(
name|write_tree_usage
argument_list|)
expr_stmt|;
name|argc
operator|--
expr_stmt|;
name|argv
operator|++
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
name|ret
operator|=
name|write_tree
argument_list|(
name|sha1
argument_list|,
name|missing_ok
argument_list|,
name|prefix
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
name|ret
return|;
block|}
end_function
end_unit
