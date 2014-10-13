begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_comment
comment|/*  * Copyright (c) 2013, 2014 Christian Couder<chriscool@tuxfamily.org>  */
end_comment
begin_enum
DECL|enum|action_where
DECL|enumerator|WHERE_END
DECL|enumerator|WHERE_AFTER
DECL|enumerator|WHERE_BEFORE
DECL|enumerator|WHERE_START
enum|enum
name|action_where
block|{
name|WHERE_END
block|,
name|WHERE_AFTER
block|,
name|WHERE_BEFORE
block|,
name|WHERE_START
block|}
enum|;
end_enum
begin_enum
DECL|enum|action_if_exists
DECL|enumerator|EXISTS_ADD_IF_DIFFERENT_NEIGHBOR
DECL|enumerator|EXISTS_ADD_IF_DIFFERENT
enum|enum
name|action_if_exists
block|{
name|EXISTS_ADD_IF_DIFFERENT_NEIGHBOR
block|,
name|EXISTS_ADD_IF_DIFFERENT
block|,
DECL|enumerator|EXISTS_ADD
DECL|enumerator|EXISTS_REPLACE
DECL|enumerator|EXISTS_DO_NOTHING
name|EXISTS_ADD
block|,
name|EXISTS_REPLACE
block|,
name|EXISTS_DO_NOTHING
block|}
enum|;
end_enum
begin_enum
DECL|enum|action_if_missing
DECL|enumerator|MISSING_ADD
DECL|enumerator|MISSING_DO_NOTHING
enum|enum
name|action_if_missing
block|{
name|MISSING_ADD
block|,
name|MISSING_DO_NOTHING
block|}
enum|;
end_enum
begin_struct
DECL|struct|conf_info
struct|struct
name|conf_info
block|{
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|key
name|char
modifier|*
name|key
decl_stmt|;
DECL|member|command
name|char
modifier|*
name|command
decl_stmt|;
DECL|member|where
name|enum
name|action_where
name|where
decl_stmt|;
DECL|member|if_exists
name|enum
name|action_if_exists
name|if_exists
decl_stmt|;
DECL|member|if_missing
name|enum
name|action_if_missing
name|if_missing
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|default_conf_info
specifier|static
name|struct
name|conf_info
name|default_conf_info
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|trailer_item
struct|struct
name|trailer_item
block|{
DECL|member|previous
name|struct
name|trailer_item
modifier|*
name|previous
decl_stmt|;
DECL|member|next
name|struct
name|trailer_item
modifier|*
name|next
decl_stmt|;
DECL|member|token
specifier|const
name|char
modifier|*
name|token
decl_stmt|;
DECL|member|value
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
DECL|member|conf
name|struct
name|conf_info
name|conf
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|first_conf_item
specifier|static
name|struct
name|trailer_item
modifier|*
name|first_conf_item
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|separators
specifier|static
name|char
modifier|*
name|separators
init|=
literal|":"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|after_or_end
specifier|static
name|int
name|after_or_end
parameter_list|(
name|enum
name|action_where
name|where
parameter_list|)
block|{
return|return
operator|(
name|where
operator|==
name|WHERE_AFTER
operator|)
operator|||
operator|(
name|where
operator|==
name|WHERE_END
operator|)
return|;
block|}
end_function
begin_comment
comment|/*  * Return the length of the string not including any final  * punctuation. E.g., the input "Signed-off-by:" would return  * 13, stripping the trailing punctuation but retaining  * internal punctuation.  */
end_comment
begin_function
DECL|function|token_len_without_separator
specifier|static
name|size_t
name|token_len_without_separator
parameter_list|(
specifier|const
name|char
modifier|*
name|token
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
while|while
condition|(
name|len
operator|>
literal|0
operator|&&
operator|!
name|isalnum
argument_list|(
name|token
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|len
operator|--
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|same_token
specifier|static
name|int
name|same_token
parameter_list|(
name|struct
name|trailer_item
modifier|*
name|a
parameter_list|,
name|struct
name|trailer_item
modifier|*
name|b
parameter_list|)
block|{
name|size_t
name|a_len
init|=
name|token_len_without_separator
argument_list|(
name|a
operator|->
name|token
argument_list|,
name|strlen
argument_list|(
name|a
operator|->
name|token
argument_list|)
argument_list|)
decl_stmt|;
name|size_t
name|b_len
init|=
name|token_len_without_separator
argument_list|(
name|b
operator|->
name|token
argument_list|,
name|strlen
argument_list|(
name|b
operator|->
name|token
argument_list|)
argument_list|)
decl_stmt|;
name|size_t
name|min_len
init|=
operator|(
name|a_len
operator|>
name|b_len
operator|)
condition|?
name|b_len
else|:
name|a_len
decl_stmt|;
return|return
operator|!
name|strncasecmp
argument_list|(
name|a
operator|->
name|token
argument_list|,
name|b
operator|->
name|token
argument_list|,
name|min_len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|same_value
specifier|static
name|int
name|same_value
parameter_list|(
name|struct
name|trailer_item
modifier|*
name|a
parameter_list|,
name|struct
name|trailer_item
modifier|*
name|b
parameter_list|)
block|{
return|return
operator|!
name|strcasecmp
argument_list|(
name|a
operator|->
name|value
argument_list|,
name|b
operator|->
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|same_trailer
specifier|static
name|int
name|same_trailer
parameter_list|(
name|struct
name|trailer_item
modifier|*
name|a
parameter_list|,
name|struct
name|trailer_item
modifier|*
name|b
parameter_list|)
block|{
return|return
name|same_token
argument_list|(
name|a
argument_list|,
name|b
argument_list|)
operator|&&
name|same_value
argument_list|(
name|a
argument_list|,
name|b
argument_list|)
return|;
block|}
end_function
end_unit
