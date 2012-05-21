begin_unit
begin_comment
comment|/*  * ident.c  *  * create git identifier lines of the form "name<email> date"  *  * Copyright (C) 2005 Linus Torvalds  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_define
DECL|macro|MAX_GITNAME
define|#
directive|define
name|MAX_GITNAME
value|(1000)
end_define
begin_decl_stmt
DECL|variable|git_default_name
specifier|static
name|char
name|git_default_name
index|[
name|MAX_GITNAME
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_default_email
specifier|static
name|char
name|git_default_email
index|[
name|MAX_GITNAME
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_default_date
specifier|static
name|char
name|git_default_date
index|[
literal|50
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|user_ident_explicitly_given
name|int
name|user_ident_explicitly_given
decl_stmt|;
end_decl_stmt
begin_ifdef
ifdef|#
directive|ifdef
name|NO_GECOS_IN_PWENT
end_ifdef
begin_define
DECL|macro|get_gecos
define|#
directive|define
name|get_gecos
parameter_list|(
name|ignored
parameter_list|)
value|"&"
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|get_gecos
define|#
directive|define
name|get_gecos
parameter_list|(
name|struct_passwd
parameter_list|)
value|((struct_passwd)->pw_gecos)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|copy_gecos
specifier|static
name|void
name|copy_gecos
parameter_list|(
specifier|const
name|struct
name|passwd
modifier|*
name|w
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|size_t
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
name|size_t
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
name|get_gecos
argument_list|(
name|w
argument_list|)
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
name|len
operator|+=
name|nlen
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
DECL|function|add_mailname_host
specifier|static
name|int
name|add_mailname_host
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|FILE
modifier|*
name|mailname
decl_stmt|;
name|mailname
operator|=
name|fopen
argument_list|(
literal|"/etc/mailname"
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mailname
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|warning
argument_list|(
literal|"cannot open /etc/mailname: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|fgets
argument_list|(
name|buf
argument_list|,
name|len
argument_list|,
name|mailname
argument_list|)
condition|)
block|{
if|if
condition|(
name|ferror
argument_list|(
name|mailname
argument_list|)
condition|)
name|warning
argument_list|(
literal|"cannot read /etc/mailname: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|mailname
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* success! */
name|fclose
argument_list|(
name|mailname
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|buf
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|buf
index|[
name|len
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_domainname
specifier|static
name|void
name|add_domainname
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|struct
name|hostent
modifier|*
name|he
decl_stmt|;
name|size_t
name|namelen
decl_stmt|;
specifier|const
name|char
modifier|*
name|domainname
decl_stmt|;
if|if
condition|(
name|gethostname
argument_list|(
name|buf
argument_list|,
name|len
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"cannot get host name: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|strlcpy
argument_list|(
name|buf
argument_list|,
literal|"(none)"
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return;
block|}
name|namelen
operator|=
name|strlen
argument_list|(
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|memchr
argument_list|(
name|buf
argument_list|,
literal|'.'
argument_list|,
name|namelen
argument_list|)
condition|)
return|return;
name|he
operator|=
name|gethostbyname
argument_list|(
name|buf
argument_list|)
expr_stmt|;
name|buf
index|[
name|namelen
operator|++
index|]
operator|=
literal|'.'
expr_stmt|;
name|buf
operator|+=
name|namelen
expr_stmt|;
name|len
operator|-=
name|namelen
expr_stmt|;
if|if
condition|(
name|he
operator|&&
operator|(
name|domainname
operator|=
name|strchr
argument_list|(
name|he
operator|->
name|h_name
argument_list|,
literal|'.'
argument_list|)
operator|)
condition|)
name|strlcpy
argument_list|(
name|buf
argument_list|,
name|domainname
operator|+
literal|1
argument_list|,
name|len
argument_list|)
expr_stmt|;
else|else
name|strlcpy
argument_list|(
name|buf
argument_list|,
literal|"(none)"
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|copy_email
specifier|static
name|void
name|copy_email
parameter_list|(
specifier|const
name|struct
name|passwd
modifier|*
name|pw
parameter_list|)
block|{
comment|/* 	 * Make up a fake email address 	 * (name + '@' + hostname [+ '.' + domainname]) 	 */
name|size_t
name|len
init|=
name|strlen
argument_list|(
name|pw
operator|->
name|pw_name
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|>
sizeof|sizeof
argument_list|(
name|git_default_email
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
name|git_default_email
argument_list|,
name|pw
operator|->
name|pw_name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|git_default_email
index|[
name|len
operator|++
index|]
operator|=
literal|'@'
expr_stmt|;
if|if
condition|(
operator|!
name|add_mailname_host
argument_list|(
name|git_default_email
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_email
argument_list|)
operator|-
name|len
argument_list|)
condition|)
return|return;
comment|/* read from "/etc/mailname" (Debian) */
name|add_domainname
argument_list|(
name|git_default_email
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_email
argument_list|)
operator|-
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ident_default_name
specifier|const
name|char
modifier|*
name|ident_default_name
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_name
index|[
literal|0
index|]
condition|)
block|{
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
name|copy_gecos
argument_list|(
name|pw
argument_list|,
name|git_default_name
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_name
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
name|git_default_name
return|;
block|}
end_function
begin_function
DECL|function|ident_default_email
specifier|const
name|char
modifier|*
name|ident_default_email
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_email
index|[
literal|0
index|]
condition|)
block|{
specifier|const
name|char
modifier|*
name|email
init|=
name|getenv
argument_list|(
literal|"EMAIL"
argument_list|)
decl_stmt|;
if|if
condition|(
name|email
operator|&&
name|email
index|[
literal|0
index|]
condition|)
block|{
name|strlcpy
argument_list|(
name|git_default_email
argument_list|,
name|email
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_email
argument_list|)
argument_list|)
expr_stmt|;
name|user_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
block|}
else|else
block|{
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
name|copy_email
argument_list|(
name|pw
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|git_default_email
return|;
block|}
end_function
begin_function
DECL|function|ident_default_date
specifier|const
name|char
modifier|*
name|ident_default_date
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_date
index|[
literal|0
index|]
condition|)
name|datestamp
argument_list|(
name|git_default_date
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_date
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|git_default_date
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
name|size_t
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
name|size_t
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
return|return
name|c
operator|<=
literal|32
operator|||
name|c
operator|==
literal|'.'
operator|||
name|c
operator|==
literal|','
operator|||
name|c
operator|==
literal|':'
operator|||
name|c
operator|==
literal|';'
operator|||
name|c
operator|==
literal|'<'
operator|||
name|c
operator|==
literal|'>'
operator|||
name|c
operator|==
literal|'"'
operator|||
name|c
operator|==
literal|'\\'
operator|||
name|c
operator|==
literal|'\''
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
name|size_t
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
name|size_t
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
comment|/* 	 * Copy the rest to the buffer, but avoid the special 	 * characters '\n' '<' and '>' that act as delimiters on 	 * an identification line 	 */
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
begin_comment
comment|/*  * Reverse of fmt_ident(); given an ident line, split the fields  * to allow the caller to parse it.  * Signal a success by returning 0, but date/tz fields of the result  * can still be NULL if the input line only has the name/email part  * (e.g. reading from a reflog entry).  */
end_comment
begin_function
DECL|function|split_ident_line
name|int
name|split_ident_line
parameter_list|(
name|struct
name|ident_split
modifier|*
name|split
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|size_t
name|span
decl_stmt|;
name|int
name|status
init|=
operator|-
literal|1
decl_stmt|;
name|memset
argument_list|(
name|split
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|split
argument_list|)
argument_list|)
expr_stmt|;
name|split
operator|->
name|name_begin
operator|=
name|line
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|line
init|;
operator|*
name|cp
operator|&&
name|cp
operator|<
name|line
operator|+
name|len
condition|;
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'<'
condition|)
block|{
name|split
operator|->
name|mail_begin
operator|=
name|cp
operator|+
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|mail_begin
condition|)
return|return
name|status
return|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|mail_begin
operator|-
literal|2
init|;
name|line
operator|<
name|cp
condition|;
name|cp
operator|--
control|)
if|if
condition|(
operator|!
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|)
block|{
name|split
operator|->
name|name_end
operator|=
name|cp
operator|+
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|name_end
condition|)
return|return
name|status
return|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|mail_begin
init|;
name|cp
operator|<
name|line
operator|+
name|len
condition|;
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'>'
condition|)
block|{
name|split
operator|->
name|mail_end
operator|=
name|cp
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|mail_end
condition|)
return|return
name|status
return|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|mail_end
operator|+
literal|1
init|;
name|cp
operator|<
name|line
operator|+
name|len
operator|&&
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
if|if
condition|(
name|line
operator|+
name|len
operator|<=
name|cp
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|date_begin
operator|=
name|cp
expr_stmt|;
name|span
operator|=
name|strspn
argument_list|(
name|cp
argument_list|,
literal|"0123456789"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|span
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|date_end
operator|=
name|split
operator|->
name|date_begin
operator|+
name|span
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|date_end
init|;
name|cp
operator|<
name|line
operator|+
name|len
operator|&&
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
if|if
condition|(
name|line
operator|+
name|len
operator|<=
name|cp
operator|||
operator|(
operator|*
name|cp
operator|!=
literal|'+'
operator|&&
operator|*
name|cp
operator|!=
literal|'-'
operator|)
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|tz_begin
operator|=
name|cp
expr_stmt|;
name|span
operator|=
name|strspn
argument_list|(
name|cp
operator|+
literal|1
argument_list|,
literal|"0123456789"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|span
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|tz_end
operator|=
name|split
operator|->
name|tz_begin
operator|+
literal|1
operator|+
name|span
expr_stmt|;
return|return
literal|0
return|;
name|person_only
label|:
name|split
operator|->
name|date_begin
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|date_end
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|tz_begin
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|tz_end
operator|=
name|NULL
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|env_hint
specifier|static
specifier|const
name|char
modifier|*
name|env_hint
init|=
literal|"\n"
literal|"*** Please tell me who you are.\n"
literal|"\n"
literal|"Run\n"
literal|"\n"
literal|"  git config --global user.email \"you@example.com\"\n"
literal|"  git config --global user.name \"Your Name\"\n"
literal|"\n"
literal|"to set your account\'s default identity.\n"
literal|"Omit --global to set the identity only in this repository.\n"
literal|"\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fmt_ident
specifier|const
name|char
modifier|*
name|fmt_ident
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
parameter_list|,
name|int
name|flag
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
name|int
name|error_on_no_name
init|=
operator|(
name|flag
operator|&
name|IDENT_ERROR_ON_NO_NAME
operator|)
decl_stmt|;
name|int
name|name_addr_only
init|=
operator|(
name|flag
operator|&
name|IDENT_NO_DATE
operator|)
decl_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
name|name
operator|=
name|ident_default_name
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|email
condition|)
name|email
operator|=
name|ident_default_email
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|name
condition|)
block|{
name|struct
name|passwd
modifier|*
name|pw
decl_stmt|;
if|if
condition|(
name|error_on_no_name
condition|)
block|{
if|if
condition|(
name|name
operator|==
name|git_default_name
condition|)
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"empty ident %s<%s> not allowed"
argument_list|,
name|name
argument_list|,
name|email
argument_list|)
expr_stmt|;
block|}
name|pw
operator|=
name|getpwuid
argument_list|(
name|getuid
argument_list|()
argument_list|)
expr_stmt|;
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
name|strlcpy
argument_list|(
name|git_default_name
argument_list|,
name|pw
operator|->
name|pw_name
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_name
argument_list|)
argument_list|)
expr_stmt|;
name|name
operator|=
name|git_default_name
expr_stmt|;
block|}
name|strcpy
argument_list|(
name|date
argument_list|,
name|ident_default_date
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|name_addr_only
operator|&&
name|date_str
operator|&&
name|date_str
index|[
literal|0
index|]
condition|)
block|{
if|if
condition|(
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
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"invalid date format: %s"
argument_list|,
name|date_str
argument_list|)
expr_stmt|;
block|}
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
if|if
condition|(
operator|!
name|name_addr_only
condition|)
block|{
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
block|}
else|else
block|{
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
literal|">"
argument_list|)
expr_stmt|;
block|}
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
DECL|function|fmt_name
specifier|const
name|char
modifier|*
name|fmt_name
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
parameter_list|)
block|{
return|return
name|fmt_ident
argument_list|(
name|name
argument_list|,
name|email
argument_list|,
name|NULL
argument_list|,
name|IDENT_ERROR_ON_NO_NAME
operator||
name|IDENT_NO_DATE
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_author_info
specifier|const
name|char
modifier|*
name|git_author_info
parameter_list|(
name|int
name|flag
parameter_list|)
block|{
return|return
name|fmt_ident
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
argument_list|,
name|flag
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_committer_info
specifier|const
name|char
modifier|*
name|git_committer_info
parameter_list|(
name|int
name|flag
parameter_list|)
block|{
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_NAME"
argument_list|)
condition|)
name|user_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_EMAIL"
argument_list|)
condition|)
name|user_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
return|return
name|fmt_ident
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
argument_list|,
name|flag
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|user_ident_sufficiently_given
name|int
name|user_ident_sufficiently_given
parameter_list|(
name|void
parameter_list|)
block|{
ifndef|#
directive|ifndef
name|WINDOWS
return|return
operator|(
name|user_ident_explicitly_given
operator|&
name|IDENT_MAIL_GIVEN
operator|)
return|;
else|#
directive|else
return|return
operator|(
name|user_ident_explicitly_given
operator|==
name|IDENT_ALL_GIVEN
operator|)
return|;
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|git_ident_config
name|int
name|git_ident_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.name"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|strlcpy
argument_list|(
name|git_default_name
argument_list|,
name|value
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_name
argument_list|)
argument_list|)
expr_stmt|;
name|user_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.email"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|strlcpy
argument_list|(
name|git_default_email
argument_list|,
name|value
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_email
argument_list|)
argument_list|)
expr_stmt|;
name|user_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
