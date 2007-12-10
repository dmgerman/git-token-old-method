begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"path-list.h"
end_include
begin_include
include|#
directive|include
file|"mailmap.h"
end_include
begin_function
DECL|function|read_mailmap
name|int
name|read_mailmap
parameter_list|(
name|struct
name|path_list
modifier|*
name|map
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|char
modifier|*
modifier|*
name|repo_abbrev
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|FILE
modifier|*
name|f
init|=
name|fopen
argument_list|(
name|filename
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
if|if
condition|(
name|f
operator|==
name|NULL
condition|)
return|return
literal|1
return|;
while|while
condition|(
name|fgets
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|f
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|char
modifier|*
name|end_of_name
decl_stmt|,
modifier|*
name|left_bracket
decl_stmt|,
modifier|*
name|right_bracket
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|,
modifier|*
name|email
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|buffer
index|[
literal|0
index|]
operator|==
literal|'#'
condition|)
block|{
specifier|static
specifier|const
name|char
name|abbrev
index|[]
init|=
literal|"# repo-abbrev:"
decl_stmt|;
name|int
name|abblen
init|=
sizeof|sizeof
argument_list|(
name|abbrev
argument_list|)
operator|-
literal|1
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buffer
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|repo_abbrev
condition|)
continue|continue;
if|if
condition|(
name|len
operator|&&
name|buffer
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buffer
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|buffer
argument_list|,
name|abbrev
argument_list|,
name|abblen
argument_list|)
condition|)
block|{
name|char
modifier|*
name|cp
decl_stmt|;
if|if
condition|(
name|repo_abbrev
condition|)
name|free
argument_list|(
operator|*
name|repo_abbrev
argument_list|)
expr_stmt|;
operator|*
name|repo_abbrev
operator|=
name|xmalloc
argument_list|(
name|len
argument_list|)
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|buffer
operator|+
name|abblen
init|;
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|;
name|cp
operator|++
control|)
empty_stmt|;
comment|/* nothing */
name|strcpy
argument_list|(
operator|*
name|repo_abbrev
argument_list|,
name|cp
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
if|if
condition|(
operator|(
name|left_bracket
operator|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|'<'
argument_list|)
operator|)
operator|==
name|NULL
condition|)
continue|continue;
if|if
condition|(
operator|(
name|right_bracket
operator|=
name|strchr
argument_list|(
name|left_bracket
operator|+
literal|1
argument_list|,
literal|'>'
argument_list|)
operator|)
operator|==
name|NULL
condition|)
continue|continue;
if|if
condition|(
name|right_bracket
operator|==
name|left_bracket
operator|+
literal|1
condition|)
continue|continue;
for|for
control|(
name|end_of_name
operator|=
name|left_bracket
init|;
name|end_of_name
operator|!=
name|buffer
operator|&&
name|isspace
argument_list|(
name|end_of_name
index|[
operator|-
literal|1
index|]
argument_list|)
condition|;
name|end_of_name
operator|--
control|)
empty_stmt|;
comment|/* keep on looking */
if|if
condition|(
name|end_of_name
operator|==
name|buffer
condition|)
continue|continue;
name|name
operator|=
name|xmalloc
argument_list|(
name|end_of_name
operator|-
name|buffer
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strlcpy
argument_list|(
name|name
argument_list|,
name|buffer
argument_list|,
name|end_of_name
operator|-
name|buffer
operator|+
literal|1
argument_list|)
expr_stmt|;
name|email
operator|=
name|xmalloc
argument_list|(
name|right_bracket
operator|-
name|left_bracket
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
name|right_bracket
operator|-
name|left_bracket
operator|-
literal|1
condition|;
name|i
operator|++
control|)
name|email
index|[
name|i
index|]
operator|=
name|tolower
argument_list|(
name|left_bracket
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
expr_stmt|;
name|email
index|[
name|right_bracket
operator|-
name|left_bracket
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
name|path_list_insert
argument_list|(
name|email
argument_list|,
name|map
argument_list|)
operator|->
name|util
operator|=
name|name
expr_stmt|;
block|}
name|fclose
argument_list|(
name|f
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|map_email
name|int
name|map_email
parameter_list|(
name|struct
name|path_list
modifier|*
name|map
parameter_list|,
specifier|const
name|char
modifier|*
name|email
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|int
name|maxlen
parameter_list|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|struct
name|path_list_item
modifier|*
name|item
decl_stmt|;
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|,
modifier|*
name|mailbuf
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* autocomplete common developers */
name|p
operator|=
name|strchr
argument_list|(
name|email
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|p
operator|-
name|email
operator|+
literal|1
operator|<
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
condition|)
name|mailbuf
operator|=
name|buf
expr_stmt|;
else|else
name|mailbuf
operator|=
name|xmalloc
argument_list|(
name|p
operator|-
name|email
operator|+
literal|1
argument_list|)
expr_stmt|;
comment|/* downcase the email address */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|p
operator|-
name|email
condition|;
name|i
operator|++
control|)
name|mailbuf
index|[
name|i
index|]
operator|=
name|tolower
argument_list|(
name|email
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|mailbuf
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
name|item
operator|=
name|path_list_lookup
argument_list|(
name|mailbuf
argument_list|,
name|map
argument_list|)
expr_stmt|;
if|if
condition|(
name|mailbuf
operator|!=
name|buf
condition|)
name|free
argument_list|(
name|mailbuf
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|!=
name|NULL
condition|)
block|{
specifier|const
name|char
modifier|*
name|realname
init|=
operator|(
specifier|const
name|char
operator|*
operator|)
name|item
operator|->
name|util
decl_stmt|;
name|strlcpy
argument_list|(
name|name
argument_list|,
name|realname
argument_list|,
name|maxlen
argument_list|)
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
end_unit
