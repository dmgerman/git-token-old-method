begin_unit
begin_comment
comment|/*  * Copyright (c) 2006 Franck Bui-Huu  * Copyright (c) 2006 Rene Scharfe  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"archive.h"
end_include
begin_include
include|#
directive|include
file|"transport.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"sideband.h"
end_include
begin_function
DECL|function|create_output_file
specifier|static
name|void
name|create_output_file
parameter_list|(
specifier|const
name|char
modifier|*
name|output_file
parameter_list|)
block|{
name|int
name|output_fd
init|=
name|open
argument_list|(
name|output_file
argument_list|,
name|O_CREAT
operator||
name|O_WRONLY
operator||
name|O_TRUNC
argument_list|,
literal|0666
argument_list|)
decl_stmt|;
if|if
condition|(
name|output_fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not create archive file '%s'"
argument_list|)
argument_list|,
name|output_file
argument_list|)
expr_stmt|;
if|if
condition|(
name|output_fd
operator|!=
literal|1
condition|)
block|{
if|if
condition|(
name|dup2
argument_list|(
name|output_fd
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not redirect output"
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|close
argument_list|(
name|output_fd
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|run_remote_archiver
specifier|static
name|int
name|run_remote_archiver
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|remote
parameter_list|,
specifier|const
name|char
modifier|*
name|exec
parameter_list|,
specifier|const
name|char
modifier|*
name|name_hint
parameter_list|)
block|{
name|char
name|buf
index|[
name|LARGE_PACKET_MAX
index|]
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|,
name|i
decl_stmt|,
name|len
decl_stmt|,
name|rv
decl_stmt|;
name|struct
name|transport
modifier|*
name|transport
decl_stmt|;
name|struct
name|remote
modifier|*
name|_remote
decl_stmt|;
name|_remote
operator|=
name|remote_get
argument_list|(
name|remote
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|_remote
operator|->
name|url
index|[
literal|0
index|]
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"git archive: Remote with no URL"
argument_list|)
argument_list|)
expr_stmt|;
name|transport
operator|=
name|transport_get
argument_list|(
name|_remote
argument_list|,
name|_remote
operator|->
name|url
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|transport_connect
argument_list|(
name|transport
argument_list|,
literal|"git-upload-archive"
argument_list|,
name|exec
argument_list|,
name|fd
argument_list|)
expr_stmt|;
comment|/* 	 * Inject a fake --format field at the beginning of the 	 * arguments, with the format inferred from our output 	 * filename. This way explicit --format options can override 	 * it. 	 */
if|if
condition|(
name|name_hint
condition|)
block|{
specifier|const
name|char
modifier|*
name|format
init|=
name|archive_format_from_filename
argument_list|(
name|name_hint
argument_list|)
decl_stmt|;
if|if
condition|(
name|format
condition|)
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"argument --format=%s\n"
argument_list|,
name|format
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"argument %s\n"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|packet_flush
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|len
operator|=
name|packet_read_line
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"git archive: expected ACK/NAK, got EOF"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buf
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"ACK"
argument_list|)
condition|)
block|{
if|if
condition|(
name|len
operator|>
literal|5
operator|&&
operator|!
name|prefixcmp
argument_list|(
name|buf
argument_list|,
literal|"NACK "
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"git archive: NACK %s"
argument_list|)
argument_list|,
name|buf
operator|+
literal|5
argument_list|)
expr_stmt|;
name|die
argument_list|(
name|_
argument_list|(
literal|"git archive: protocol error"
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|len
operator|=
name|packet_read_line
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"git archive: expected a flush"
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Now, start reading from fd[0] and spit it out to stdout */
name|rv
operator|=
name|recv_sideband
argument_list|(
literal|"archive"
argument_list|,
name|fd
index|[
literal|0
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|rv
operator||=
name|transport_disconnect
argument_list|(
name|transport
argument_list|)
expr_stmt|;
return|return
operator|!
operator|!
name|rv
return|;
block|}
end_function
begin_define
DECL|macro|PARSE_OPT_KEEP_ALL
define|#
directive|define
name|PARSE_OPT_KEEP_ALL
value|( PARSE_OPT_KEEP_DASHDASH | 	\ 			     PARSE_OPT_KEEP_ARGV0 | 	\ 			     PARSE_OPT_KEEP_UNKNOWN |	\ 			     PARSE_OPT_NO_INTERNAL_HELP	)
end_define
begin_function
DECL|function|cmd_archive
name|int
name|cmd_archive
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|exec
init|=
literal|"git-upload-archive"
decl_stmt|;
specifier|const
name|char
modifier|*
name|output
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|local_opts
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|'o'
argument_list|,
literal|"output"
argument_list|,
operator|&
name|output
argument_list|,
literal|"file"
argument_list|,
literal|"write the archive to this file"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"remote"
argument_list|,
operator|&
name|remote
argument_list|,
literal|"repo"
argument_list|,
literal|"retrieve the archive from remote repository<repo>"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"exec"
argument_list|,
operator|&
name|exec
argument_list|,
literal|"cmd"
argument_list|,
literal|"path to the remote git-upload-archive command"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|local_opts
argument_list|,
name|NULL
argument_list|,
name|PARSE_OPT_KEEP_ALL
argument_list|)
expr_stmt|;
if|if
condition|(
name|output
condition|)
name|create_output_file
argument_list|(
name|output
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
condition|)
return|return
name|run_remote_archiver
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|remote
argument_list|,
name|exec
argument_list|,
name|output
argument_list|)
return|;
name|setvbuf
argument_list|(
name|stderr
argument_list|,
name|NULL
argument_list|,
name|_IOLBF
argument_list|,
name|BUFSIZ
argument_list|)
expr_stmt|;
return|return
name|write_archive
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
literal|1
argument_list|,
name|output
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
end_unit
