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
DECL|variable|object_type_strings
specifier|static
specifier|const
name|char
modifier|*
name|object_type_strings
index|[]
init|=
block|{
name|NULL
block|,
comment|/* OBJ_NONE = 0 */
literal|"commit"
block|,
comment|/* OBJ_COMMIT = 1 */
literal|"tree"
block|,
comment|/* OBJ_TREE = 2 */
literal|"blob"
block|,
comment|/* OBJ_BLOB = 3 */
literal|"tag"
block|,
comment|/* OBJ_TAG = 4 */
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|typename
specifier|const
name|char
modifier|*
name|typename
parameter_list|(
name|unsigned
name|int
name|type
parameter_list|)
block|{
if|if
condition|(
name|type
operator|>=
name|ARRAY_SIZE
argument_list|(
name|object_type_strings
argument_list|)
condition|)
return|return
name|NULL
return|;
return|return
name|object_type_strings
index|[
name|type
index|]
return|;
block|}
end_function
begin_function
DECL|function|type_from_string_gently
name|int
name|type_from_string_gently
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|ssize_t
name|len
parameter_list|,
name|int
name|gentle
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
name|len
operator|=
name|strlen
argument_list|(
name|str
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
name|ARRAY_SIZE
argument_list|(
name|object_type_strings
argument_list|)
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|str
argument_list|,
name|object_type_strings
index|[
name|i
index|]
argument_list|,
name|len
argument_list|)
condition|)
return|return
name|i
return|;
if|if
condition|(
name|gentle
condition|)
return|return
operator|-
literal|1
return|;
name|die
argument_list|(
literal|"invalid object type \"%s\""
argument_list|,
name|str
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Return a numerical hash value between 0 and n-1 for the object with  * the specified sha1.  n must be a power of 2.  Please note that the  * return value is *not* consistent across computer architectures.  */
end_comment
begin_function
DECL|function|hash_obj
specifier|static
name|unsigned
name|int
name|hash_obj
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|int
name|n
parameter_list|)
block|{
return|return
name|sha1hash
argument_list|(
name|sha1
argument_list|)
operator|&
operator|(
name|n
operator|-
literal|1
operator|)
return|;
block|}
end_function
begin_comment
comment|/*  * Insert obj into the hash table hash, which has length size (which  * must be a power of 2).  On collisions, simply overflow to the next  * empty bucket.  */
end_comment
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
name|unsigned
name|int
name|j
init|=
name|hash_obj
argument_list|(
name|obj
operator|->
name|sha1
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
begin_comment
comment|/*  * Look up the record for the given sha1 in the hash map stored in  * obj_hash.  Return NULL if it was not found.  */
end_comment
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
name|unsigned
name|int
name|i
decl_stmt|,
name|first
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
name|first
operator|=
name|i
operator|=
name|hash_obj
argument_list|(
name|sha1
argument_list|,
name|obj_hash_size
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
if|if
condition|(
name|obj
operator|&&
name|i
operator|!=
name|first
condition|)
block|{
comment|/* 		 * Move object to where we started to look for it so 		 * that we do not need to walk the hash table the next 		 * time we look for it. 		 */
name|struct
name|object
modifier|*
name|tmp
init|=
name|obj_hash
index|[
name|i
index|]
decl_stmt|;
name|obj_hash
index|[
name|i
index|]
operator|=
name|obj_hash
index|[
name|first
index|]
expr_stmt|;
name|obj_hash
index|[
name|first
index|]
operator|=
name|tmp
expr_stmt|;
block|}
return|return
name|obj
return|;
block|}
end_function
begin_comment
comment|/*  * Increase the size of the hash map stored in obj_hash to the next  * power of 2 (but at least 32).  Copy the existing values to the new  * hash map.  */
end_comment
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
comment|/* 	 * Note that this size must always be power-of-2 to match hash_obj 	 * above. 	 */
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
DECL|function|create_object
name|void
modifier|*
name|create_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|o
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|o
decl_stmt|;
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
return|return
name|obj
return|;
block|}
end_function
begin_function
DECL|function|object_as_type
name|void
modifier|*
name|object_as_type
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
name|int
name|quiet
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|type
condition|)
return|return
name|obj
return|;
elseif|else
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_NONE
condition|)
block|{
if|if
condition|(
name|type
operator|==
name|OBJ_COMMIT
condition|)
operator|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
operator|)
operator|->
name|index
operator|=
name|alloc_commit_index
argument_list|()
expr_stmt|;
name|obj
operator|->
name|type
operator|=
name|type
expr_stmt|;
return|return
name|obj
return|;
block|}
else|else
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|error
argument_list|(
literal|"object %s is a %s, not a %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
name|typename
argument_list|(
name|obj
operator|->
name|type
argument_list|)
argument_list|,
name|typename
argument_list|(
name|type
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
end_function
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
name|obj
operator|=
name|create_object
argument_list|(
name|sha1
argument_list|,
name|alloc_object_node
argument_list|()
argument_list|)
expr_stmt|;
return|return
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_object_buffer
name|struct
name|object
modifier|*
name|parse_object_buffer
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|int
modifier|*
name|eaten_p
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
operator|*
name|eaten_p
operator|=
literal|0
expr_stmt|;
name|obj
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|type
operator|==
name|OBJ_BLOB
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
if|if
condition|(
name|blob
condition|)
block|{
if|if
condition|(
name|parse_blob_buffer
argument_list|(
name|blob
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
condition|)
return|return
name|NULL
return|;
name|obj
operator|=
operator|&
name|blob
operator|->
name|object
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|type
operator|==
name|OBJ_TREE
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
if|if
condition|(
name|tree
condition|)
block|{
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
name|buffer
condition|)
name|tree
operator|->
name|object
operator|.
name|parsed
operator|=
literal|0
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
if|if
condition|(
name|parse_tree_buffer
argument_list|(
name|tree
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
condition|)
return|return
name|NULL
return|;
operator|*
name|eaten_p
operator|=
literal|1
expr_stmt|;
block|}
block|}
block|}
elseif|else
if|if
condition|(
name|type
operator|==
name|OBJ_COMMIT
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
if|if
condition|(
name|commit
condition|)
block|{
if|if
condition|(
name|parse_commit_buffer
argument_list|(
name|commit
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|get_cached_commit_buffer
argument_list|(
name|commit
argument_list|,
name|NULL
argument_list|)
condition|)
block|{
name|set_commit_buffer
argument_list|(
name|commit
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
operator|*
name|eaten_p
operator|=
literal|1
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
block|}
elseif|else
if|if
condition|(
name|type
operator|==
name|OBJ_TAG
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
if|if
condition|(
name|tag
condition|)
block|{
if|if
condition|(
name|parse_tag_buffer
argument_list|(
name|tag
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
condition|)
return|return
name|NULL
return|;
name|obj
operator|=
operator|&
name|tag
operator|->
name|object
expr_stmt|;
block|}
block|}
else|else
block|{
name|warning
argument_list|(
literal|"object %s has unknown type id %d"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|obj
operator|=
name|NULL
expr_stmt|;
block|}
return|return
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_object_or_die
name|struct
name|object
modifier|*
name|parse_object_or_die
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
name|name
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|o
condition|)
return|return
name|o
return|;
name|die
argument_list|(
name|_
argument_list|(
literal|"unable to parse object: %s"
argument_list|)
argument_list|,
name|name
condition|?
name|name
else|:
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
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
name|enum
name|object_type
name|type
decl_stmt|;
name|int
name|eaten
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|repl
init|=
name|lookup_replace_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
name|obj
operator|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj
operator|&&
name|obj
operator|->
name|parsed
condition|)
return|return
name|obj
return|;
if|if
condition|(
operator|(
name|obj
operator|&&
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
operator|)
operator|||
operator|(
operator|!
name|obj
operator|&&
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
operator|&&
name|sha1_object_info
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
operator|==
name|OBJ_BLOB
operator|)
condition|)
block|{
if|if
condition|(
name|check_sha1_signature
argument_list|(
name|repl
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
operator|<
literal|0
condition|)
block|{
name|error
argument_list|(
literal|"sha1 mismatch %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|repl
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|parse_blob_buffer
argument_list|(
name|lookup_blob
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|lookup_object
argument_list|(
name|sha1
argument_list|)
return|;
block|}
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
condition|)
block|{
if|if
condition|(
name|check_sha1_signature
argument_list|(
name|repl
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|,
name|typename
argument_list|(
name|type
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"sha1 mismatch %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|repl
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|obj
operator|=
name|parse_object_buffer
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
name|size
argument_list|,
name|buffer
argument_list|,
operator|&
name|eaten
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eaten
condition|)
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
begin_comment
comment|/*  * A zero-length string to which object_array_entry::name can be  * initialized without requiring a malloc/free.  */
end_comment
begin_decl_stmt
DECL|variable|object_array_slopbuf
specifier|static
name|char
name|object_array_slopbuf
index|[
literal|1
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_object_array_with_mode_context
specifier|static
name|void
name|add_object_array_with_mode_context
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
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|struct
name|object_context
modifier|*
name|context
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
name|struct
name|object_array_entry
modifier|*
name|entry
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
name|REALLOC_ARRAY
argument_list|(
name|objects
argument_list|,
name|alloc
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
name|entry
operator|=
operator|&
name|objects
index|[
name|nr
index|]
expr_stmt|;
name|entry
operator|->
name|item
operator|=
name|obj
expr_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
name|entry
operator|->
name|name
operator|=
name|NULL
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
operator|*
name|name
condition|)
comment|/* Use our own empty string instead of allocating one: */
name|entry
operator|->
name|name
operator|=
name|object_array_slopbuf
expr_stmt|;
else|else
name|entry
operator|->
name|name
operator|=
name|xstrdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|entry
operator|->
name|mode
operator|=
name|mode
expr_stmt|;
name|entry
operator|->
name|context
operator|=
name|context
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
name|add_object_array_with_mode
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
name|array
argument_list|,
name|S_IFINVALID
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_object_array_with_mode
name|void
name|add_object_array_with_mode
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
parameter_list|,
name|unsigned
name|mode
parameter_list|)
block|{
name|add_object_array_with_mode_context
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
name|array
argument_list|,
name|mode
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_object_array_with_context
name|void
name|add_object_array_with_context
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
parameter_list|,
name|struct
name|object_context
modifier|*
name|context
parameter_list|)
block|{
if|if
condition|(
name|context
condition|)
name|add_object_array_with_mode_context
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
name|array
argument_list|,
name|context
operator|->
name|mode
argument_list|,
name|context
argument_list|)
expr_stmt|;
else|else
name|add_object_array_with_mode_context
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
name|array
argument_list|,
name|S_IFINVALID
argument_list|,
name|context
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|object_array_filter
name|void
name|object_array_filter
parameter_list|(
name|struct
name|object_array
modifier|*
name|array
parameter_list|,
name|object_array_each_func_t
name|want
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|unsigned
name|nr
init|=
name|array
operator|->
name|nr
decl_stmt|,
name|src
decl_stmt|,
name|dst
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
for|for
control|(
name|src
operator|=
name|dst
operator|=
literal|0
init|;
name|src
operator|<
name|nr
condition|;
name|src
operator|++
control|)
block|{
if|if
condition|(
name|want
argument_list|(
operator|&
name|objects
index|[
name|src
index|]
argument_list|,
name|cb_data
argument_list|)
condition|)
block|{
if|if
condition|(
name|src
operator|!=
name|dst
condition|)
name|objects
index|[
name|dst
index|]
operator|=
name|objects
index|[
name|src
index|]
expr_stmt|;
name|dst
operator|++
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|objects
index|[
name|src
index|]
operator|.
name|name
operator|!=
name|object_array_slopbuf
condition|)
name|free
argument_list|(
name|objects
index|[
name|src
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
block|}
name|array
operator|->
name|nr
operator|=
name|dst
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Return true iff array already contains an entry with name.  */
end_comment
begin_function
DECL|function|contains_name
specifier|static
name|int
name|contains_name
parameter_list|(
name|struct
name|object_array
modifier|*
name|array
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|unsigned
name|nr
init|=
name|array
operator|->
name|nr
decl_stmt|,
name|i
decl_stmt|;
name|struct
name|object_array_entry
modifier|*
name|object
init|=
name|array
operator|->
name|objects
decl_stmt|;
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
operator|,
name|object
operator|++
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|object
operator|->
name|name
argument_list|,
name|name
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
begin_function
DECL|function|object_array_remove_duplicates
name|void
name|object_array_remove_duplicates
parameter_list|(
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
decl_stmt|,
name|src
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
name|array
operator|->
name|nr
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|src
operator|=
literal|0
init|;
name|src
operator|<
name|nr
condition|;
name|src
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|contains_name
argument_list|(
name|array
argument_list|,
name|objects
index|[
name|src
index|]
operator|.
name|name
argument_list|)
condition|)
block|{
if|if
condition|(
name|src
operator|!=
name|array
operator|->
name|nr
condition|)
name|objects
index|[
name|array
operator|->
name|nr
index|]
operator|=
name|objects
index|[
name|src
index|]
expr_stmt|;
name|array
operator|->
name|nr
operator|++
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|objects
index|[
name|src
index|]
operator|.
name|name
operator|!=
name|object_array_slopbuf
condition|)
name|free
argument_list|(
name|objects
index|[
name|src
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|clear_object_flags
name|void
name|clear_object_flags
parameter_list|(
name|unsigned
name|flags
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
name|obj
condition|)
name|obj
operator|->
name|flags
operator|&=
operator|~
name|flags
expr_stmt|;
block|}
block|}
end_function
end_unit
