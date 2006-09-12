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
begin_decl_stmt
DECL|variable|git_symbolic_ref_usage
specifier|static
specifier|const
name|char
name|git_symbolic_ref_usage
index|[]
init|=
literal|"git-symbolic-ref name [ref]"
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
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|refs_heads_master
init|=
name|resolve_ref
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1
argument_list|,
literal|0
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
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|2
case|:
name|check_symref
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
break|break;
case|case
literal|3
case|:
name|create_symref
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|git_symbolic_ref_usage
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
