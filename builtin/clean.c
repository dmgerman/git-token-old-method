begin_unit
begin_comment
comment|/*  * "git clean" builtin command  *  * Copyright (C) 2007 Shawn Bohrer  *  * Based on git-clean.sh by Pavel Roskin  */
end_comment
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
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_decl_stmt
DECL|variable|force
specifier|static
name|int
name|force
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|force
comment|/* unset */
end_comment
begin_decl_stmt
DECL|variable|builtin_clean_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_clean_usage
index|[]
init|=
block|{
literal|"git clean [-d] [-f] [-n] [-q] [-e<pattern>] [-x | -X] [--]<paths>..."
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_clean_config
specifier|static
name|int
name|git_clean_config
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
literal|"clean.requireforce"
argument_list|)
condition|)
name|force
operator|=
operator|!
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|exclude_cb
specifier|static
name|int
name|exclude_cb
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|string_list
modifier|*
name|exclude_list
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|string_list_append
argument_list|(
name|exclude_list
argument_list|,
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_clean
name|int
name|cmd_clean
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
name|int
name|i
decl_stmt|;
name|int
name|show_only
init|=
literal|0
decl_stmt|,
name|remove_directories
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|,
name|ignored
init|=
literal|0
decl_stmt|;
name|int
name|ignored_only
init|=
literal|0
decl_stmt|,
name|baselen
init|=
literal|0
decl_stmt|,
name|config_set
init|=
literal|0
decl_stmt|,
name|errors
init|=
literal|0
decl_stmt|;
name|int
name|rm_flags
init|=
name|REMOVE_DIR_KEEP_NESTED_GIT
decl_stmt|;
name|struct
name|strbuf
name|directory
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|dir_struct
name|dir
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|string_list
name|exclude_list
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
specifier|const
name|char
modifier|*
name|qname
decl_stmt|;
name|char
modifier|*
name|seen
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
literal|"be quiet"
argument_list|)
block|,
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|,
literal|"dry run"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'f'
argument_list|,
literal|"force"
argument_list|,
operator|&
name|force
argument_list|,
literal|"force"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'d'
argument_list|,
name|NULL
argument_list|,
operator|&
name|remove_directories
argument_list|,
literal|"remove whole directories"
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'e'
block|,
literal|"exclude"
block|,
operator|&
name|exclude_list
block|,
literal|"pattern"
block|,
literal|"exclude<pattern>"
block|,
name|PARSE_OPT_NONEG
block|,
name|exclude_cb
block|}
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'x'
argument_list|,
name|NULL
argument_list|,
operator|&
name|ignored
argument_list|,
literal|"remove ignored files, too"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'X'
argument_list|,
name|NULL
argument_list|,
operator|&
name|ignored_only
argument_list|,
literal|"remove only ignored files"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_clean_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
operator|<
literal|0
condition|)
name|force
operator|=
literal|0
expr_stmt|;
else|else
name|config_set
operator|=
literal|1
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|builtin_clean_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|dir
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|dir
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ignored_only
condition|)
name|dir
operator|.
name|flags
operator||=
name|DIR_SHOW_IGNORED
expr_stmt|;
if|if
condition|(
name|ignored
operator|&&
name|ignored_only
condition|)
name|die
argument_list|(
literal|"-x and -X cannot be used together"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
operator|&&
operator|!
name|force
condition|)
name|die
argument_list|(
literal|"clean.requireForce %s to true and neither -n nor -f given; "
literal|"refusing to clean"
argument_list|,
name|config_set
condition|?
literal|"set"
else|:
literal|"defaults"
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
operator|>
literal|1
condition|)
name|rm_flags
operator|=
literal|0
expr_stmt|;
name|dir
operator|.
name|flags
operator||=
name|DIR_SHOW_OTHER_DIRECTORIES
expr_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"index file corrupt"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ignored
condition|)
name|setup_standard_excludes
argument_list|(
operator|&
name|dir
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
name|exclude_list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
name|add_exclude
argument_list|(
name|exclude_list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|dir
operator|.
name|exclude_list
argument_list|)
expr_stmt|;
name|pathspec
operator|=
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
name|fill_directory
argument_list|(
operator|&
name|dir
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathspec
condition|)
name|seen
operator|=
name|xmalloc
argument_list|(
name|argc
operator|>
literal|0
condition|?
name|argc
else|:
literal|1
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
name|dir
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|.
name|entries
index|[
name|i
index|]
decl_stmt|;
name|int
name|len
decl_stmt|,
name|pos
decl_stmt|;
name|int
name|matches
init|=
literal|0
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
comment|/* 		 * Remove the '/' at the end that directory 		 * walking adds for directory entries. 		 */
name|len
operator|=
name|ent
operator|->
name|len
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|ent
operator|->
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|len
operator|--
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
continue|continue;
comment|/* exact match */
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|==
name|len
operator|&&
operator|!
name|memcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|len
argument_list|)
condition|)
continue|continue;
comment|/* Yup, this one exists unmerged */
block|}
comment|/* 		 * we might have removed this as part of earlier 		 * recursive directory removal, so lstat() here could 		 * fail with ENOENT. 		 */
if|if
condition|(
name|lstat
argument_list|(
name|ent
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|pathspec
condition|)
block|{
name|memset
argument_list|(
name|seen
argument_list|,
literal|0
argument_list|,
name|argc
operator|>
literal|0
condition|?
name|argc
else|:
literal|1
argument_list|)
expr_stmt|;
name|matches
operator|=
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|len
argument_list|,
name|baselen
argument_list|,
name|seen
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|directory
argument_list|,
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
name|qname
operator|=
name|quote_path_relative
argument_list|(
name|directory
operator|.
name|buf
argument_list|,
name|directory
operator|.
name|len
argument_list|,
operator|&
name|buf
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_only
operator|&&
operator|(
name|remove_directories
operator|||
operator|(
name|matches
operator|==
name|MATCHED_EXACTLY
operator|)
operator|)
condition|)
block|{
name|printf
argument_list|(
literal|"Would remove %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|remove_directories
operator|||
operator|(
name|matches
operator|==
name|MATCHED_EXACTLY
operator|)
condition|)
block|{
if|if
condition|(
operator|!
name|quiet
condition|)
name|printf
argument_list|(
literal|"Removing %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
if|if
condition|(
name|remove_dir_recursively
argument_list|(
operator|&
name|directory
argument_list|,
name|rm_flags
argument_list|)
operator|!=
literal|0
condition|)
block|{
name|warning
argument_list|(
literal|"failed to remove '%s'"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
name|errors
operator|++
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|show_only
condition|)
block|{
name|printf
argument_list|(
literal|"Would not remove %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf
argument_list|(
literal|"Not removing %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|directory
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|pathspec
operator|&&
operator|!
name|matches
condition|)
continue|continue;
name|qname
operator|=
name|quote_path_relative
argument_list|(
name|ent
operator|->
name|name
argument_list|,
operator|-
literal|1
argument_list|,
operator|&
name|buf
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_only
condition|)
block|{
name|printf
argument_list|(
literal|"Would remove %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|quiet
condition|)
block|{
name|printf
argument_list|(
literal|"Removing %s\n"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|unlink
argument_list|(
name|ent
operator|->
name|name
argument_list|)
operator|!=
literal|0
condition|)
block|{
name|warning
argument_list|(
literal|"failed to remove '%s'"
argument_list|,
name|qname
argument_list|)
expr_stmt|;
name|errors
operator|++
expr_stmt|;
block|}
block|}
block|}
name|free
argument_list|(
name|seen
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|directory
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|exclude_list
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
operator|(
name|errors
operator|!=
literal|0
operator|)
return|;
block|}
end_function
end_unit
