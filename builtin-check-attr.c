begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_decl_stmt
DECL|variable|check_attr_usage
specifier|static
specifier|const
name|char
name|check_attr_usage
index|[]
init|=
literal|"git-check-attr attr... [--] pathname..."
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_check_attr
name|int
name|cmd_check_attr
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
name|git_attr_check
modifier|*
name|check
decl_stmt|;
name|int
name|cnt
decl_stmt|,
name|i
decl_stmt|,
name|doubledash
decl_stmt|;
name|doubledash
operator|=
operator|-
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|doubledash
operator|<
literal|0
operator|&&
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
name|doubledash
operator|=
name|i
expr_stmt|;
block|}
comment|/* If there is no double dash, we handle only one attribute */
if|if
condition|(
name|doubledash
operator|<
literal|0
condition|)
block|{
name|cnt
operator|=
literal|1
expr_stmt|;
name|doubledash
operator|=
literal|1
expr_stmt|;
block|}
else|else
name|cnt
operator|=
name|doubledash
operator|-
literal|1
expr_stmt|;
name|doubledash
operator|++
expr_stmt|;
if|if
condition|(
name|cnt
operator|<=
literal|0
operator|||
name|argc
operator|<
name|doubledash
condition|)
name|usage
argument_list|(
name|check_attr_usage
argument_list|)
expr_stmt|;
name|check
operator|=
name|xcalloc
argument_list|(
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|check
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|cnt
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|struct
name|git_attr
modifier|*
name|a
decl_stmt|;
name|name
operator|=
name|argv
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
name|a
operator|=
name|git_attr
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|a
condition|)
return|return
name|error
argument_list|(
literal|"%s: not a valid attribute name"
argument_list|,
name|name
argument_list|)
return|;
name|check
index|[
name|i
index|]
operator|.
name|attr
operator|=
name|a
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
name|doubledash
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|int
name|j
decl_stmt|;
if|if
condition|(
name|git_checkattr
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
name|cnt
argument_list|,
name|check
argument_list|)
condition|)
name|die
argument_list|(
literal|"git_checkattr died"
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|cnt
condition|;
name|j
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|value
init|=
name|check
index|[
name|j
index|]
operator|.
name|value
decl_stmt|;
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|value
argument_list|)
condition|)
name|value
operator|=
literal|"set"
expr_stmt|;
elseif|else
if|if
condition|(
name|ATTR_FALSE
argument_list|(
name|value
argument_list|)
condition|)
name|value
operator|=
literal|"unset"
expr_stmt|;
elseif|else
if|if
condition|(
name|ATTR_UNSET
argument_list|(
name|value
argument_list|)
condition|)
name|value
operator|=
literal|"unspecified"
expr_stmt|;
name|write_name_quoted
argument_list|(
literal|""
argument_list|,
literal|0
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|": %s: %s\n"
argument_list|,
name|argv
index|[
name|j
operator|+
literal|1
index|]
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
