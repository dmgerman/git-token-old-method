begin_unit
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
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
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|"rsh.h"
end_include
begin_decl_stmt
DECL|variable|tree
specifier|static
name|int
name|tree
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|commits
specifier|static
name|int
name|commits
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|all
specifier|static
name|int
name|all
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|fd_in
specifier|static
name|int
name|fd_in
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|fd_out
specifier|static
name|int
name|fd_out
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fetch
specifier|static
name|int
name|fetch
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
name|write
argument_list|(
name|fd_out
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|write_sha1_from_fd
argument_list|(
name|sha1
argument_list|,
name|fd_in
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|process_tree
specifier|static
name|int
name|process_tree
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
init|=
name|lookup_tree
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|tree_entry_list
modifier|*
name|entries
decl_stmt|;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|entries
operator|=
name|tree
operator|->
name|entries
init|;
name|entries
condition|;
name|entries
operator|=
name|entries
operator|->
name|next
control|)
block|{
comment|/* 		  fprintf(stderr, "Tree %s ", sha1_to_hex(sha1)); 		  fprintf(stderr, "needs %s\n",  		  sha1_to_hex(entries->item.tree->object.sha1)); 		*/
if|if
condition|(
name|fetch
argument_list|(
name|entries
operator|->
name|item
operator|.
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
block|{
return|return
name|error
argument_list|(
literal|"Missing item %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entries
operator|->
name|item
operator|.
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
block|}
if|if
condition|(
name|entries
operator|->
name|directory
condition|)
block|{
if|if
condition|(
name|process_tree
argument_list|(
name|entries
operator|->
name|item
operator|.
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|process_commit
specifier|static
name|int
name|process_commit
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
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
name|fetch
argument_list|(
name|sha1
argument_list|)
condition|)
block|{
return|return
name|error
argument_list|(
literal|"Fetching %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
block|}
if|if
condition|(
name|parse_commit
argument_list|(
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|tree
condition|)
block|{
if|if
condition|(
name|fetch
argument_list|(
name|obj
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|process_tree
argument_list|(
name|obj
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|all
condition|)
name|tree
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|commits
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|obj
operator|->
name|parents
decl_stmt|;
for|for
control|(
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
block|{
if|if
condition|(
name|has_sha1_file
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|fetch
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
condition|)
block|{
comment|/* The server might not have it, and 				 * we don't mind.  				 */
name|error
argument_list|(
literal|"Missing tree %s; continuing"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
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
condition|)
return|return
operator|-
literal|1
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
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
name|char
modifier|*
name|commit_id
decl_stmt|;
name|char
modifier|*
name|url
decl_stmt|;
name|int
name|arg
init|=
literal|1
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
name|arg
operator|<
name|argc
operator|&&
name|argv
index|[
name|arg
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'t'
condition|)
block|{
name|tree
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'c'
condition|)
block|{
name|commits
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'a'
condition|)
block|{
name|all
operator|=
literal|1
expr_stmt|;
name|tree
operator|=
literal|1
expr_stmt|;
name|commits
operator|=
literal|1
expr_stmt|;
block|}
name|arg
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|<
name|arg
operator|+
literal|2
condition|)
block|{
name|usage
argument_list|(
literal|"rpull [-c] [-t] [-a] commit-id url"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|commit_id
operator|=
name|argv
index|[
name|arg
index|]
expr_stmt|;
name|url
operator|=
name|argv
index|[
name|arg
operator|+
literal|1
index|]
expr_stmt|;
if|if
condition|(
name|setup_connection
argument_list|(
operator|&
name|fd_in
argument_list|,
operator|&
name|fd_out
argument_list|,
literal|"rpush"
argument_list|,
name|url
argument_list|,
name|arg
argument_list|,
name|argv
operator|+
literal|1
argument_list|)
condition|)
return|return
literal|1
return|;
name|get_sha1_hex
argument_list|(
name|commit_id
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|fetch
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|process_commit
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
