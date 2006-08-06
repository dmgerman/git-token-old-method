begin_unit
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
file|"delta.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_include
include|#
directive|include
file|"csum-file.h"
end_include
begin_decl_stmt
DECL|variable|max_depth
specifier|static
name|int
name|max_depth
init|=
literal|10
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|object_count
specifier|static
name|unsigned
name|long
name|object_count
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|duplicate_count
specifier|static
name|unsigned
name|long
name|duplicate_count
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|packoff
specifier|static
name|unsigned
name|long
name|packoff
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|overflow_count
specifier|static
name|unsigned
name|long
name|overflow_count
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|packfd
specifier|static
name|int
name|packfd
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|current_depth
specifier|static
name|int
name|current_depth
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|lastdat
specifier|static
name|void
modifier|*
name|lastdat
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|lastdatlen
specifier|static
name|unsigned
name|long
name|lastdatlen
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|lastsha1
specifier|static
name|unsigned
name|char
name|lastsha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|packsha1
specifier|static
name|unsigned
name|char
name|packsha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|object_entry
struct|struct
name|object_entry
block|{
DECL|member|next
name|struct
name|object_entry
modifier|*
name|next
decl_stmt|;
DECL|member|offset
name|unsigned
name|long
name|offset
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|overflow_object_entry
struct|struct
name|overflow_object_entry
block|{
DECL|member|next
name|struct
name|overflow_object_entry
modifier|*
name|next
decl_stmt|;
DECL|member|oe
name|struct
name|object_entry
name|oe
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|pool_start
name|struct
name|object_entry
modifier|*
name|pool_start
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pool_next
name|struct
name|object_entry
modifier|*
name|pool_next
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pool_end
name|struct
name|object_entry
modifier|*
name|pool_end
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|overflow
name|struct
name|overflow_object_entry
modifier|*
name|overflow
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|table
name|struct
name|object_entry
modifier|*
name|table
index|[
literal|1
operator|<<
literal|16
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|new_object
specifier|static
name|struct
name|object_entry
modifier|*
name|new_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|pool_next
operator|!=
name|pool_end
condition|)
block|{
name|struct
name|object_entry
modifier|*
name|e
init|=
name|pool_next
operator|++
decl_stmt|;
name|memcpy
argument_list|(
name|e
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|e
return|;
block|}
else|else
block|{
name|struct
name|overflow_object_entry
modifier|*
name|e
decl_stmt|;
name|e
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|overflow_object_entry
argument_list|)
argument_list|)
expr_stmt|;
name|e
operator|->
name|next
operator|=
name|overflow
expr_stmt|;
name|memcpy
argument_list|(
name|e
operator|->
name|oe
operator|.
name|sha1
argument_list|,
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|e
operator|->
name|oe
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|overflow
operator|=
name|e
expr_stmt|;
name|overflow_count
operator|++
expr_stmt|;
return|return
operator|&
name|e
operator|->
name|oe
return|;
block|}
block|}
end_function
begin_function
DECL|function|insert_object
specifier|static
name|struct
name|object_entry
modifier|*
name|insert_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|int
name|h
init|=
name|sha1
index|[
literal|0
index|]
operator|<<
literal|8
operator||
name|sha1
index|[
literal|1
index|]
decl_stmt|;
name|struct
name|object_entry
modifier|*
name|e
init|=
name|table
index|[
name|h
index|]
decl_stmt|;
name|struct
name|object_entry
modifier|*
name|p
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|e
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|e
operator|->
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
argument_list|)
condition|)
return|return
name|e
return|;
name|p
operator|=
name|e
expr_stmt|;
name|e
operator|=
name|e
operator|->
name|next
expr_stmt|;
block|}
name|e
operator|=
name|new_object
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|e
operator|->
name|next
operator|=
literal|0
expr_stmt|;
name|e
operator|->
name|offset
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|p
condition|)
name|p
operator|->
name|next
operator|=
name|e
expr_stmt|;
else|else
name|table
index|[
name|h
index|]
operator|=
name|e
expr_stmt|;
return|return
name|e
return|;
block|}
end_function
begin_function
DECL|function|yread
specifier|static
name|ssize_t
name|yread
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|size_t
name|length
parameter_list|)
block|{
name|ssize_t
name|ret
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|ret
operator|<
name|length
condition|)
block|{
name|ssize_t
name|size
init|=
name|xread
argument_list|(
name|fd
argument_list|,
operator|(
name|char
operator|*
operator|)
name|buffer
operator|+
name|ret
argument_list|,
name|length
operator|-
name|ret
argument_list|)
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|0
condition|)
block|{
return|return
name|size
return|;
block|}
if|if
condition|(
name|size
operator|==
literal|0
condition|)
block|{
return|return
name|ret
return|;
block|}
name|ret
operator|+=
name|size
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|ywrite
specifier|static
name|ssize_t
name|ywrite
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|size_t
name|length
parameter_list|)
block|{
name|ssize_t
name|ret
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|ret
operator|<
name|length
condition|)
block|{
name|ssize_t
name|size
init|=
name|xwrite
argument_list|(
name|fd
argument_list|,
operator|(
name|char
operator|*
operator|)
name|buffer
operator|+
name|ret
argument_list|,
name|length
operator|-
name|ret
argument_list|)
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|0
condition|)
block|{
return|return
name|size
return|;
block|}
if|if
condition|(
name|size
operator|==
literal|0
condition|)
block|{
return|return
name|ret
return|;
block|}
name|ret
operator|+=
name|size
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|encode_header
specifier|static
name|unsigned
name|long
name|encode_header
parameter_list|(
name|enum
name|object_type
name|type
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|hdr
parameter_list|)
block|{
name|int
name|n
init|=
literal|1
decl_stmt|;
name|unsigned
name|char
name|c
decl_stmt|;
if|if
condition|(
name|type
operator|<
name|OBJ_COMMIT
operator|||
name|type
operator|>
name|OBJ_DELTA
condition|)
name|die
argument_list|(
literal|"bad type %d"
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|c
operator|=
operator|(
name|type
operator|<<
literal|4
operator|)
operator||
operator|(
name|size
operator|&
literal|15
operator|)
expr_stmt|;
name|size
operator|>>=
literal|4
expr_stmt|;
while|while
condition|(
name|size
condition|)
block|{
operator|*
name|hdr
operator|++
operator|=
name|c
operator||
literal|0x80
expr_stmt|;
name|c
operator|=
name|size
operator|&
literal|0x7f
expr_stmt|;
name|size
operator|>>=
literal|7
expr_stmt|;
name|n
operator|++
expr_stmt|;
block|}
operator|*
name|hdr
operator|=
name|c
expr_stmt|;
return|return
name|n
return|;
block|}
end_function
begin_function
DECL|function|write_blob
specifier|static
name|void
name|write_blob
parameter_list|(
name|void
modifier|*
name|dat
parameter_list|,
name|unsigned
name|long
name|datlen
parameter_list|)
block|{
name|z_stream
name|s
decl_stmt|;
name|void
modifier|*
name|out
decl_stmt|,
modifier|*
name|delta
decl_stmt|;
name|unsigned
name|char
name|hdr
index|[
literal|64
index|]
decl_stmt|;
name|unsigned
name|long
name|hdrlen
decl_stmt|,
name|deltalen
decl_stmt|;
if|if
condition|(
name|lastdat
operator|&&
name|current_depth
operator|<
name|max_depth
condition|)
block|{
name|delta
operator|=
name|diff_delta
argument_list|(
name|lastdat
argument_list|,
name|lastdatlen
argument_list|,
name|dat
argument_list|,
name|datlen
argument_list|,
operator|&
name|deltalen
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
name|delta
operator|=
literal|0
expr_stmt|;
name|memset
argument_list|(
operator|&
name|s
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|s
argument_list|)
argument_list|)
expr_stmt|;
name|deflateInit
argument_list|(
operator|&
name|s
argument_list|,
name|zlib_compression_level
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta
condition|)
block|{
name|current_depth
operator|++
expr_stmt|;
name|s
operator|.
name|next_in
operator|=
name|delta
expr_stmt|;
name|s
operator|.
name|avail_in
operator|=
name|deltalen
expr_stmt|;
name|hdrlen
operator|=
name|encode_header
argument_list|(
name|OBJ_DELTA
argument_list|,
name|deltalen
argument_list|,
name|hdr
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
name|hdr
argument_list|,
name|hdrlen
argument_list|)
operator|!=
name|hdrlen
condition|)
name|die
argument_list|(
literal|"Can't write object header: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
name|lastsha1
argument_list|,
sizeof|sizeof
argument_list|(
name|lastsha1
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|lastsha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Can't write object base: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|packoff
operator|+=
name|hdrlen
operator|+
sizeof|sizeof
argument_list|(
name|lastsha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|current_depth
operator|=
literal|0
expr_stmt|;
name|s
operator|.
name|next_in
operator|=
name|dat
expr_stmt|;
name|s
operator|.
name|avail_in
operator|=
name|datlen
expr_stmt|;
name|hdrlen
operator|=
name|encode_header
argument_list|(
name|OBJ_BLOB
argument_list|,
name|datlen
argument_list|,
name|hdr
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
name|hdr
argument_list|,
name|hdrlen
argument_list|)
operator|!=
name|hdrlen
condition|)
name|die
argument_list|(
literal|"Can't write object header: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|packoff
operator|+=
name|hdrlen
expr_stmt|;
block|}
name|s
operator|.
name|avail_out
operator|=
name|deflateBound
argument_list|(
operator|&
name|s
argument_list|,
name|s
operator|.
name|avail_in
argument_list|)
expr_stmt|;
name|s
operator|.
name|next_out
operator|=
name|out
operator|=
name|xmalloc
argument_list|(
name|s
operator|.
name|avail_out
argument_list|)
expr_stmt|;
while|while
condition|(
name|deflate
argument_list|(
operator|&
name|s
argument_list|,
name|Z_FINISH
argument_list|)
operator|==
name|Z_OK
condition|)
comment|/* nothing */
empty_stmt|;
name|deflateEnd
argument_list|(
operator|&
name|s
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
name|out
argument_list|,
name|s
operator|.
name|total_out
argument_list|)
operator|!=
name|s
operator|.
name|total_out
condition|)
name|die
argument_list|(
literal|"Failed writing compressed data %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|packoff
operator|+=
name|s
operator|.
name|total_out
expr_stmt|;
name|free
argument_list|(
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta
condition|)
name|free
argument_list|(
name|delta
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|init_pack_header
specifier|static
name|void
name|init_pack_header
parameter_list|()
block|{
specifier|const
name|char
modifier|*
name|magic
init|=
literal|"PACK"
decl_stmt|;
name|unsigned
name|long
name|version
init|=
literal|2
decl_stmt|;
name|unsigned
name|long
name|zero
init|=
literal|0
decl_stmt|;
name|version
operator|=
name|htonl
argument_list|(
name|version
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
operator|(
name|char
operator|*
operator|)
name|magic
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
name|die
argument_list|(
literal|"Can't write pack magic: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
operator|&
name|version
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
name|die
argument_list|(
literal|"Can't write pack version: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
operator|&
name|zero
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
name|die
argument_list|(
literal|"Can't write 0 object count: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|packoff
operator|=
literal|4
operator|*
literal|3
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fixup_header_footer
specifier|static
name|void
name|fixup_header_footer
parameter_list|()
block|{
name|SHA_CTX
name|c
decl_stmt|;
name|char
name|hdr
index|[
literal|8
index|]
decl_stmt|;
name|unsigned
name|long
name|cnt
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
name|size_t
name|n
decl_stmt|;
if|if
condition|(
name|lseek
argument_list|(
name|packfd
argument_list|,
literal|0
argument_list|,
name|SEEK_SET
argument_list|)
operator|!=
literal|0
condition|)
name|die
argument_list|(
literal|"Failed seeking to start: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|yread
argument_list|(
name|packfd
argument_list|,
name|hdr
argument_list|,
literal|8
argument_list|)
operator|!=
literal|8
condition|)
name|die
argument_list|(
literal|"Failed reading header: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|hdr
argument_list|,
literal|8
argument_list|)
expr_stmt|;
name|cnt
operator|=
name|htonl
argument_list|(
name|object_count
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
operator|&
name|cnt
argument_list|,
literal|4
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
operator|&
name|cnt
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
name|die
argument_list|(
literal|"Failed writing object count: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|buf
operator|=
name|xmalloc
argument_list|(
literal|128
operator|*
literal|1024
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|n
operator|=
name|xread
argument_list|(
name|packfd
argument_list|,
name|buf
argument_list|,
literal|128
operator|*
literal|1024
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|<=
literal|0
condition|)
break|break;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|buf
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|packsha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|ywrite
argument_list|(
name|packfd
argument_list|,
name|packsha1
argument_list|,
sizeof|sizeof
argument_list|(
name|packsha1
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|packsha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed writing pack checksum: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|oecmp
specifier|static
name|int
name|oecmp
parameter_list|(
specifier|const
name|void
modifier|*
name|_a
parameter_list|,
specifier|const
name|void
modifier|*
name|_b
parameter_list|)
block|{
name|struct
name|object_entry
modifier|*
name|a
init|=
operator|*
operator|(
operator|(
expr|struct
name|object_entry
operator|*
operator|*
operator|)
name|_a
operator|)
decl_stmt|;
name|struct
name|object_entry
modifier|*
name|b
init|=
operator|*
operator|(
operator|(
expr|struct
name|object_entry
operator|*
operator|*
operator|)
name|_b
operator|)
decl_stmt|;
return|return
name|memcmp
argument_list|(
name|a
operator|->
name|sha1
argument_list|,
name|b
operator|->
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|a
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_index
specifier|static
name|void
name|write_index
parameter_list|(
specifier|const
name|char
modifier|*
name|idx_name
parameter_list|)
block|{
name|struct
name|sha1file
modifier|*
name|f
decl_stmt|;
name|struct
name|object_entry
modifier|*
modifier|*
name|idx
decl_stmt|,
modifier|*
modifier|*
name|c
decl_stmt|,
modifier|*
modifier|*
name|last
decl_stmt|;
name|struct
name|object_entry
modifier|*
name|e
decl_stmt|;
name|struct
name|overflow_object_entry
modifier|*
name|o
decl_stmt|;
name|unsigned
name|int
name|array
index|[
literal|256
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* Build the sorted table of object IDs. */
name|idx
operator|=
name|xmalloc
argument_list|(
name|object_count
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|object_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|c
operator|=
name|idx
expr_stmt|;
for|for
control|(
name|e
operator|=
name|pool_start
init|;
name|e
operator|!=
name|pool_next
condition|;
name|e
operator|++
control|)
operator|*
name|c
operator|++
operator|=
name|e
expr_stmt|;
for|for
control|(
name|o
operator|=
name|overflow
init|;
name|o
condition|;
name|o
operator|=
name|o
operator|->
name|next
control|)
operator|*
name|c
operator|++
operator|=
operator|&
name|o
operator|->
name|oe
expr_stmt|;
name|last
operator|=
name|idx
operator|+
name|object_count
expr_stmt|;
name|qsort
argument_list|(
name|idx
argument_list|,
name|object_count
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|object_entry
operator|*
argument_list|)
argument_list|,
name|oecmp
argument_list|)
expr_stmt|;
comment|/* Generate the fan-out array. */
name|c
operator|=
name|idx
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|256
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_entry
modifier|*
modifier|*
name|next
init|=
name|c
decl_stmt|;
empty_stmt|;
while|while
condition|(
name|next
operator|<
name|last
condition|)
block|{
if|if
condition|(
operator|(
operator|*
name|next
operator|)
operator|->
name|sha1
index|[
literal|0
index|]
operator|!=
name|i
condition|)
break|break;
name|next
operator|++
expr_stmt|;
block|}
name|array
index|[
name|i
index|]
operator|=
name|htonl
argument_list|(
name|next
operator|-
name|idx
argument_list|)
expr_stmt|;
name|c
operator|=
name|next
expr_stmt|;
block|}
name|f
operator|=
name|sha1create
argument_list|(
literal|"%s"
argument_list|,
name|idx_name
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
name|array
argument_list|,
literal|256
operator|*
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|c
operator|=
name|idx
init|;
name|c
operator|!=
name|last
condition|;
name|c
operator|++
control|)
block|{
name|unsigned
name|int
name|offset
init|=
name|htonl
argument_list|(
operator|(
operator|*
name|c
operator|)
operator|->
name|offset
argument_list|)
decl_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|offset
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|(
operator|*
name|c
operator|)
operator|->
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
operator|(
operator|*
name|c
operator|)
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|sha1write
argument_list|(
name|f
argument_list|,
name|packsha1
argument_list|,
sizeof|sizeof
argument_list|(
name|packsha1
argument_list|)
argument_list|)
expr_stmt|;
name|sha1close
argument_list|(
name|f
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|idx
argument_list|)
expr_stmt|;
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
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|base_name
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
name|int
name|est_obj_cnt
init|=
name|atoi
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
decl_stmt|;
name|char
modifier|*
name|pack_name
decl_stmt|;
name|char
modifier|*
name|idx_name
decl_stmt|;
name|pack_name
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|base_name
argument_list|)
operator|+
literal|6
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|pack_name
argument_list|,
literal|"%s.pack"
argument_list|,
name|base_name
argument_list|)
expr_stmt|;
name|idx_name
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|base_name
argument_list|)
operator|+
literal|5
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|idx_name
argument_list|,
literal|"%s.idx"
argument_list|,
name|base_name
argument_list|)
expr_stmt|;
name|packfd
operator|=
name|open
argument_list|(
name|pack_name
argument_list|,
name|O_RDWR
operator||
name|O_CREAT
operator||
name|O_TRUNC
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|packfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Can't create pack file %s: %s"
argument_list|,
name|pack_name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|pool_start
operator|=
name|xmalloc
argument_list|(
name|est_obj_cnt
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|object_entry
argument_list|)
argument_list|)
expr_stmt|;
name|pool_next
operator|=
name|pool_start
expr_stmt|;
name|pool_end
operator|=
name|pool_start
operator|+
name|est_obj_cnt
expr_stmt|;
name|init_pack_header
argument_list|()
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|long
name|datlen
decl_stmt|;
name|int
name|hdrlen
decl_stmt|;
name|void
modifier|*
name|dat
decl_stmt|;
name|char
name|hdr
index|[
literal|128
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|SHA_CTX
name|c
decl_stmt|;
name|struct
name|object_entry
modifier|*
name|e
decl_stmt|;
if|if
condition|(
name|yread
argument_list|(
literal|0
argument_list|,
operator|&
name|datlen
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
break|break;
name|dat
operator|=
name|xmalloc
argument_list|(
name|datlen
argument_list|)
expr_stmt|;
if|if
condition|(
name|yread
argument_list|(
literal|0
argument_list|,
name|dat
argument_list|,
name|datlen
argument_list|)
operator|!=
name|datlen
condition|)
break|break;
name|hdrlen
operator|=
name|sprintf
argument_list|(
name|hdr
argument_list|,
literal|"blob %lu"
argument_list|,
name|datlen
argument_list|)
operator|+
literal|1
expr_stmt|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|hdr
argument_list|,
name|hdrlen
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|dat
argument_list|,
name|datlen
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
name|e
operator|=
name|insert_object
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|e
operator|->
name|offset
condition|)
block|{
name|e
operator|->
name|offset
operator|=
name|packoff
expr_stmt|;
name|write_blob
argument_list|(
name|dat
argument_list|,
name|datlen
argument_list|)
expr_stmt|;
name|object_count
operator|++
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
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|lastdat
condition|)
name|free
argument_list|(
name|lastdat
argument_list|)
expr_stmt|;
name|lastdat
operator|=
name|dat
expr_stmt|;
name|lastdatlen
operator|=
name|datlen
expr_stmt|;
name|memcpy
argument_list|(
name|lastsha1
argument_list|,
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|duplicate_count
operator|++
expr_stmt|;
name|free
argument_list|(
name|dat
argument_list|)
expr_stmt|;
block|}
block|}
name|fixup_header_footer
argument_list|()
expr_stmt|;
name|close
argument_list|(
name|packfd
argument_list|)
expr_stmt|;
name|write_index
argument_list|(
name|idx_name
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%lu objects, %lu duplicates, %lu pool overflow\n"
argument_list|,
name|object_count
argument_list|,
name|duplicate_count
argument_list|,
name|overflow_count
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
