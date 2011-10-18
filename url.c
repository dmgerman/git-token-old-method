begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"url.h"
end_include
begin_function
DECL|function|is_urlschemechar
name|int
name|is_urlschemechar
parameter_list|(
name|int
name|first_flag
parameter_list|,
name|int
name|ch
parameter_list|)
block|{
comment|/* 	 * The set of valid URL schemes, as per STD66 (RFC3986) is 	 * '[A-Za-z][A-Za-z0-9+.-]*'. But use sightly looser check 	 * of '[A-Za-z0-9][A-Za-z0-9+.-]*' because earlier version 	 * of check used '[A-Za-z0-9]+' so not to break any remote 	 * helpers. 	 */
name|int
name|alphanumeric
decl_stmt|,
name|special
decl_stmt|;
name|alphanumeric
operator|=
name|ch
operator|>
literal|0
operator|&&
name|isalnum
argument_list|(
name|ch
argument_list|)
expr_stmt|;
name|special
operator|=
name|ch
operator|==
literal|'+'
operator|||
name|ch
operator|==
literal|'-'
operator|||
name|ch
operator|==
literal|'.'
expr_stmt|;
return|return
name|alphanumeric
operator|||
operator|(
operator|!
name|first_flag
operator|&&
name|special
operator|)
return|;
block|}
end_function
begin_function
DECL|function|is_url
name|int
name|is_url
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|)
block|{
comment|/* Is "scheme" part reasonable? */
if|if
condition|(
operator|!
name|url
operator|||
operator|!
name|is_urlschemechar
argument_list|(
literal|1
argument_list|,
operator|*
name|url
operator|++
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|url
operator|&&
operator|*
name|url
operator|!=
literal|':'
condition|)
block|{
if|if
condition|(
operator|!
name|is_urlschemechar
argument_list|(
literal|0
argument_list|,
operator|*
name|url
operator|++
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
comment|/* We've seen "scheme"; we want colon-slash-slash */
return|return
operator|(
name|url
index|[
literal|0
index|]
operator|==
literal|':'
operator|&&
name|url
index|[
literal|1
index|]
operator|==
literal|'/'
operator|&&
name|url
index|[
literal|2
index|]
operator|==
literal|'/'
operator|)
return|;
block|}
end_function
begin_function
DECL|function|url_decode_char
specifier|static
name|int
name|url_decode_char
parameter_list|(
specifier|const
name|char
modifier|*
name|q
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|unsigned
name|char
name|val
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|2
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|q
operator|++
decl_stmt|;
name|val
operator|<<=
literal|4
expr_stmt|;
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
name|val
operator|+=
name|c
operator|-
literal|'0'
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
name|val
operator|+=
name|c
operator|-
literal|'a'
operator|+
literal|10
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
name|val
operator|+=
name|c
operator|-
literal|'A'
operator|+
literal|10
expr_stmt|;
else|else
return|return
operator|-
literal|1
return|;
block|}
return|return
name|val
return|;
block|}
end_function
begin_function
DECL|function|url_decode_internal
specifier|static
name|char
modifier|*
name|url_decode_internal
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|query
parameter_list|,
name|int
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|stop_at
parameter_list|,
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
name|int
name|decode_plus
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|q
init|=
operator|*
name|query
decl_stmt|;
while|while
condition|(
name|len
condition|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|q
decl_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
break|break;
if|if
condition|(
name|stop_at
operator|&&
name|strchr
argument_list|(
name|stop_at
argument_list|,
name|c
argument_list|)
condition|)
block|{
name|q
operator|++
expr_stmt|;
name|len
operator|--
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|c
operator|==
literal|'%'
condition|)
block|{
name|int
name|val
init|=
name|url_decode_char
argument_list|(
name|q
operator|+
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|val
condition|)
block|{
name|strbuf_addch
argument_list|(
name|out
argument_list|,
name|val
argument_list|)
expr_stmt|;
name|q
operator|+=
literal|3
expr_stmt|;
name|len
operator|-=
literal|3
expr_stmt|;
continue|continue;
block|}
block|}
if|if
condition|(
name|decode_plus
operator|&&
name|c
operator|==
literal|'+'
condition|)
name|strbuf_addch
argument_list|(
name|out
argument_list|,
literal|' '
argument_list|)
expr_stmt|;
else|else
name|strbuf_addch
argument_list|(
name|out
argument_list|,
name|c
argument_list|)
expr_stmt|;
name|q
operator|++
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
operator|*
name|query
operator|=
name|q
expr_stmt|;
return|return
name|strbuf_detach
argument_list|(
name|out
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|url_decode
name|char
modifier|*
name|url_decode
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|)
block|{
return|return
name|url_decode_mem
argument_list|(
name|url
argument_list|,
name|strlen
argument_list|(
name|url
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|url_decode_mem
name|char
modifier|*
name|url_decode_mem
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|struct
name|strbuf
name|out
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|colon
init|=
name|memchr
argument_list|(
name|url
argument_list|,
literal|':'
argument_list|,
name|len
argument_list|)
decl_stmt|;
comment|/* Skip protocol part if present */
if|if
condition|(
name|colon
operator|&&
name|url
operator|<
name|colon
condition|)
block|{
name|strbuf_add
argument_list|(
operator|&
name|out
argument_list|,
name|url
argument_list|,
name|colon
operator|-
name|url
argument_list|)
expr_stmt|;
name|len
operator|-=
name|colon
operator|-
name|url
expr_stmt|;
name|url
operator|=
name|colon
expr_stmt|;
block|}
return|return
name|url_decode_internal
argument_list|(
operator|&
name|url
argument_list|,
name|len
argument_list|,
name|NULL
argument_list|,
operator|&
name|out
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|url_decode_parameter_name
name|char
modifier|*
name|url_decode_parameter_name
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|query
parameter_list|)
block|{
name|struct
name|strbuf
name|out
init|=
name|STRBUF_INIT
decl_stmt|;
return|return
name|url_decode_internal
argument_list|(
name|query
argument_list|,
operator|-
literal|1
argument_list|,
literal|"&="
argument_list|,
operator|&
name|out
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|url_decode_parameter_value
name|char
modifier|*
name|url_decode_parameter_value
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|query
parameter_list|)
block|{
name|struct
name|strbuf
name|out
init|=
name|STRBUF_INIT
decl_stmt|;
return|return
name|url_decode_internal
argument_list|(
name|query
argument_list|,
operator|-
literal|1
argument_list|,
literal|"&"
argument_list|,
operator|&
name|out
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|end_url_with_slash
name|void
name|end_url_with_slash
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|)
block|{
name|strbuf_addstr
argument_list|(
name|buf
argument_list|,
name|url
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
operator|->
name|len
operator|&&
name|buf
operator|->
name|buf
index|[
name|buf
operator|->
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
name|strbuf_addstr
argument_list|(
name|buf
argument_list|,
literal|"/"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|str_end_url_with_slash
name|void
name|str_end_url_with_slash
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|char
modifier|*
modifier|*
name|dest
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|end_url_with_slash
argument_list|(
operator|&
name|buf
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|free
argument_list|(
operator|*
name|dest
argument_list|)
expr_stmt|;
operator|*
name|dest
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
