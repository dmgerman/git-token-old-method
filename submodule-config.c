begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"submodule-config.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_comment
comment|/*  * submodule cache lookup structure  * There is one shared set of 'struct submodule' entries which can be  * looked up by their sha1 blob id of the .gitmodule file and either  * using path or name as key.  * for_path stores submodule entries with path as key  * for_name stores submodule entries with name as key  */
end_comment
begin_struct
DECL|struct|submodule_cache
struct|struct
name|submodule_cache
block|{
DECL|member|for_path
name|struct
name|hashmap
name|for_path
decl_stmt|;
DECL|member|for_name
name|struct
name|hashmap
name|for_name
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * thin wrapper struct needed to insert 'struct submodule' entries to  * the hashmap  */
end_comment
begin_struct
DECL|struct|submodule_entry
struct|struct
name|submodule_entry
block|{
DECL|member|ent
name|struct
name|hashmap_entry
name|ent
decl_stmt|;
DECL|member|config
name|struct
name|submodule
modifier|*
name|config
decl_stmt|;
block|}
struct|;
end_struct
begin_enum
DECL|enum|lookup_type
enum|enum
name|lookup_type
block|{
DECL|enumerator|lookup_name
name|lookup_name
block|,
DECL|enumerator|lookup_path
name|lookup_path
block|}
enum|;
end_enum
begin_decl_stmt
DECL|variable|cache
specifier|static
name|struct
name|submodule_cache
name|cache
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|is_cache_init
specifier|static
name|int
name|is_cache_init
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|config_path_cmp
specifier|static
name|int
name|config_path_cmp
parameter_list|(
specifier|const
name|struct
name|submodule_entry
modifier|*
name|a
parameter_list|,
specifier|const
name|struct
name|submodule_entry
modifier|*
name|b
parameter_list|,
specifier|const
name|void
modifier|*
name|unused
parameter_list|)
block|{
return|return
name|strcmp
argument_list|(
name|a
operator|->
name|config
operator|->
name|path
argument_list|,
name|b
operator|->
name|config
operator|->
name|path
argument_list|)
operator|||
name|hashcmp
argument_list|(
name|a
operator|->
name|config
operator|->
name|gitmodules_sha1
argument_list|,
name|b
operator|->
name|config
operator|->
name|gitmodules_sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|config_name_cmp
specifier|static
name|int
name|config_name_cmp
parameter_list|(
specifier|const
name|struct
name|submodule_entry
modifier|*
name|a
parameter_list|,
specifier|const
name|struct
name|submodule_entry
modifier|*
name|b
parameter_list|,
specifier|const
name|void
modifier|*
name|unused
parameter_list|)
block|{
return|return
name|strcmp
argument_list|(
name|a
operator|->
name|config
operator|->
name|name
argument_list|,
name|b
operator|->
name|config
operator|->
name|name
argument_list|)
operator|||
name|hashcmp
argument_list|(
name|a
operator|->
name|config
operator|->
name|gitmodules_sha1
argument_list|,
name|b
operator|->
name|config
operator|->
name|gitmodules_sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cache_init
specifier|static
name|void
name|cache_init
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|)
block|{
name|hashmap_init
argument_list|(
operator|&
name|cache
operator|->
name|for_path
argument_list|,
operator|(
name|hashmap_cmp_fn
operator|)
name|config_path_cmp
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|hashmap_init
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
operator|(
name|hashmap_cmp_fn
operator|)
name|config_name_cmp
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_one_config
specifier|static
name|void
name|free_one_config
parameter_list|(
name|struct
name|submodule_entry
modifier|*
name|entry
parameter_list|)
block|{
name|free
argument_list|(
operator|(
name|void
operator|*
operator|)
name|entry
operator|->
name|config
operator|->
name|path
argument_list|)
expr_stmt|;
name|free
argument_list|(
operator|(
name|void
operator|*
operator|)
name|entry
operator|->
name|config
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|entry
operator|->
name|config
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cache_free
specifier|static
name|void
name|cache_free
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|)
block|{
name|struct
name|hashmap_iter
name|iter
decl_stmt|;
name|struct
name|submodule_entry
modifier|*
name|entry
decl_stmt|;
comment|/* 	 * We iterate over the name hash here to be symmetric with the 	 * allocation of struct submodule entries. Each is allocated by 	 * their .gitmodule blob sha1 and submodule name. 	 */
name|hashmap_iter_init
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
operator|&
name|iter
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|entry
operator|=
name|hashmap_iter_next
argument_list|(
operator|&
name|iter
argument_list|)
operator|)
condition|)
name|free_one_config
argument_list|(
name|entry
argument_list|)
expr_stmt|;
name|hashmap_free
argument_list|(
operator|&
name|cache
operator|->
name|for_path
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|hashmap_free
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hash_sha1_string
specifier|static
name|unsigned
name|int
name|hash_sha1_string
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
return|return
name|memhash
argument_list|(
name|sha1
argument_list|,
literal|20
argument_list|)
operator|+
name|strhash
argument_list|(
name|string
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cache_put_path
specifier|static
name|void
name|cache_put_path
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
name|struct
name|submodule
modifier|*
name|submodule
parameter_list|)
block|{
name|unsigned
name|int
name|hash
init|=
name|hash_sha1_string
argument_list|(
name|submodule
operator|->
name|gitmodules_sha1
argument_list|,
name|submodule
operator|->
name|path
argument_list|)
decl_stmt|;
name|struct
name|submodule_entry
modifier|*
name|e
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|e
argument_list|)
argument_list|)
decl_stmt|;
name|hashmap_entry_init
argument_list|(
name|e
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|e
operator|->
name|config
operator|=
name|submodule
expr_stmt|;
name|hashmap_put
argument_list|(
operator|&
name|cache
operator|->
name|for_path
argument_list|,
name|e
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cache_remove_path
specifier|static
name|void
name|cache_remove_path
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
name|struct
name|submodule
modifier|*
name|submodule
parameter_list|)
block|{
name|unsigned
name|int
name|hash
init|=
name|hash_sha1_string
argument_list|(
name|submodule
operator|->
name|gitmodules_sha1
argument_list|,
name|submodule
operator|->
name|path
argument_list|)
decl_stmt|;
name|struct
name|submodule_entry
name|e
decl_stmt|;
name|struct
name|submodule_entry
modifier|*
name|removed
decl_stmt|;
name|hashmap_entry_init
argument_list|(
operator|&
name|e
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|e
operator|.
name|config
operator|=
name|submodule
expr_stmt|;
name|removed
operator|=
name|hashmap_remove
argument_list|(
operator|&
name|cache
operator|->
name|for_path
argument_list|,
operator|&
name|e
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|removed
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cache_add
specifier|static
name|void
name|cache_add
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
name|struct
name|submodule
modifier|*
name|submodule
parameter_list|)
block|{
name|unsigned
name|int
name|hash
init|=
name|hash_sha1_string
argument_list|(
name|submodule
operator|->
name|gitmodules_sha1
argument_list|,
name|submodule
operator|->
name|name
argument_list|)
decl_stmt|;
name|struct
name|submodule_entry
modifier|*
name|e
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|e
argument_list|)
argument_list|)
decl_stmt|;
name|hashmap_entry_init
argument_list|(
name|e
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|e
operator|->
name|config
operator|=
name|submodule
expr_stmt|;
name|hashmap_add
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
name|e
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cache_lookup_path
specifier|static
specifier|const
name|struct
name|submodule
modifier|*
name|cache_lookup_path
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|gitmodules_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|submodule_entry
modifier|*
name|entry
decl_stmt|;
name|unsigned
name|int
name|hash
init|=
name|hash_sha1_string
argument_list|(
name|gitmodules_sha1
argument_list|,
name|path
argument_list|)
decl_stmt|;
name|struct
name|submodule_entry
name|key
decl_stmt|;
name|struct
name|submodule
name|key_config
decl_stmt|;
name|hashcpy
argument_list|(
name|key_config
operator|.
name|gitmodules_sha1
argument_list|,
name|gitmodules_sha1
argument_list|)
expr_stmt|;
name|key_config
operator|.
name|path
operator|=
name|path
expr_stmt|;
name|hashmap_entry_init
argument_list|(
operator|&
name|key
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|key
operator|.
name|config
operator|=
operator|&
name|key_config
expr_stmt|;
name|entry
operator|=
name|hashmap_get
argument_list|(
operator|&
name|cache
operator|->
name|for_path
argument_list|,
operator|&
name|key
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|entry
condition|)
return|return
name|entry
operator|->
name|config
return|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|cache_lookup_name
specifier|static
name|struct
name|submodule
modifier|*
name|cache_lookup_name
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|gitmodules_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|submodule_entry
modifier|*
name|entry
decl_stmt|;
name|unsigned
name|int
name|hash
init|=
name|hash_sha1_string
argument_list|(
name|gitmodules_sha1
argument_list|,
name|name
argument_list|)
decl_stmt|;
name|struct
name|submodule_entry
name|key
decl_stmt|;
name|struct
name|submodule
name|key_config
decl_stmt|;
name|hashcpy
argument_list|(
name|key_config
operator|.
name|gitmodules_sha1
argument_list|,
name|gitmodules_sha1
argument_list|)
expr_stmt|;
name|key_config
operator|.
name|name
operator|=
name|name
expr_stmt|;
name|hashmap_entry_init
argument_list|(
operator|&
name|key
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|key
operator|.
name|config
operator|=
operator|&
name|key_config
expr_stmt|;
name|entry
operator|=
name|hashmap_get
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
operator|&
name|key
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|entry
condition|)
return|return
name|entry
operator|->
name|config
return|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|name_and_item_from_var
specifier|static
name|int
name|name_and_item_from_var
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
name|struct
name|strbuf
modifier|*
name|name
parameter_list|,
name|struct
name|strbuf
modifier|*
name|item
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|subsection
decl_stmt|,
modifier|*
name|key
decl_stmt|;
name|int
name|subsection_len
decl_stmt|,
name|parse
decl_stmt|;
name|parse
operator|=
name|parse_config_key
argument_list|(
name|var
argument_list|,
literal|"submodule"
argument_list|,
operator|&
name|subsection
argument_list|,
operator|&
name|subsection_len
argument_list|,
operator|&
name|key
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse
operator|<
literal|0
operator|||
operator|!
name|subsection
condition|)
return|return
literal|0
return|;
name|strbuf_add
argument_list|(
name|name
argument_list|,
name|subsection
argument_list|,
name|subsection_len
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|item
argument_list|,
name|key
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|lookup_or_create_by_name
specifier|static
name|struct
name|submodule
modifier|*
name|lookup_or_create_by_name
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|gitmodules_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|submodule
modifier|*
name|submodule
decl_stmt|;
name|struct
name|strbuf
name|name_buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|submodule
operator|=
name|cache_lookup_name
argument_list|(
name|cache
argument_list|,
name|gitmodules_sha1
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|submodule
condition|)
return|return
name|submodule
return|;
name|submodule
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|submodule
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|name_buf
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|submodule
operator|->
name|name
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|name_buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|submodule
operator|->
name|path
operator|=
name|NULL
expr_stmt|;
name|submodule
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|submodule
operator|->
name|fetch_recurse
operator|=
name|RECURSE_SUBMODULES_NONE
expr_stmt|;
name|submodule
operator|->
name|ignore
operator|=
name|NULL
expr_stmt|;
name|hashcpy
argument_list|(
name|submodule
operator|->
name|gitmodules_sha1
argument_list|,
name|gitmodules_sha1
argument_list|)
expr_stmt|;
name|cache_add
argument_list|(
name|cache
argument_list|,
name|submodule
argument_list|)
expr_stmt|;
return|return
name|submodule
return|;
block|}
end_function
begin_function
DECL|function|parse_fetch_recurse
specifier|static
name|int
name|parse_fetch_recurse
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|die_on_error
parameter_list|)
block|{
switch|switch
condition|(
name|git_config_maybe_bool
argument_list|(
name|opt
argument_list|,
name|arg
argument_list|)
condition|)
block|{
case|case
literal|1
case|:
return|return
name|RECURSE_SUBMODULES_ON
return|;
case|case
literal|0
case|:
return|return
name|RECURSE_SUBMODULES_OFF
return|;
default|default:
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"on-demand"
argument_list|)
condition|)
return|return
name|RECURSE_SUBMODULES_ON_DEMAND
return|;
if|if
condition|(
name|die_on_error
condition|)
name|die
argument_list|(
literal|"bad %s argument: %s"
argument_list|,
name|opt
argument_list|,
name|arg
argument_list|)
expr_stmt|;
else|else
return|return
name|RECURSE_SUBMODULES_ERROR
return|;
block|}
block|}
end_function
begin_function
DECL|function|parse_fetch_recurse_submodules_arg
name|int
name|parse_fetch_recurse_submodules_arg
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
return|return
name|parse_fetch_recurse
argument_list|(
name|opt
argument_list|,
name|arg
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_push_recurse
specifier|static
name|int
name|parse_push_recurse
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|die_on_error
parameter_list|)
block|{
switch|switch
condition|(
name|git_config_maybe_bool
argument_list|(
name|opt
argument_list|,
name|arg
argument_list|)
condition|)
block|{
case|case
literal|1
case|:
comment|/* There's no simple "on" value when pushing */
if|if
condition|(
name|die_on_error
condition|)
name|die
argument_list|(
literal|"bad %s argument: %s"
argument_list|,
name|opt
argument_list|,
name|arg
argument_list|)
expr_stmt|;
else|else
return|return
name|RECURSE_SUBMODULES_ERROR
return|;
case|case
literal|0
case|:
return|return
name|RECURSE_SUBMODULES_OFF
return|;
default|default:
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"on-demand"
argument_list|)
condition|)
return|return
name|RECURSE_SUBMODULES_ON_DEMAND
return|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"check"
argument_list|)
condition|)
return|return
name|RECURSE_SUBMODULES_CHECK
return|;
elseif|else
if|if
condition|(
name|die_on_error
condition|)
name|die
argument_list|(
literal|"bad %s argument: %s"
argument_list|,
name|opt
argument_list|,
name|arg
argument_list|)
expr_stmt|;
else|else
return|return
name|RECURSE_SUBMODULES_ERROR
return|;
block|}
block|}
end_function
begin_function
DECL|function|parse_push_recurse_submodules_arg
name|int
name|parse_push_recurse_submodules_arg
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
return|return
name|parse_push_recurse
argument_list|(
name|opt
argument_list|,
name|arg
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|warn_multiple_config
specifier|static
name|void
name|warn_multiple_config
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|option
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|commit_string
init|=
literal|"WORKTREE"
decl_stmt|;
if|if
condition|(
name|commit_sha1
condition|)
name|commit_string
operator|=
name|sha1_to_hex
argument_list|(
name|commit_sha1
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"%s:.gitmodules, multiple configurations found for "
literal|"'submodule.%s.%s'. Skipping second one!"
argument_list|,
name|commit_string
argument_list|,
name|name
argument_list|,
name|option
argument_list|)
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|parse_config_parameter
struct|struct
name|parse_config_parameter
block|{
DECL|member|cache
name|struct
name|submodule_cache
modifier|*
name|cache
decl_stmt|;
DECL|member|commit_sha1
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
decl_stmt|;
DECL|member|gitmodules_sha1
specifier|const
name|unsigned
name|char
modifier|*
name|gitmodules_sha1
decl_stmt|;
DECL|member|overwrite
name|int
name|overwrite
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|parse_config
specifier|static
name|int
name|parse_config
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
name|struct
name|parse_config_parameter
modifier|*
name|me
init|=
name|data
decl_stmt|;
name|struct
name|submodule
modifier|*
name|submodule
decl_stmt|;
name|struct
name|strbuf
name|name
init|=
name|STRBUF_INIT
decl_stmt|,
name|item
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|ret
init|=
literal|0
decl_stmt|;
comment|/* this also ensures that we only parse submodule entries */
if|if
condition|(
operator|!
name|name_and_item_from_var
argument_list|(
name|var
argument_list|,
operator|&
name|name
argument_list|,
operator|&
name|item
argument_list|)
condition|)
return|return
literal|0
return|;
name|submodule
operator|=
name|lookup_or_create_by_name
argument_list|(
name|me
operator|->
name|cache
argument_list|,
name|me
operator|->
name|gitmodules_sha1
argument_list|,
name|name
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"path"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
name|ret
operator|=
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|me
operator|->
name|overwrite
operator|&&
name|submodule
operator|->
name|path
operator|!=
name|NULL
condition|)
name|warn_multiple_config
argument_list|(
name|me
operator|->
name|commit_sha1
argument_list|,
name|submodule
operator|->
name|name
argument_list|,
literal|"path"
argument_list|)
expr_stmt|;
else|else
block|{
if|if
condition|(
name|submodule
operator|->
name|path
condition|)
name|cache_remove_path
argument_list|(
name|me
operator|->
name|cache
argument_list|,
name|submodule
argument_list|)
expr_stmt|;
name|free
argument_list|(
operator|(
name|void
operator|*
operator|)
name|submodule
operator|->
name|path
argument_list|)
expr_stmt|;
name|submodule
operator|->
name|path
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
name|cache_put_path
argument_list|(
name|me
operator|->
name|cache
argument_list|,
name|submodule
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"fetchrecursesubmodules"
argument_list|)
condition|)
block|{
comment|/* when parsing worktree configurations we can die early */
name|int
name|die_on_error
init|=
name|is_null_sha1
argument_list|(
name|me
operator|->
name|gitmodules_sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|me
operator|->
name|overwrite
operator|&&
name|submodule
operator|->
name|fetch_recurse
operator|!=
name|RECURSE_SUBMODULES_NONE
condition|)
name|warn_multiple_config
argument_list|(
name|me
operator|->
name|commit_sha1
argument_list|,
name|submodule
operator|->
name|name
argument_list|,
literal|"fetchrecursesubmodules"
argument_list|)
expr_stmt|;
else|else
name|submodule
operator|->
name|fetch_recurse
operator|=
name|parse_fetch_recurse
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|die_on_error
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"ignore"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
name|ret
operator|=
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|me
operator|->
name|overwrite
operator|&&
name|submodule
operator|->
name|ignore
operator|!=
name|NULL
condition|)
name|warn_multiple_config
argument_list|(
name|me
operator|->
name|commit_sha1
argument_list|,
name|submodule
operator|->
name|name
argument_list|,
literal|"ignore"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"untracked"
argument_list|)
operator|&&
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"dirty"
argument_list|)
operator|&&
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"all"
argument_list|)
operator|&&
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"none"
argument_list|)
condition|)
name|warning
argument_list|(
literal|"Invalid parameter '%s' for config option "
literal|"'submodule.%s.ignore'"
argument_list|,
name|value
argument_list|,
name|var
argument_list|)
expr_stmt|;
else|else
block|{
name|free
argument_list|(
operator|(
name|void
operator|*
operator|)
name|submodule
operator|->
name|ignore
argument_list|)
expr_stmt|;
name|submodule
operator|->
name|ignore
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"url"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
block|{
name|ret
operator|=
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|me
operator|->
name|overwrite
operator|&&
name|submodule
operator|->
name|url
operator|!=
name|NULL
condition|)
block|{
name|warn_multiple_config
argument_list|(
name|me
operator|->
name|commit_sha1
argument_list|,
name|submodule
operator|->
name|name
argument_list|,
literal|"url"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|free
argument_list|(
operator|(
name|void
operator|*
operator|)
name|submodule
operator|->
name|url
argument_list|)
expr_stmt|;
name|submodule
operator|->
name|url
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
block|}
block|}
name|strbuf_release
argument_list|(
operator|&
name|name
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|item
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|gitmodule_sha1_from_commit
specifier|static
name|int
name|gitmodule_sha1_from_commit
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
name|unsigned
name|char
modifier|*
name|gitmodules_sha1
parameter_list|)
block|{
name|struct
name|strbuf
name|rev
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|ret
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|is_null_sha1
argument_list|(
name|commit_sha1
argument_list|)
condition|)
block|{
name|hashcpy
argument_list|(
name|gitmodules_sha1
argument_list|,
name|null_sha1
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|strbuf_addf
argument_list|(
operator|&
name|rev
argument_list|,
literal|"%s:.gitmodules"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|rev
operator|.
name|buf
argument_list|,
name|gitmodules_sha1
argument_list|)
operator|>=
literal|0
condition|)
name|ret
operator|=
literal|1
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|rev
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/* This does a lookup of a submodule configuration by name or by path  * (key) with on-demand reading of the appropriate .gitmodules from  * revisions.  */
end_comment
begin_function
DECL|function|config_from
specifier|static
specifier|const
name|struct
name|submodule
modifier|*
name|config_from
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|key
parameter_list|,
name|enum
name|lookup_type
name|lookup_type
parameter_list|)
block|{
name|struct
name|strbuf
name|rev
init|=
name|STRBUF_INIT
decl_stmt|;
name|unsigned
name|long
name|config_size
decl_stmt|;
name|char
modifier|*
name|config
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
specifier|const
name|struct
name|submodule
modifier|*
name|submodule
init|=
name|NULL
decl_stmt|;
name|struct
name|parse_config_parameter
name|parameter
decl_stmt|;
comment|/* 	 * If any parameter except the cache is a NULL pointer just 	 * return the first submodule. Can be used to check whether 	 * there are any submodules parsed. 	 */
if|if
condition|(
operator|!
name|commit_sha1
operator|||
operator|!
name|key
condition|)
block|{
name|struct
name|hashmap_iter
name|iter
decl_stmt|;
name|struct
name|submodule_entry
modifier|*
name|entry
decl_stmt|;
name|hashmap_iter_init
argument_list|(
operator|&
name|cache
operator|->
name|for_name
argument_list|,
operator|&
name|iter
argument_list|)
expr_stmt|;
name|entry
operator|=
name|hashmap_iter_next
argument_list|(
operator|&
name|iter
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|entry
condition|)
return|return
name|NULL
return|;
return|return
name|entry
operator|->
name|config
return|;
block|}
if|if
condition|(
operator|!
name|gitmodule_sha1_from_commit
argument_list|(
name|commit_sha1
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|NULL
return|;
switch|switch
condition|(
name|lookup_type
condition|)
block|{
case|case
name|lookup_name
case|:
name|submodule
operator|=
name|cache_lookup_name
argument_list|(
name|cache
argument_list|,
name|sha1
argument_list|,
name|key
argument_list|)
expr_stmt|;
break|break;
case|case
name|lookup_path
case|:
name|submodule
operator|=
name|cache_lookup_path
argument_list|(
name|cache
argument_list|,
name|sha1
argument_list|,
name|key
argument_list|)
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|submodule
condition|)
return|return
name|submodule
return|;
name|config
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|config_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|config
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|type
operator|!=
name|OBJ_BLOB
condition|)
block|{
name|free
argument_list|(
name|config
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* fill the submodule config into the cache */
name|parameter
operator|.
name|cache
operator|=
name|cache
expr_stmt|;
name|parameter
operator|.
name|commit_sha1
operator|=
name|commit_sha1
expr_stmt|;
name|parameter
operator|.
name|gitmodules_sha1
operator|=
name|sha1
expr_stmt|;
name|parameter
operator|.
name|overwrite
operator|=
literal|0
expr_stmt|;
name|git_config_from_mem
argument_list|(
name|parse_config
argument_list|,
name|rev
operator|.
name|buf
argument_list|,
name|config
argument_list|,
name|config_size
argument_list|,
operator|&
name|parameter
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|config
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|lookup_type
condition|)
block|{
case|case
name|lookup_name
case|:
return|return
name|cache_lookup_name
argument_list|(
name|cache
argument_list|,
name|sha1
argument_list|,
name|key
argument_list|)
return|;
case|case
name|lookup_path
case|:
return|return
name|cache_lookup_path
argument_list|(
name|cache
argument_list|,
name|sha1
argument_list|,
name|key
argument_list|)
return|;
default|default:
return|return
name|NULL
return|;
block|}
block|}
end_function
begin_function
DECL|function|config_from_path
specifier|static
specifier|const
name|struct
name|submodule
modifier|*
name|config_from_path
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
return|return
name|config_from
argument_list|(
name|cache
argument_list|,
name|commit_sha1
argument_list|,
name|path
argument_list|,
name|lookup_path
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|config_from_name
specifier|static
specifier|const
name|struct
name|submodule
modifier|*
name|config_from_name
parameter_list|(
name|struct
name|submodule_cache
modifier|*
name|cache
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
return|return
name|config_from
argument_list|(
name|cache
argument_list|,
name|commit_sha1
argument_list|,
name|name
argument_list|,
name|lookup_name
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ensure_cache_init
specifier|static
name|void
name|ensure_cache_init
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|is_cache_init
condition|)
return|return;
name|cache_init
argument_list|(
operator|&
name|cache
argument_list|)
expr_stmt|;
name|is_cache_init
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_submodule_config_option
name|int
name|parse_submodule_config_option
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
parameter_list|)
block|{
name|struct
name|parse_config_parameter
name|parameter
decl_stmt|;
name|parameter
operator|.
name|cache
operator|=
operator|&
name|cache
expr_stmt|;
name|parameter
operator|.
name|commit_sha1
operator|=
name|NULL
expr_stmt|;
name|parameter
operator|.
name|gitmodules_sha1
operator|=
name|null_sha1
expr_stmt|;
name|parameter
operator|.
name|overwrite
operator|=
literal|1
expr_stmt|;
name|ensure_cache_init
argument_list|()
expr_stmt|;
return|return
name|parse_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
operator|&
name|parameter
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|submodule_from_name
specifier|const
name|struct
name|submodule
modifier|*
name|submodule_from_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|ensure_cache_init
argument_list|()
expr_stmt|;
return|return
name|config_from_name
argument_list|(
operator|&
name|cache
argument_list|,
name|commit_sha1
argument_list|,
name|name
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|submodule_from_path
specifier|const
name|struct
name|submodule
modifier|*
name|submodule_from_path
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|commit_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|ensure_cache_init
argument_list|()
expr_stmt|;
return|return
name|config_from_path
argument_list|(
operator|&
name|cache
argument_list|,
name|commit_sha1
argument_list|,
name|path
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|submodule_free
name|void
name|submodule_free
parameter_list|(
name|void
parameter_list|)
block|{
name|cache_free
argument_list|(
operator|&
name|cache
argument_list|)
expr_stmt|;
name|is_cache_init
operator|=
literal|0
expr_stmt|;
block|}
end_function
end_unit
