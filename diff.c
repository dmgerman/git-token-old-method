begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
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
begin_decl_stmt
DECL|variable|diff_cmd
specifier|static
name|char
modifier|*
name|diff_cmd
init|=
literal|"diff -L'k/%s' -L'l/%s'"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_opts
specifier|static
name|char
modifier|*
name|diff_opts
init|=
literal|"-pu"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|external_diff
specifier|static
specifier|const
name|char
modifier|*
name|external_diff
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|external_diff_cmd
init|=
name|NULL
decl_stmt|;
specifier|static
name|int
name|done_preparing
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|done_preparing
condition|)
return|return
name|external_diff_cmd
return|;
comment|/* 	 * Default values above are meant to match the 	 * Linux kernel development style.  Examples of 	 * alternative styles you can specify via environment 	 * variables are: 	 * 	 * GIT_DIFF_CMD="diff -L '%s' -L '%s'" 	 * GIT_DIFF_OPTS="-c"; 	 */
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_EXTERNAL_DIFF"
argument_list|)
condition|)
name|external_diff_cmd
operator|=
name|getenv
argument_list|(
literal|"GIT_EXTERNAL_DIFF"
argument_list|)
expr_stmt|;
comment|/* In case external diff fails... */
name|diff_cmd
operator|=
name|getenv
argument_list|(
literal|"GIT_DIFF_CMD"
argument_list|)
condition|?
else|:
name|diff_cmd
expr_stmt|;
name|diff_opts
operator|=
name|getenv
argument_list|(
literal|"GIT_DIFF_OPTS"
argument_list|)
condition|?
else|:
name|diff_opts
expr_stmt|;
name|done_preparing
operator|=
literal|1
expr_stmt|;
return|return
name|external_diff_cmd
return|;
block|}
end_function
begin_comment
comment|/* Help to copy the thing properly quoted for the shell safety.  * any single quote is replaced with '\'', and the caller is  * expected to enclose the result within a single quote pair.  *  * E.g.  *  original     sq_expand     result  *  name     ==> name      ==> 'name'  *  a b      ==> a b       ==> 'a b'  *  a'b      ==> a'\''b    ==> 'a'\''b'  */
end_comment
begin_function
DECL|function|sq_expand
specifier|static
name|char
modifier|*
name|sq_expand
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|buf
init|=
name|NULL
decl_stmt|;
name|int
name|cnt
decl_stmt|,
name|c
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|char
modifier|*
name|bp
decl_stmt|;
comment|/* count bytes needed to store the quoted string. */
for|for
control|(
name|cnt
operator|=
literal|1
operator|,
name|cp
operator|=
name|src
init|;
operator|*
name|cp
condition|;
name|cnt
operator|++
operator|,
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'\''
condition|)
name|cnt
operator|+=
literal|3
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|buf
operator|=
name|malloc
argument_list|(
name|cnt
argument_list|)
operator|)
condition|)
return|return
name|buf
return|;
name|bp
operator|=
name|buf
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|c
operator|!=
literal|'\''
condition|)
operator|*
name|bp
operator|++
operator|=
name|c
expr_stmt|;
else|else
block|{
name|bp
operator|=
name|strcpy
argument_list|(
name|bp
argument_list|,
literal|"'\\''"
argument_list|)
expr_stmt|;
name|bp
operator|+=
literal|4
expr_stmt|;
block|}
block|}
operator|*
name|bp
operator|=
literal|0
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_struct
DECL|struct|diff_tempfile
specifier|static
struct|struct
name|diff_tempfile
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|hex
name|char
name|hex
index|[
literal|41
index|]
decl_stmt|;
DECL|member|mode
name|char
name|mode
index|[
literal|10
index|]
decl_stmt|;
DECL|member|tmp_path
name|char
name|tmp_path
index|[
literal|50
index|]
decl_stmt|;
block|}
DECL|variable|diff_temp
name|diff_temp
index|[
literal|2
index|]
struct|;
end_struct
begin_function
DECL|function|builtin_diff
specifier|static
name|void
name|builtin_diff
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_tempfile
modifier|*
name|temp
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|diff_arg
init|=
literal|"'%s' '%s'"
decl_stmt|;
specifier|const
name|char
modifier|*
name|name_1_sq
init|=
name|sq_expand
argument_list|(
name|temp
index|[
literal|0
index|]
operator|.
name|name
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|name_2_sq
init|=
name|sq_expand
argument_list|(
name|temp
index|[
literal|1
index|]
operator|.
name|name
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|name_sq
init|=
name|sq_expand
argument_list|(
name|name
argument_list|)
decl_stmt|;
comment|/* diff_cmd and diff_arg have 4 %s in total which makes 	 * the sum of these strings 8 bytes larger than required. 	 * we use 2 spaces around diff-opts, and we need to count 	 * terminating NUL, so we subtract 5 here. 	 */
name|int
name|cmd_size
init|=
operator|(
name|strlen
argument_list|(
name|diff_cmd
argument_list|)
operator|+
name|strlen
argument_list|(
name|name_sq
argument_list|)
operator|*
literal|2
operator|+
name|strlen
argument_list|(
name|diff_opts
argument_list|)
operator|+
name|strlen
argument_list|(
name|diff_arg
argument_list|)
operator|+
name|strlen
argument_list|(
name|name_1_sq
argument_list|)
operator|+
name|strlen
argument_list|(
name|name_2_sq
argument_list|)
operator|-
literal|5
operator|)
decl_stmt|;
name|char
modifier|*
name|cmd
init|=
name|malloc
argument_list|(
name|cmd_size
argument_list|)
decl_stmt|;
name|int
name|next_at
init|=
literal|0
decl_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
name|diff_cmd
argument_list|,
name|name_sq
argument_list|,
name|name_sq
argument_list|)
expr_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
literal|" %s "
argument_list|,
name|diff_opts
argument_list|)
expr_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
name|diff_arg
argument_list|,
name|name_1_sq
argument_list|,
name|name_2_sq
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
literal|"/bin/sh"
argument_list|,
literal|"sh"
argument_list|,
literal|"-c"
argument_list|,
name|cmd
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|prepare_temp_file
specifier|static
name|void
name|prepare_temp_file
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_tempfile
modifier|*
name|temp
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|one
parameter_list|)
block|{
specifier|static
name|unsigned
name|char
name|null_sha1
index|[
literal|20
index|]
init|=
block|{
literal|0
block|, }
decl_stmt|;
if|if
condition|(
operator|!
name|one
operator|->
name|file_valid
condition|)
block|{
name|not_a_valid_file
label|:
name|temp
operator|->
name|name
operator|=
literal|"/dev/null"
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|one
operator|->
name|sha1_valid
operator|&&
operator|!
name|memcmp
argument_list|(
name|one
operator|->
name|u
operator|.
name|sha1
argument_list|,
name|null_sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
condition|)
block|{
name|one
operator|->
name|sha1_valid
operator|=
literal|0
expr_stmt|;
name|one
operator|->
name|u
operator|.
name|name
operator|=
name|name
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|one
operator|->
name|sha1_valid
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|temp
operator|->
name|name
operator|=
name|one
operator|->
name|u
operator|.
name|name
expr_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|temp
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
goto|goto
name|not_a_valid_file
goto|;
name|die
argument_list|(
literal|"stat(%s): %s"
argument_list|,
name|temp
operator|->
name|name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"%06o"
argument_list|,
name|S_IFREG
operator||
name|ce_permissions
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|int
name|fd
decl_stmt|;
name|void
modifier|*
name|blob
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
decl_stmt|;
name|blob
operator|=
name|read_sha1_file
argument_list|(
name|one
operator|->
name|u
operator|.
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
name|blob
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to read blob object for %s (%s)"
argument_list|,
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|one
operator|->
name|u
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|tmp_path
argument_list|,
literal|".diff_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|temp
operator|->
name|tmp_path
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create temp-file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|blob
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
name|die
argument_list|(
literal|"unable to write temp-file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|blob
argument_list|)
expr_stmt|;
name|temp
operator|->
name|name
operator|=
name|temp
operator|->
name|tmp_path
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|one
operator|->
name|u
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|temp
operator|->
name|hex
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
name|sprintf
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"%06o"
argument_list|,
name|one
operator|->
name|mode
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|remove_tempfile
specifier|static
name|void
name|remove_tempfile
parameter_list|(
name|void
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
literal|2
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
operator|==
name|diff_temp
index|[
name|i
index|]
operator|.
name|tmp_path
condition|)
block|{
name|unlink
argument_list|(
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
operator|=
name|NULL
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* An external diff command takes:  *  * diff-cmd name infile1 infile1-sha1 infile1-mode \  *               infile2 infile2-sha1 infile2-mode.  *  */
end_comment
begin_function
DECL|function|run_external_diff
name|void
name|run_external_diff
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|one
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|two
parameter_list|)
block|{
name|struct
name|diff_tempfile
modifier|*
name|temp
init|=
name|diff_temp
decl_stmt|;
name|int
name|pid
decl_stmt|,
name|status
decl_stmt|;
specifier|static
name|int
name|atexit_asked
init|=
literal|0
decl_stmt|;
name|prepare_temp_file
argument_list|(
name|name
argument_list|,
operator|&
name|temp
index|[
literal|0
index|]
argument_list|,
name|one
argument_list|)
expr_stmt|;
name|prepare_temp_file
argument_list|(
name|name
argument_list|,
operator|&
name|temp
index|[
literal|1
index|]
argument_list|,
name|two
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|atexit_asked
operator|&&
operator|(
name|temp
index|[
literal|0
index|]
operator|.
name|name
operator|==
name|temp
index|[
literal|0
index|]
operator|.
name|tmp_path
operator|||
name|temp
index|[
literal|1
index|]
operator|.
name|name
operator|==
name|temp
index|[
literal|1
index|]
operator|.
name|tmp_path
operator|)
condition|)
block|{
name|atexit_asked
operator|=
literal|1
expr_stmt|;
name|atexit
argument_list|(
name|remove_tempfile
argument_list|)
expr_stmt|;
block|}
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to fork"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
specifier|const
name|char
modifier|*
name|pgm
init|=
name|external_diff
argument_list|()
decl_stmt|;
if|if
condition|(
name|pgm
condition|)
name|execlp
argument_list|(
name|pgm
argument_list|,
name|pgm
argument_list|,
name|name
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|name
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|hex
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|name
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|hex
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* 		 * otherwise we use the built-in one. 		 */
name|builtin_diff
argument_list|(
name|name
argument_list|,
name|temp
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
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
operator|||
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
name|die
argument_list|(
literal|"diff program failed"
argument_list|)
expr_stmt|;
name|remove_tempfile
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_diff_empty
name|void
name|show_diff_empty
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|reverse
parameter_list|)
block|{
name|struct
name|diff_spec
name|spec
index|[
literal|2
index|]
decl_stmt|,
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
name|memcpy
argument_list|(
name|spec
index|[
literal|0
index|]
operator|.
name|u
operator|.
name|sha1
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|sha1_valid
operator|=
name|spec
index|[
literal|0
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|file_valid
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|reverse
condition|)
block|{
name|one
operator|=
name|spec
operator|+
literal|1
expr_stmt|;
name|two
operator|=
name|spec
expr_stmt|;
block|}
else|else
block|{
name|one
operator|=
name|spec
expr_stmt|;
name|two
operator|=
name|one
operator|+
literal|1
expr_stmt|;
block|}
name|run_external_diff
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_differences
name|void
name|show_differences
parameter_list|(
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|int
name|reverse
parameter_list|)
block|{
name|struct
name|diff_spec
name|spec
index|[
literal|2
index|]
decl_stmt|,
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
name|memcpy
argument_list|(
name|spec
index|[
literal|0
index|]
operator|.
name|u
operator|.
name|sha1
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|sha1_valid
operator|=
name|spec
index|[
literal|0
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|u
operator|.
name|name
operator|=
name|ce
operator|->
name|name
expr_stmt|;
comment|/* the name we stated */
name|spec
index|[
literal|1
index|]
operator|.
name|sha1_valid
operator|=
literal|0
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|reverse
condition|)
block|{
name|one
operator|=
name|spec
operator|+
literal|1
expr_stmt|;
name|two
operator|=
name|spec
expr_stmt|;
block|}
else|else
block|{
name|one
operator|=
name|spec
expr_stmt|;
name|two
operator|=
name|one
operator|+
literal|1
expr_stmt|;
block|}
name|run_external_diff
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
