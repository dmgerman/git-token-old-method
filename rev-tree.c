begin_unit
begin_define
DECL|macro|_XOPEN_SOURCE
define|#
directive|define
name|_XOPEN_SOURCE
end_define
begin_comment
DECL|macro|_XOPEN_SOURCE
comment|/* glibc2 needs this */
end_comment
begin_define
DECL|macro|_BSD_SOURCE
define|#
directive|define
name|_BSD_SOURCE
end_define
begin_comment
DECL|macro|_BSD_SOURCE
comment|/* for tm.tm_gmtoff */
end_comment
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_include
include|#
directive|include
file|<ctype.h>
end_include
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
begin_comment
comment|/*  * revision.h leaves the low 16 bits of the "flags" field of the  * revision data structure unused. We use it for a "reachable from  * this commit<N>" bitmask.  */
end_comment
begin_define
DECL|macro|MAX_COMMITS
define|#
directive|define
name|MAX_COMMITS
value|16
end_define
begin_decl_stmt
DECL|variable|show_edges
specifier|static
name|int
name|show_edges
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|basemask
specifier|static
name|int
name|basemask
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_cache_file
specifier|static
name|void
name|read_cache_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|die
argument_list|(
literal|"no revtree cache file yet"
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Some revisions are less interesting than others.  *  * For example, if we use a cache-file, that one may contain  * revisions that were never used. They are never interesting.  *  * And sometimes we're only interested in "edge" commits, ie  * places where the marking changes between parent and child.  */
end_comment
begin_function
DECL|function|interesting
specifier|static
name|int
name|interesting
parameter_list|(
name|struct
name|commit
modifier|*
name|rev
parameter_list|)
block|{
name|unsigned
name|mask
init|=
name|rev
operator|->
name|object
operator|.
name|flags
decl_stmt|;
if|if
condition|(
operator|!
name|mask
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|show_edges
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|p
init|=
name|rev
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
name|mask
operator|!=
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|flags
condition|)
return|return
literal|1
return|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
name|mask
operator|&
name|basemask
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|process_commit
name|void
name|process_commit
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|struct
name|commit
modifier|*
name|obj
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|object
operator|.
name|parsed
condition|)
return|return;
name|parse_commit
argument_list|(
name|obj
argument_list|)
expr_stmt|;
name|parents
operator|=
name|obj
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|process_commit
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Usage: rev-tree [--edges] [--cache<cache-file>]<commit-id> [<commit-id2>]  *  * The cache-file can be quite important for big trees. This is an  * expensive operation if you have to walk the whole chain of  * parents in a tree with a long revision history.  */
end_comment
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
decl_stmt|;
name|int
name|nr
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
name|MAX_COMMITS
index|]
index|[
literal|20
index|]
decl_stmt|;
comment|/* 	 * First - pick up all the revisions we can (both from 	 * caches and from commit file chains). 	 */
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
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cache"
argument_list|)
condition|)
block|{
name|read_cache_file
argument_list|(
name|argv
index|[
operator|++
name|i
index|]
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--edges"
argument_list|)
condition|)
block|{
name|show_edges
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|==
literal|'^'
condition|)
block|{
name|arg
operator|++
expr_stmt|;
name|basemask
operator||=
literal|1
operator|<<
name|nr
expr_stmt|;
block|}
if|if
condition|(
name|nr
operator|>=
name|MAX_COMMITS
operator|||
name|get_sha1_hex
argument_list|(
name|arg
argument_list|,
name|sha1
index|[
name|nr
index|]
argument_list|)
condition|)
name|usage
argument_list|(
literal|"rev-tree [--edges] [--cache<cache-file>]<commit-id> [<commit-id>]"
argument_list|)
expr_stmt|;
name|process_commit
argument_list|(
name|sha1
index|[
name|nr
index|]
argument_list|)
expr_stmt|;
name|nr
operator|++
expr_stmt|;
block|}
comment|/* 	 * Now we have the maximal tree. Walk the different sha files back to the root. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr
condition|;
name|i
operator|++
control|)
name|mark_reachable
argument_list|(
operator|&
name|lookup_commit
argument_list|(
name|sha1
index|[
name|i
index|]
argument_list|)
operator|->
name|object
argument_list|,
literal|1
operator|<<
name|i
argument_list|)
expr_stmt|;
comment|/* 	 * Now print out the results.. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objs
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|objs
index|[
name|i
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|commit_type
condition|)
continue|continue;
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
expr_stmt|;
if|if
condition|(
operator|!
name|interesting
argument_list|(
name|commit
argument_list|)
condition|)
continue|continue;
name|printf
argument_list|(
literal|"%lu %s:%d"
argument_list|,
name|commit
operator|->
name|date
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|flags
argument_list|)
expr_stmt|;
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|printf
argument_list|(
literal|" %s:%d"
argument_list|,
name|sha1_to_hex
argument_list|(
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|flags
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
