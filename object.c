begin_unit
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
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
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_decl_stmt
DECL|variable|objs
name|struct
name|object
modifier|*
modifier|*
name|objs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_objs
name|int
name|nr_objs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|obj_allocs
specifier|static
name|int
name|obj_allocs
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|find_object
specifier|static
name|int
name|find_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|first
init|=
literal|0
decl_stmt|,
name|last
init|=
name|nr_objs
decl_stmt|;
while|while
condition|(
name|first
operator|<
name|last
condition|)
block|{
name|int
name|next
init|=
operator|(
name|first
operator|+
name|last
operator|)
operator|/
literal|2
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
name|objs
index|[
name|next
index|]
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|obj
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|next
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
block|{
name|last
operator|=
name|next
expr_stmt|;
continue|continue;
block|}
name|first
operator|=
name|next
operator|+
literal|1
expr_stmt|;
block|}
return|return
operator|-
name|first
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|lookup_object
name|struct
name|object
modifier|*
name|lookup_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|pos
init|=
name|find_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|pos
operator|>=
literal|0
condition|)
return|return
name|objs
index|[
name|pos
index|]
return|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|created_object
name|void
name|created_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
name|int
name|pos
init|=
name|find_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|obj
operator|->
name|parsed
operator|=
literal|0
expr_stmt|;
name|memcpy
argument_list|(
name|obj
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|obj
operator|->
name|type
operator|=
name|NULL
expr_stmt|;
name|obj
operator|->
name|refs
operator|=
name|NULL
expr_stmt|;
name|obj
operator|->
name|used
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|pos
operator|>=
literal|0
condition|)
name|die
argument_list|(
literal|"Inserting %s twice\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|obj_allocs
operator|==
name|nr_objs
condition|)
block|{
name|obj_allocs
operator|=
name|alloc_nr
argument_list|(
name|obj_allocs
argument_list|)
expr_stmt|;
name|objs
operator|=
name|xrealloc
argument_list|(
name|objs
argument_list|,
name|obj_allocs
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|object
operator|*
argument_list|)
argument_list|)
expr_stmt|;
block|}
comment|/* Insert it into the right place */
name|memmove
argument_list|(
name|objs
operator|+
name|pos
operator|+
literal|1
argument_list|,
name|objs
operator|+
name|pos
argument_list|,
operator|(
name|nr_objs
operator|-
name|pos
operator|)
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|object
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|objs
index|[
name|pos
index|]
operator|=
name|obj
expr_stmt|;
name|nr_objs
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_ref
name|void
name|add_ref
parameter_list|(
name|struct
name|object
modifier|*
name|refer
parameter_list|,
name|struct
name|object
modifier|*
name|target
parameter_list|)
block|{
name|struct
name|object_list
modifier|*
modifier|*
name|pp
init|=
operator|&
name|refer
operator|->
name|refs
decl_stmt|;
name|struct
name|object_list
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
operator|==
name|target
condition|)
return|return;
name|pp
operator|=
operator|&
name|p
operator|->
name|next
expr_stmt|;
block|}
name|target
operator|->
name|used
operator|=
literal|1
expr_stmt|;
name|p
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|p
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|->
name|item
operator|=
name|target
expr_stmt|;
name|p
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
operator|*
name|pp
operator|=
name|p
expr_stmt|;
block|}
end_function
begin_function
DECL|function|mark_reachable
name|void
name|mark_reachable
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|unsigned
name|int
name|mask
parameter_list|)
block|{
name|struct
name|object_list
modifier|*
name|p
init|=
name|obj
operator|->
name|refs
decl_stmt|;
comment|/* If we've been here already, don't bother */
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
name|mask
condition|)
return|return;
name|obj
operator|->
name|flags
operator||=
name|mask
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|mark_reachable
argument_list|(
name|p
operator|->
name|item
argument_list|,
name|mask
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|parse_object
name|struct
name|object
modifier|*
name|parse_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|long
name|mapsize
decl_stmt|;
name|void
modifier|*
name|map
init|=
name|map_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|mapsize
argument_list|)
decl_stmt|;
if|if
condition|(
name|map
condition|)
block|{
name|char
name|type
index|[
literal|100
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|buffer
init|=
name|unpack_sha1_file
argument_list|(
name|map
argument_list|,
name|mapsize
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
decl_stmt|;
name|munmap
argument_list|(
name|map
argument_list|,
name|mapsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|check_sha1_signature
argument_list|(
name|sha1
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|,
name|type
argument_list|)
operator|<
literal|0
condition|)
name|printf
argument_list|(
literal|"sha1 mismatch %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
block|{
name|struct
name|blob
modifier|*
name|ret
init|=
name|lookup_blob
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_blob
argument_list|(
name|ret
argument_list|)
expr_stmt|;
return|return
operator|&
name|ret
operator|->
name|object
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"tree"
argument_list|)
condition|)
block|{
name|struct
name|tree
modifier|*
name|ret
init|=
name|lookup_tree
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_tree
argument_list|(
name|ret
argument_list|)
expr_stmt|;
return|return
operator|&
name|ret
operator|->
name|object
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"commit"
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|ret
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_commit
argument_list|(
name|ret
argument_list|)
expr_stmt|;
return|return
operator|&
name|ret
operator|->
name|object
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"tag"
argument_list|)
condition|)
block|{
name|struct
name|tag
modifier|*
name|ret
init|=
name|lookup_tag
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_tag
argument_list|(
name|ret
argument_list|)
expr_stmt|;
return|return
operator|&
name|ret
operator|->
name|object
return|;
block|}
else|else
block|{
return|return
name|NULL
return|;
block|}
block|}
return|return
name|NULL
return|;
block|}
end_function
end_unit
