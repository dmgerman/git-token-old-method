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
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
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
DECL|function|get_sha1_file
name|int
name|get_sha1_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|60
index|]
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
name|int
name|len
decl_stmt|;
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
name|len
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|40
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|get_sha1_hex
argument_list|(
name|buffer
argument_list|,
name|result
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|git_dir
DECL|variable|git_object_dir
DECL|variable|git_index_file
specifier|static
name|char
modifier|*
name|git_dir
decl_stmt|,
modifier|*
name|git_object_dir
decl_stmt|,
modifier|*
name|git_index_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|setup_git_env
specifier|static
name|void
name|setup_git_env
parameter_list|(
name|void
parameter_list|)
block|{
name|git_dir
operator|=
name|gitenv
argument_list|(
name|GIT_DIR_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_dir
condition|)
name|git_dir
operator|=
name|DEFAULT_GIT_DIR_ENVIRONMENT
expr_stmt|;
name|git_object_dir
operator|=
name|gitenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_object_dir
condition|)
block|{
name|git_object_dir
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|git_dir
argument_list|)
operator|+
literal|9
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|git_object_dir
argument_list|,
literal|"%s/objects"
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
block|}
name|git_index_file
operator|=
name|gitenv
argument_list|(
name|INDEX_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_index_file
condition|)
block|{
name|git_index_file
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|git_dir
argument_list|)
operator|+
literal|7
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|git_index_file
argument_list|,
literal|"%s/index"
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|get_object_directory
name|char
modifier|*
name|get_object_directory
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_object_dir
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_object_dir
return|;
block|}
end_function
begin_function
DECL|function|get_index_file
name|char
modifier|*
name|get_index_file
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_index_file
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
return|return
name|git_index_file
return|;
block|}
end_function
begin_function
DECL|function|get_sha1
name|int
name|get_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|char
name|pathname
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|prefix
index|[]
init|=
block|{
literal|""
block|,
literal|"refs"
block|,
literal|"refs/tags"
block|,
literal|"refs/heads"
block|,
literal|"refs/snap"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|str
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|git_dir
condition|)
name|setup_git_env
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|prefix
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
block|{
name|snprintf
argument_list|(
name|pathname
argument_list|,
sizeof|sizeof
argument_list|(
name|pathname
argument_list|)
argument_list|,
literal|"%s/%s/%s"
argument_list|,
name|git_dir
argument_list|,
operator|*
name|p
argument_list|,
name|str
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1_file
argument_list|(
name|pathname
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
return|return
operator|-
literal|1
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
begin_function
DECL|function|fill_sha1_path
specifier|static
name|void
name|fill_sha1_path
parameter_list|(
name|char
modifier|*
name|pathbuf
parameter_list|,
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
name|pathbuf
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
block|}
end_function
begin_comment
comment|/*  * NOTE! This returns a statically allocated buffer, so you have to be  * careful about using it. Do a "strdup()" if you need to save the  * filename.  *  * Also note that this returns the location for creating.  Reading  * SHA1 file can happen from any alternate directory listed in the  * DB_ENVIRONMENT environment variable if it is not found in  * the primary object database.  */
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
specifier|const
name|char
modifier|*
name|sha1_file_directory
init|=
name|get_object_directory
argument_list|()
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
name|xmalloc
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
name|fill_sha1_path
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
name|base
return|;
block|}
end_function
begin_struct
DECL|struct|alternate_object_database
specifier|static
struct|struct
name|alternate_object_database
block|{
DECL|member|base
name|char
modifier|*
name|base
decl_stmt|;
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
block|}
DECL|variable|alt_odb
modifier|*
name|alt_odb
struct|;
end_struct
begin_comment
comment|/*  * Prepare alternate object database registry.  * alt_odb points at an array of struct alternate_object_database.  * This array is terminated with an element that has both its base  * and name set to NULL.  alt_odb[n] comes from n'th non-empty  * element from colon separated ALTERNATE_DB_ENVIRONMENT environment  * variable, and its base points at a statically allocated buffer  * that contains "/the/directory/corresponding/to/.git/objects/...",  * while its name points just after the slash at the end of  * ".git/objects/" in the example above, and has enough space to hold  * 40-byte hex SHA1, an extra slash for the first level indirection,  * and the terminating NUL.  * This function allocates the alt_odb array and all the strings  * pointed by base fields of the array elements with one xmalloc();  * the string pool immediately follows the array.  */
end_comment
begin_function
DECL|function|prepare_alt_odb
specifier|static
name|void
name|prepare_alt_odb
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|pass
decl_stmt|,
name|totlen
decl_stmt|,
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|last
decl_stmt|;
name|char
modifier|*
name|op
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|alt
init|=
name|gitenv
argument_list|(
name|ALTERNATE_DB_ENVIRONMENT
argument_list|)
condition|?
else|:
literal|""
decl_stmt|;
comment|/* The first pass counts how large an area to allocate to 	 * hold the entire alt_odb structure, including array of 	 * structs and path buffers for them.  The second pass fills 	 * the structure and prepares the path buffers for use by 	 * fill_sha1_path(). 	 */
for|for
control|(
name|totlen
operator|=
name|pass
operator|=
literal|0
init|;
name|pass
operator|<
literal|2
condition|;
name|pass
operator|++
control|)
block|{
name|last
operator|=
name|alt
expr_stmt|;
name|i
operator|=
literal|0
expr_stmt|;
do|do
block|{
name|cp
operator|=
name|strchr
argument_list|(
name|last
argument_list|,
literal|':'
argument_list|)
condition|?
else|:
name|last
operator|+
name|strlen
argument_list|(
name|last
argument_list|)
expr_stmt|;
if|if
condition|(
name|last
operator|!=
name|cp
condition|)
block|{
comment|/* 43 = 40-byte + 2 '/' + terminating NUL */
name|int
name|pfxlen
init|=
name|cp
operator|-
name|last
decl_stmt|;
name|int
name|entlen
init|=
name|pfxlen
operator|+
literal|43
decl_stmt|;
if|if
condition|(
name|pass
operator|==
literal|0
condition|)
name|totlen
operator|+=
name|entlen
expr_stmt|;
else|else
block|{
name|alt_odb
index|[
name|i
index|]
operator|.
name|base
operator|=
name|op
expr_stmt|;
name|alt_odb
index|[
name|i
index|]
operator|.
name|name
operator|=
name|op
operator|+
name|pfxlen
operator|+
literal|1
expr_stmt|;
name|memcpy
argument_list|(
name|op
argument_list|,
name|last
argument_list|,
name|pfxlen
argument_list|)
expr_stmt|;
name|op
index|[
name|pfxlen
index|]
operator|=
name|op
index|[
name|pfxlen
operator|+
literal|3
index|]
operator|=
literal|'/'
expr_stmt|;
name|op
index|[
name|entlen
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
name|op
operator|+=
name|entlen
expr_stmt|;
block|}
name|i
operator|++
expr_stmt|;
block|}
while|while
condition|(
operator|*
name|cp
operator|&&
operator|*
name|cp
operator|==
literal|':'
condition|)
name|cp
operator|++
expr_stmt|;
name|last
operator|=
name|cp
expr_stmt|;
block|}
do|while
condition|(
operator|*
name|cp
condition|)
do|;
if|if
condition|(
name|pass
condition|)
break|break;
name|alt_odb
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|alt_odb
argument_list|)
operator|*
operator|(
name|i
operator|+
literal|1
operator|)
operator|+
name|totlen
argument_list|)
expr_stmt|;
name|alt_odb
index|[
name|i
index|]
operator|.
name|base
operator|=
name|alt_odb
index|[
name|i
index|]
operator|.
name|name
operator|=
literal|0
expr_stmt|;
name|op
operator|=
operator|(
name|char
operator|*
operator|)
operator|(
operator|&
name|alt_odb
index|[
name|i
operator|+
literal|1
index|]
operator|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|find_sha1_file
specifier|static
name|char
modifier|*
name|find_sha1_file
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|char
modifier|*
name|name
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|name
argument_list|,
name|st
argument_list|)
condition|)
return|return
name|name
return|;
if|if
condition|(
operator|!
name|alt_odb
condition|)
name|prepare_alt_odb
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
operator|(
name|name
operator|=
name|alt_odb
index|[
name|i
index|]
operator|.
name|name
operator|)
operator|!=
name|NULL
condition|;
name|i
operator|++
control|)
block|{
name|fill_sha1_path
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|alt_odb
index|[
name|i
index|]
operator|.
name|base
argument_list|,
name|st
argument_list|)
condition|)
return|return
name|alt_odb
index|[
name|i
index|]
operator|.
name|base
return|;
block|}
return|return
name|NULL
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
name|char
modifier|*
name|filename
init|=
name|find_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|st
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|filename
condition|)
block|{
name|error
argument_list|(
literal|"cannot map sha1 file %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
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
name|xmalloc
argument_list|(
operator|*
name|size
argument_list|)
expr_stmt|;
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
DECL|function|read_object_with_reference
name|void
modifier|*
name|read_object_with_reference
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|required_type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|actual_sha1_return
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
name|unsigned
name|char
name|actual_sha1
index|[
literal|20
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|actual_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|int
name|ref_length
init|=
operator|-
literal|1
decl_stmt|;
specifier|const
name|char
modifier|*
name|ref_type
init|=
name|NULL
decl_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|actual_sha1
argument_list|,
name|type
argument_list|,
operator|&
name|isize
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
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|required_type
argument_list|)
condition|)
block|{
operator|*
name|size
operator|=
name|isize
expr_stmt|;
if|if
condition|(
name|actual_sha1_return
condition|)
name|memcpy
argument_list|(
name|actual_sha1_return
argument_list|,
name|actual_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
comment|/* Handle references */
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
name|ref_type
operator|=
literal|"tree "
expr_stmt|;
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
name|ref_type
operator|=
literal|"object "
expr_stmt|;
else|else
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
name|ref_length
operator|=
name|strlen
argument_list|(
name|ref_type
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buffer
argument_list|,
name|ref_type
argument_list|,
name|ref_length
argument_list|)
operator|||
name|get_sha1_hex
argument_list|(
name|buffer
operator|+
name|ref_length
argument_list|,
name|actual_sha1
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
comment|/* Now we have the ID of the referred-to object in 		 * actual_sha1.  Check again. */
block|}
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
name|long
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
specifier|static
name|char
name|tmpfile
index|[
name|PATH_MAX
index|]
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
decl_stmt|,
name|ret
decl_stmt|;
comment|/* Generate the header */
name|hdrlen
operator|=
name|sprintf
argument_list|(
name|hdr
argument_list|,
literal|"%s %lu"
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
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
block|{
comment|/* 		 * FIXME!!! We might do collision checking here, but we'd 		 * need to uncompress the old file and check it. Later. 		 */
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"sha1 file %s: %s"
argument_list|,
name|filename
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|snprintf
argument_list|(
name|tmpfile
argument_list|,
sizeof|sizeof
argument_list|(
name|tmpfile
argument_list|)
argument_list|,
literal|"%s/obj_XXXXXX"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"unable to create temporary sha1 filename %s: %s"
argument_list|,
name|tmpfile
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
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
name|xmalloc
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
name|fchmod
argument_list|(
name|fd
argument_list|,
literal|0444
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|compressed
argument_list|)
expr_stmt|;
name|ret
operator|=
name|link
argument_list|(
name|tmpfile
argument_list|,
name|filename
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
block|{
name|ret
operator|=
name|errno
expr_stmt|;
comment|/* 		 * Coda hack - coda doesn't like cross-directory links, 		 * so we fall back to a rename, which will mean that it 		 * won't be able to check collisions, but that's not a 		 * big deal. 		 * 		 * When this succeeds, we just return 0. We have nothing 		 * left to unlink. 		 */
if|if
condition|(
name|ret
operator|==
name|EXDEV
operator|&&
operator|!
name|rename
argument_list|(
name|tmpfile
argument_list|,
name|filename
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
name|unlink
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
block|{
if|if
condition|(
name|ret
operator|!=
name|EEXIST
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"unable to write sha1 filename %s: %s"
argument_list|,
name|filename
argument_list|,
name|strerror
argument_list|(
name|ret
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* FIXME!!! Collision check here ? */
block|}
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
name|struct
name|stat
name|st
decl_stmt|;
return|return
operator|!
operator|!
name|find_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|st
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|index_fd
name|int
name|index_fd
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|,
name|struct
name|stat
modifier|*
name|st
parameter_list|)
block|{
name|unsigned
name|long
name|size
init|=
name|st
operator|->
name|st_size
decl_stmt|;
name|void
modifier|*
name|buf
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|buf
operator|=
literal|""
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|buf
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
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|buf
operator|==
operator|-
literal|1
condition|)
return|return
operator|-
literal|1
return|;
name|ret
operator|=
name|write_sha1_file
argument_list|(
name|buf
argument_list|,
name|size
argument_list|,
literal|"blob"
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|munmap
argument_list|(
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
