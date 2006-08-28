begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
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
file|"tag.h"
end_include
begin_decl_stmt
DECL|variable|obj_hash
specifier|static
name|struct
name|object
modifier|*
modifier|*
name|obj_hash
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_objs
DECL|variable|obj_hash_size
specifier|static
name|int
name|nr_objs
decl_stmt|,
name|obj_hash_size
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_max_object_index
name|unsigned
name|int
name|get_max_object_index
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|obj_hash_size
return|;
block|}
end_function
begin_function
DECL|function|get_indexed_object
name|struct
name|object
modifier|*
name|get_indexed_object
parameter_list|(
name|unsigned
name|int
name|idx
parameter_list|)
block|{
return|return
name|obj_hash
index|[
name|idx
index|]
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|type_names
specifier|const
name|char
modifier|*
name|type_names
index|[]
init|=
block|{
literal|"none"
block|,
literal|"commit"
block|,
literal|"tree"
block|,
literal|"blob"
block|,
literal|"tag"
block|,
literal|"bad type 5"
block|,
literal|"bad type 6"
block|,
literal|"delta"
block|,
literal|"bad"
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|hash_obj
specifier|static
name|unsigned
name|int
name|hash_obj
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|unsigned
name|int
name|n
parameter_list|)
block|{
name|unsigned
name|int
name|hash
init|=
operator|*
operator|(
name|unsigned
name|int
operator|*
operator|)
name|obj
operator|->
name|sha1
decl_stmt|;
return|return
name|hash
operator|%
name|n
return|;
block|}
end_function
begin_function
DECL|function|insert_obj_hash
specifier|static
name|void
name|insert_obj_hash
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|struct
name|object
modifier|*
modifier|*
name|hash
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|int
name|j
init|=
name|hash_obj
argument_list|(
name|obj
argument_list|,
name|size
argument_list|)
decl_stmt|;
while|while
condition|(
name|hash
index|[
name|j
index|]
condition|)
block|{
name|j
operator|++
expr_stmt|;
if|if
condition|(
name|j
operator|>=
name|size
condition|)
name|j
operator|=
literal|0
expr_stmt|;
block|}
name|hash
index|[
name|j
index|]
operator|=
name|obj
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hashtable_index
specifier|static
name|int
name|hashtable_index
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|int
name|i
decl_stmt|;
name|memcpy
argument_list|(
operator|&
name|i
argument_list|,
name|sha1
argument_list|,
expr|sizeof
operator|(
name|unsigned
name|int
operator|)
argument_list|)
expr_stmt|;
return|return
call|(
name|int
call|)
argument_list|(
name|i
operator|%
name|obj_hash_size
argument_list|)
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
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
if|if
condition|(
operator|!
name|obj_hash
condition|)
return|return
name|NULL
return|;
name|i
operator|=
name|hashtable_index
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|obj
operator|=
name|obj_hash
index|[
name|i
index|]
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
operator|!
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|obj
operator|->
name|sha1
argument_list|)
condition|)
break|break;
name|i
operator|++
expr_stmt|;
if|if
condition|(
name|i
operator|==
name|obj_hash_size
condition|)
name|i
operator|=
literal|0
expr_stmt|;
block|}
return|return
name|obj
return|;
block|}
end_function
begin_function
DECL|function|grow_object_hash
specifier|static
name|void
name|grow_object_hash
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|new_hash_size
init|=
name|obj_hash_size
operator|<
literal|32
condition|?
literal|32
else|:
literal|2
operator|*
name|obj_hash_size
decl_stmt|;
name|struct
name|object
modifier|*
modifier|*
name|new_hash
decl_stmt|;
name|new_hash
operator|=
name|xcalloc
argument_list|(
name|new_hash_size
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|object
operator|*
argument_list|)
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
name|obj_hash_size
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
name|obj_hash
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
continue|continue;
name|insert_obj_hash
argument_list|(
name|obj
argument_list|,
name|new_hash
argument_list|,
name|new_hash_size
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|obj_hash
argument_list|)
expr_stmt|;
name|obj_hash
operator|=
name|new_hash
expr_stmt|;
name|obj_hash_size
operator|=
name|new_hash_size
expr_stmt|;
block|}
end_function
begin_function
DECL|function|created_object
name|void
name|created_object
parameter_list|(
specifier|const
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
name|obj
operator|->
name|parsed
operator|=
literal|0
expr_stmt|;
name|obj
operator|->
name|used
operator|=
literal|0
expr_stmt|;
name|obj
operator|->
name|type
operator|=
name|OBJ_NONE
expr_stmt|;
name|obj
operator|->
name|flags
operator|=
literal|0
expr_stmt|;
name|hashcpy
argument_list|(
name|obj
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj_hash_size
operator|-
literal|1
operator|<=
name|nr_objs
operator|*
literal|2
condition|)
name|grow_object_hash
argument_list|()
expr_stmt|;
name|insert_obj_hash
argument_list|(
name|obj
argument_list|,
name|obj_hash
argument_list|,
name|obj_hash_size
argument_list|)
expr_stmt|;
name|nr_objs
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|lookup_object_type
name|struct
name|object
modifier|*
name|lookup_object_type
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
name|type
parameter_list|)
block|{
if|if
condition|(
operator|!
name|type
condition|)
block|{
return|return
name|lookup_unknown_object
argument_list|(
name|sha1
argument_list|)
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
name|blob_type
argument_list|)
condition|)
block|{
return|return
operator|&
name|lookup_blob
argument_list|(
name|sha1
argument_list|)
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
name|tree_type
argument_list|)
condition|)
block|{
return|return
operator|&
name|lookup_tree
argument_list|(
name|sha1
argument_list|)
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
name|commit_type
argument_list|)
condition|)
block|{
return|return
operator|&
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
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
name|tag_type
argument_list|)
condition|)
block|{
return|return
operator|&
name|lookup_tag
argument_list|(
name|sha1
argument_list|)
operator|->
name|object
return|;
block|}
else|else
block|{
name|error
argument_list|(
literal|"Unknown type %s"
argument_list|,
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
end_function
begin_union
DECL|union|any_object
union|union
name|any_object
block|{
DECL|member|object
name|struct
name|object
name|object
decl_stmt|;
DECL|member|commit
name|struct
name|commit
name|commit
decl_stmt|;
DECL|member|tree
name|struct
name|tree
name|tree
decl_stmt|;
DECL|member|blob
name|struct
name|blob
name|blob
decl_stmt|;
DECL|member|tag
name|struct
name|tag
name|tag
decl_stmt|;
block|}
union|;
end_union
begin_function
DECL|function|lookup_unknown_object
name|struct
name|object
modifier|*
name|lookup_unknown_object
parameter_list|(
specifier|const
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
name|union
name|any_object
modifier|*
name|ret
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ret
argument_list|)
argument_list|)
decl_stmt|;
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
name|OBJ_NONE
expr_stmt|;
return|return
operator|&
name|ret
operator|->
name|object
return|;
block|}
return|return
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_object
name|struct
name|object
modifier|*
name|parse_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buffer
init|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|buffer
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|blob_type
argument_list|)
condition|)
block|{
name|struct
name|blob
modifier|*
name|blob
init|=
name|lookup_blob
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_blob_buffer
argument_list|(
name|blob
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|obj
operator|=
operator|&
name|blob
operator|->
name|object
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|tree_type
argument_list|)
condition|)
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
name|obj
operator|=
operator|&
name|tree
operator|->
name|object
expr_stmt|;
if|if
condition|(
operator|!
name|tree
operator|->
name|object
operator|.
name|parsed
condition|)
block|{
name|parse_tree_buffer
argument_list|(
name|tree
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|commit_type
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_commit_buffer
argument_list|(
name|commit
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|buffer
condition|)
block|{
name|commit
operator|->
name|buffer
operator|=
name|buffer
expr_stmt|;
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
name|obj
operator|=
operator|&
name|commit
operator|->
name|object
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|tag_type
argument_list|)
condition|)
block|{
name|struct
name|tag
modifier|*
name|tag
init|=
name|lookup_tag
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|parse_tag_buffer
argument_list|(
name|tag
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|obj
operator|=
operator|&
name|tag
operator|->
name|object
expr_stmt|;
block|}
else|else
block|{
name|obj
operator|=
name|NULL
expr_stmt|;
block|}
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|obj
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|object_list_insert
name|struct
name|object_list
modifier|*
name|object_list_insert
parameter_list|(
name|struct
name|object
modifier|*
name|item
parameter_list|,
name|struct
name|object_list
modifier|*
modifier|*
name|list_p
parameter_list|)
block|{
name|struct
name|object_list
modifier|*
name|new_list
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|object_list
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
return|return
name|new_list
return|;
block|}
end_function
begin_function
DECL|function|object_list_append
name|void
name|object_list_append
parameter_list|(
name|struct
name|object
modifier|*
name|item
parameter_list|,
name|struct
name|object_list
modifier|*
modifier|*
name|list_p
parameter_list|)
block|{
while|while
condition|(
operator|*
name|list_p
condition|)
block|{
name|list_p
operator|=
operator|&
operator|(
operator|(
operator|*
name|list_p
operator|)
operator|->
name|next
operator|)
expr_stmt|;
block|}
operator|*
name|list_p
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|object_list
argument_list|)
argument_list|)
expr_stmt|;
operator|(
operator|*
name|list_p
operator|)
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
operator|(
operator|*
name|list_p
operator|)
operator|->
name|item
operator|=
name|item
expr_stmt|;
block|}
end_function
begin_function
DECL|function|object_list_length
name|unsigned
name|object_list_length
parameter_list|(
name|struct
name|object_list
modifier|*
name|list
parameter_list|)
block|{
name|unsigned
name|ret
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|list
condition|)
block|{
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
name|ret
operator|++
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|object_list_contains
name|int
name|object_list_contains
parameter_list|(
name|struct
name|object_list
modifier|*
name|list
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
if|if
condition|(
name|list
operator|->
name|item
operator|==
name|obj
condition|)
return|return
literal|1
return|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_object_array
name|void
name|add_object_array
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|object_array
modifier|*
name|array
parameter_list|)
block|{
name|unsigned
name|nr
init|=
name|array
operator|->
name|nr
decl_stmt|;
name|unsigned
name|alloc
init|=
name|array
operator|->
name|alloc
decl_stmt|;
name|struct
name|object_array_entry
modifier|*
name|objects
init|=
name|array
operator|->
name|objects
decl_stmt|;
if|if
condition|(
name|nr
operator|>=
name|alloc
condition|)
block|{
name|alloc
operator|=
operator|(
name|alloc
operator|+
literal|32
operator|)
operator|*
literal|2
expr_stmt|;
name|objects
operator|=
name|xrealloc
argument_list|(
name|objects
argument_list|,
name|alloc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|objects
argument_list|)
argument_list|)
expr_stmt|;
name|array
operator|->
name|alloc
operator|=
name|alloc
expr_stmt|;
name|array
operator|->
name|objects
operator|=
name|objects
expr_stmt|;
block|}
name|objects
index|[
name|nr
index|]
operator|.
name|item
operator|=
name|obj
expr_stmt|;
name|objects
index|[
name|nr
index|]
operator|.
name|name
operator|=
name|name
expr_stmt|;
name|array
operator|->
name|nr
operator|=
operator|++
name|nr
expr_stmt|;
block|}
end_function
end_unit
