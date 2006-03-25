begin_unit
begin_comment
comment|/*  * Copyright (c) 2005, 2006 Rene Scharfe  */
end_comment
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"tar.h"
end_include
begin_define
DECL|macro|RECORDSIZE
define|#
directive|define
name|RECORDSIZE
value|(512)
end_define
begin_define
DECL|macro|BLOCKSIZE
define|#
directive|define
name|BLOCKSIZE
value|(RECORDSIZE * 20)
end_define
begin_decl_stmt
DECL|variable|tar_tree_usage
specifier|static
specifier|const
name|char
name|tar_tree_usage
index|[]
init|=
literal|"git-tar-tree<key> [basedir]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|block
specifier|static
name|char
name|block
index|[
name|BLOCKSIZE
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|offset
specifier|static
name|unsigned
name|long
name|offset
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|archive_time
specifier|static
name|time_t
name|archive_time
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* tries hard to write, either succeeds or dies in the attempt */
end_comment
begin_function
DECL|function|reliable_write
specifier|static
name|void
name|reliable_write
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
while|while
condition|(
name|size
operator|>
literal|0
condition|)
block|{
name|long
name|ret
init|=
name|xwrite
argument_list|(
literal|1
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EPIPE
condition|)
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"git-tar-tree: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|die
argument_list|(
literal|"git-tar-tree: disk full?"
argument_list|)
expr_stmt|;
block|}
name|size
operator|-=
name|ret
expr_stmt|;
name|buf
operator|+=
name|ret
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* writes out the whole block, but only if it is full */
end_comment
begin_function
DECL|function|write_if_needed
specifier|static
name|void
name|write_if_needed
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|offset
operator|==
name|BLOCKSIZE
condition|)
block|{
name|reliable_write
argument_list|(
name|block
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
name|offset
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* acquire the next record from the buffer; user must call write_if_needed() */
end_comment
begin_function
DECL|function|get_record
specifier|static
name|char
modifier|*
name|get_record
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
name|block
operator|+
name|offset
decl_stmt|;
name|memset
argument_list|(
name|p
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
expr_stmt|;
return|return
name|p
return|;
block|}
end_function
begin_comment
comment|/*  * The end of tar archives is marked by 1024 nul bytes and after that  * follows the rest of the block (if any).  */
end_comment
begin_function
DECL|function|write_trailer
specifier|static
name|void
name|write_trailer
parameter_list|(
name|void
parameter_list|)
block|{
name|get_record
argument_list|()
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
name|get_record
argument_list|()
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
while|while
condition|(
name|offset
condition|)
block|{
name|get_record
argument_list|()
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * queues up writes, so that all our write(2) calls write exactly one  * full block; pads writes to RECORDSIZE  */
end_comment
begin_function
DECL|function|write_blocked
specifier|static
name|void
name|write_blocked
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|tail
decl_stmt|;
if|if
condition|(
name|offset
condition|)
block|{
name|unsigned
name|long
name|chunk
init|=
name|BLOCKSIZE
operator|-
name|offset
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|chunk
condition|)
name|chunk
operator|=
name|size
expr_stmt|;
name|memcpy
argument_list|(
name|block
operator|+
name|offset
argument_list|,
name|buf
argument_list|,
name|chunk
argument_list|)
expr_stmt|;
name|size
operator|-=
name|chunk
expr_stmt|;
name|offset
operator|+=
name|chunk
expr_stmt|;
name|buf
operator|+=
name|chunk
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
while|while
condition|(
name|size
operator|>=
name|BLOCKSIZE
condition|)
block|{
name|reliable_write
argument_list|(
name|buf
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
name|size
operator|-=
name|BLOCKSIZE
expr_stmt|;
name|buf
operator|+=
name|BLOCKSIZE
expr_stmt|;
block|}
if|if
condition|(
name|size
condition|)
block|{
name|memcpy
argument_list|(
name|block
operator|+
name|offset
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buf
operator|+=
name|size
expr_stmt|;
name|offset
operator|+=
name|size
expr_stmt|;
block|}
name|tail
operator|=
name|offset
operator|%
name|RECORDSIZE
expr_stmt|;
if|if
condition|(
name|tail
condition|)
block|{
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
operator|-
name|tail
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
operator|-
name|tail
expr_stmt|;
block|}
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_append_string
specifier|static
name|void
name|strbuf_append_string
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|int
name|slen
init|=
name|strlen
argument_list|(
name|s
argument_list|)
decl_stmt|;
name|int
name|total
init|=
name|sb
operator|->
name|len
operator|+
name|slen
decl_stmt|;
if|if
condition|(
name|total
operator|>
name|sb
operator|->
name|alloc
condition|)
block|{
name|sb
operator|->
name|buf
operator|=
name|xrealloc
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|total
argument_list|)
expr_stmt|;
name|sb
operator|->
name|alloc
operator|=
name|total
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|s
argument_list|,
name|slen
argument_list|)
expr_stmt|;
name|sb
operator|->
name|len
operator|=
name|total
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * pax extended header records have the format "%u %s=%s\n".  %u contains  * the size of the whole string (including the %u), the first %s is the  * keyword, the second one is the value.  This function constructs such a  * string and appends it to a struct strbuf.  */
end_comment
begin_function
DECL|function|strbuf_append_ext_header
specifier|static
name|void
name|strbuf_append_ext_header
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|keyword
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|unsigned
name|int
name|valuelen
parameter_list|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|int
name|len
decl_stmt|,
name|total
decl_stmt|,
name|tmp
decl_stmt|;
comment|/* "%u %s=%s\n" */
name|len
operator|=
literal|1
operator|+
literal|1
operator|+
name|strlen
argument_list|(
name|keyword
argument_list|)
operator|+
literal|1
operator|+
name|valuelen
operator|+
literal|1
expr_stmt|;
for|for
control|(
name|tmp
operator|=
name|len
init|;
name|tmp
operator|>
literal|9
condition|;
name|tmp
operator|/=
literal|10
control|)
name|len
operator|++
expr_stmt|;
name|total
operator|=
name|sb
operator|->
name|len
operator|+
name|len
expr_stmt|;
if|if
condition|(
name|total
operator|>
name|sb
operator|->
name|alloc
condition|)
block|{
name|sb
operator|->
name|buf
operator|=
name|xrealloc
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|total
argument_list|)
expr_stmt|;
name|sb
operator|->
name|alloc
operator|=
name|total
expr_stmt|;
block|}
name|p
operator|=
name|sb
operator|->
name|buf
expr_stmt|;
name|p
operator|+=
name|sprintf
argument_list|(
name|p
argument_list|,
literal|"%u %s="
argument_list|,
name|len
argument_list|,
name|keyword
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
name|value
argument_list|,
name|valuelen
argument_list|)
expr_stmt|;
name|p
operator|+=
name|valuelen
expr_stmt|;
operator|*
name|p
operator|=
literal|'\n'
expr_stmt|;
name|sb
operator|->
name|len
operator|=
name|total
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ustar_header_chksum
specifier|static
name|unsigned
name|int
name|ustar_header_chksum
parameter_list|(
specifier|const
name|struct
name|ustar_header
modifier|*
name|header
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
operator|(
name|char
operator|*
operator|)
name|header
decl_stmt|;
name|unsigned
name|int
name|chksum
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|p
operator|<
name|header
operator|->
name|chksum
condition|)
name|chksum
operator|+=
operator|*
name|p
operator|++
expr_stmt|;
name|chksum
operator|+=
sizeof|sizeof
argument_list|(
name|header
operator|->
name|chksum
argument_list|)
operator|*
literal|' '
expr_stmt|;
name|p
operator|+=
sizeof|sizeof
argument_list|(
name|header
operator|->
name|chksum
argument_list|)
expr_stmt|;
while|while
condition|(
name|p
operator|<
operator|(
name|char
operator|*
operator|)
name|header
operator|+
sizeof|sizeof
argument_list|(
expr|struct
name|ustar_header
argument_list|)
condition|)
name|chksum
operator|+=
operator|*
name|p
operator|++
expr_stmt|;
return|return
name|chksum
return|;
block|}
end_function
begin_function
DECL|function|get_path_prefix
specifier|static
name|int
name|get_path_prefix
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|path
parameter_list|,
name|int
name|maxlen
parameter_list|)
block|{
name|int
name|i
init|=
name|path
operator|->
name|len
decl_stmt|;
if|if
condition|(
name|i
operator|>
name|maxlen
condition|)
name|i
operator|=
name|maxlen
expr_stmt|;
while|while
condition|(
name|i
operator|>
literal|0
operator|&&
name|path
operator|->
name|buf
index|[
name|i
index|]
operator|!=
literal|'/'
condition|)
name|i
operator|--
expr_stmt|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|write_entry
specifier|static
name|void
name|write_entry
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|strbuf
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|struct
name|ustar_header
name|header
decl_stmt|;
name|struct
name|strbuf
name|ext_header
decl_stmt|;
name|memset
argument_list|(
operator|&
name|header
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
name|ext_header
operator|.
name|buf
operator|=
name|NULL
expr_stmt|;
name|ext_header
operator|.
name|len
operator|=
name|ext_header
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|sha1
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_GLOBAL_HEADER
expr_stmt|;
name|mode
operator|=
literal|0100666
expr_stmt|;
name|strcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"pax_global_header"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|path
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_EXT_HEADER
expr_stmt|;
name|mode
operator|=
literal|0100666
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"%s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_DIR
expr_stmt|;
name|mode
operator||=
literal|0777
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_LNK
expr_stmt|;
name|mode
operator||=
literal|0777
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_REG
expr_stmt|;
name|mode
operator||=
operator|(
name|mode
operator|&
literal|0100
operator|)
condition|?
literal|0777
else|:
literal|0666
expr_stmt|;
block|}
else|else
block|{
name|error
argument_list|(
literal|"unsupported file mode: 0%o (SHA1: %s)"
argument_list|,
name|mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|path
operator|->
name|len
operator|>
sizeof|sizeof
argument_list|(
name|header
operator|.
name|name
argument_list|)
condition|)
block|{
name|int
name|plen
init|=
name|get_path_prefix
argument_list|(
name|path
argument_list|,
sizeof|sizeof
argument_list|(
name|header
operator|.
name|prefix
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|rest
init|=
name|path
operator|->
name|len
operator|-
name|plen
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|plen
operator|>
literal|0
operator|&&
name|rest
operator|<=
sizeof|sizeof
argument_list|(
name|header
operator|.
name|name
argument_list|)
condition|)
block|{
name|memcpy
argument_list|(
name|header
operator|.
name|prefix
argument_list|,
name|path
operator|->
name|buf
argument_list|,
name|plen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
name|path
operator|->
name|buf
operator|+
name|plen
operator|+
literal|1
argument_list|,
name|rest
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|sprintf
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"%s.data"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
argument_list|,
literal|"path"
argument_list|,
name|path
operator|->
name|buf
argument_list|,
name|path
operator|->
name|len
argument_list|)
expr_stmt|;
block|}
block|}
else|else
name|memcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
name|path
operator|->
name|buf
argument_list|,
name|path
operator|->
name|len
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
operator|&&
name|buffer
condition|)
block|{
if|if
condition|(
name|size
operator|>
sizeof|sizeof
argument_list|(
name|header
operator|.
name|linkname
argument_list|)
condition|)
block|{
name|sprintf
argument_list|(
name|header
operator|.
name|linkname
argument_list|,
literal|"see %s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
argument_list|,
literal|"linkpath"
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
else|else
name|memcpy
argument_list|(
name|header
operator|.
name|linkname
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|sprintf
argument_list|(
name|header
operator|.
name|mode
argument_list|,
literal|"%07o"
argument_list|,
name|mode
operator|&
literal|07777
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|size
argument_list|,
literal|"%011lo"
argument_list|,
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|?
name|size
else|:
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|mtime
argument_list|,
literal|"%011lo"
argument_list|,
name|archive_time
argument_list|)
expr_stmt|;
comment|/* XXX: should we provide more meaningful info here? */
name|sprintf
argument_list|(
name|header
operator|.
name|uid
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|gid
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|header
operator|.
name|uname
argument_list|,
literal|"git"
argument_list|,
literal|31
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|header
operator|.
name|gname
argument_list|,
literal|"git"
argument_list|,
literal|31
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|devmajor
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|devminor
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|.
name|magic
argument_list|,
literal|"ustar"
argument_list|,
literal|6
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|.
name|version
argument_list|,
literal|"00"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|chksum
argument_list|,
literal|"%07o"
argument_list|,
name|ustar_header_chksum
argument_list|(
operator|&
name|header
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ext_header
operator|.
name|len
operator|>
literal|0
condition|)
block|{
name|write_entry
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|ext_header
operator|.
name|buf
argument_list|,
name|ext_header
operator|.
name|len
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ext_header
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|write_blocked
argument_list|(
operator|&
name|header
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISREG
argument_list|(
name|mode
argument_list|)
operator|&&
name|buffer
operator|&&
name|size
operator|>
literal|0
condition|)
name|write_blocked
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_global_extended_header
specifier|static
name|void
name|write_global_extended_header
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|strbuf
name|ext_header
decl_stmt|;
name|ext_header
operator|.
name|buf
operator|=
name|NULL
expr_stmt|;
name|ext_header
operator|.
name|len
operator|=
name|ext_header
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
argument_list|,
literal|"comment"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_entry
argument_list|(
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|ext_header
operator|.
name|buf
argument_list|,
name|ext_header
operator|.
name|len
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ext_header
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|traverse_tree
specifier|static
name|void
name|traverse_tree
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|tree
parameter_list|,
name|struct
name|strbuf
modifier|*
name|path
parameter_list|)
block|{
name|int
name|pathlen
init|=
name|path
operator|->
name|len
decl_stmt|;
while|while
condition|(
name|tree
operator|->
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
name|void
modifier|*
name|eltbuf
decl_stmt|;
name|char
name|elttype
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|eltsize
decl_stmt|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
name|tree
argument_list|,
operator|&
name|name
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|eltbuf
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|elttype
argument_list|,
operator|&
name|eltsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eltbuf
condition|)
name|die
argument_list|(
literal|"cannot read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|path
operator|->
name|len
operator|=
name|pathlen
expr_stmt|;
name|strbuf_append_string
argument_list|(
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|strbuf_append_string
argument_list|(
name|path
argument_list|,
literal|"/"
argument_list|)
expr_stmt|;
name|write_entry
argument_list|(
name|sha1
argument_list|,
name|path
argument_list|,
name|mode
argument_list|,
name|eltbuf
argument_list|,
name|eltsize
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|struct
name|tree_desc
name|subtree
decl_stmt|;
name|subtree
operator|.
name|buf
operator|=
name|eltbuf
expr_stmt|;
name|subtree
operator|.
name|size
operator|=
name|eltsize
expr_stmt|;
name|traverse_tree
argument_list|(
operator|&
name|subtree
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|eltbuf
argument_list|)
expr_stmt|;
block|}
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
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|,
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|tree_desc
name|tree
decl_stmt|;
name|struct
name|strbuf
name|current_path
decl_stmt|;
name|current_path
operator|.
name|buf
operator|=
name|xmalloc
argument_list|(
name|PATH_MAX
argument_list|)
expr_stmt|;
name|current_path
operator|.
name|alloc
operator|=
name|PATH_MAX
expr_stmt|;
name|current_path
operator|.
name|len
operator|=
name|current_path
operator|.
name|eof
operator|=
literal|0
expr_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|3
case|:
name|strbuf_append_string
argument_list|(
operator|&
name|current_path
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|strbuf_append_string
argument_list|(
operator|&
name|current_path
argument_list|,
literal|"/"
argument_list|)
expr_stmt|;
comment|/* FALLTHROUGH */
case|case
literal|2
case|:
if|if
condition|(
name|get_sha1
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|usage
argument_list|(
name|tar_tree_usage
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|tar_tree_usage
argument_list|)
expr_stmt|;
block|}
name|commit
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|sha1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
condition|)
block|{
name|write_global_extended_header
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|archive_time
operator|=
name|commit
operator|->
name|date
expr_stmt|;
block|}
else|else
name|archive_time
operator|=
name|time
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|tree
operator|.
name|buf
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
literal|"tree"
argument_list|,
operator|&
name|tree
operator|.
name|size
argument_list|,
name|tree_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
operator|.
name|buf
condition|)
name|die
argument_list|(
literal|"not a reference to a tag, commit or tree object: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|current_path
operator|.
name|len
operator|>
literal|0
condition|)
name|write_entry
argument_list|(
name|tree_sha1
argument_list|,
operator|&
name|current_path
argument_list|,
literal|040777
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|traverse_tree
argument_list|(
operator|&
name|tree
argument_list|,
operator|&
name|current_path
argument_list|)
expr_stmt|;
name|write_trailer
argument_list|()
expr_stmt|;
name|free
argument_list|(
name|current_path
operator|.
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
