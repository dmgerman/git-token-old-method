begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  *  * This handles basic git sha1 object files - packing, unpacking,  * creation etc.  */
end_comment
begin_include
include|#
directive|include
file|<stdarg.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|sha1_file_directory
specifier|const
name|char
modifier|*
name|sha1_file_directory
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_ifndef
ifndef|#
directive|ifndef
name|O_NOATIME
end_ifndef
begin_if
if|#
directive|if
name|defined
argument_list|(
name|__linux__
argument_list|)
operator|&&
operator|(
name|defined
argument_list|(
name|__i386__
argument_list|)
operator|||
name|defined
argument_list|(
name|__PPC__
argument_list|)
operator|)
end_if
begin_define
DECL|macro|O_NOATIME
define|#
directive|define
name|O_NOATIME
value|01000000
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|O_NOATIME
define|#
directive|define
name|O_NOATIME
value|0
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_decl_stmt
DECL|variable|sha1_file_open_flag
specifier|static
name|unsigned
name|int
name|sha1_file_open_flag
init|=
name|O_NOATIME
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|hexval
specifier|static
name|unsigned
name|hexval
parameter_list|(
name|char
name|c
parameter_list|)
block|{
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
return|return
name|c
operator|-
literal|'0'
return|;
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
return|return
name|c
operator|-
literal|'a'
operator|+
literal|10
return|;
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
return|return
name|c
operator|-
literal|'A'
operator|+
literal|10
return|;
return|return
operator|~
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_sha1_hex
name|int
name|get_sha1_hex
parameter_list|(
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
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
literal|20
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
init|=
operator|(
name|hexval
argument_list|(
name|hex
index|[
literal|0
index|]
argument_list|)
operator|<<
literal|4
operator|)
operator||
name|hexval
argument_list|(
name|hex
index|[
literal|1
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
name|val
operator|&
operator|~
literal|0xff
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|sha1
operator|++
operator|=
name|val
expr_stmt|;
name|hex
operator|+=
literal|2
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|sha1_to_hex
name|char
modifier|*
name|sha1_to_hex
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|50
index|]
decl_stmt|;
specifier|static
specifier|const
name|char
name|hex
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|buffer
decl_stmt|;
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
literal|20
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
init|=
operator|*
name|sha1
operator|++
decl_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|>>
literal|4
index|]
expr_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|&
literal|0xf
index|]
expr_stmt|;
block|}
return|return
name|buffer
return|;
block|}
end_function
begin_comment
comment|/*  * NOTE! This returns a statically allocated buffer, so you have to be  * careful about using it. Do a "strdup()" if you need to save the  * filename.  */
end_comment
begin_function
DECL|function|sha1_file_name
name|char
modifier|*
name|sha1_file_name
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
specifier|static
name|char
modifier|*
name|name
decl_stmt|,
modifier|*
name|base
decl_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
block|{
name|char
modifier|*
name|sha1_file_directory
init|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
condition|?
else|:
name|DEFAULT_DB_ENVIRONMENT
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|sha1_file_directory
argument_list|)
decl_stmt|;
name|base
operator|=
name|malloc
argument_list|(
name|len
operator|+
literal|60
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|base
argument_list|,
name|sha1_file_directory
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|base
operator|+
name|len
argument_list|,
literal|0
argument_list|,
literal|60
argument_list|)
expr_stmt|;
name|base
index|[
name|len
index|]
operator|=
literal|'/'
expr_stmt|;
name|base
index|[
name|len
operator|+
literal|3
index|]
operator|=
literal|'/'
expr_stmt|;
name|name
operator|=
name|base
operator|+
name|len
operator|+
literal|1
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|20
condition|;
name|i
operator|++
control|)
block|{
specifier|static
name|char
name|hex
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|unsigned
name|int
name|val
init|=
name|sha1
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|pos
init|=
name|name
operator|+
name|i
operator|*
literal|2
operator|+
operator|(
name|i
operator|>
literal|0
operator|)
decl_stmt|;
operator|*
name|pos
operator|++
operator|=
name|hex
index|[
name|val
operator|>>
literal|4
index|]
expr_stmt|;
operator|*
name|pos
operator|=
name|hex
index|[
name|val
operator|&
literal|0xf
index|]
expr_stmt|;
block|}
return|return
name|base
return|;
block|}
end_function
begin_function
DECL|function|check_sha1_signature
name|int
name|check_sha1_signature
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
block|{
name|char
name|header
index|[
literal|100
index|]
decl_stmt|;
name|unsigned
name|char
name|real_sha1
index|[
literal|20
index|]
decl_stmt|;
name|SHA_CTX
name|c
decl_stmt|;
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
name|header
argument_list|,
literal|1
operator|+
name|sprintf
argument_list|(
name|header
argument_list|,
literal|"%s %lu"
argument_list|,
name|type
argument_list|,
name|size
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|map
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|real_sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
return|return
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|real_sha1
argument_list|,
literal|20
argument_list|)
condition|?
operator|-
literal|1
else|:
literal|0
return|;
block|}
end_function
begin_function
DECL|function|map_sha1_file
name|void
modifier|*
name|map_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
operator||
name|sha1_file_open_flag
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
comment|/* See if it works without O_NOATIME */
switch|switch
condition|(
name|sha1_file_open_flag
condition|)
block|{
default|default:
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
break|break;
comment|/* Fallthrough */
case|case
literal|0
case|:
name|perror
argument_list|(
name|filename
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* If it failed once, it will probably fail again. Stop using O_NOATIME */
name|sha1_file_open_flag
operator|=
literal|0
expr_stmt|;
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
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
name|NULL
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
operator|-
literal|1
operator|==
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|map
condition|)
return|return
name|NULL
return|;
operator|*
name|size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
return|return
name|map
return|;
block|}
end_function
begin_function
DECL|function|unpack_sha1_file
name|void
modifier|*
name|unpack_sha1_file
parameter_list|(
name|void
modifier|*
name|map
parameter_list|,
name|unsigned
name|long
name|mapsize
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|int
name|ret
decl_stmt|,
name|bytes
decl_stmt|;
name|z_stream
name|stream
decl_stmt|;
name|char
name|buffer
index|[
literal|8192
index|]
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
comment|/* Get the data stream */
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|stream
operator|.
name|next_in
operator|=
name|map
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|mapsize
expr_stmt|;
name|stream
operator|.
name|next_out
operator|=
name|buffer
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|ret
operator|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
name|Z_OK
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|sscanf
argument_list|(
name|buffer
argument_list|,
literal|"%10s %lu"
argument_list|,
name|type
argument_list|,
name|size
argument_list|)
operator|!=
literal|2
condition|)
return|return
name|NULL
return|;
name|bytes
operator|=
name|strlen
argument_list|(
name|buffer
argument_list|)
operator|+
literal|1
expr_stmt|;
name|buf
operator|=
name|malloc
argument_list|(
operator|*
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
return|return
name|NULL
return|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|buffer
operator|+
name|bytes
argument_list|,
name|stream
operator|.
name|total_out
operator|-
name|bytes
argument_list|)
expr_stmt|;
name|bytes
operator|=
name|stream
operator|.
name|total_out
operator|-
name|bytes
expr_stmt|;
if|if
condition|(
name|bytes
operator|<
operator|*
name|size
operator|&&
name|ret
operator|==
name|Z_OK
condition|)
block|{
name|stream
operator|.
name|next_out
operator|=
name|buf
operator|+
name|bytes
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
operator|*
name|size
operator|-
name|bytes
expr_stmt|;
while|while
condition|(
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_FINISH
argument_list|)
operator|==
name|Z_OK
condition|)
comment|/* nothing */
empty_stmt|;
block|}
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|read_sha1_file
name|void
modifier|*
name|read_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|mapsize
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|,
modifier|*
name|buf
decl_stmt|;
name|map
operator|=
name|map_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|mapsize
argument_list|)
expr_stmt|;
if|if
condition|(
name|map
condition|)
block|{
name|buf
operator|=
name|unpack_sha1_file
argument_list|(
name|map
argument_list|,
name|mapsize
argument_list|,
name|type
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|map
argument_list|,
name|mapsize
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|read_tree_with_tree_or_commit_sha1
name|void
modifier|*
name|read_tree_with_tree_or_commit_sha1
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|tree_sha1_return
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
decl_stmt|;
name|unsigned
name|long
name|isize
decl_stmt|;
name|int
name|was_commit
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|isize
argument_list|)
expr_stmt|;
comment|/*  	 * We might have read a commit instead of a tree, in which case 	 * we parse out the tree_sha1 and attempt to read from there. 	 * (buffer + 5) is because the tree sha1 is always at offset 5 	 * in a commit record ("tree "). 	 */
if|if
condition|(
name|buffer
operator|&&
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"commit"
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|buffer
operator|+
literal|5
argument_list|,
name|tree_sha1
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|tree_sha1
argument_list|,
name|type
argument_list|,
operator|&
name|isize
argument_list|)
expr_stmt|;
name|was_commit
operator|=
literal|1
expr_stmt|;
block|}
comment|/* 	 * Now do we have something and if so is it a tree? 	 */
if|if
condition|(
operator|!
name|buffer
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"tree"
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
operator|*
name|size
operator|=
name|isize
expr_stmt|;
if|if
condition|(
name|tree_sha1_return
condition|)
name|memcpy
argument_list|(
name|tree_sha1_return
argument_list|,
name|was_commit
condition|?
name|tree_sha1
else|:
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|write_sha1_file
name|int
name|write_sha1_file
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|char
modifier|*
name|returnsha1
parameter_list|)
block|{
name|int
name|size
decl_stmt|;
name|char
modifier|*
name|compressed
decl_stmt|;
name|z_stream
name|stream
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
name|char
modifier|*
name|filename
decl_stmt|;
name|char
name|hdr
index|[
literal|50
index|]
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|hdrlen
decl_stmt|;
comment|/* Generate the header */
name|hdrlen
operator|=
name|sprintf
argument_list|(
name|hdr
argument_list|,
literal|"%s %d"
argument_list|,
name|type
argument_list|,
name|len
argument_list|)
operator|+
literal|1
expr_stmt|;
comment|/* Sha1.. */
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
name|buf
argument_list|,
name|len
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
if|if
condition|(
name|returnsha1
condition|)
name|memcpy
argument_list|(
name|returnsha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|filename
operator|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
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
name|errno
operator|!=
name|EEXIST
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 		 * We might do collision checking here, but we'd need to 		 * uncompress the old file and check it. Later. 		 */
return|return
literal|0
return|;
block|}
comment|/* Set it up */
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|deflateInit
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_BEST_COMPRESSION
argument_list|)
expr_stmt|;
name|size
operator|=
name|deflateBound
argument_list|(
operator|&
name|stream
argument_list|,
name|len
operator|+
name|hdrlen
argument_list|)
expr_stmt|;
name|compressed
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
comment|/* Compress it */
name|stream
operator|.
name|next_out
operator|=
name|compressed
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
name|size
expr_stmt|;
comment|/* First header.. */
name|stream
operator|.
name|next_in
operator|=
name|hdr
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|hdrlen
expr_stmt|;
while|while
condition|(
name|deflate
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|)
operator|==
name|Z_OK
condition|)
comment|/* nothing */
comment|/* Then the data itself.. */
name|stream
operator|.
name|next_in
operator|=
name|buf
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|len
expr_stmt|;
while|while
condition|(
name|deflate
argument_list|(
operator|&
name|stream
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
name|stream
argument_list|)
expr_stmt|;
name|size
operator|=
name|stream
operator|.
name|total_out
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|compressed
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
name|die
argument_list|(
literal|"unable to write file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|collision_check
specifier|static
specifier|inline
name|int
name|collision_check
parameter_list|(
name|char
modifier|*
name|filename
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|)
block|{
ifdef|#
directive|ifdef
name|COLLISION_CHECK
name|void
modifier|*
name|map
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|cmp
decl_stmt|;
comment|/* Unreadable object, or object went away? Strange. */
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
operator|||
name|size
operator|!=
name|st
operator|.
name|st_size
condition|)
return|return
operator|-
literal|1
return|;
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|size
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
name|cmp
operator|=
name|memcmp
argument_list|(
name|buf
argument_list|,
name|map
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|map
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
operator|-
literal|1
return|;
endif|#
directive|endif
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_sha1_buffer
name|int
name|write_sha1_buffer
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
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
name|errno
operator|!=
name|EEXIST
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|collision_check
argument_list|(
name|filename
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"SHA1 collision detected!"
literal|" This is bad, bad, BAD!\a\n"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
name|write
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_sha1_from_fd
name|int
name|write_sha1_from_fd
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|int
name|local
decl_stmt|;
name|z_stream
name|stream
decl_stmt|;
name|unsigned
name|char
name|real_sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
name|buf
index|[
literal|4096
index|]
decl_stmt|;
name|char
name|discard
index|[
literal|4096
index|]
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|SHA_CTX
name|c
decl_stmt|;
name|local
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|local
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't open %s\n"
argument_list|,
name|filename
argument_list|)
return|;
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
do|do
block|{
name|ssize_t
name|size
decl_stmt|;
name|size
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
literal|4096
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|<=
literal|0
condition|)
block|{
name|close
argument_list|(
name|local
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|filename
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|size
condition|)
return|return
name|error
argument_list|(
literal|"Connection closed?"
argument_list|)
return|;
name|perror
argument_list|(
literal|"Reading from connection"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|write
argument_list|(
name|local
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|size
expr_stmt|;
name|stream
operator|.
name|next_in
operator|=
name|buf
expr_stmt|;
do|do
block|{
name|stream
operator|.
name|next_out
operator|=
name|discard
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
sizeof|sizeof
argument_list|(
name|discard
argument_list|)
expr_stmt|;
name|ret
operator|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_SYNC_FLUSH
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|discard
argument_list|,
sizeof|sizeof
argument_list|(
name|discard
argument_list|)
operator|-
name|stream
operator|.
name|avail_out
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
name|stream
operator|.
name|avail_in
operator|&&
name|ret
operator|==
name|Z_OK
condition|)
do|;
block|}
do|while
condition|(
name|ret
operator|==
name|Z_OK
condition|)
do|;
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|local
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|real_sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|!=
name|Z_STREAM_END
condition|)
block|{
name|unlink
argument_list|(
name|filename
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"File %s corrupted"
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
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|real_sha1
argument_list|,
literal|20
argument_list|)
condition|)
block|{
name|unlink
argument_list|(
name|filename
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"File %s has bad hash\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|has_sha1_file
name|int
name|has_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|filename
argument_list|,
operator|&
name|st
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
