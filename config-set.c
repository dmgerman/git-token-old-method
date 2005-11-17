begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|git_config_set_usage
specifier|static
specifier|const
name|char
name|git_config_set_usage
index|[]
init|=
literal|"git-config-set name [value [value_regex]] | --unset name [value_regex]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|setup_git_directory
argument_list|()
expr_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|2
case|:
return|return
name|git_config_set
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|NULL
argument_list|)
return|;
case|case
literal|3
case|:
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--unset"
argument_list|)
condition|)
return|return
name|git_config_set
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|NULL
argument_list|)
return|;
else|else
return|return
name|git_config_set
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
return|;
case|case
literal|4
case|:
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--unset"
argument_list|)
condition|)
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|NULL
argument_list|,
name|argv
index|[
literal|3
index|]
argument_list|)
return|;
else|else
return|return
name|git_config_set_multivar
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
argument_list|,
name|argv
index|[
literal|3
index|]
argument_list|)
return|;
default|default:
name|usage
argument_list|(
name|git_config_set_usage
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
