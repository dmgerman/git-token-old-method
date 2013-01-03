begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|DEFAULT_EDITOR
end_ifndef
begin_define
DECL|macro|DEFAULT_EDITOR
define|#
directive|define
name|DEFAULT_EDITOR
value|"vi"
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|git_editor
specifier|const
name|char
modifier|*
name|git_editor
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|editor
init|=
name|getenv
argument_list|(
literal|"GIT_EDITOR"
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|terminal
init|=
name|getenv
argument_list|(
literal|"TERM"
argument_list|)
decl_stmt|;
name|int
name|terminal_is_dumb
init|=
operator|!
name|terminal
operator|||
operator|!
name|strcmp
argument_list|(
name|terminal
argument_list|,
literal|"dumb"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|editor
operator|&&
name|editor_program
condition|)
name|editor
operator|=
name|editor_program
expr_stmt|;
if|if
condition|(
operator|!
name|editor
operator|&&
operator|!
name|terminal_is_dumb
condition|)
name|editor
operator|=
name|getenv
argument_list|(
literal|"VISUAL"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|editor
condition|)
name|editor
operator|=
name|getenv
argument_list|(
literal|"EDITOR"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|editor
operator|&&
name|terminal_is_dumb
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|editor
condition|)
name|editor
operator|=
name|DEFAULT_EDITOR
expr_stmt|;
return|return
name|editor
return|;
block|}
end_function
begin_function
DECL|function|launch_editor
name|int
name|launch_editor
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buffer
parameter_list|,
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|env
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|editor
init|=
name|git_editor
argument_list|()
decl_stmt|;
if|if
condition|(
operator|!
name|editor
condition|)
return|return
name|error
argument_list|(
literal|"Terminal is dumb, but EDITOR unset"
argument_list|)
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|editor
argument_list|,
literal|":"
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|args
index|[]
init|=
block|{
name|editor
block|,
name|path
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|child_process
name|p
decl_stmt|;
name|int
name|ret
decl_stmt|,
name|sig
decl_stmt|;
name|memset
argument_list|(
operator|&
name|p
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|p
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|.
name|argv
operator|=
name|args
expr_stmt|;
name|p
operator|.
name|env
operator|=
name|env
expr_stmt|;
name|p
operator|.
name|use_shell
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|p
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"unable to start editor '%s'"
argument_list|,
name|editor
argument_list|)
return|;
name|sigchain_push
argument_list|(
name|SIGINT
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
name|sigchain_push
argument_list|(
name|SIGQUIT
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
name|ret
operator|=
name|finish_command
argument_list|(
operator|&
name|p
argument_list|)
expr_stmt|;
name|sig
operator|=
name|ret
operator|+
literal|128
expr_stmt|;
name|sigchain_pop
argument_list|(
name|SIGINT
argument_list|)
expr_stmt|;
name|sigchain_pop
argument_list|(
name|SIGQUIT
argument_list|)
expr_stmt|;
if|if
condition|(
name|sig
operator|==
name|SIGINT
operator|||
name|sig
operator|==
name|SIGQUIT
condition|)
name|raise
argument_list|(
name|sig
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|error
argument_list|(
literal|"There was a problem with the editor '%s'."
argument_list|,
name|editor
argument_list|)
return|;
block|}
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|strbuf_read_file
argument_list|(
name|buffer
argument_list|,
name|path
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"could not read file '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
