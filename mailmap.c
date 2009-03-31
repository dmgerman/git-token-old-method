begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"mailmap.h"
end_include
begin_define
DECL|macro|DEBUG_MAILMAP
define|#
directive|define
name|DEBUG_MAILMAP
value|0
end_define
begin_if
if|#
directive|if
name|DEBUG_MAILMAP
end_if
begin_define
DECL|macro|debug_mm
define|#
directive|define
name|debug_mm
parameter_list|(
modifier|...
parameter_list|)
value|fprintf(stderr, __VA_ARGS__)
end_define
begin_else
else|#
directive|else
end_else
begin_function
DECL|function|debug_mm
specifier|static
specifier|inline
name|void
name|debug_mm
parameter_list|(
specifier|const
name|char
modifier|*
name|format
parameter_list|,
modifier|...
parameter_list|)
block|{}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_decl_stmt
DECL|variable|git_mailmap_file
specifier|const
name|char
modifier|*
name|git_mailmap_file
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|mailmap_info
struct|struct
name|mailmap_info
block|{
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|email
name|char
modifier|*
name|email
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|mailmap_entry
struct|struct
name|mailmap_entry
block|{
comment|/* name and email for the simple mail-only case */
DECL|member|name
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|email
name|char
modifier|*
name|email
decl_stmt|;
comment|/* name and email for the complex mail and name matching case */
DECL|member|namemap
name|struct
name|string_list
name|namemap
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|free_mailmap_info
specifier|static
name|void
name|free_mailmap_info
parameter_list|(
name|void
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|mailmap_info
modifier|*
name|mi
init|=
operator|(
expr|struct
name|mailmap_info
operator|*
operator|)
name|p
decl_stmt|;
name|debug_mm
argument_list|(
literal|"mailmap: -- complex: '%s' -> '%s'<%s>\n"
argument_list|,
name|s
argument_list|,
name|mi
operator|->
name|name
argument_list|,
name|mi
operator|->
name|email
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|mi
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|mi
operator|->
name|email
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_mailmap_entry
specifier|static
name|void
name|free_mailmap_entry
parameter_list|(
name|void
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|mailmap_entry
modifier|*
name|me
init|=
operator|(
expr|struct
name|mailmap_entry
operator|*
operator|)
name|p
decl_stmt|;
name|debug_mm
argument_list|(
literal|"mailmap: removing entries for<%s>, with %d sub-entries\n"
argument_list|,
name|s
argument_list|,
name|me
operator|->
name|namemap
operator|.
name|nr
argument_list|)
expr_stmt|;
name|debug_mm
argument_list|(
literal|"mailmap: - simple: '%s'<%s>\n"
argument_list|,
name|me
operator|->
name|name
argument_list|,
name|me
operator|->
name|email
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|me
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|me
operator|->
name|email
argument_list|)
expr_stmt|;
name|me
operator|->
name|namemap
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear_func
argument_list|(
operator|&
name|me
operator|->
name|namemap
argument_list|,
name|free_mailmap_info
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_mapping
specifier|static
name|void
name|add_mapping
parameter_list|(
name|struct
name|string_list
modifier|*
name|map
parameter_list|,
name|char
modifier|*
name|new_name
parameter_list|,
name|char
modifier|*
name|new_email
parameter_list|,
name|char
modifier|*
name|old_name
parameter_list|,
name|char
modifier|*
name|old_email
parameter_list|)
block|{
name|struct
name|mailmap_entry
modifier|*
name|me
decl_stmt|;
name|int
name|index
decl_stmt|;
if|if
condition|(
name|old_email
operator|==
name|NULL
condition|)
block|{
name|old_email
operator|=
name|new_email
expr_stmt|;
name|new_email
operator|=
name|NULL
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|index
operator|=
name|string_list_find_insert_index
argument_list|(
name|map
argument_list|,
name|old_email
argument_list|,
literal|1
argument_list|)
operator|)
operator|<
literal|0
condition|)
block|{
comment|/* mailmap entry exists, invert index value */
name|index
operator|=
operator|-
literal|1
operator|-
name|index
expr_stmt|;
block|}
else|else
block|{
comment|/* create mailmap entry */
name|struct
name|string_list_item
modifier|*
name|item
init|=
name|string_list_insert_at_index
argument_list|(
name|index
argument_list|,
name|old_email
argument_list|,
name|map
argument_list|)
decl_stmt|;
name|item
operator|->
name|util
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|mailmap_entry
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|item
operator|->
name|util
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|mailmap_entry
argument_list|)
argument_list|)
expr_stmt|;
operator|(
operator|(
expr|struct
name|mailmap_entry
operator|*
operator|)
name|item
operator|->
name|util
operator|)
operator|->
name|namemap
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
block|}
name|me
operator|=
operator|(
expr|struct
name|mailmap_entry
operator|*
operator|)
name|map
operator|->
name|items
index|[
name|index
index|]
operator|.
name|util
expr_stmt|;
if|if
condition|(
name|old_name
operator|==
name|NULL
condition|)
block|{
name|debug_mm
argument_list|(
literal|"mailmap: adding (simple) entry for %s at index %d\n"
argument_list|,
name|old_email
argument_list|,
name|index
argument_list|)
expr_stmt|;
comment|/* Replace current name and new email for simple entry */
name|free
argument_list|(
name|me
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|me
operator|->
name|email
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_name
condition|)
name|me
operator|->
name|name
operator|=
name|xstrdup
argument_list|(
name|new_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_email
condition|)
name|me
operator|->
name|email
operator|=
name|xstrdup
argument_list|(
name|new_email
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|struct
name|mailmap_info
modifier|*
name|mi
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|mailmap_info
argument_list|)
argument_list|)
decl_stmt|;
name|debug_mm
argument_list|(
literal|"mailmap: adding (complex) entry for %s at index %d\n"
argument_list|,
name|old_email
argument_list|,
name|index
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_name
condition|)
name|mi
operator|->
name|name
operator|=
name|xstrdup
argument_list|(
name|new_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_email
condition|)
name|mi
operator|->
name|email
operator|=
name|xstrdup
argument_list|(
name|new_email
argument_list|)
expr_stmt|;
name|string_list_insert
argument_list|(
name|old_name
argument_list|,
operator|&
name|me
operator|->
name|namemap
argument_list|)
operator|->
name|util
operator|=
name|mi
expr_stmt|;
block|}
name|debug_mm
argument_list|(
literal|"mailmap:  '%s'<%s> -> '%s'<%s>\n"
argument_list|,
name|old_name
argument_list|,
name|old_email
argument_list|,
name|new_name
argument_list|,
name|new_email
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_name_and_email
specifier|static
name|char
modifier|*
name|parse_name_and_email
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|char
modifier|*
modifier|*
name|name
parameter_list|,
name|char
modifier|*
modifier|*
name|email
parameter_list|,
name|int
name|allow_empty_email
parameter_list|)
block|{
name|char
modifier|*
name|left
decl_stmt|,
modifier|*
name|right
decl_stmt|,
modifier|*
name|nstart
decl_stmt|,
modifier|*
name|nend
decl_stmt|;
operator|*
name|name
operator|=
operator|*
name|email
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|(
name|left
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
return|return
name|NULL
return|;
if|if
condition|(
operator|(
name|right
operator|=
name|strchr
argument_list|(
name|left
operator|+
literal|1
argument_list|,
literal|'>'
argument_list|)
operator|)
operator|==
name|NULL
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|allow_empty_email
operator|&&
operator|(
name|left
operator|+
literal|1
operator|==
name|right
operator|)
condition|)
return|return
name|NULL
return|;
comment|/* remove whitespace from beginning and end of name */
name|nstart
operator|=
name|buffer
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|nstart
argument_list|)
operator|&&
name|nstart
operator|<
name|left
condition|)
operator|++
name|nstart
expr_stmt|;
name|nend
operator|=
name|left
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|isspace
argument_list|(
operator|*
name|nend
argument_list|)
operator|&&
name|nend
operator|>
name|nstart
condition|)
operator|--
name|nend
expr_stmt|;
operator|*
name|name
operator|=
operator|(
name|nstart
operator|<
name|nend
condition|?
name|nstart
else|:
name|NULL
operator|)
expr_stmt|;
operator|*
name|email
operator|=
name|left
operator|+
literal|1
expr_stmt|;
operator|*
operator|(
name|nend
operator|+
literal|1
operator|)
operator|=
literal|'\0'
expr_stmt|;
operator|*
name|right
operator|++
operator|=
literal|'\0'
expr_stmt|;
return|return
operator|(
operator|*
name|right
operator|==
literal|'\0'
condition|?
name|NULL
else|:
name|right
operator|)
return|;
block|}
end_function
begin_function
DECL|function|read_single_mailmap
specifier|static
name|int
name|read_single_mailmap
parameter_list|(
name|struct
name|string_list
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
operator|(
name|filename
operator|==
name|NULL
condition|?
name|NULL
else|:
name|fopen
argument_list|(
name|filename
argument_list|,
literal|"r"
argument_list|)
operator|)
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
name|name1
init|=
literal|0
decl_stmt|,
modifier|*
name|email1
init|=
literal|0
decl_stmt|,
modifier|*
name|name2
init|=
literal|0
decl_stmt|,
modifier|*
name|email2
init|=
literal|0
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
name|name2
operator|=
name|parse_name_and_email
argument_list|(
name|buffer
argument_list|,
operator|&
name|name1
argument_list|,
operator|&
name|email1
argument_list|,
literal|0
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
name|parse_name_and_email
argument_list|(
name|name2
argument_list|,
operator|&
name|name2
argument_list|,
operator|&
name|email2
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|email1
condition|)
name|add_mapping
argument_list|(
name|map
argument_list|,
name|name1
argument_list|,
name|email1
argument_list|,
name|name2
argument_list|,
name|email2
argument_list|)
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
DECL|function|read_mailmap
name|int
name|read_mailmap
parameter_list|(
name|struct
name|string_list
modifier|*
name|map
parameter_list|,
name|char
modifier|*
modifier|*
name|repo_abbrev
parameter_list|)
block|{
name|map
operator|->
name|strdup_strings
operator|=
literal|1
expr_stmt|;
comment|/* each failure returns 1, so>1 means both calls failed */
return|return
name|read_single_mailmap
argument_list|(
name|map
argument_list|,
literal|".mailmap"
argument_list|,
name|repo_abbrev
argument_list|)
operator|+
name|read_single_mailmap
argument_list|(
name|map
argument_list|,
name|git_mailmap_file
argument_list|,
name|repo_abbrev
argument_list|)
operator|>
literal|1
return|;
block|}
end_function
begin_function
DECL|function|clear_mailmap
name|void
name|clear_mailmap
parameter_list|(
name|struct
name|string_list
modifier|*
name|map
parameter_list|)
block|{
name|debug_mm
argument_list|(
literal|"mailmap: clearing %d entries...\n"
argument_list|,
name|map
operator|->
name|nr
argument_list|)
expr_stmt|;
name|map
operator|->
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear_func
argument_list|(
name|map
argument_list|,
name|free_mailmap_entry
argument_list|)
expr_stmt|;
name|debug_mm
argument_list|(
literal|"mailmap: cleared\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|map_user
name|int
name|map_user
parameter_list|(
name|struct
name|string_list
modifier|*
name|map
parameter_list|,
name|char
modifier|*
name|email
parameter_list|,
name|int
name|maxlen_email
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|int
name|maxlen_name
parameter_list|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|struct
name|mailmap_entry
modifier|*
name|me
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
comment|/* figure out space requirement for email */
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
block|{
comment|/* email passed in might not be wrapped in<>, but end with a \0 */
name|p
operator|=
name|memchr
argument_list|(
name|email
argument_list|,
literal|'\0'
argument_list|,
name|maxlen_email
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
operator|==
literal|0
condition|)
return|return
literal|0
return|;
block|}
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
name|debug_mm
argument_list|(
literal|"map_user: map '%s'<%s>\n"
argument_list|,
name|name
argument_list|,
name|mailbuf
argument_list|)
expr_stmt|;
name|item
operator|=
name|string_list_lookup
argument_list|(
name|mailbuf
argument_list|,
name|map
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|!=
name|NULL
condition|)
block|{
name|me
operator|=
operator|(
expr|struct
name|mailmap_entry
operator|*
operator|)
name|item
operator|->
name|util
expr_stmt|;
if|if
condition|(
name|me
operator|->
name|namemap
operator|.
name|nr
condition|)
block|{
comment|/* The item has multiple items, so we'll look up on name too */
comment|/* If the name is not found, we choose the simple entry      */
name|struct
name|string_list_item
modifier|*
name|subitem
init|=
name|string_list_lookup
argument_list|(
name|name
argument_list|,
operator|&
name|me
operator|->
name|namemap
argument_list|)
decl_stmt|;
if|if
condition|(
name|subitem
condition|)
name|item
operator|=
name|subitem
expr_stmt|;
block|}
block|}
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
name|struct
name|mailmap_info
modifier|*
name|mi
init|=
operator|(
expr|struct
name|mailmap_info
operator|*
operator|)
name|item
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|mi
operator|->
name|name
operator|==
name|NULL
operator|&&
operator|(
name|mi
operator|->
name|email
operator|==
name|NULL
operator|||
name|maxlen_email
operator|==
literal|0
operator|)
condition|)
block|{
name|debug_mm
argument_list|(
literal|"map_user:  -- (no simple mapping)\n"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|maxlen_email
operator|&&
name|mi
operator|->
name|email
condition|)
name|strlcpy
argument_list|(
name|email
argument_list|,
name|mi
operator|->
name|email
argument_list|,
name|maxlen_email
argument_list|)
expr_stmt|;
if|if
condition|(
name|maxlen_name
operator|&&
name|mi
operator|->
name|name
condition|)
name|strlcpy
argument_list|(
name|name
argument_list|,
name|mi
operator|->
name|name
argument_list|,
name|maxlen_name
argument_list|)
expr_stmt|;
name|debug_mm
argument_list|(
literal|"map_user:  to '%s'<%s>\n"
argument_list|,
name|name
argument_list|,
name|mi
operator|->
name|email
condition|?
name|mi
operator|->
name|email
else|:
literal|""
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|debug_mm
argument_list|(
literal|"map_user:  --\n"
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
name|string_list
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
return|return
name|map_user
argument_list|(
name|map
argument_list|,
operator|(
name|char
operator|*
operator|)
name|email
argument_list|,
literal|0
argument_list|,
name|name
argument_list|,
name|maxlen
argument_list|)
return|;
block|}
end_function
end_unit
