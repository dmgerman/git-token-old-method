begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_function
DECL|function|check_pipe
specifier|static
name|void
name|check_pipe
parameter_list|(
name|int
name|err
parameter_list|)
block|{
if|if
condition|(
name|err
operator|==
name|EPIPE
condition|)
block|{
if|if
condition|(
name|in_async
argument_list|()
condition|)
name|async_exit
argument_list|(
literal|141
argument_list|)
expr_stmt|;
name|signal
argument_list|(
name|SIGPIPE
argument_list|,
name|SIG_DFL
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|SIGPIPE
argument_list|)
expr_stmt|;
comment|/* Should never happen, but just in case... */
name|exit
argument_list|(
literal|141
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * Some cases use stdio, but want to flush after the write  * to get error handling (and to get better interactive  * behaviour - not buffering excessively).  *  * Of course, if the flush happened within the write itself,  * we've already lost the error code, and cannot report it any  * more. So we just ignore that case instead (and hope we get  * the right error code on the flush).  *  * If the file handle is stdout, and stdout is a file, then skip the  * flush entirely since it's not needed.  */
end_comment
begin_function
DECL|function|maybe_flush_or_die
name|void
name|maybe_flush_or_die
parameter_list|(
name|FILE
modifier|*
name|f
parameter_list|,
specifier|const
name|char
modifier|*
name|desc
parameter_list|)
block|{
specifier|static
name|int
name|skip_stdout_flush
init|=
operator|-
literal|1
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|char
modifier|*
name|cp
decl_stmt|;
if|if
condition|(
name|f
operator|==
name|stdout
condition|)
block|{
if|if
condition|(
name|skip_stdout_flush
operator|<
literal|0
condition|)
block|{
name|cp
operator|=
name|getenv
argument_list|(
literal|"GIT_FLUSH"
argument_list|)
expr_stmt|;
if|if
condition|(
name|cp
condition|)
name|skip_stdout_flush
operator|=
operator|(
name|atoi
argument_list|(
name|cp
argument_list|)
operator|==
literal|0
operator|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|(
name|fstat
argument_list|(
name|fileno
argument_list|(
name|stdout
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
operator|==
literal|0
operator|)
operator|&&
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
name|skip_stdout_flush
operator|=
literal|1
expr_stmt|;
else|else
name|skip_stdout_flush
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|skip_stdout_flush
operator|&&
operator|!
name|ferror
argument_list|(
name|f
argument_list|)
condition|)
return|return;
block|}
if|if
condition|(
name|fflush
argument_list|(
name|f
argument_list|)
condition|)
block|{
name|check_pipe
argument_list|(
name|errno
argument_list|)
expr_stmt|;
name|die_errno
argument_list|(
literal|"write failure on '%s'"
argument_list|,
name|desc
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|fprintf_or_die
name|void
name|fprintf_or_die
parameter_list|(
name|FILE
modifier|*
name|f
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|ap
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|ret
operator|=
name|vfprintf
argument_list|(
name|f
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
block|{
name|check_pipe
argument_list|(
name|errno
argument_list|)
expr_stmt|;
name|die_errno
argument_list|(
literal|"write error"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|fsync_or_die
name|void
name|fsync_or_die
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
block|{
if|if
condition|(
name|fsync
argument_list|(
name|fd
argument_list|)
operator|<
literal|0
condition|)
block|{
name|die_errno
argument_list|(
literal|"fsync error on '%s'"
argument_list|,
name|msg
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|write_or_die
name|void
name|write_or_die
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|count
parameter_list|)
block|{
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|count
argument_list|)
operator|<
literal|0
condition|)
block|{
name|check_pipe
argument_list|(
name|errno
argument_list|)
expr_stmt|;
name|die_errno
argument_list|(
literal|"write error"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|write_or_whine_pipe
name|int
name|write_or_whine_pipe
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|count
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
block|{
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|count
argument_list|)
operator|<
literal|0
condition|)
block|{
name|check_pipe
argument_list|(
name|errno
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: write error (%s)\n"
argument_list|,
name|msg
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
end_unit
