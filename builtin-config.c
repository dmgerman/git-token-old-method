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
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|builtin_config_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_config_usage
index|[]
init|=
block|{
literal|"git config [options]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|key
specifier|static
name|char
modifier|*
name|key
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|key_regexp
specifier|static
name|regex_t
modifier|*
name|key_regexp
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|regexp
specifier|static
name|regex_t
modifier|*
name|regexp
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_keys
specifier|static
name|int
name|show_keys
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_key_regexp
specifier|static
name|int
name|use_key_regexp
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|do_all
specifier|static
name|int
name|do_all
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|do_not_match
specifier|static
name|int
name|do_not_match
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|seen
specifier|static
name|int
name|seen
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|delim
specifier|static
name|char
name|delim
init|=
literal|'='
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|key_delim
specifier|static
name|char
name|key_delim
init|=
literal|' '
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|term
specifier|static
name|char
name|term
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|use_global_config
DECL|variable|use_system_config
specifier|static
name|int
name|use_global_config
decl_stmt|,
name|use_system_config
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|given_config_file
specifier|static
specifier|const
name|char
modifier|*
name|given_config_file
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|actions
DECL|variable|types
specifier|static
name|int
name|actions
decl_stmt|,
name|types
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_color_slot
DECL|variable|get_colorbool_slot
specifier|static
specifier|const
name|char
modifier|*
name|get_color_slot
decl_stmt|,
modifier|*
name|get_colorbool_slot
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|end_null
specifier|static
name|int
name|end_null
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|ACTION_GET
define|#
directive|define
name|ACTION_GET
value|(1<<0)
end_define
begin_define
DECL|macro|ACTION_GET_ALL
define|#
directive|define
name|ACTION_GET_ALL
value|(1<<1)
end_define
begin_define
DECL|macro|ACTION_GET_REGEXP
define|#
directive|define
name|ACTION_GET_REGEXP
value|(1<<2)
end_define
begin_define
DECL|macro|ACTION_REPLACE_ALL
define|#
directive|define
name|ACTION_REPLACE_ALL
value|(1<<3)
end_define
begin_define
DECL|macro|ACTION_ADD
define|#
directive|define
name|ACTION_ADD
value|(1<<4)
end_define
begin_define
DECL|macro|ACTION_UNSET
define|#
directive|define
name|ACTION_UNSET
value|(1<<5)
end_define
begin_define
DECL|macro|ACTION_UNSET_ALL
define|#
directive|define
name|ACTION_UNSET_ALL
value|(1<<6)
end_define
begin_define
DECL|macro|ACTION_RENAME_SECTION
define|#
directive|define
name|ACTION_RENAME_SECTION
value|(1<<7)
end_define
begin_define
DECL|macro|ACTION_REMOVE_SECTION
define|#
directive|define
name|ACTION_REMOVE_SECTION
value|(1<<8)
end_define
begin_define
DECL|macro|ACTION_LIST
define|#
directive|define
name|ACTION_LIST
value|(1<<9)
end_define
begin_define
DECL|macro|ACTION_EDIT
define|#
directive|define
name|ACTION_EDIT
value|(1<<10)
end_define
begin_define
DECL|macro|ACTION_SET
define|#
directive|define
name|ACTION_SET
value|(1<<11)
end_define
begin_define
DECL|macro|ACTION_SET_ALL
define|#
directive|define
name|ACTION_SET_ALL
value|(1<<12)
end_define
begin_define
DECL|macro|ACTION_GET_COLOR
define|#
directive|define
name|ACTION_GET_COLOR
value|(1<<13)
end_define
begin_define
DECL|macro|ACTION_GET_COLORBOOL
define|#
directive|define
name|ACTION_GET_COLORBOOL
value|(1<<14)
end_define
begin_define
DECL|macro|TYPE_BOOL
define|#
directive|define
name|TYPE_BOOL
value|(1<<0)
end_define
begin_define
DECL|macro|TYPE_INT
define|#
directive|define
name|TYPE_INT
value|(1<<1)
end_define
begin_define
DECL|macro|TYPE_BOOL_OR_INT
define|#
directive|define
name|TYPE_BOOL_OR_INT
value|(1<<2)
end_define
begin_decl_stmt
DECL|variable|builtin_config_options
specifier|static
name|struct
name|option
name|builtin_config_options
index|[]
init|=
block|{
name|OPT_GROUP
argument_list|(
literal|"Config file location"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"global"
argument_list|,
operator|&
name|use_global_config
argument_list|,
literal|"use global config file"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"system"
argument_list|,
operator|&
name|use_system_config
argument_list|,
literal|"use system config file"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'f'
argument_list|,
literal|"file"
argument_list|,
operator|&
name|given_config_file
argument_list|,
literal|"FILE"
argument_list|,
literal|"use given config file"
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"Action"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"get"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"get value: name [value-regex]"
argument_list|,
name|ACTION_GET
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"get-all"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"get all values: key [value-regex]"
argument_list|,
name|ACTION_GET_ALL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"get-regexp"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"get values for regexp: name-regex [value-regex]"
argument_list|,
name|ACTION_GET_REGEXP
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"replace-all"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"replace all matching variables: name [value [value_regex]"
argument_list|,
name|ACTION_REPLACE_ALL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"add"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"adds a new variable: name value"
argument_list|,
name|ACTION_ADD
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"unset"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"removes a variable: name [value-regex]"
argument_list|,
name|ACTION_UNSET
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"unset-all"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"removes all matches: name [value-regex]"
argument_list|,
name|ACTION_UNSET_ALL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"rename-section"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"rename section: old-name new-name"
argument_list|,
name|ACTION_RENAME_SECTION
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"remove-section"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"remove a section: name"
argument_list|,
name|ACTION_REMOVE_SECTION
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'l'
argument_list|,
literal|"list"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"list all"
argument_list|,
name|ACTION_LIST
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'e'
argument_list|,
literal|"edit"
argument_list|,
operator|&
name|actions
argument_list|,
literal|"opens an editor"
argument_list|,
name|ACTION_EDIT
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"get-color"
argument_list|,
operator|&
name|get_color_slot
argument_list|,
literal|"slot"
argument_list|,
literal|"find the color configured: [default]"
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"get-colorbool"
argument_list|,
operator|&
name|get_colorbool_slot
argument_list|,
literal|"slot"
argument_list|,
literal|"find the color setting: [stdout-is-tty]"
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"Type"
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"bool"
argument_list|,
operator|&
name|types
argument_list|,
literal|"value is \"true\" or \"false\""
argument_list|,
name|TYPE_BOOL
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"int"
argument_list|,
operator|&
name|types
argument_list|,
literal|"value is decimal number"
argument_list|,
name|TYPE_INT
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|0
argument_list|,
literal|"bool-or-int"
argument_list|,
operator|&
name|types
argument_list|,
name|NULL
argument_list|,
name|TYPE_BOOL_OR_INT
argument_list|)
block|,
name|OPT_GROUP
argument_list|(
literal|"Other"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'z'
argument_list|,
literal|"null"
argument_list|,
operator|&
name|end_null
argument_list|,
literal|"terminate values with NUL byte"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|check_argc
specifier|static
name|void
name|check_argc
parameter_list|(
name|int
name|argc
parameter_list|,
name|int
name|min
parameter_list|,
name|int
name|max
parameter_list|)
block|{
if|if
condition|(
name|argc
operator|>=
name|min
operator|&&
name|argc
operator|<=
name|max
condition|)
return|return;
name|error
argument_list|(
literal|"wrong number of arguments"
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|builtin_config_usage
argument_list|,
name|builtin_config_options
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_all_config
specifier|static
name|int
name|show_all_config
parameter_list|(
specifier|const
name|char
modifier|*
name|key_
parameter_list|,
specifier|const
name|char
modifier|*
name|value_
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
if|if
condition|(
name|value_
condition|)
name|printf
argument_list|(
literal|"%s%c%s%c"
argument_list|,
name|key_
argument_list|,
name|delim
argument_list|,
name|value_
argument_list|,
name|term
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|key_
argument_list|,
name|term
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_config
specifier|static
name|int
name|show_config
parameter_list|(
specifier|const
name|char
modifier|*
name|key_
parameter_list|,
specifier|const
name|char
modifier|*
name|value_
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
name|char
name|value
index|[
literal|256
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|vptr
init|=
name|value
decl_stmt|;
name|int
name|dup_error
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|use_key_regexp
operator|&&
name|strcmp
argument_list|(
name|key_
argument_list|,
name|key
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|use_key_regexp
operator|&&
name|regexec
argument_list|(
name|key_regexp
argument_list|,
name|key_
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|regexp
operator|!=
name|NULL
operator|&&
operator|(
name|do_not_match
operator|^
operator|!
operator|!
name|regexec
argument_list|(
name|regexp
argument_list|,
operator|(
name|value_
condition|?
name|value_
else|:
literal|""
operator|)
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
operator|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|show_keys
condition|)
block|{
if|if
condition|(
name|value_
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|key_
argument_list|,
name|key_delim
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|key_
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|seen
operator|&&
operator|!
name|do_all
condition|)
name|dup_error
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|types
operator|==
name|TYPE_INT
condition|)
name|sprintf
argument_list|(
name|value
argument_list|,
literal|"%d"
argument_list|,
name|git_config_int
argument_list|(
name|key_
argument_list|,
name|value_
condition|?
name|value_
else|:
literal|""
argument_list|)
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|types
operator|==
name|TYPE_BOOL
condition|)
name|vptr
operator|=
name|git_config_bool
argument_list|(
name|key_
argument_list|,
name|value_
argument_list|)
condition|?
literal|"true"
else|:
literal|"false"
expr_stmt|;
elseif|else
if|if
condition|(
name|types
operator|==
name|TYPE_BOOL_OR_INT
condition|)
block|{
name|int
name|is_bool
decl_stmt|,
name|v
decl_stmt|;
name|v
operator|=
name|git_config_bool_or_int
argument_list|(
name|key_
argument_list|,
name|value_
argument_list|,
operator|&
name|is_bool
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_bool
condition|)
name|vptr
operator|=
name|v
condition|?
literal|"true"
else|:
literal|"false"
expr_stmt|;
else|else
name|sprintf
argument_list|(
name|value
argument_list|,
literal|"%d"
argument_list|,
name|v
argument_list|)
expr_stmt|;
block|}
else|else
name|vptr
operator|=
name|value_
condition|?
name|value_
else|:
literal|""
expr_stmt|;
name|seen
operator|++
expr_stmt|;
if|if
condition|(
name|dup_error
condition|)
block|{
name|error
argument_list|(
literal|"More than one value for the key %s: %s"
argument_list|,
name|key_
argument_list|,
name|vptr
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|vptr
argument_list|,
name|term
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_value
specifier|static
name|int
name|get_value
parameter_list|(
specifier|const
name|char
modifier|*
name|key_
parameter_list|,
specifier|const
name|char
modifier|*
name|regex_
parameter_list|)
block|{
name|int
name|ret
init|=
operator|-
literal|1
decl_stmt|;
name|char
modifier|*
name|tl
decl_stmt|;
name|char
modifier|*
name|global
init|=
name|NULL
decl_stmt|,
modifier|*
name|repo_config
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|system_wide
init|=
name|NULL
decl_stmt|,
modifier|*
name|local
decl_stmt|;
name|local
operator|=
name|config_exclusive_filename
expr_stmt|;
if|if
condition|(
operator|!
name|local
condition|)
block|{
specifier|const
name|char
modifier|*
name|home
init|=
name|getenv
argument_list|(
literal|"HOME"
argument_list|)
decl_stmt|;
name|local
operator|=
name|repo_config
operator|=
name|git_pathdup
argument_list|(
literal|"config"
argument_list|)
expr_stmt|;
if|if
condition|(
name|git_config_global
argument_list|()
operator|&&
name|home
condition|)
name|global
operator|=
name|xstrdup
argument_list|(
name|mkpath
argument_list|(
literal|"%s/.gitconfig"
argument_list|,
name|home
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|git_config_system
argument_list|()
condition|)
name|system_wide
operator|=
name|git_etc_gitconfig
argument_list|()
expr_stmt|;
block|}
name|key
operator|=
name|xstrdup
argument_list|(
name|key_
argument_list|)
expr_stmt|;
for|for
control|(
name|tl
operator|=
name|key
operator|+
name|strlen
argument_list|(
name|key
argument_list|)
operator|-
literal|1
init|;
name|tl
operator|>=
name|key
operator|&&
operator|*
name|tl
operator|!=
literal|'.'
condition|;
operator|--
name|tl
control|)
operator|*
name|tl
operator|=
name|tolower
argument_list|(
operator|*
name|tl
argument_list|)
expr_stmt|;
for|for
control|(
name|tl
operator|=
name|key
init|;
operator|*
name|tl
operator|&&
operator|*
name|tl
operator|!=
literal|'.'
condition|;
operator|++
name|tl
control|)
operator|*
name|tl
operator|=
name|tolower
argument_list|(
operator|*
name|tl
argument_list|)
expr_stmt|;
if|if
condition|(
name|use_key_regexp
condition|)
block|{
name|key_regexp
operator|=
operator|(
name|regex_t
operator|*
operator|)
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|regex_t
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|regcomp
argument_list|(
name|key_regexp
argument_list|,
name|key
argument_list|,
name|REG_EXTENDED
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Invalid key pattern: %s\n"
argument_list|,
name|key_
argument_list|)
expr_stmt|;
goto|goto
name|free_strings
goto|;
block|}
block|}
if|if
condition|(
name|regex_
condition|)
block|{
if|if
condition|(
name|regex_
index|[
literal|0
index|]
operator|==
literal|'!'
condition|)
block|{
name|do_not_match
operator|=
literal|1
expr_stmt|;
name|regex_
operator|++
expr_stmt|;
block|}
name|regexp
operator|=
operator|(
name|regex_t
operator|*
operator|)
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|regex_t
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|regcomp
argument_list|(
name|regexp
argument_list|,
name|regex_
argument_list|,
name|REG_EXTENDED
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Invalid pattern: %s\n"
argument_list|,
name|regex_
argument_list|)
expr_stmt|;
goto|goto
name|free_strings
goto|;
block|}
block|}
if|if
condition|(
name|do_all
operator|&&
name|system_wide
condition|)
name|git_config_from_file
argument_list|(
name|show_config
argument_list|,
name|system_wide
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|do_all
operator|&&
name|global
condition|)
name|git_config_from_file
argument_list|(
name|show_config
argument_list|,
name|global
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|git_config_from_file
argument_list|(
name|show_config
argument_list|,
name|local
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|do_all
operator|&&
operator|!
name|seen
operator|&&
name|global
condition|)
name|git_config_from_file
argument_list|(
name|show_config
argument_list|,
name|global
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|do_all
operator|&&
operator|!
name|seen
operator|&&
name|system_wide
condition|)
name|git_config_from_file
argument_list|(
name|show_config
argument_list|,
name|system_wide
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|key
argument_list|)
expr_stmt|;
if|if
condition|(
name|regexp
condition|)
block|{
name|regfree
argument_list|(
name|regexp
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|regexp
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|do_all
condition|)
name|ret
operator|=
operator|!
name|seen
expr_stmt|;
else|else
name|ret
operator|=
operator|(
name|seen
operator|==
literal|1
operator|)
condition|?
literal|0
else|:
name|seen
operator|>
literal|1
condition|?
literal|2
else|:
literal|1
expr_stmt|;
name|free_strings
label|:
name|free
argument_list|(
name|repo_config
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|global
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|normalize_value
specifier|static
name|char
modifier|*
name|normalize_value
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
name|char
modifier|*
name|normalized
decl_stmt|;
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|types
operator|==
literal|0
condition|)
name|normalized
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
else|else
block|{
name|normalized
operator|=
name|xmalloc
argument_list|(
literal|64
argument_list|)
expr_stmt|;
if|if
condition|(
name|types
operator|==
name|TYPE_INT
condition|)
block|{
name|int
name|v
init|=
name|git_config_int
argument_list|(
name|key
argument_list|,
name|value
argument_list|)
decl_stmt|;
name|sprintf
argument_list|(
name|normalized
argument_list|,
literal|"%d"
argument_list|,
name|v
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|types
operator|==
name|TYPE_BOOL
condition|)
name|sprintf
argument_list|(
name|normalized
argument_list|,
literal|"%s"
argument_list|,
name|git_config_bool
argument_list|(
name|key
argument_list|,
name|value
argument_list|)
condition|?
literal|"true"
else|:
literal|"false"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|types
operator|==
name|TYPE_BOOL_OR_INT
condition|)
block|{
name|int
name|is_bool
decl_stmt|,
name|v
decl_stmt|;
name|v
operator|=
name|git_config_bool_or_int
argument_list|(
name|key
argument_list|,
name|value
argument_list|,
operator|&
name|is_bool
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|is_bool
condition|)
name|sprintf
argument_list|(
name|normalized
argument_list|,
literal|"%d"
argument_list|,
name|v
argument_list|)
expr_stmt|;
else|else
name|sprintf
argument_list|(
name|normalized
argument_list|,
literal|"%s"
argument_list|,
name|v
condition|?
literal|"true"
else|:
literal|"false"
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|normalized
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|get_color_found
specifier|static
name|int
name|get_color_found
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_color_slot
specifier|static
specifier|const
name|char
modifier|*
name|get_color_slot
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_colorbool_slot
specifier|static
specifier|const
name|char
modifier|*
name|get_colorbool_slot
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|parsed_color
specifier|static
name|char
name|parsed_color
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_get_color_config
specifier|static
name|int
name|git_get_color_config
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
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
name|get_color_slot
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
expr_stmt|;
name|color_parse
argument_list|(
name|value
argument_list|,
name|var
argument_list|,
name|parsed_color
argument_list|)
expr_stmt|;
name|get_color_found
operator|=
literal|1
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_color
specifier|static
name|void
name|get_color
parameter_list|(
specifier|const
name|char
modifier|*
name|def_color
parameter_list|)
block|{
name|get_color_found
operator|=
literal|0
expr_stmt|;
name|parsed_color
index|[
literal|0
index|]
operator|=
literal|'\0'
expr_stmt|;
name|git_config
argument_list|(
name|git_get_color_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|get_color_found
operator|&&
name|def_color
condition|)
name|color_parse
argument_list|(
name|def_color
argument_list|,
literal|"command line"
argument_list|,
name|parsed_color
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|parsed_color
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|stdout_is_tty
specifier|static
name|int
name|stdout_is_tty
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_colorbool_found
specifier|static
name|int
name|get_colorbool_found
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_diff_color_found
specifier|static
name|int
name|get_diff_color_found
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_get_colorbool_config
specifier|static
name|int
name|git_get_colorbool_config
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
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
name|get_colorbool_slot
argument_list|)
condition|)
block|{
name|get_colorbool_found
operator|=
name|git_config_colorbool
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|stdout_is_tty
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"diff.color"
argument_list|)
condition|)
block|{
name|get_diff_color_found
operator|=
name|git_config_colorbool
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|stdout_is_tty
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"color.ui"
argument_list|)
condition|)
block|{
name|git_use_color_default
operator|=
name|git_config_colorbool
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|stdout_is_tty
argument_list|)
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
begin_function
DECL|function|get_colorbool
specifier|static
name|int
name|get_colorbool
parameter_list|(
name|int
name|print
parameter_list|)
block|{
name|get_colorbool_found
operator|=
operator|-
literal|1
expr_stmt|;
name|get_diff_color_found
operator|=
operator|-
literal|1
expr_stmt|;
name|git_config
argument_list|(
name|git_get_colorbool_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_colorbool_found
operator|<
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|get_colorbool_slot
argument_list|,
literal|"color.diff"
argument_list|)
condition|)
name|get_colorbool_found
operator|=
name|get_diff_color_found
expr_stmt|;
if|if
condition|(
name|get_colorbool_found
operator|<
literal|0
condition|)
name|get_colorbool_found
operator|=
name|git_use_color_default
expr_stmt|;
block|}
if|if
condition|(
name|print
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|get_colorbool_found
condition|?
literal|"true"
else|:
literal|"false"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
else|else
return|return
name|get_colorbool_found
condition|?
literal|0
else|:
literal|1
return|;
block|}
end_function
begin_function
DECL|function|cmd_config
name|int
name|cmd_config
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
name|unused_prefix
parameter_list|)
block|{
name|int
name|nongit
decl_stmt|;
name|char
modifier|*
name|value
decl_stmt|;
specifier|const
name|char
modifier|*
name|prefix
init|=
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
decl_stmt|;
name|config_exclusive_filename
operator|=
name|getenv
argument_list|(
name|CONFIG_ENVIRONMENT
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|builtin_config_options
argument_list|,
name|builtin_config_usage
argument_list|,
name|PARSE_OPT_STOP_AT_NON_OPTION
argument_list|)
expr_stmt|;
if|if
condition|(
name|use_global_config
operator|+
name|use_system_config
operator|+
operator|!
operator|!
name|given_config_file
operator|>
literal|1
condition|)
block|{
name|error
argument_list|(
literal|"only one config file at a time."
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|builtin_config_usage
argument_list|,
name|builtin_config_options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|use_global_config
condition|)
block|{
name|char
modifier|*
name|home
init|=
name|getenv
argument_list|(
literal|"HOME"
argument_list|)
decl_stmt|;
if|if
condition|(
name|home
condition|)
block|{
name|char
modifier|*
name|user_config
init|=
name|xstrdup
argument_list|(
name|mkpath
argument_list|(
literal|"%s/.gitconfig"
argument_list|,
name|home
argument_list|)
argument_list|)
decl_stmt|;
name|config_exclusive_filename
operator|=
name|user_config
expr_stmt|;
block|}
else|else
block|{
name|die
argument_list|(
literal|"$HOME not set"
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|use_system_config
condition|)
name|config_exclusive_filename
operator|=
name|git_etc_gitconfig
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
name|given_config_file
condition|)
block|{
if|if
condition|(
operator|!
name|is_absolute_path
argument_list|(
name|given_config_file
argument_list|)
operator|&&
name|prefix
condition|)
name|config_exclusive_filename
operator|=
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|strlen
argument_list|(
name|prefix
argument_list|)
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
else|else
name|config_exclusive_filename
operator|=
name|given_config_file
expr_stmt|;
block|}
if|if
condition|(
name|end_null
condition|)
block|{
name|term
operator|=
literal|'\0'
expr_stmt|;
name|delim
operator|=
literal|'\n'
expr_stmt|;
name|key_delim
operator|=
literal|'\n'
expr_stmt|;
block|}
if|if
condition|(
name|HAS_MULTI_BITS
argument_list|(
name|types
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"only one type at a time."
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|builtin_config_usage
argument_list|,
name|builtin_config_options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|get_color_slot
condition|)
name|actions
operator||=
name|ACTION_GET_COLOR
expr_stmt|;
if|if
condition|(
name|get_colorbool_slot
condition|)
name|actions
operator||=
name|ACTION_GET_COLORBOOL
expr_stmt|;
if|if
condition|(
name|HAS_MULTI_BITS
argument_list|(
name|actions
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"only one action at a time."
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|builtin_config_usage
argument_list|,
name|builtin_config_options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|actions
operator|==
literal|0
condition|)
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|1
case|:
name|actions
operator|=
name|ACTION_GET
expr_stmt|;
break|break;
case|case
literal|2
case|:
name|actions
operator|=
name|ACTION_SET
expr_stmt|;
break|break;
case|case
literal|3
case|:
name|actions
operator|=
name|ACTION_SET_ALL
expr_stmt|;
break|break;
default|default:
name|usage_with_options
argument_list|(
name|builtin_config_usage
argument_list|,
name|builtin_config_options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|actions
operator|==
name|ACTION_LIST
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|git_config
argument_list|(
name|show_all_config
argument_list|,
name|NULL
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|config_exclusive_filename
condition|)
name|die
argument_list|(
literal|"unable to read config file %s: %s"
argument_list|,
name|config_exclusive_filename
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|die
argument_list|(
literal|"error processing config file(s)"
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_EDIT
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|launch_editor
argument_list|(
name|config_exclusive_filename
condition|?
name|config_exclusive_filename
else|:
name|git_path
argument_list|(
literal|"config"
argument_list|)
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_SET
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|2
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|value
operator|=
name|normalize_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
name|git_config_set
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|value
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_SET_ALL
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|2
argument_list|,
literal|3
argument_list|)
expr_stmt|;
name|value
operator|=
name|normalize_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|value
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|,
literal|0
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_ADD
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|2
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|value
operator|=
name|normalize_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|value
argument_list|,
literal|"^$"
argument_list|,
literal|0
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_REPLACE_ALL
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|2
argument_list|,
literal|3
argument_list|)
expr_stmt|;
name|value
operator|=
name|normalize_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|value
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|,
literal|1
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_GET
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|get_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_GET_ALL
condition|)
block|{
name|do_all
operator|=
literal|1
expr_stmt|;
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|get_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_GET_REGEXP
condition|)
block|{
name|show_keys
operator|=
literal|1
expr_stmt|;
name|use_key_regexp
operator|=
literal|1
expr_stmt|;
name|do_all
operator|=
literal|1
expr_stmt|;
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|get_value
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_UNSET
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|2
condition|)
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|,
literal|0
argument_list|)
return|;
else|else
return|return
name|git_config_set
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_UNSET_ALL
condition|)
block|{
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|2
argument_list|)
expr_stmt|;
return|return
name|git_config_set_multivar
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
return|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_RENAME_SECTION
condition|)
block|{
name|int
name|ret
decl_stmt|;
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|2
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|ret
operator|=
name|git_config_rename_section
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
return|return
name|ret
return|;
if|if
condition|(
name|ret
operator|==
literal|0
condition|)
name|die
argument_list|(
literal|"No such section!"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_REMOVE_SECTION
condition|)
block|{
name|int
name|ret
decl_stmt|;
name|check_argc
argument_list|(
name|argc
argument_list|,
literal|1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|ret
operator|=
name|git_config_rename_section
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
return|return
name|ret
return|;
if|if
condition|(
name|ret
operator|==
literal|0
condition|)
name|die
argument_list|(
literal|"No such section!"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_GET_COLOR
condition|)
block|{
name|get_color
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|actions
operator|==
name|ACTION_GET_COLORBOOL
condition|)
block|{
if|if
condition|(
name|argc
operator|==
literal|1
condition|)
name|stdout_is_tty
operator|=
name|git_config_bool
argument_list|(
literal|"command line"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|==
literal|0
condition|)
name|stdout_is_tty
operator|=
name|isatty
argument_list|(
literal|1
argument_list|)
expr_stmt|;
return|return
name|get_colorbool
argument_list|(
name|argc
operator|!=
literal|0
argument_list|)
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
