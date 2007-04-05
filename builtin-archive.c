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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
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
begin_decl_stmt
DECL|variable|archive_usage
specifier|static
specifier|const
name|char
name|archive_usage
index|[]
init|= \
literal|"git-archive --format=<fmt> [--prefix=<prefix>/] [--verbose] [<extra>]<tree-ish> [path...]"
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|archiver_desc
specifier|static
struct|struct
name|archiver_desc
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|write_archive
name|write_archive_fn_t
name|write_archive
decl_stmt|;
DECL|member|parse_extra
name|parse_extra_args_fn_t
name|parse_extra
decl_stmt|;
block|}
DECL|variable|archivers
name|archivers
index|[]
init|=
block|{
block|{
literal|"tar"
block|,
name|write_tar_archive
block|,
name|NULL
block|}
block|,
block|{
literal|"zip"
block|,
name|write_zip_archive
block|,
name|parse_extra_zip_args
block|}
block|, }
struct|;
end_struct
begin_function
DECL|function|run_remote_archiver
specifier|static
name|int
name|run_remote_archiver
parameter_list|(
specifier|const
name|char
modifier|*
name|remote
parameter_list|,
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
name|char
modifier|*
name|url
decl_stmt|,
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
name|pid_t
name|pid
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec
init|=
literal|"git-upload-archive"
decl_stmt|;
name|int
name|exec_at
init|=
literal|0
decl_stmt|;
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
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--exec="
argument_list|)
condition|)
block|{
if|if
condition|(
name|exec_at
condition|)
name|die
argument_list|(
literal|"multiple --exec specified"
argument_list|)
expr_stmt|;
name|exec
operator|=
name|arg
operator|+
literal|7
expr_stmt|;
name|exec_at
operator|=
name|i
expr_stmt|;
break|break;
block|}
block|}
name|url
operator|=
name|xstrdup
argument_list|(
name|remote
argument_list|)
expr_stmt|;
name|pid
operator|=
name|git_connect
argument_list|(
name|fd
argument_list|,
name|url
argument_list|,
name|exec
argument_list|)
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return
name|pid
return|;
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
block|{
if|if
condition|(
name|i
operator|==
name|exec_at
condition|)
continue|continue;
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
block|}
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
literal|"git-archive: expected ACK/NAK, got EOF"
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
literal|"git-archive: NACK %s"
argument_list|,
name|buf
operator|+
literal|5
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"git-archive: protocol error"
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
literal|"git-archive: expected a flush"
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
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|rv
operator||=
name|finish_connect
argument_list|(
name|pid
argument_list|)
expr_stmt|;
return|return
operator|!
operator|!
name|rv
return|;
block|}
end_function
begin_function
DECL|function|init_archiver
specifier|static
name|int
name|init_archiver
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|archiver
modifier|*
name|ar
parameter_list|)
block|{
name|int
name|rv
init|=
operator|-
literal|1
decl_stmt|,
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
name|ARRAY_SIZE
argument_list|(
name|archivers
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|name
argument_list|,
name|archivers
index|[
name|i
index|]
operator|.
name|name
argument_list|)
condition|)
block|{
name|memset
argument_list|(
name|ar
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ar
argument_list|)
argument_list|)
expr_stmt|;
name|ar
operator|->
name|name
operator|=
name|archivers
index|[
name|i
index|]
operator|.
name|name
expr_stmt|;
name|ar
operator|->
name|write_archive
operator|=
name|archivers
index|[
name|i
index|]
operator|.
name|write_archive
expr_stmt|;
name|ar
operator|->
name|parse_extra
operator|=
name|archivers
index|[
name|i
index|]
operator|.
name|parse_extra
expr_stmt|;
name|rv
operator|=
literal|0
expr_stmt|;
break|break;
block|}
block|}
return|return
name|rv
return|;
block|}
end_function
begin_function
DECL|function|parse_pathspec_arg
name|void
name|parse_pathspec_arg
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|ar_args
parameter_list|)
block|{
name|ar_args
operator|->
name|pathspec
operator|=
name|get_pathspec
argument_list|(
name|ar_args
operator|->
name|base
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_treeish_arg
name|void
name|parse_treeish_arg
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|ar_args
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
name|name
init|=
name|argv
index|[
literal|0
index|]
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
decl_stmt|;
name|time_t
name|archive_time
decl_stmt|;
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name"
argument_list|)
expr_stmt|;
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
name|commit_sha1
operator|=
name|commit
operator|->
name|object
operator|.
name|sha1
expr_stmt|;
name|archive_time
operator|=
name|commit
operator|->
name|date
expr_stmt|;
block|}
else|else
block|{
name|commit_sha1
operator|=
name|NULL
expr_stmt|;
name|archive_time
operator|=
name|time
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
block|}
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|tree
operator|==
name|NULL
condition|)
name|die
argument_list|(
literal|"not a tree object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
block|{
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|int
name|err
decl_stmt|;
name|err
operator|=
name|get_tree_entry
argument_list|(
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|prefix
argument_list|,
name|tree_sha1
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|||
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|die
argument_list|(
literal|"current working directory is untracked"
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|tree_sha1
argument_list|)
expr_stmt|;
block|}
name|ar_args
operator|->
name|tree
operator|=
name|tree
expr_stmt|;
name|ar_args
operator|->
name|commit_sha1
operator|=
name|commit_sha1
expr_stmt|;
name|ar_args
operator|->
name|time
operator|=
name|archive_time
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_archive_args
name|int
name|parse_archive_args
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
name|struct
name|archiver
modifier|*
name|ar
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|extra_argv
index|[
name|MAX_EXTRA_ARGS
index|]
decl_stmt|;
name|int
name|extra_argc
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|format
init|=
name|NULL
decl_stmt|;
comment|/* might want to default to "tar" */
specifier|const
name|char
modifier|*
name|base
init|=
literal|""
decl_stmt|;
name|int
name|verbose
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
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
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--list"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-l"
argument_list|)
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|archivers
argument_list|)
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|archivers
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--verbose"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-v"
argument_list|)
condition|)
block|{
name|verbose
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--format="
argument_list|)
condition|)
block|{
name|format
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--prefix="
argument_list|)
condition|)
block|{
name|base
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|i
operator|++
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|extra_argc
operator|>
name|MAX_EXTRA_ARGS
operator|-
literal|1
condition|)
name|die
argument_list|(
literal|"Too many extra options"
argument_list|)
expr_stmt|;
name|extra_argv
index|[
name|extra_argc
operator|++
index|]
operator|=
name|arg
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
comment|/* We need at least one parameter -- tree-ish */
if|if
condition|(
name|argc
operator|-
literal|1
operator|<
name|i
condition|)
name|usage
argument_list|(
name|archive_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|format
condition|)
name|die
argument_list|(
literal|"You must specify an archive format"
argument_list|)
expr_stmt|;
if|if
condition|(
name|init_archiver
argument_list|(
name|format
argument_list|,
name|ar
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Unknown archive format '%s'"
argument_list|,
name|format
argument_list|)
expr_stmt|;
if|if
condition|(
name|extra_argc
condition|)
block|{
if|if
condition|(
operator|!
name|ar
operator|->
name|parse_extra
condition|)
name|die
argument_list|(
literal|"'%s' format does not handle %s"
argument_list|,
name|ar
operator|->
name|name
argument_list|,
name|extra_argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|ar
operator|->
name|args
operator|.
name|extra
operator|=
name|ar
operator|->
name|parse_extra
argument_list|(
name|extra_argc
argument_list|,
name|extra_argv
argument_list|)
expr_stmt|;
block|}
name|ar
operator|->
name|args
operator|.
name|verbose
operator|=
name|verbose
expr_stmt|;
name|ar
operator|->
name|args
operator|.
name|base
operator|=
name|base
expr_stmt|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|extract_remote_arg
specifier|static
specifier|const
name|char
modifier|*
name|extract_remote_arg
parameter_list|(
name|int
modifier|*
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|int
name|ix
decl_stmt|,
name|iy
decl_stmt|,
name|cnt
init|=
operator|*
name|ac
decl_stmt|;
name|int
name|no_more_options
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote
init|=
name|NULL
decl_stmt|;
for|for
control|(
name|ix
operator|=
name|iy
operator|=
literal|1
init|;
name|ix
operator|<
name|cnt
condition|;
name|ix
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|av
index|[
name|ix
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
name|no_more_options
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|no_more_options
condition|)
block|{
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--remote="
argument_list|)
condition|)
block|{
if|if
condition|(
name|remote
condition|)
name|die
argument_list|(
literal|"Multiple --remote specified"
argument_list|)
expr_stmt|;
name|remote
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
name|no_more_options
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|ix
operator|!=
name|iy
condition|)
name|av
index|[
name|iy
index|]
operator|=
name|arg
expr_stmt|;
name|iy
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|remote
condition|)
block|{
name|av
index|[
operator|--
name|cnt
index|]
operator|=
name|NULL
expr_stmt|;
operator|*
name|ac
operator|=
name|cnt
expr_stmt|;
block|}
return|return
name|remote
return|;
block|}
end_function
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
name|struct
name|archiver
name|ar
decl_stmt|;
name|int
name|tree_idx
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote
init|=
name|NULL
decl_stmt|;
name|remote
operator|=
name|extract_remote_arg
argument_list|(
operator|&
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
condition|)
return|return
name|run_remote_archiver
argument_list|(
name|remote
argument_list|,
name|argc
argument_list|,
name|argv
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
name|memset
argument_list|(
operator|&
name|ar
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|ar
argument_list|)
argument_list|)
expr_stmt|;
name|tree_idx
operator|=
name|parse_archive_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|ar
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefix
operator|==
name|NULL
condition|)
name|prefix
operator|=
name|setup_git_directory
argument_list|()
expr_stmt|;
name|argv
operator|+=
name|tree_idx
expr_stmt|;
name|parse_treeish_arg
argument_list|(
name|argv
argument_list|,
operator|&
name|ar
operator|.
name|args
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|parse_pathspec_arg
argument_list|(
name|argv
operator|+
literal|1
argument_list|,
operator|&
name|ar
operator|.
name|args
argument_list|)
expr_stmt|;
return|return
name|ar
operator|.
name|write_archive
argument_list|(
operator|&
name|ar
operator|.
name|args
argument_list|)
return|;
block|}
end_function
end_unit
