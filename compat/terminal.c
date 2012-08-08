begin_unit
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"compat/terminal.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_ifdef
ifdef|#
directive|ifdef
name|HAVE_DEV_TTY
end_ifdef
begin_decl_stmt
DECL|variable|term_fd
specifier|static
name|int
name|term_fd
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|old_term
specifier|static
name|struct
name|termios
name|old_term
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|restore_term
specifier|static
name|void
name|restore_term
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|term_fd
operator|<
literal|0
condition|)
return|return;
name|tcsetattr
argument_list|(
name|term_fd
argument_list|,
name|TCSAFLUSH
argument_list|,
operator|&
name|old_term
argument_list|)
expr_stmt|;
name|term_fd
operator|=
operator|-
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|restore_term_on_signal
specifier|static
name|void
name|restore_term_on_signal
parameter_list|(
name|int
name|sig
parameter_list|)
block|{
name|restore_term
argument_list|()
expr_stmt|;
name|sigchain_pop
argument_list|(
name|sig
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|sig
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_terminal_prompt
name|char
modifier|*
name|git_terminal_prompt
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|,
name|int
name|echo
parameter_list|)
block|{
specifier|static
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|r
decl_stmt|;
name|FILE
modifier|*
name|fh
decl_stmt|;
name|fh
operator|=
name|fopen
argument_list|(
literal|"/dev/tty"
argument_list|,
literal|"w+"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fh
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|echo
condition|)
block|{
name|struct
name|termios
name|t
decl_stmt|;
if|if
condition|(
name|tcgetattr
argument_list|(
name|fileno
argument_list|(
name|fh
argument_list|)
argument_list|,
operator|&
name|t
argument_list|)
operator|<
literal|0
condition|)
block|{
name|fclose
argument_list|(
name|fh
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|old_term
operator|=
name|t
expr_stmt|;
name|term_fd
operator|=
name|fileno
argument_list|(
name|fh
argument_list|)
expr_stmt|;
name|sigchain_push_common
argument_list|(
name|restore_term_on_signal
argument_list|)
expr_stmt|;
name|t
operator|.
name|c_lflag
operator|&=
operator|~
name|ECHO
expr_stmt|;
if|if
condition|(
name|tcsetattr
argument_list|(
name|fileno
argument_list|(
name|fh
argument_list|)
argument_list|,
name|TCSAFLUSH
argument_list|,
operator|&
name|t
argument_list|)
operator|<
literal|0
condition|)
block|{
name|term_fd
operator|=
operator|-
literal|1
expr_stmt|;
name|fclose
argument_list|(
name|fh
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
name|fputs
argument_list|(
name|prompt
argument_list|,
name|fh
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|fh
argument_list|)
expr_stmt|;
name|r
operator|=
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|fh
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|echo
condition|)
block|{
name|fseek
argument_list|(
name|fh
argument_list|,
name|SEEK_CUR
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|putc
argument_list|(
literal|'\n'
argument_list|,
name|fh
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|fh
argument_list|)
expr_stmt|;
block|}
name|restore_term
argument_list|()
expr_stmt|;
name|fclose
argument_list|(
name|fh
argument_list|)
expr_stmt|;
if|if
condition|(
name|r
operator|==
name|EOF
condition|)
return|return
name|NULL
return|;
return|return
name|buf
operator|.
name|buf
return|;
block|}
end_function
begin_else
else|#
directive|else
end_else
begin_function
DECL|function|git_terminal_prompt
name|char
modifier|*
name|git_terminal_prompt
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|,
name|int
name|echo
parameter_list|)
block|{
return|return
name|getpass
argument_list|(
name|prompt
argument_list|)
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
end_unit
