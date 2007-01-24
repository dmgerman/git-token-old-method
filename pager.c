begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<sys/select.h>
end_include
begin_comment
comment|/*  * This is split up from the rest of git so that we might do  * something different on Windows, for example.  */
end_comment
begin_function
DECL|function|run_pager
specifier|static
name|void
name|run_pager
parameter_list|(
specifier|const
name|char
modifier|*
name|pager
parameter_list|)
block|{
comment|/* 	 * Work around bug in "less" by not starting it until we 	 * have real input 	 */
name|fd_set
name|in
decl_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|in
argument_list|)
expr_stmt|;
name|FD_SET
argument_list|(
literal|0
argument_list|,
operator|&
name|in
argument_list|)
expr_stmt|;
name|select
argument_list|(
literal|1
argument_list|,
operator|&
name|in
argument_list|,
name|NULL
argument_list|,
operator|&
name|in
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
name|pager
argument_list|,
name|pager
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|execl
argument_list|(
literal|"/bin/sh"
argument_list|,
literal|"sh"
argument_list|,
literal|"-c"
argument_list|,
name|pager
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|setup_pager
name|void
name|setup_pager
parameter_list|(
name|void
parameter_list|)
block|{
name|pid_t
name|pid
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|pager
init|=
name|getenv
argument_list|(
literal|"GIT_PAGER"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|isatty
argument_list|(
literal|1
argument_list|)
condition|)
return|return;
if|if
condition|(
operator|!
name|pager
condition|)
name|pager
operator|=
name|getenv
argument_list|(
literal|"PAGER"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pager
condition|)
name|pager
operator|=
literal|"less"
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
operator|*
name|pager
operator|||
operator|!
name|strcmp
argument_list|(
name|pager
argument_list|,
literal|"cat"
argument_list|)
condition|)
return|return;
name|pager_in_use
operator|=
literal|1
expr_stmt|;
comment|/* means we are emitting to terminal */
if|if
condition|(
name|pipe
argument_list|(
name|fd
argument_list|)
operator|<
literal|0
condition|)
return|return;
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
block|{
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
return|return;
block|}
comment|/* return in the child */
if|if
condition|(
operator|!
name|pid
condition|)
block|{
name|dup2
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|1
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
return|return;
block|}
comment|/* The original process turns into the PAGER */
name|dup2
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
literal|0
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
name|setenv
argument_list|(
literal|"LESS"
argument_list|,
literal|"FRSX"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|run_pager
argument_list|(
name|pager
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"unable to execute pager '%s'"
argument_list|,
name|pager
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|255
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
