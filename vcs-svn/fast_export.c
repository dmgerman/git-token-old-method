begin_unit
begin_comment
comment|/*  * Licensed under a two-clause BSD-style license.  * See LICENSE for details.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"fast_export.h"
end_include
begin_include
include|#
directive|include
file|"line_buffer.h"
end_include
begin_include
include|#
directive|include
file|"repo_tree.h"
end_include
begin_include
include|#
directive|include
file|"string_pool.h"
end_include
begin_define
DECL|macro|MAX_GITSVN_LINE_LEN
define|#
directive|define
name|MAX_GITSVN_LINE_LEN
value|4096
end_define
begin_decl_stmt
DECL|variable|first_commit_done
specifier|static
name|uint32_t
name|first_commit_done
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fast_export_delete
name|void
name|fast_export_delete
parameter_list|(
name|uint32_t
name|depth
parameter_list|,
name|uint32_t
modifier|*
name|path
parameter_list|)
block|{
name|putchar
argument_list|(
literal|'D'
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
name|pool_print_seq
argument_list|(
name|depth
argument_list|,
name|path
argument_list|,
literal|'/'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_modify
name|void
name|fast_export_modify
parameter_list|(
name|uint32_t
name|depth
parameter_list|,
name|uint32_t
modifier|*
name|path
parameter_list|,
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|mark
parameter_list|)
block|{
comment|/* Mode must be 100644, 100755, 120000, or 160000. */
name|printf
argument_list|(
literal|"M %06"
name|PRIo32
literal|" :%"
name|PRIu32
literal|" "
argument_list|,
name|mode
argument_list|,
name|mark
argument_list|)
expr_stmt|;
name|pool_print_seq
argument_list|(
name|depth
argument_list|,
name|path
argument_list|,
literal|'/'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|gitsvnline
specifier|static
name|char
name|gitsvnline
index|[
name|MAX_GITSVN_LINE_LEN
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fast_export_commit
name|void
name|fast_export_commit
parameter_list|(
name|uint32_t
name|revision
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|log
parameter_list|,
specifier|const
name|char
modifier|*
name|uuid
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|unsigned
name|long
name|timestamp
parameter_list|)
block|{
specifier|static
specifier|const
name|struct
name|strbuf
name|empty
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
operator|!
name|log
condition|)
name|log
operator|=
operator|&
name|empty
expr_stmt|;
if|if
condition|(
operator|*
name|uuid
operator|&&
operator|*
name|url
condition|)
block|{
name|snprintf
argument_list|(
name|gitsvnline
argument_list|,
name|MAX_GITSVN_LINE_LEN
argument_list|,
literal|"\n\ngit-svn-id: %s@%"
name|PRIu32
literal|" %s\n"
argument_list|,
name|url
argument_list|,
name|revision
argument_list|,
name|uuid
argument_list|)
expr_stmt|;
block|}
else|else
block|{
operator|*
name|gitsvnline
operator|=
literal|'\0'
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"commit refs/heads/master\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"committer %s<%s@%s> %ld +0000\n"
argument_list|,
operator|*
name|author
condition|?
name|author
else|:
literal|"nobody"
argument_list|,
operator|*
name|author
condition|?
name|author
else|:
literal|"nobody"
argument_list|,
operator|*
name|uuid
condition|?
name|uuid
else|:
literal|"local"
argument_list|,
name|timestamp
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"data %"
name|PRIuMAX
literal|"\n"
argument_list|,
name|log
operator|->
name|len
operator|+
name|strlen
argument_list|(
name|gitsvnline
argument_list|)
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|log
operator|->
name|buf
argument_list|,
name|log
operator|->
name|len
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|gitsvnline
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|first_commit_done
condition|)
block|{
if|if
condition|(
name|revision
operator|>
literal|1
condition|)
name|printf
argument_list|(
literal|"from refs/heads/master^0\n"
argument_list|)
expr_stmt|;
name|first_commit_done
operator|=
literal|1
expr_stmt|;
block|}
name|repo_diff
argument_list|(
name|revision
operator|-
literal|1
argument_list|,
name|revision
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"progress Imported commit %"
name|PRIu32
literal|".\n\n"
argument_list|,
name|revision
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|die_short_read
specifier|static
name|void
name|die_short_read
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|buffer_ferror
argument_list|(
name|input
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"error reading dump file"
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"invalid dump: unexpected end of file"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_blob
name|void
name|fast_export_blob
parameter_list|(
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|mark
parameter_list|,
name|uint32_t
name|len
parameter_list|,
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|mode
operator|==
name|REPO_MODE_LNK
condition|)
block|{
comment|/* svn symlink blobs start with "link " */
name|len
operator|-=
literal|5
expr_stmt|;
if|if
condition|(
name|buffer_skip_bytes
argument_list|(
name|input
argument_list|,
literal|5
argument_list|)
operator|!=
literal|5
condition|)
name|die_short_read
argument_list|(
name|input
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"blob\nmark :%"
name|PRIu32
literal|"\ndata %"
name|PRIu32
literal|"\n"
argument_list|,
name|mark
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer_copy_bytes
argument_list|(
name|input
argument_list|,
name|len
argument_list|)
operator|!=
name|len
condition|)
name|die_short_read
argument_list|(
name|input
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
