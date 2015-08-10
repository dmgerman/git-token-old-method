begin_unit
begin_comment
comment|/*  * State diagram and cleanup  * -------------------------  *  * If the program exits while a temporary file is active, we want to  * make sure that we remove it. This is done by remembering the active  * temporary files in a linked list, `tempfile_list`. An `atexit(3)`  * handler and a signal handler are registered, to clean up any active  * temporary files.  *  * Because the signal handler can run at any time, `tempfile_list` and  * the `tempfile` objects that comprise it must be kept in  * self-consistent states at all times.  *  * The possible states of a `tempfile` object are as follows:  *  * - Uninitialized. In this state the object's `on_list` field must be  *   zero but the rest of its contents need not be initialized. As  *   soon as the object is used in any way, it is irrevocably  *   registered in `tempfile_list`, and `on_list` is set.  *  * - Active, file open (after `create_tempfile()` or  *   `reopen_tempfile()`). In this state:  *  *   - the temporary file exists  *   - `active` is set  *   - `filename` holds the filename of the temporary file  *   - `fd` holds a file descriptor open for writing to it  *   - `fp` holds a pointer to an open `FILE` object if and only if  *     `fdopen_tempfile()` has been called on the object  *   - `owner` holds the PID of the process that created the file  *  * - Active, file closed (after successful `close_tempfile()`). Same  *   as the previous state, except that the temporary file is closed,  *   `fd` is -1, and `fp` is `NULL`.  *  * - Inactive (after `delete_tempfile()`, `rename_tempfile()`, a  *   failed attempt to create a temporary file, or a failed  *   `close_tempfile()`). In this state:  *  *   - `active` is unset  *   - `filename` is empty (usually, though there are transitory  *     states in which this condition doesn't hold). Client code should  *     *not* rely on the filename being empty in this state.  *   - `fd` is -1 and `fp` is `NULL`  *   - the object is left registered in the `tempfile_list`, and  *     `on_list` is set.  *  * A temporary file is owned by the process that created it. The  * `tempfile` has an `owner` field that records the owner's PID. This  * field is used to prevent a forked process from deleting a temporary  * file created by its parent.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tempfile.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_decl_stmt
DECL|variable|tempfile_list
specifier|static
name|struct
name|tempfile
modifier|*
specifier|volatile
name|tempfile_list
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|remove_tempfiles
specifier|static
name|void
name|remove_tempfiles
parameter_list|(
name|int
name|skip_fclose
parameter_list|)
block|{
name|pid_t
name|me
init|=
name|getpid
argument_list|()
decl_stmt|;
while|while
condition|(
name|tempfile_list
condition|)
block|{
if|if
condition|(
name|tempfile_list
operator|->
name|owner
operator|==
name|me
condition|)
block|{
comment|/* fclose() is not safe to call in a signal handler */
if|if
condition|(
name|skip_fclose
condition|)
name|tempfile_list
operator|->
name|fp
operator|=
name|NULL
expr_stmt|;
name|delete_tempfile
argument_list|(
name|tempfile_list
argument_list|)
expr_stmt|;
block|}
name|tempfile_list
operator|=
name|tempfile_list
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|remove_tempfiles_on_exit
specifier|static
name|void
name|remove_tempfiles_on_exit
parameter_list|(
name|void
parameter_list|)
block|{
name|remove_tempfiles
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_tempfiles_on_signal
specifier|static
name|void
name|remove_tempfiles_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_tempfiles
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|sigchain_pop
argument_list|(
name|signo
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|signo
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Initialize *tempfile if necessary and add it to tempfile_list.  */
end_comment
begin_function
DECL|function|prepare_tempfile_object
specifier|static
name|void
name|prepare_tempfile_object
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile_list
condition|)
block|{
comment|/* One-time initialization */
name|sigchain_push_common
argument_list|(
name|remove_tempfiles_on_signal
argument_list|)
expr_stmt|;
name|atexit
argument_list|(
name|remove_tempfiles_on_exit
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: prepare_tempfile_object called for active object"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tempfile
operator|->
name|on_list
condition|)
block|{
comment|/* Initialize *tempfile and add it to tempfile_list: */
name|tempfile
operator|->
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
name|tempfile
operator|->
name|fp
operator|=
name|NULL
expr_stmt|;
name|tempfile
operator|->
name|active
operator|=
literal|0
expr_stmt|;
name|tempfile
operator|->
name|owner
operator|=
literal|0
expr_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|next
operator|=
name|tempfile_list
expr_stmt|;
name|tempfile_list
operator|=
name|tempfile
expr_stmt|;
name|tempfile
operator|->
name|on_list
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|tempfile
operator|->
name|filename
operator|.
name|len
condition|)
block|{
comment|/* This shouldn't happen, but better safe than sorry. */
name|die
argument_list|(
literal|"BUG: prepare_tempfile_object called for improperly-reset object"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* Make sure errno contains a meaningful value on error */
end_comment
begin_function
DECL|function|create_tempfile
name|int
name|create_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|prepare_tempfile_object
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|strbuf_add_absolute_path
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|fd
operator|=
name|open
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|,
name|O_RDWR
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
name|tempfile
operator|->
name|fd
operator|<
literal|0
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|tempfile
operator|->
name|owner
operator|=
name|getpid
argument_list|()
expr_stmt|;
name|tempfile
operator|->
name|active
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|adjust_shared_perm
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|)
condition|)
block|{
name|int
name|save_errno
init|=
name|errno
decl_stmt|;
name|error
argument_list|(
literal|"cannot fix permission bits on %s"
argument_list|,
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|)
expr_stmt|;
name|delete_tempfile
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|errno
operator|=
name|save_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|tempfile
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|mks_tempfile_sm
name|int
name|mks_tempfile_sm
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|,
name|int
name|suffixlen
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|prepare_tempfile_object
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|strbuf_add_absolute_path
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|,
name|template
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|fd
operator|=
name|git_mkstemps_mode
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|,
name|suffixlen
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|tempfile
operator|->
name|fd
operator|<
literal|0
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|tempfile
operator|->
name|owner
operator|=
name|getpid
argument_list|()
expr_stmt|;
name|tempfile
operator|->
name|active
operator|=
literal|1
expr_stmt|;
return|return
name|tempfile
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|mks_tempfile_tsm
name|int
name|mks_tempfile_tsm
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|,
name|int
name|suffixlen
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tmpdir
decl_stmt|;
name|prepare_tempfile_object
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|tmpdir
operator|=
name|getenv
argument_list|(
literal|"TMPDIR"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tmpdir
condition|)
name|tmpdir
operator|=
literal|"/tmp"
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|,
literal|"%s/%s"
argument_list|,
name|tmpdir
argument_list|,
name|template
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|fd
operator|=
name|git_mkstemps_mode
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|,
name|suffixlen
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|tempfile
operator|->
name|fd
operator|<
literal|0
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|tempfile
operator|->
name|owner
operator|=
name|getpid
argument_list|()
expr_stmt|;
name|tempfile
operator|->
name|active
operator|=
literal|1
expr_stmt|;
return|return
name|tempfile
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|xmks_tempfile_m
name|int
name|xmks_tempfile_m
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|template
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|struct
name|strbuf
name|full_template
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_add_absolute_path
argument_list|(
operator|&
name|full_template
argument_list|,
name|template
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mks_tempfile_m
argument_list|(
name|tempfile
argument_list|,
name|full_template
operator|.
name|buf
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"Unable to create temporary file '%s'"
argument_list|,
name|full_template
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|full_template
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|fdopen_tempfile
name|FILE
modifier|*
name|fdopen_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|mode
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: fdopen_tempfile() called for inactive object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|tempfile
operator|->
name|fp
condition|)
name|die
argument_list|(
literal|"BUG: fdopen_tempfile() called for open object"
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|fp
operator|=
name|fdopen
argument_list|(
name|tempfile
operator|->
name|fd
argument_list|,
name|mode
argument_list|)
expr_stmt|;
return|return
name|tempfile
operator|->
name|fp
return|;
block|}
end_function
begin_function
DECL|function|get_tempfile_path
specifier|const
name|char
modifier|*
name|get_tempfile_path
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: get_tempfile_path() called for inactive object"
argument_list|)
expr_stmt|;
return|return
name|tempfile
operator|->
name|filename
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|get_tempfile_fd
name|int
name|get_tempfile_fd
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: get_tempfile_fd() called for inactive object"
argument_list|)
expr_stmt|;
return|return
name|tempfile
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|get_tempfile_fp
name|FILE
modifier|*
name|get_tempfile_fp
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: get_tempfile_fp() called for inactive object"
argument_list|)
expr_stmt|;
return|return
name|tempfile
operator|->
name|fp
return|;
block|}
end_function
begin_function
DECL|function|close_tempfile
name|int
name|close_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
name|int
name|fd
init|=
name|tempfile
operator|->
name|fd
decl_stmt|;
name|FILE
modifier|*
name|fp
init|=
name|tempfile
operator|->
name|fp
decl_stmt|;
name|int
name|err
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|tempfile
operator|->
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|fp
condition|)
block|{
name|tempfile
operator|->
name|fp
operator|=
name|NULL
expr_stmt|;
comment|/* 		 * Note: no short-circuiting here; we want to fclose() 		 * in any case! 		 */
name|err
operator|=
name|ferror
argument_list|(
name|fp
argument_list|)
operator||
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|err
operator|=
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|err
condition|)
block|{
name|int
name|save_errno
init|=
name|errno
decl_stmt|;
name|delete_tempfile
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|errno
operator|=
name|save_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|reopen_tempfile
name|int
name|reopen_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
literal|0
operator|<=
name|tempfile
operator|->
name|fd
condition|)
name|die
argument_list|(
literal|"BUG: reopen_tempfile called for an open object"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: reopen_tempfile called for an inactive object"
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|fd
operator|=
name|open
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|,
name|O_WRONLY
argument_list|)
expr_stmt|;
return|return
name|tempfile
operator|->
name|fd
return|;
block|}
end_function
begin_function
DECL|function|rename_tempfile
name|int
name|rename_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
name|die
argument_list|(
literal|"BUG: rename_tempfile called for inactive object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|close_tempfile
argument_list|(
name|tempfile
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|rename
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|,
name|path
argument_list|)
condition|)
block|{
name|int
name|save_errno
init|=
name|errno
decl_stmt|;
name|delete_tempfile
argument_list|(
name|tempfile
argument_list|)
expr_stmt|;
name|errno
operator|=
name|save_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|tempfile
operator|->
name|active
operator|=
literal|0
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|delete_tempfile
name|void
name|delete_tempfile
parameter_list|(
name|struct
name|tempfile
modifier|*
name|tempfile
parameter_list|)
block|{
if|if
condition|(
operator|!
name|tempfile
operator|->
name|active
condition|)
return|return;
if|if
condition|(
operator|!
name|close_tempfile
argument_list|(
name|tempfile
argument_list|)
condition|)
block|{
name|unlink_or_warn
argument_list|(
name|tempfile
operator|->
name|filename
operator|.
name|buf
argument_list|)
expr_stmt|;
name|tempfile
operator|->
name|active
operator|=
literal|0
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|tempfile
operator|->
name|filename
argument_list|)
expr_stmt|;
block|}
block|}
end_function
end_unit
