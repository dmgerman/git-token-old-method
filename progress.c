begin_unit
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"progress.h"
end_include
begin_decl_stmt
DECL|variable|progress_update
specifier|static
specifier|volatile
name|sig_atomic_t
name|progress_update
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|progress_interval
specifier|static
name|void
name|progress_interval
parameter_list|(
name|int
name|signum
parameter_list|)
block|{
name|progress_update
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|set_progress_signal
specifier|static
name|void
name|set_progress_signal
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|sigaction
name|sa
decl_stmt|;
name|struct
name|itimerval
name|v
decl_stmt|;
name|progress_update
operator|=
literal|0
expr_stmt|;
name|memset
argument_list|(
operator|&
name|sa
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|sa
argument_list|)
argument_list|)
expr_stmt|;
name|sa
operator|.
name|sa_handler
operator|=
name|progress_interval
expr_stmt|;
name|sigemptyset
argument_list|(
operator|&
name|sa
operator|.
name|sa_mask
argument_list|)
expr_stmt|;
name|sa
operator|.
name|sa_flags
operator|=
name|SA_RESTART
expr_stmt|;
name|sigaction
argument_list|(
name|SIGALRM
argument_list|,
operator|&
name|sa
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|v
operator|.
name|it_interval
operator|.
name|tv_sec
operator|=
literal|1
expr_stmt|;
name|v
operator|.
name|it_interval
operator|.
name|tv_usec
operator|=
literal|0
expr_stmt|;
name|v
operator|.
name|it_value
operator|=
name|v
operator|.
name|it_interval
expr_stmt|;
name|setitimer
argument_list|(
name|ITIMER_REAL
argument_list|,
operator|&
name|v
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|clear_progress_signal
specifier|static
name|void
name|clear_progress_signal
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|itimerval
name|v
init|=
block|{
block|{
literal|0
block|,}
block|,}
decl_stmt|;
name|setitimer
argument_list|(
name|ITIMER_REAL
argument_list|,
operator|&
name|v
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|signal
argument_list|(
name|SIGALRM
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
name|progress_update
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|display_progress
name|int
name|display_progress
parameter_list|(
name|struct
name|progress
modifier|*
name|progress
parameter_list|,
name|unsigned
name|n
parameter_list|)
block|{
if|if
condition|(
name|progress
operator|->
name|delay
condition|)
block|{
name|char
name|buf
index|[
literal|80
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|progress_update
operator|||
operator|--
name|progress
operator|->
name|delay
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|progress
operator|->
name|total
condition|)
block|{
name|unsigned
name|percent
init|=
name|n
operator|*
literal|100
operator|/
name|progress
operator|->
name|total
decl_stmt|;
if|if
condition|(
name|percent
operator|>
name|progress
operator|->
name|delayed_percent_treshold
condition|)
block|{
comment|/* inhibit this progress report entirely */
name|clear_progress_signal
argument_list|()
expr_stmt|;
name|progress
operator|->
name|delay
operator|=
operator|-
literal|1
expr_stmt|;
name|progress
operator|->
name|total
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
if|if
condition|(
name|snprintf
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|progress
operator|->
name|delayed_title
argument_list|,
name|progress
operator|->
name|total
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|progress
operator|->
name|total
condition|)
block|{
name|unsigned
name|percent
init|=
name|n
operator|*
literal|100
operator|/
name|progress
operator|->
name|total
decl_stmt|;
if|if
condition|(
name|percent
operator|!=
name|progress
operator|->
name|last_percent
operator|||
name|progress_update
condition|)
block|{
name|progress
operator|->
name|last_percent
operator|=
name|percent
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s%4u%% (%u/%u) done\r"
argument_list|,
name|progress
operator|->
name|prefix
argument_list|,
name|percent
argument_list|,
name|n
argument_list|,
name|progress
operator|->
name|total
argument_list|)
expr_stmt|;
name|progress_update
operator|=
literal|0
expr_stmt|;
name|progress
operator|->
name|need_lf
operator|=
literal|1
expr_stmt|;
return|return
literal|1
return|;
block|}
block|}
elseif|else
if|if
condition|(
name|progress_update
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s%u\r"
argument_list|,
name|progress
operator|->
name|prefix
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|progress_update
operator|=
literal|0
expr_stmt|;
name|progress
operator|->
name|need_lf
operator|=
literal|1
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|start_progress
name|void
name|start_progress
parameter_list|(
name|struct
name|progress
modifier|*
name|progress
parameter_list|,
specifier|const
name|char
modifier|*
name|title
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|unsigned
name|total
parameter_list|)
block|{
name|char
name|buf
index|[
literal|80
index|]
decl_stmt|;
name|progress
operator|->
name|prefix
operator|=
name|prefix
expr_stmt|;
name|progress
operator|->
name|total
operator|=
name|total
expr_stmt|;
name|progress
operator|->
name|last_percent
operator|=
operator|-
literal|1
expr_stmt|;
name|progress
operator|->
name|delay
operator|=
literal|0
expr_stmt|;
name|progress
operator|->
name|need_lf
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|snprintf
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|title
argument_list|,
name|total
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
name|set_progress_signal
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|start_progress_delay
name|void
name|start_progress_delay
parameter_list|(
name|struct
name|progress
modifier|*
name|progress
parameter_list|,
specifier|const
name|char
modifier|*
name|title
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|unsigned
name|total
parameter_list|,
name|unsigned
name|percent_treshold
parameter_list|,
name|unsigned
name|delay
parameter_list|)
block|{
name|progress
operator|->
name|prefix
operator|=
name|prefix
expr_stmt|;
name|progress
operator|->
name|total
operator|=
name|total
expr_stmt|;
name|progress
operator|->
name|last_percent
operator|=
operator|-
literal|1
expr_stmt|;
name|progress
operator|->
name|delayed_percent_treshold
operator|=
name|percent_treshold
expr_stmt|;
name|progress
operator|->
name|delayed_title
operator|=
name|title
expr_stmt|;
name|progress
operator|->
name|delay
operator|=
name|delay
expr_stmt|;
name|progress
operator|->
name|need_lf
operator|=
literal|0
expr_stmt|;
name|set_progress_signal
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|stop_progress
name|void
name|stop_progress
parameter_list|(
name|struct
name|progress
modifier|*
name|progress
parameter_list|)
block|{
name|clear_progress_signal
argument_list|()
expr_stmt|;
if|if
condition|(
name|progress
operator|->
name|need_lf
condition|)
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
