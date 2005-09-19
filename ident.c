begin_unit
begin_comment
comment|/*  * ident.c  *  * create git identifier lines of the form "name<email> date"  *  * Copyright (C) 2005 Linus Torvalds  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<pwd.h>
end_include
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_include
include|#
directive|include
file|<ctype.h>
end_include
begin_decl_stmt
DECL|variable|real_email
specifier|static
name|char
name|real_email
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|real_name
specifier|static
name|char
name|real_name
index|[
literal|1000
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|real_date
specifier|static
name|char
name|real_date
index|[
literal|50
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|copy_gecos
specifier|static
name|void
name|copy_gecos
parameter_list|(
name|struct
name|passwd
modifier|*
name|w
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|int
name|sz
parameter_list|)
block|{
name|char
modifier|*
name|src
decl_stmt|,
modifier|*
name|dst
decl_stmt|;
name|int
name|len
decl_stmt|,
name|nlen
decl_stmt|;
name|nlen
operator|=
name|strlen
argument_list|(
name|w
operator|->
name|pw_name
argument_list|)
expr_stmt|;
comment|/* Traditionally GECOS field had office phone numbers etc, separated 	 * with commas.  Also& stands for capitalized form of the login name. 	 */
for|for
control|(
name|len
operator|=
literal|0
operator|,
name|dst
operator|=
name|name
operator|,
name|src
operator|=
name|w
operator|->
name|pw_gecos
init|;
name|len
operator|<
name|sz
condition|;
name|src
operator|++
control|)
block|{
name|int
name|ch
init|=
operator|*
name|src
decl_stmt|;
if|if
condition|(
name|ch
operator|!=
literal|'&'
condition|)
block|{
operator|*
name|dst
operator|++
operator|=
name|ch
expr_stmt|;
if|if
condition|(
name|ch
operator|==
literal|0
operator|||
name|ch
operator|==
literal|','
condition|)
break|break;
name|len
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|len
operator|+
name|nlen
operator|<
name|sz
condition|)
block|{
comment|/* Sorry, Mr. McDonald... */
operator|*
name|dst
operator|++
operator|=
name|toupper
argument_list|(
operator|*
name|w
operator|->
name|pw_name
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|dst
argument_list|,
name|w
operator|->
name|pw_name
operator|+
literal|1
argument_list|,
name|nlen
operator|-
literal|1
argument_list|)
expr_stmt|;
name|dst
operator|+=
name|nlen
operator|-
literal|1
expr_stmt|;
block|}
block|}
if|if
condition|(
name|len
operator|<
name|sz
condition|)
name|name
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
else|else
name|die
argument_list|(
literal|"Your parents must have hated you!"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|setup_ident
name|int
name|setup_ident
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|struct
name|passwd
modifier|*
name|pw
init|=
name|getpwuid
argument_list|(
name|getuid
argument_list|()
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|pw
condition|)
name|die
argument_list|(
literal|"You don't exist. Go away!"
argument_list|)
expr_stmt|;
comment|/* Get the name ("gecos") */
name|copy_gecos
argument_list|(
name|pw
argument_list|,
name|real_name
argument_list|,
sizeof|sizeof
argument_list|(
name|real_name
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Make up a fake email address (name + '@' + hostname [+ '.' + domainname]) */
name|len
operator|=
name|strlen
argument_list|(
name|pw
operator|->
name|pw_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
sizeof|sizeof
argument_list|(
name|real_email
argument_list|)
operator|/
literal|2
condition|)
name|die
argument_list|(
literal|"Your sysadmin must hate you!"
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|real_email
argument_list|,
name|pw
operator|->
name|pw_name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|real_email
index|[
name|len
operator|++
index|]
operator|=
literal|'@'
expr_stmt|;
name|gethostname
argument_list|(
name|real_email
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|real_email
argument_list|)
operator|-
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strchr
argument_list|(
name|real_email
operator|+
name|len
argument_list|,
literal|'.'
argument_list|)
condition|)
block|{
name|len
operator|=
name|strlen
argument_list|(
name|real_email
argument_list|)
expr_stmt|;
name|real_email
index|[
name|len
operator|++
index|]
operator|=
literal|'.'
expr_stmt|;
name|getdomainname
argument_list|(
name|real_email
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|real_email
argument_list|)
operator|-
name|len
argument_list|)
expr_stmt|;
block|}
comment|/* And set the default date */
name|datestamp
argument_list|(
name|real_date
argument_list|,
sizeof|sizeof
argument_list|(
name|real_date
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_raw
specifier|static
name|int
name|add_raw
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|size
parameter_list|,
name|int
name|offset
parameter_list|,
specifier|const
name|char
modifier|*
name|str
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|str
argument_list|)
decl_stmt|;
if|if
condition|(
name|offset
operator|+
name|len
operator|>
name|size
condition|)
return|return
name|size
return|;
name|memcpy
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
name|str
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
name|offset
operator|+
name|len
return|;
block|}
end_function
begin_function
DECL|function|crud
specifier|static
name|int
name|crud
parameter_list|(
name|unsigned
name|char
name|c
parameter_list|)
block|{
specifier|static
name|char
name|crud_array
index|[
literal|256
index|]
decl_stmt|;
specifier|static
name|int
name|crud_array_initialized
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|crud_array_initialized
condition|)
block|{
name|int
name|k
decl_stmt|;
for|for
control|(
name|k
operator|=
literal|0
init|;
name|k
operator|<=
literal|31
condition|;
operator|++
name|k
control|)
name|crud_array
index|[
name|k
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|' '
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|'.'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|','
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|':'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|';'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|'<'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|'>'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|'"'
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array
index|[
literal|'\''
index|]
operator|=
literal|1
expr_stmt|;
name|crud_array_initialized
operator|=
literal|1
expr_stmt|;
block|}
return|return
name|crud_array
index|[
name|c
index|]
return|;
block|}
end_function
begin_comment
comment|/*  * Copy over a string to the destination, but avoid special  * characters ('\n', '<' and '>') and remove crud at the end  */
end_comment
begin_function
DECL|function|copy
specifier|static
name|int
name|copy
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|size
parameter_list|,
name|int
name|offset
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|len
decl_stmt|;
name|unsigned
name|char
name|c
decl_stmt|;
comment|/* Remove crud from the beginning.. */
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|)
operator|!=
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|crud
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|src
operator|++
expr_stmt|;
block|}
comment|/* Remove crud from the end.. */
name|len
operator|=
name|strlen
argument_list|(
name|src
argument_list|)
expr_stmt|;
while|while
condition|(
name|len
operator|>
literal|0
condition|)
block|{
name|c
operator|=
name|src
index|[
name|len
operator|-
literal|1
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|crud
argument_list|(
name|c
argument_list|)
condition|)
break|break;
operator|--
name|len
expr_stmt|;
block|}
comment|/* 	 * Copy the rest to the buffer, but avoid the special 	 * characters '\n' '<' and '>' that act as delimeters on 	 * a identification line 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|len
condition|;
name|i
operator|++
control|)
block|{
name|c
operator|=
operator|*
name|src
operator|++
expr_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|'\n'
case|:
case|case
literal|'<'
case|:
case|case
literal|'>'
case|:
continue|continue;
block|}
if|if
condition|(
name|offset
operator|>=
name|size
condition|)
return|return
name|size
return|;
name|buf
index|[
name|offset
operator|++
index|]
operator|=
name|c
expr_stmt|;
block|}
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|get_ident
name|char
modifier|*
name|get_ident
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|email
parameter_list|,
specifier|const
name|char
modifier|*
name|date_str
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|1000
index|]
decl_stmt|;
name|char
name|date
index|[
literal|50
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
name|name
operator|=
name|real_name
expr_stmt|;
if|if
condition|(
operator|!
name|email
condition|)
name|email
operator|=
name|real_email
expr_stmt|;
name|strcpy
argument_list|(
name|date
argument_list|,
name|real_date
argument_list|)
expr_stmt|;
if|if
condition|(
name|date_str
condition|)
name|parse_date
argument_list|(
name|date_str
argument_list|,
name|date
argument_list|,
sizeof|sizeof
argument_list|(
name|date
argument_list|)
argument_list|)
expr_stmt|;
name|i
operator|=
name|copy
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|0
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|i
operator|=
name|add_raw
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|i
argument_list|,
literal|"<"
argument_list|)
expr_stmt|;
name|i
operator|=
name|copy
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|i
argument_list|,
name|email
argument_list|)
expr_stmt|;
name|i
operator|=
name|add_raw
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|i
argument_list|,
literal|"> "
argument_list|)
expr_stmt|;
name|i
operator|=
name|copy
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|i
argument_list|,
name|date
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|>=
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
condition|)
name|die
argument_list|(
literal|"Impossibly long personal identifier"
argument_list|)
expr_stmt|;
name|buffer
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|git_author_info
name|char
modifier|*
name|git_author_info
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|get_ident
argument_list|(
name|getenv
argument_list|(
literal|"GIT_AUTHOR_NAME"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_AUTHOR_EMAIL"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_AUTHOR_DATE"
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_committer_info
name|char
modifier|*
name|git_committer_info
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|get_ident
argument_list|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_NAME"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_COMMITTER_EMAIL"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_COMMITTER_DATE"
argument_list|)
argument_list|)
return|;
block|}
end_function
end_unit
