begin_unit
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
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"rerere.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"sequencer.h"
end_include
begin_comment
comment|/*  * This implements the builtins revert and cherry-pick.  *  * Copyright (c) 2007 Johannes E. Schindelin  *  * Based on git-revert.sh, which is  *  * Copyright (c) 2005 Linus Torvalds  * Copyright (c) 2005 Junio C Hamano  */
end_comment
begin_decl_stmt
DECL|variable|revert_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|revert_usage
index|[]
init|=
block|{
literal|"git revert [options]<commit-ish>"
block|,
literal|"git revert<subcommand>"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|cherry_pick_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|cherry_pick_usage
index|[]
init|=
block|{
literal|"git cherry-pick [options]<commit-ish>"
block|,
literal|"git cherry-pick<subcommand>"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|action_name
specifier|static
specifier|const
name|char
modifier|*
name|action_name
parameter_list|(
specifier|const
name|struct
name|replay_opts
modifier|*
name|opts
parameter_list|)
block|{
return|return
name|opts
operator|->
name|action
operator|==
name|REPLAY_REVERT
condition|?
literal|"revert"
else|:
literal|"cherry-pick"
return|;
block|}
end_function
begin_function
DECL|function|revert_or_cherry_pick_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|revert_or_cherry_pick_usage
parameter_list|(
name|struct
name|replay_opts
modifier|*
name|opts
parameter_list|)
block|{
return|return
name|opts
operator|->
name|action
operator|==
name|REPLAY_REVERT
condition|?
name|revert_usage
else|:
name|cherry_pick_usage
return|;
block|}
end_function
begin_function
DECL|function|option_parse_x
specifier|static
name|int
name|option_parse_x
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|replay_opts
modifier|*
modifier|*
name|opts_ptr
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|struct
name|replay_opts
modifier|*
name|opts
init|=
operator|*
name|opts_ptr
decl_stmt|;
if|if
condition|(
name|unset
condition|)
return|return
literal|0
return|;
name|ALLOC_GROW
argument_list|(
name|opts
operator|->
name|xopts
argument_list|,
name|opts
operator|->
name|xopts_nr
operator|+
literal|1
argument_list|,
name|opts
operator|->
name|xopts_alloc
argument_list|)
expr_stmt|;
name|opts
operator|->
name|xopts
index|[
name|opts
operator|->
name|xopts_nr
operator|++
index|]
operator|=
name|xstrdup
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|verify_opt_compatible
specifier|static
name|void
name|verify_opt_compatible
parameter_list|(
specifier|const
name|char
modifier|*
name|me
parameter_list|,
specifier|const
name|char
modifier|*
name|base_opt
parameter_list|,
modifier|...
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|this_opt
decl_stmt|;
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|base_opt
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|this_opt
operator|=
name|va_arg
argument_list|(
name|ap
argument_list|,
specifier|const
name|char
operator|*
argument_list|)
operator|)
condition|)
block|{
if|if
condition|(
name|va_arg
argument_list|(
name|ap
argument_list|,
name|int
argument_list|)
condition|)
break|break;
block|}
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
if|if
condition|(
name|this_opt
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"%s: %s cannot be used with %s"
argument_list|)
argument_list|,
name|me
argument_list|,
name|this_opt
argument_list|,
name|base_opt
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|verify_opt_mutually_compatible
specifier|static
name|void
name|verify_opt_mutually_compatible
parameter_list|(
specifier|const
name|char
modifier|*
name|me
parameter_list|,
modifier|...
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|opt1
decl_stmt|,
modifier|*
name|opt2
init|=
name|NULL
decl_stmt|;
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|me
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|opt1
operator|=
name|va_arg
argument_list|(
name|ap
argument_list|,
specifier|const
name|char
operator|*
argument_list|)
operator|)
condition|)
block|{
if|if
condition|(
name|va_arg
argument_list|(
name|ap
argument_list|,
name|int
argument_list|)
condition|)
break|break;
block|}
if|if
condition|(
name|opt1
condition|)
block|{
while|while
condition|(
operator|(
name|opt2
operator|=
name|va_arg
argument_list|(
name|ap
argument_list|,
specifier|const
name|char
operator|*
argument_list|)
operator|)
condition|)
block|{
if|if
condition|(
name|va_arg
argument_list|(
name|ap
argument_list|,
name|int
argument_list|)
condition|)
break|break;
block|}
block|}
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt1
operator|&&
name|opt2
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"%s: %s cannot be used with %s"
argument_list|)
argument_list|,
name|me
argument_list|,
name|opt1
argument_list|,
name|opt2
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_args
specifier|static
name|void
name|parse_args
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
name|replay_opts
modifier|*
name|opts
parameter_list|)
block|{
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|usage_str
init|=
name|revert_or_cherry_pick_usage
argument_list|(
name|opts
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|me
init|=
name|action_name
argument_list|(
name|opts
argument_list|)
decl_stmt|;
name|int
name|remove_state
init|=
literal|0
decl_stmt|;
name|int
name|contin
init|=
literal|0
decl_stmt|;
name|int
name|rollback
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"quit"
argument_list|,
operator|&
name|remove_state
argument_list|,
literal|"end revert or cherry-pick sequence"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"continue"
argument_list|,
operator|&
name|contin
argument_list|,
literal|"resume revert or cherry-pick sequence"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"abort"
argument_list|,
operator|&
name|rollback
argument_list|,
literal|"cancel revert or cherry-pick sequence"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'n'
argument_list|,
literal|"no-commit"
argument_list|,
operator|&
name|opts
operator|->
name|no_commit
argument_list|,
literal|"don't automatically commit"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'e'
argument_list|,
literal|"edit"
argument_list|,
operator|&
name|opts
operator|->
name|edit
argument_list|,
literal|"edit the commit message"
argument_list|)
block|,
name|OPT_NOOP_NOARG
argument_list|(
literal|'r'
argument_list|,
name|NULL
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'s'
argument_list|,
literal|"signoff"
argument_list|,
operator|&
name|opts
operator|->
name|signoff
argument_list|,
literal|"add Signed-off-by:"
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|'m'
argument_list|,
literal|"mainline"
argument_list|,
operator|&
name|opts
operator|->
name|mainline
argument_list|,
literal|"parent number"
argument_list|)
block|,
name|OPT_RERERE_AUTOUPDATE
argument_list|(
operator|&
name|opts
operator|->
name|allow_rerere_auto
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"strategy"
argument_list|,
operator|&
name|opts
operator|->
name|strategy
argument_list|,
literal|"strategy"
argument_list|,
literal|"merge strategy"
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|'X'
argument_list|,
literal|"strategy-option"
argument_list|,
operator|&
name|opts
argument_list|,
literal|"option"
argument_list|,
literal|"option for merge strategy"
argument_list|,
name|option_parse_x
argument_list|)
block|,
name|OPT_END
argument_list|()
block|,
name|OPT_END
argument_list|()
block|,
name|OPT_END
argument_list|()
block|,
name|OPT_END
argument_list|()
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
if|if
condition|(
name|opts
operator|->
name|action
operator|==
name|REPLAY_PICK
condition|)
block|{
name|struct
name|option
name|cp_extra
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'x'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|->
name|record_origin
argument_list|,
literal|"append commit name"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"ff"
argument_list|,
operator|&
name|opts
operator|->
name|allow_ff
argument_list|,
literal|"allow fast-forward"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"allow-empty"
argument_list|,
operator|&
name|opts
operator|->
name|allow_empty
argument_list|,
literal|"preserve initially empty commits"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"keep-redundant-commits"
argument_list|,
operator|&
name|opts
operator|->
name|keep_redundant_commits
argument_list|,
literal|"keep redundant, empty commits"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 		}
decl_stmt|;
if|if
condition|(
name|parse_options_concat
argument_list|(
name|options
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|options
argument_list|)
argument_list|,
name|cp_extra
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"program error"
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|NULL
argument_list|,
name|options
argument_list|,
name|usage_str
argument_list|,
name|PARSE_OPT_KEEP_ARGV0
operator||
name|PARSE_OPT_KEEP_UNKNOWN
argument_list|)
expr_stmt|;
comment|/* Check for incompatible subcommands */
name|verify_opt_mutually_compatible
argument_list|(
name|me
argument_list|,
literal|"--quit"
argument_list|,
name|remove_state
argument_list|,
literal|"--continue"
argument_list|,
name|contin
argument_list|,
literal|"--abort"
argument_list|,
name|rollback
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* implies allow_empty */
if|if
condition|(
name|opts
operator|->
name|keep_redundant_commits
condition|)
name|opts
operator|->
name|allow_empty
operator|=
literal|1
expr_stmt|;
comment|/* Set the subcommand */
if|if
condition|(
name|remove_state
condition|)
name|opts
operator|->
name|subcommand
operator|=
name|REPLAY_REMOVE_STATE
expr_stmt|;
elseif|else
if|if
condition|(
name|contin
condition|)
name|opts
operator|->
name|subcommand
operator|=
name|REPLAY_CONTINUE
expr_stmt|;
elseif|else
if|if
condition|(
name|rollback
condition|)
name|opts
operator|->
name|subcommand
operator|=
name|REPLAY_ROLLBACK
expr_stmt|;
else|else
name|opts
operator|->
name|subcommand
operator|=
name|REPLAY_NONE
expr_stmt|;
comment|/* Check for incompatible command line arguments */
if|if
condition|(
name|opts
operator|->
name|subcommand
operator|!=
name|REPLAY_NONE
condition|)
block|{
name|char
modifier|*
name|this_operation
decl_stmt|;
if|if
condition|(
name|opts
operator|->
name|subcommand
operator|==
name|REPLAY_REMOVE_STATE
condition|)
name|this_operation
operator|=
literal|"--quit"
expr_stmt|;
elseif|else
if|if
condition|(
name|opts
operator|->
name|subcommand
operator|==
name|REPLAY_CONTINUE
condition|)
name|this_operation
operator|=
literal|"--continue"
expr_stmt|;
else|else
block|{
name|assert
argument_list|(
name|opts
operator|->
name|subcommand
operator|==
name|REPLAY_ROLLBACK
argument_list|)
expr_stmt|;
name|this_operation
operator|=
literal|"--abort"
expr_stmt|;
block|}
name|verify_opt_compatible
argument_list|(
name|me
argument_list|,
name|this_operation
argument_list|,
literal|"--no-commit"
argument_list|,
name|opts
operator|->
name|no_commit
argument_list|,
literal|"--signoff"
argument_list|,
name|opts
operator|->
name|signoff
argument_list|,
literal|"--mainline"
argument_list|,
name|opts
operator|->
name|mainline
argument_list|,
literal|"--strategy"
argument_list|,
name|opts
operator|->
name|strategy
condition|?
literal|1
else|:
literal|0
argument_list|,
literal|"--strategy-option"
argument_list|,
name|opts
operator|->
name|xopts
condition|?
literal|1
else|:
literal|0
argument_list|,
literal|"-x"
argument_list|,
name|opts
operator|->
name|record_origin
argument_list|,
literal|"--ff"
argument_list|,
name|opts
operator|->
name|allow_ff
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|->
name|allow_ff
condition|)
name|verify_opt_compatible
argument_list|(
name|me
argument_list|,
literal|"--ff"
argument_list|,
literal|"--signoff"
argument_list|,
name|opts
operator|->
name|signoff
argument_list|,
literal|"--no-commit"
argument_list|,
name|opts
operator|->
name|no_commit
argument_list|,
literal|"-x"
argument_list|,
name|opts
operator|->
name|record_origin
argument_list|,
literal|"--edit"
argument_list|,
name|opts
operator|->
name|edit
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|subcommand
operator|!=
name|REPLAY_NONE
condition|)
block|{
name|opts
operator|->
name|revs
operator|=
name|NULL
expr_stmt|;
block|}
else|else
block|{
name|struct
name|setup_revision_opt
name|s_r_opt
decl_stmt|;
name|opts
operator|->
name|revs
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|opts
operator|->
name|revs
argument_list|)
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
name|opts
operator|->
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|opts
operator|->
name|revs
operator|->
name|no_walk
operator|=
name|REVISION_WALK_NO_WALK_UNSORTED
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
name|usage_with_options
argument_list|(
name|usage_str
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|s_r_opt
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|s_r_opt
argument_list|)
argument_list|)
expr_stmt|;
name|s_r_opt
operator|.
name|assume_dashdash
operator|=
literal|1
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|opts
operator|->
name|revs
argument_list|,
operator|&
name|s_r_opt
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage_with_options
argument_list|(
name|usage_str
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_revert
name|int
name|cmd_revert
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
name|replay_opts
name|opts
decl_stmt|;
name|int
name|res
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|isatty
argument_list|(
literal|0
argument_list|)
condition|)
name|opts
operator|.
name|edit
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|action
operator|=
name|REPLAY_REVERT
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|parse_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|opts
argument_list|)
expr_stmt|;
name|res
operator|=
name|sequencer_pick_revisions
argument_list|(
operator|&
name|opts
argument_list|)
expr_stmt|;
if|if
condition|(
name|res
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"revert failed"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|cmd_cherry_pick
name|int
name|cmd_cherry_pick
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
name|replay_opts
name|opts
decl_stmt|;
name|int
name|res
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|opts
operator|.
name|action
operator|=
name|REPLAY_PICK
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|parse_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|opts
argument_list|)
expr_stmt|;
name|res
operator|=
name|sequencer_pick_revisions
argument_list|(
operator|&
name|opts
argument_list|)
expr_stmt|;
if|if
condition|(
name|res
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"cherry-pick failed"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
end_unit
