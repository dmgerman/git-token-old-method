begin_unit
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"rev-cache.h"
end_include
begin_decl_stmt
DECL|variable|rev_cache
name|struct
name|rev_cache
modifier|*
modifier|*
name|rev_cache
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_revs
DECL|variable|alloc_revs
name|int
name|nr_revs
decl_stmt|,
name|alloc_revs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|rle_free
name|struct
name|rev_list_elem
modifier|*
name|rle_free
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|BATCH_SIZE
define|#
directive|define
name|BATCH_SIZE
value|512
end_define
begin_function
DECL|function|find_rev_cache
name|int
name|find_rev_cache
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|lo
init|=
literal|0
decl_stmt|,
name|hi
init|=
name|nr_revs
decl_stmt|;
while|while
condition|(
name|lo
operator|<
name|hi
condition|)
block|{
name|int
name|mi
init|=
operator|(
name|lo
operator|+
name|hi
operator|)
operator|/
literal|2
decl_stmt|;
name|struct
name|rev_cache
modifier|*
name|ri
init|=
name|rev_cache
index|[
name|mi
index|]
decl_stmt|;
name|int
name|cmp
init|=
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|ri
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|mi
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
name|hi
operator|=
name|mi
expr_stmt|;
else|else
name|lo
operator|=
name|mi
operator|+
literal|1
expr_stmt|;
block|}
return|return
operator|-
name|lo
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|alloc_list_elem
specifier|static
name|struct
name|rev_list_elem
modifier|*
name|alloc_list_elem
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|rev_list_elem
modifier|*
name|rle
decl_stmt|;
if|if
condition|(
operator|!
name|rle_free
condition|)
block|{
name|int
name|i
decl_stmt|;
name|rle
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|rle
argument_list|)
operator|*
name|BATCH_SIZE
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
name|BATCH_SIZE
operator|-
literal|1
condition|;
name|i
operator|++
control|)
block|{
name|rle
index|[
name|i
index|]
operator|.
name|ri
operator|=
name|NULL
expr_stmt|;
name|rle
index|[
name|i
index|]
operator|.
name|next
operator|=
operator|&
name|rle
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
block|}
name|rle
index|[
name|BATCH_SIZE
operator|-
literal|1
index|]
operator|.
name|ri
operator|=
name|NULL
expr_stmt|;
name|rle
index|[
name|BATCH_SIZE
operator|-
literal|1
index|]
operator|.
name|next
operator|=
name|NULL
expr_stmt|;
name|rle_free
operator|=
name|rle
expr_stmt|;
block|}
name|rle
operator|=
name|rle_free
expr_stmt|;
name|rle_free
operator|=
name|rle
operator|->
name|next
expr_stmt|;
return|return
name|rle
return|;
block|}
end_function
begin_function
DECL|function|create_rev_cache
specifier|static
name|struct
name|rev_cache
modifier|*
name|create_rev_cache
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|rev_cache
modifier|*
name|ri
decl_stmt|;
name|int
name|pos
init|=
name|find_rev_cache
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
return|return
name|rev_cache
index|[
name|pos
index|]
return|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|alloc_revs
operator|<=
operator|++
name|nr_revs
condition|)
block|{
name|alloc_revs
operator|=
name|alloc_nr
argument_list|(
name|alloc_revs
argument_list|)
expr_stmt|;
name|rev_cache
operator|=
name|xrealloc
argument_list|(
name|rev_cache
argument_list|,
sizeof|sizeof
argument_list|(
name|ri
argument_list|)
operator|*
name|alloc_revs
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|pos
operator|<
name|nr_revs
condition|)
name|memmove
argument_list|(
name|rev_cache
operator|+
name|pos
operator|+
literal|1
argument_list|,
name|rev_cache
operator|+
name|pos
argument_list|,
operator|(
name|nr_revs
operator|-
name|pos
operator|-
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|ri
argument_list|)
argument_list|)
expr_stmt|;
name|ri
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ri
argument_list|)
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ri
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|rev_cache
index|[
name|pos
index|]
operator|=
name|ri
expr_stmt|;
return|return
name|ri
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|last_sha1
specifier|static
name|unsigned
name|char
name|last_sha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|write_one_rev_cache
specifier|static
name|void
name|write_one_rev_cache
parameter_list|(
name|FILE
modifier|*
name|rev_cache_file
parameter_list|,
name|struct
name|rev_cache
modifier|*
name|ri
parameter_list|)
block|{
name|unsigned
name|char
name|flag
decl_stmt|;
name|struct
name|rev_list_elem
modifier|*
name|rle
decl_stmt|;
if|if
condition|(
name|ri
operator|->
name|written
condition|)
return|return;
if|if
condition|(
name|ri
operator|->
name|parsed
condition|)
block|{
comment|/* We use last_sha1 compression only for the first parent; 		 * otherwise the resulting rev-cache would lose the parent 		 * order information. 		 */
if|if
condition|(
name|ri
operator|->
name|parents
operator|&&
operator|!
name|memcmp
argument_list|(
name|ri
operator|->
name|parents
operator|->
name|ri
operator|->
name|sha1
argument_list|,
name|last_sha1
argument_list|,
literal|20
argument_list|)
condition|)
name|flag
operator|=
operator|(
name|ri
operator|->
name|num_parents
operator|-
literal|1
operator|)
operator||
literal|0x80
expr_stmt|;
else|else
name|flag
operator|=
name|ri
operator|->
name|num_parents
expr_stmt|;
name|fwrite
argument_list|(
name|ri
operator|->
name|sha1
argument_list|,
literal|20
argument_list|,
literal|1
argument_list|,
name|rev_cache_file
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
operator|&
name|flag
argument_list|,
literal|1
argument_list|,
literal|1
argument_list|,
name|rev_cache_file
argument_list|)
expr_stmt|;
for|for
control|(
name|rle
operator|=
name|ri
operator|->
name|parents
init|;
name|rle
condition|;
name|rle
operator|=
name|rle
operator|->
name|next
control|)
block|{
if|if
condition|(
name|flag
operator|&
literal|0x80
operator|&&
name|rle
operator|==
name|ri
operator|->
name|parents
condition|)
continue|continue;
name|fwrite
argument_list|(
name|rle
operator|->
name|ri
operator|->
name|sha1
argument_list|,
literal|20
argument_list|,
literal|1
argument_list|,
name|rev_cache_file
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|last_sha1
argument_list|,
name|ri
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|ri
operator|->
name|written
operator|=
literal|1
expr_stmt|;
block|}
comment|/* recursively write children depth first */
for|for
control|(
name|rle
operator|=
name|ri
operator|->
name|children
init|;
name|rle
condition|;
name|rle
operator|=
name|rle
operator|->
name|next
control|)
name|write_one_rev_cache
argument_list|(
name|rev_cache_file
argument_list|,
name|rle
operator|->
name|ri
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_rev_cache
name|void
name|write_rev_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|newpath
parameter_list|,
specifier|const
name|char
modifier|*
name|oldpath
parameter_list|)
block|{
comment|/* write the following commit ancestry information in 	 * $GIT_DIR/info/rev-cache. 	 * 	 * The format is: 	 * 20-byte SHA1 (commit ID) 	 * 1-byte flag: 	 * - bit 0-6 records "number of parent commit SHA1s to 	 *   follow" (i.e. up to 127 children can be listed). 	 * - when the bit 7 is on, then "the entry immediately 	 *   before this entry is one of the parents of this          *   commit". 	 * N x 20-byte SHA1 (parent commit IDs) 	 */
name|FILE
modifier|*
name|rev_cache_file
decl_stmt|;
name|int
name|i
decl_stmt|;
name|struct
name|rev_cache
modifier|*
name|ri
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|newpath
argument_list|,
name|oldpath
argument_list|)
condition|)
block|{
comment|/* If we are doing it in place */
name|rev_cache_file
operator|=
name|fopen
argument_list|(
name|newpath
argument_list|,
literal|"a"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|char
name|buf
index|[
literal|8096
index|]
decl_stmt|;
name|size_t
name|sz
decl_stmt|;
name|FILE
modifier|*
name|oldfp
init|=
name|fopen
argument_list|(
name|oldpath
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
name|rev_cache_file
operator|=
name|fopen
argument_list|(
name|newpath
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
if|if
condition|(
name|oldfp
condition|)
block|{
while|while
condition|(
literal|1
condition|)
block|{
name|sz
operator|=
name|fread
argument_list|(
name|buf
argument_list|,
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|oldfp
argument_list|)
expr_stmt|;
if|if
condition|(
name|sz
operator|==
literal|0
condition|)
break|break;
name|fwrite
argument_list|(
name|buf
argument_list|,
literal|1
argument_list|,
name|sz
argument_list|,
name|rev_cache_file
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|oldfp
argument_list|)
expr_stmt|;
block|}
block|}
name|memset
argument_list|(
name|last_sha1
argument_list|,
literal|0
argument_list|,
literal|20
argument_list|)
expr_stmt|;
comment|/* Go through available rev_cache structures, starting from 	 * parentless ones first, so that we would get most out of 	 * last_sha1 optimization by the depth first behaviour of 	 * write_one_rev_cache(). 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_revs
condition|;
name|i
operator|++
control|)
block|{
name|ri
operator|=
name|rev_cache
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|ri
operator|->
name|num_parents
condition|)
continue|continue;
name|write_one_rev_cache
argument_list|(
name|rev_cache_file
argument_list|,
name|ri
argument_list|)
expr_stmt|;
block|}
comment|/* Then the rest */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_revs
condition|;
name|i
operator|++
control|)
block|{
name|ri
operator|=
name|rev_cache
index|[
name|i
index|]
expr_stmt|;
name|write_one_rev_cache
argument_list|(
name|rev_cache_file
argument_list|,
name|ri
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|rev_cache_file
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_parent
specifier|static
name|void
name|add_parent
parameter_list|(
name|struct
name|rev_cache
modifier|*
name|child
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|parent_sha1
parameter_list|)
block|{
name|struct
name|rev_cache
modifier|*
name|parent
init|=
name|create_rev_cache
argument_list|(
name|parent_sha1
argument_list|)
decl_stmt|;
name|struct
name|rev_list_elem
modifier|*
name|e
init|=
name|alloc_list_elem
argument_list|()
decl_stmt|;
comment|/* Keep the parent list ordered in the same way the commit 	 * object records them. 	 */
name|e
operator|->
name|ri
operator|=
name|parent
expr_stmt|;
name|e
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|child
operator|->
name|parents_tail
condition|)
name|child
operator|->
name|parents
operator|=
name|e
expr_stmt|;
else|else
name|child
operator|->
name|parents_tail
operator|->
name|next
operator|=
name|e
expr_stmt|;
name|child
operator|->
name|parents_tail
operator|=
name|e
expr_stmt|;
name|child
operator|->
name|num_parents
operator|++
expr_stmt|;
comment|/* There is no inherent order of the children so we just 	 * LIFO them together. 	 */
name|e
operator|=
name|alloc_list_elem
argument_list|()
expr_stmt|;
name|e
operator|->
name|next
operator|=
name|parent
operator|->
name|children
expr_stmt|;
name|parent
operator|->
name|children
operator|=
name|e
expr_stmt|;
name|e
operator|->
name|ri
operator|=
name|child
expr_stmt|;
name|parent
operator|->
name|num_children
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|read_rev_cache
name|int
name|read_rev_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|FILE
modifier|*
name|dumpfile
parameter_list|,
name|int
name|dry_run
parameter_list|)
block|{
name|unsigned
name|char
modifier|*
name|map
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|unsigned
name|long
name|ofs
decl_stmt|,
name|len
decl_stmt|;
name|struct
name|rev_cache
modifier|*
name|ri
init|=
name|NULL
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|dry_run
condition|)
return|return
name|error
argument_list|(
literal|"cannot open %s"
argument_list|,
name|path
argument_list|)
return|;
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
return|return
literal|0
return|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|map
operator|==
name|MAP_FAILED
condition|)
return|return
operator|-
literal|1
return|;
name|memset
argument_list|(
name|last_sha1
argument_list|,
literal|0
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|ofs
operator|=
literal|0
expr_stmt|;
name|len
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
while|while
condition|(
name|ofs
operator|<
name|len
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|flag
decl_stmt|,
name|cnt
decl_stmt|,
name|i
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|ofs
operator|+
literal|21
condition|)
name|die
argument_list|(
literal|"rev-cache too short"
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|map
operator|+
name|ofs
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|flag
operator|=
name|map
index|[
name|ofs
operator|+
literal|20
index|]
expr_stmt|;
name|ofs
operator|+=
literal|21
expr_stmt|;
name|cnt
operator|=
operator|(
name|flag
operator|&
literal|0x7f
operator|)
operator|+
operator|(
operator|(
name|flag
operator|&
literal|0x80
operator|)
operator|!=
literal|0
operator|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
name|ofs
operator|+
operator|(
name|flag
operator|&
literal|0x7f
operator|)
operator|*
literal|20
condition|)
name|die
argument_list|(
literal|"rev-cache too short to have %d more parents"
argument_list|,
operator|(
name|flag
operator|&
literal|0x7f
operator|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|dumpfile
condition|)
name|fprintf
argument_list|(
name|dumpfile
argument_list|,
literal|"%s"
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
name|dry_run
condition|)
block|{
name|ri
operator|=
name|create_rev_cache
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ri
condition|)
name|die
argument_list|(
literal|"cannot create rev-cache for %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|ri
operator|->
name|written
operator|=
name|ri
operator|->
name|parsed
operator|=
literal|1
expr_stmt|;
block|}
name|i
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|flag
operator|&
literal|0x80
condition|)
block|{
if|if
condition|(
operator|!
name|dry_run
condition|)
name|add_parent
argument_list|(
name|ri
argument_list|,
name|last_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|dumpfile
condition|)
name|fprintf
argument_list|(
name|dumpfile
argument_list|,
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|last_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
while|while
condition|(
name|i
operator|++
operator|<
name|cnt
condition|)
block|{
if|if
condition|(
operator|!
name|dry_run
condition|)
name|add_parent
argument_list|(
name|ri
argument_list|,
name|map
operator|+
name|ofs
argument_list|)
expr_stmt|;
if|if
condition|(
name|dumpfile
condition|)
name|fprintf
argument_list|(
name|dumpfile
argument_list|,
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|last_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|ofs
operator|+=
literal|20
expr_stmt|;
block|}
if|if
condition|(
name|dumpfile
condition|)
name|fprintf
argument_list|(
name|dumpfile
argument_list|,
literal|"\n"
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|last_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|ofs
operator|!=
name|len
condition|)
name|die
argument_list|(
literal|"rev-cache truncated?"
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|map
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|record_rev_cache
name|int
name|record_rev_cache
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|FILE
modifier|*
name|dumpfile
parameter_list|)
block|{
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|,
name|ofs
decl_stmt|;
name|unsigned
name|int
name|cnt
decl_stmt|,
name|i
decl_stmt|;
name|void
modifier|*
name|buf
decl_stmt|;
name|struct
name|rev_cache
modifier|*
name|ri
decl_stmt|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
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
name|buf
condition|)
return|return
name|error
argument_list|(
literal|"%s: not found"
argument_list|,
name|sha1_to_hex
argument_list|(
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
literal|"commit"
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"%s: not a commit but a %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|type
argument_list|)
return|;
block|}
name|ri
operator|=
name|create_rev_cache
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|ri
operator|->
name|parsed
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|dumpfile
condition|)
name|fprintf
argument_list|(
name|dumpfile
argument_list|,
literal|"commit %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|cnt
operator|=
literal|0
expr_stmt|;
name|ofs
operator|=
literal|46
expr_stmt|;
comment|/* "tree " + hex-sha1 + "\n" */
while|while
condition|(
operator|!
name|memcmp
argument_list|(
name|buf
operator|+
name|ofs
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|buf
operator|+
name|ofs
operator|+
literal|7
argument_list|,
name|parent
argument_list|)
condition|)
block|{
name|ofs
operator|+=
literal|48
expr_stmt|;
name|cnt
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|cnt
operator|*
literal|48
operator|+
literal|46
operator|!=
name|ofs
condition|)
block|{
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"internal error in record_rev_cache"
argument_list|)
expr_stmt|;
block|}
name|ri
operator|=
name|create_rev_cache
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|ri
operator|->
name|parsed
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|cnt
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|char
name|parent_sha1
index|[
literal|20
index|]
decl_stmt|;
name|ofs
operator|=
literal|46
operator|+
name|i
operator|*
literal|48
operator|+
literal|7
expr_stmt|;
name|get_sha1_hex
argument_list|(
name|buf
operator|+
name|ofs
argument_list|,
name|parent_sha1
argument_list|)
expr_stmt|;
name|add_parent
argument_list|(
name|ri
argument_list|,
name|parent_sha1
argument_list|)
expr_stmt|;
name|record_rev_cache
argument_list|(
name|parent_sha1
argument_list|,
name|dumpfile
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
