begin_unit
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_decl_stmt
DECL|variable|commit_type
specifier|const
name|char
modifier|*
name|commit_type
init|=
literal|"commit"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|lookup_commit
name|struct
name|commit
modifier|*
name|lookup_commit
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
block|{
name|struct
name|commit
modifier|*
name|ret
init|=
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit
argument_list|)
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ret
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|commit
argument_list|)
argument_list|)
expr_stmt|;
name|created_object
argument_list|(
name|sha1
argument_list|,
operator|&
name|ret
operator|->
name|object
argument_list|)
expr_stmt|;
name|ret
operator|->
name|object
operator|.
name|type
operator|=
name|commit_type
expr_stmt|;
return|return
name|ret
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|commit_type
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a commit"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_commit_date
specifier|static
name|unsigned
name|long
name|parse_commit_date
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|)
block|{
name|unsigned
name|long
name|date
decl_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"author"
argument_list|,
literal|6
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'\n'
condition|)
comment|/* nada */
empty_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"committer"
argument_list|,
literal|9
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'>'
condition|)
comment|/* nada */
empty_stmt|;
name|date
operator|=
name|strtoul
argument_list|(
name|buf
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|date
operator|==
name|ULONG_MAX
condition|)
name|date
operator|=
literal|0
expr_stmt|;
return|return
name|date
return|;
block|}
end_function
begin_function
DECL|function|parse_commit
name|int
name|parse_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|)
block|{
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|bufptr
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|item
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
name|buffer
operator|=
name|bufptr
operator|=
name|read_sha1_file
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
name|error
argument_list|(
literal|"Could not read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
name|commit_type
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Object %s not a commit"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
name|get_sha1_hex
argument_list|(
name|bufptr
operator|+
literal|5
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|item
operator|->
name|tree
operator|=
name|lookup_tree
argument_list|(
name|parent
argument_list|)
expr_stmt|;
name|add_ref
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
operator|&
name|item
operator|->
name|tree
operator|->
name|object
argument_list|)
expr_stmt|;
name|bufptr
operator|+=
literal|46
expr_stmt|;
comment|/* "tree " + "hex sha1" + "\n" */
while|while
condition|(
operator|!
name|memcmp
argument_list|(
name|bufptr
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|bufptr
operator|+
literal|7
argument_list|,
name|parent
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|new_parent
init|=
name|lookup_commit
argument_list|(
name|parent
argument_list|)
decl_stmt|;
name|commit_list_insert
argument_list|(
name|new_parent
argument_list|,
operator|&
name|item
operator|->
name|parents
argument_list|)
expr_stmt|;
name|add_ref
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
operator|&
name|new_parent
operator|->
name|object
argument_list|)
expr_stmt|;
name|bufptr
operator|+=
literal|48
expr_stmt|;
block|}
name|item
operator|->
name|date
operator|=
name|parse_commit_date
argument_list|(
name|bufptr
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|commit_list_insert
name|void
name|commit_list_insert
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|list_p
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|new_list
init|=
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_list
argument_list|)
argument_list|)
decl_stmt|;
name|new_list
operator|->
name|item
operator|=
name|item
expr_stmt|;
name|new_list
operator|->
name|next
operator|=
operator|*
name|list_p
expr_stmt|;
operator|*
name|list_p
operator|=
name|new_list
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_commit_list
name|void
name|free_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|temp
init|=
name|list
decl_stmt|;
name|list
operator|=
name|temp
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|temp
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|insert_by_date
specifier|static
name|void
name|insert_by_date
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|struct
name|commit
modifier|*
name|item
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
modifier|*
name|pp
init|=
name|list
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
while|while
condition|(
operator|(
name|p
operator|=
operator|*
name|pp
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|p
operator|->
name|item
operator|->
name|date
operator|<
name|item
operator|->
name|date
condition|)
block|{
break|break;
block|}
name|pp
operator|=
operator|&
name|p
operator|->
name|next
expr_stmt|;
block|}
name|commit_list_insert
argument_list|(
name|item
argument_list|,
name|pp
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sort_by_date
name|void
name|sort_by_date
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|ret
init|=
name|NULL
decl_stmt|;
while|while
condition|(
operator|*
name|list
condition|)
block|{
name|insert_by_date
argument_list|(
operator|&
name|ret
argument_list|,
operator|(
operator|*
name|list
operator|)
operator|->
name|item
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
operator|(
operator|*
name|list
operator|)
operator|->
name|next
expr_stmt|;
block|}
operator|*
name|list
operator|=
name|ret
expr_stmt|;
block|}
end_function
begin_function
DECL|function|pop_most_recent_commit
name|struct
name|commit
modifier|*
name|pop_most_recent_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|unsigned
name|int
name|mark
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|ret
init|=
operator|(
operator|*
name|list
operator|)
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|ret
operator|->
name|parents
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|old
init|=
operator|*
name|list
decl_stmt|;
operator|*
name|list
operator|=
operator|(
operator|*
name|list
operator|)
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|old
argument_list|)
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parse_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|mark
operator|)
condition|)
block|{
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|mark
expr_stmt|;
name|insert_by_date
argument_list|(
name|list
argument_list|,
name|commit
argument_list|)
expr_stmt|;
block|}
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
end_unit
