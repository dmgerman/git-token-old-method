begin_unit
begin_comment
comment|/*  * Builtin "git verify-tag"  *  * Copyright (c) 2007 Carlos Rica<jasampler@gmail.com>  *  * Based on git-verify-tag.sh  */
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
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|<signal.h>
end_include
begin_decl_stmt
DECL|variable|builtin_verify_tag_usage
specifier|static
specifier|const
name|char
name|builtin_verify_tag_usage
index|[]
init|=
literal|"git verify-tag [-v|--verbose]<tag>..."
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|PGP_SIGNATURE
define|#
directive|define
name|PGP_SIGNATURE
value|"-----BEGIN PGP SIGNATURE-----"
end_define
begin_function
DECL|function|run_gpg_verify
specifier|static
name|int
name|run_gpg_verify
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
name|struct
name|child_process
name|gpg
decl_stmt|;
specifier|const
name|char
modifier|*
name|args_gpg
index|[]
init|=
block|{
literal|"gpg"
block|,
literal|"--verify"
block|,
literal|"FILE"
block|,
literal|"-"
block|,
name|NULL
block|}
decl_stmt|;
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|,
modifier|*
name|eol
decl_stmt|;
name|size_t
name|len
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|ret
decl_stmt|;
name|fd
operator|=
name|git_mkstemp
argument_list|(
name|path
argument_list|,
name|PATH_MAX
argument_list|,
literal|".git_vtag_tmpXXXXXX"
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"could not create temporary file '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"failed writing temporary file '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
comment|/* find the length without signature */
name|len
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|len
operator|<
name|size
operator|&&
name|prefixcmp
argument_list|(
name|buf
operator|+
name|len
argument_list|,
name|PGP_SIGNATURE
argument_list|)
condition|)
block|{
name|eol
operator|=
name|memchr
argument_list|(
name|buf
operator|+
name|len
argument_list|,
literal|'\n'
argument_list|,
name|size
operator|-
name|len
argument_list|)
expr_stmt|;
name|len
operator|+=
name|eol
condition|?
name|eol
operator|-
operator|(
name|buf
operator|+
name|len
operator|)
operator|+
literal|1
else|:
name|size
operator|-
name|len
expr_stmt|;
block|}
if|if
condition|(
name|verbose
condition|)
name|write_in_full
argument_list|(
literal|1
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|gpg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|gpg
argument_list|)
argument_list|)
expr_stmt|;
name|gpg
operator|.
name|argv
operator|=
name|args_gpg
expr_stmt|;
name|gpg
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
name|args_gpg
index|[
literal|2
index|]
operator|=
name|path
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|gpg
argument_list|)
condition|)
block|{
name|unlink
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"could not run gpg."
argument_list|)
return|;
block|}
name|write_in_full
argument_list|(
name|gpg
operator|.
name|in
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|gpg
operator|.
name|in
argument_list|)
expr_stmt|;
name|ret
operator|=
name|finish_command
argument_list|(
operator|&
name|gpg
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|verify_tag
specifier|static
name|int
name|verify_tag
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|ret
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
return|return
name|error
argument_list|(
literal|"tag '%s' not found."
argument_list|,
name|name
argument_list|)
return|;
name|type
operator|=
name|sha1_object_info
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|type
operator|!=
name|OBJ_TAG
condition|)
return|return
name|error
argument_list|(
literal|"%s: cannot verify a non-tag object of type %s."
argument_list|,
name|name
argument_list|,
name|typename
argument_list|(
name|type
argument_list|)
argument_list|)
return|;
name|buf
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
operator|!
name|buf
condition|)
return|return
name|error
argument_list|(
literal|"%s: unable to read file."
argument_list|,
name|name
argument_list|)
return|;
name|ret
operator|=
name|run_gpg_verify
argument_list|(
name|buf
argument_list|,
name|size
argument_list|,
name|verbose
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|cmd_verify_tag
name|int
name|cmd_verify_tag
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
name|int
name|i
init|=
literal|1
decl_stmt|,
name|verbose
init|=
literal|0
decl_stmt|,
name|had_error
init|=
literal|0
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|1
condition|)
name|usage
argument_list|(
name|builtin_verify_tag_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"-v"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--verbose"
argument_list|)
condition|)
block|{
name|verbose
operator|=
literal|1
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
comment|/* sometimes the program was terminated because this signal 	 * was received in the process of writing the gpg input: */
name|signal
argument_list|(
name|SIGPIPE
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|argc
condition|)
if|if
condition|(
name|verify_tag
argument_list|(
name|argv
index|[
name|i
operator|++
index|]
argument_list|,
name|verbose
argument_list|)
condition|)
name|had_error
operator|=
literal|1
expr_stmt|;
return|return
name|had_error
return|;
block|}
end_function
end_unit
