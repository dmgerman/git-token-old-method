begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|git_symbolic_ref_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_symbolic_ref_usage
index|[]
init|=
block|{
literal|"git-symbolic-ref [options] name [ref]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_symref
specifier|static
name|void
name|check_symref
parameter_list|(
specifier|const
name|char
modifier|*
name|HEAD
parameter_list|,
name|int
name|quiet
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|flag
decl_stmt|;
specifier|const
name|char
modifier|*
name|refs_heads_master
init|=
name|resolve_ref
argument_list|(
name|HEAD
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|,
operator|&
name|flag
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|refs_heads_master
condition|)
name|die
argument_list|(
literal|"No such ref: %s"
argument_list|,
name|HEAD
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
operator|(
name|flag
operator|&
name|REF_ISSYMREF
operator|)
condition|)
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|die
argument_list|(
literal|"ref %s is not a symbolic ref"
argument_list|,
name|HEAD
argument_list|)
expr_stmt|;
else|else
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
name|refs_heads_master
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_symbolic_ref
name|int
name|cmd_symbolic_ref
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
name|quiet
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|msg
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'m'
argument_list|,
name|NULL
argument_list|,
operator|&
name|msg
argument_list|,
literal|"reason"
argument_list|,
literal|"reason of the update"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|options
argument_list|,
name|git_symbolic_ref_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|msg
operator|&&
operator|!
operator|*
name|msg
condition|)
name|die
argument_list|(
literal|"Refusing to perform update with empty message"
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|1
case|:
name|check_symref
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|quiet
argument_list|)
expr_stmt|;
break|break;
case|case
literal|2
case|:
name|create_symref
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|,
name|msg
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage_with_options
argument_list|(
name|git_symbolic_ref_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
