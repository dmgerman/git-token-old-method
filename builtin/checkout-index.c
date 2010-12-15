begin_unit
begin_comment
comment|/*  * Check-out files from the "current cache directory"  *  * Copyright (C) 2005 Linus Torvalds  *  * Careful: order of argument flags does matter. For example,  *  *	git checkout-index -a -f file.c  *  * Will first check out all files listed in the cache (but not  * overwrite any old ones), and then force-checkout "file.c" a  * second time (ie that one _will_ overwrite any old contents  * with the same filename).  *  * Also, just doing "git checkout-index" does nothing. You probably  * meant "git checkout-index -a". And if you want to force it, you  * want "git checkout-index -f -a".  *  * Intuitiveness is not the goal here. Repeatability is. The  * reason for the "no arguments means no work" thing is that  * from scripts you are supposed to be able to do things like  *  *	find . -name '*.h' -print0 | xargs -0 git checkout-index -f --  *  * or:  *  *	find . -name '*.h' -print0 | git checkout-index -f -z --stdin  *  * which will force all existing *.h files to be replaced with  * their cached copies. If an empty command line implied "all",  * then this would force-refresh everything in the cache, which  * was not the point.  *  * Oh, and the "--" is just a good idea when you know the rest  * will be filenames. Just so that you wouldn't have a filename  * of "-a" causing problems (not possible in the above example,  * but get used to it in scripting!).  */
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
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_define
DECL|macro|CHECKOUT_ALL
define|#
directive|define
name|CHECKOUT_ALL
value|4
end_define
begin_decl_stmt
DECL|variable|line_termination
specifier|static
name|int
name|line_termination
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|checkout_stage
specifier|static
name|int
name|checkout_stage
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|checkout_stage
comment|/* default to checkout stage0 */
end_comment
begin_decl_stmt
DECL|variable|to_tempfile
specifier|static
name|int
name|to_tempfile
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|topath
specifier|static
name|char
name|topath
index|[
literal|4
index|]
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|state
specifier|static
name|struct
name|checkout
name|state
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|write_tempfile_record
specifier|static
name|void
name|write_tempfile_record
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|prefix_length
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
name|CHECKOUT_ALL
operator|==
name|checkout_stage
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
literal|4
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|i
operator|>
literal|1
condition|)
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
if|if
condition|(
name|topath
index|[
name|i
index|]
index|[
literal|0
index|]
condition|)
name|fputs
argument_list|(
name|topath
index|[
name|i
index|]
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'.'
argument_list|)
expr_stmt|;
block|}
block|}
else|else
name|fputs
argument_list|(
name|topath
index|[
name|checkout_stage
index|]
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\t'
argument_list|)
expr_stmt|;
name|write_name_quoted
argument_list|(
name|name
operator|+
name|prefix_length
argument_list|,
name|stdout
argument_list|,
name|line_termination
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
literal|4
condition|;
name|i
operator|++
control|)
block|{
name|topath
index|[
name|i
index|]
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|checkout_file
specifier|static
name|int
name|checkout_file
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|prefix_length
parameter_list|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
decl_stmt|;
name|int
name|has_same_name
init|=
literal|0
decl_stmt|;
name|int
name|did_checkout
init|=
literal|0
decl_stmt|;
name|int
name|errs
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|pos
index|]
decl_stmt|;
if|if
condition|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|!=
name|namelen
operator|||
name|memcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|namelen
argument_list|)
condition|)
break|break;
name|has_same_name
operator|=
literal|1
expr_stmt|;
name|pos
operator|++
expr_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|!=
name|checkout_stage
operator|&&
operator|(
name|CHECKOUT_ALL
operator|!=
name|checkout_stage
operator|||
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|)
condition|)
continue|continue;
name|did_checkout
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|checkout_entry
argument_list|(
name|ce
argument_list|,
operator|&
name|state
argument_list|,
name|to_tempfile
condition|?
name|topath
index|[
name|ce_stage
argument_list|(
name|ce
argument_list|)
index|]
else|:
name|NULL
argument_list|)
operator|<
literal|0
condition|)
name|errs
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|did_checkout
condition|)
block|{
if|if
condition|(
name|to_tempfile
condition|)
name|write_tempfile_record
argument_list|(
name|name
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
return|return
name|errs
operator|>
literal|0
condition|?
operator|-
literal|1
else|:
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|state
operator|.
name|quiet
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git checkout-index: %s "
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_same_name
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"is not in the cache"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|checkout_stage
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"does not exist at stage %d"
argument_list|,
name|checkout_stage
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"is unmerged"
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|checkout_all
specifier|static
name|void
name|checkout_all
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|prefix_length
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|errs
init|=
literal|0
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|last_ce
init|=
name|NULL
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|!=
name|checkout_stage
operator|&&
operator|(
name|CHECKOUT_ALL
operator|!=
name|checkout_stage
operator|||
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|)
condition|)
continue|continue;
if|if
condition|(
name|prefix
operator|&&
operator|*
name|prefix
operator|&&
operator|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|<=
name|prefix_length
operator|||
name|memcmp
argument_list|(
name|prefix
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|prefix_length
argument_list|)
operator|)
condition|)
continue|continue;
if|if
condition|(
name|last_ce
operator|&&
name|to_tempfile
condition|)
block|{
if|if
condition|(
name|ce_namelen
argument_list|(
name|last_ce
argument_list|)
operator|!=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|||
name|memcmp
argument_list|(
name|last_ce
operator|->
name|name
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ce_namelen
argument_list|(
name|ce
argument_list|)
argument_list|)
condition|)
name|write_tempfile_record
argument_list|(
name|last_ce
operator|->
name|name
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|checkout_entry
argument_list|(
name|ce
argument_list|,
operator|&
name|state
argument_list|,
name|to_tempfile
condition|?
name|topath
index|[
name|ce_stage
argument_list|(
name|ce
argument_list|)
index|]
else|:
name|NULL
argument_list|)
operator|<
literal|0
condition|)
name|errs
operator|++
expr_stmt|;
name|last_ce
operator|=
name|ce
expr_stmt|;
block|}
if|if
condition|(
name|last_ce
operator|&&
name|to_tempfile
condition|)
name|write_tempfile_record
argument_list|(
name|last_ce
operator|->
name|name
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
if|if
condition|(
name|errs
condition|)
comment|/* we have already done our error reporting. 		 * exit with the same code as die(). 		 */
name|exit
argument_list|(
literal|128
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|builtin_checkout_index_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_checkout_index_usage
index|[]
init|=
block|{
literal|"git checkout-index [options] [--] [<file>...]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|lock_file
specifier|static
name|struct
name|lock_file
name|lock_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|option_parse_u
specifier|static
name|int
name|option_parse_u
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
name|int
modifier|*
name|newfd
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|state
operator|.
name|refresh_cache
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|*
name|newfd
operator|<
literal|0
condition|)
operator|*
name|newfd
operator|=
name|hold_locked_index
argument_list|(
operator|&
name|lock_file
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_z
specifier|static
name|int
name|option_parse_z
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
if|if
condition|(
name|unset
condition|)
name|line_termination
operator|=
literal|'\n'
expr_stmt|;
else|else
name|line_termination
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_prefix
specifier|static
name|int
name|option_parse_prefix
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
name|state
operator|.
name|base_dir
operator|=
name|arg
expr_stmt|;
name|state
operator|.
name|base_dir_len
operator|=
name|strlen
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|option_parse_stage
specifier|static
name|int
name|option_parse_stage
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"all"
argument_list|)
condition|)
block|{
name|to_tempfile
operator|=
literal|1
expr_stmt|;
name|checkout_stage
operator|=
name|CHECKOUT_ALL
expr_stmt|;
block|}
else|else
block|{
name|int
name|ch
init|=
name|arg
index|[
literal|0
index|]
decl_stmt|;
if|if
condition|(
literal|'1'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'3'
condition|)
name|checkout_stage
operator|=
name|arg
index|[
literal|0
index|]
operator|-
literal|'0'
expr_stmt|;
else|else
name|die
argument_list|(
literal|"stage should be between 1 and 3 or all"
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_checkout_index
name|int
name|cmd_checkout_index
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
name|newfd
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|all
init|=
literal|0
decl_stmt|;
name|int
name|read_from_stdin
init|=
literal|0
decl_stmt|;
name|int
name|prefix_length
decl_stmt|;
name|int
name|force
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|,
name|not_new
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|builtin_checkout_index_options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'a'
argument_list|,
literal|"all"
argument_list|,
operator|&
name|all
argument_list|,
literal|"checks out all files in the index"
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
literal|"forces overwrite of existing files"
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'n'
argument_list|,
literal|"no-create"
argument_list|,
operator|&
name|not_new
argument_list|,
literal|"don't checkout new files"
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'u'
block|,
literal|"index"
block|,
operator|&
name|newfd
block|,
name|NULL
block|,
literal|"update stat information in the index file"
block|,
name|PARSE_OPT_NOARG
block|,
name|option_parse_u
block|}
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'z'
block|,
name|NULL
block|,
name|NULL
block|,
name|NULL
block|,
literal|"paths are separated with NUL character"
block|,
name|PARSE_OPT_NOARG
block|,
name|option_parse_z
block|}
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"stdin"
argument_list|,
operator|&
name|read_from_stdin
argument_list|,
literal|"read list of paths from the standard input"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|0
argument_list|,
literal|"temp"
argument_list|,
operator|&
name|to_tempfile
argument_list|,
literal|"write the content to temporary files"
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|0
argument_list|,
literal|"prefix"
argument_list|,
name|NULL
argument_list|,
literal|"string"
argument_list|,
literal|"when creating files, prepend<string>"
argument_list|,
name|option_parse_prefix
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|0
argument_list|,
literal|"stage"
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|"copy out the files from named stage"
argument_list|,
name|option_parse_stage
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|state
operator|.
name|base_dir
operator|=
literal|""
expr_stmt|;
name|prefix_length
operator|=
name|prefix
condition|?
name|strlen
argument_list|(
name|prefix
argument_list|)
else|:
literal|0
expr_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
block|{
name|die
argument_list|(
literal|"invalid cache"
argument_list|)
expr_stmt|;
block|}
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
name|builtin_checkout_index_options
argument_list|,
name|builtin_checkout_index_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|state
operator|.
name|force
operator|=
name|force
expr_stmt|;
name|state
operator|.
name|quiet
operator|=
name|quiet
expr_stmt|;
name|state
operator|.
name|not_new
operator|=
name|not_new
expr_stmt|;
if|if
condition|(
name|state
operator|.
name|base_dir_len
operator|||
name|to_tempfile
condition|)
block|{
comment|/* when --prefix is specified we do not 		 * want to update cache. 		 */
if|if
condition|(
name|state
operator|.
name|refresh_cache
condition|)
block|{
name|rollback_lock_file
argument_list|(
operator|&
name|lock_file
argument_list|)
expr_stmt|;
name|newfd
operator|=
operator|-
literal|1
expr_stmt|;
block|}
name|state
operator|.
name|refresh_cache
operator|=
literal|0
expr_stmt|;
block|}
comment|/* Check out named files first */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|all
condition|)
name|die
argument_list|(
literal|"git checkout-index: don't mix '--all' and explicit filenames"
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_from_stdin
condition|)
name|die
argument_list|(
literal|"git checkout-index: don't mix '--stdin' and explicit filenames"
argument_list|)
expr_stmt|;
name|p
operator|=
name|prefix_path
argument_list|(
name|prefix
argument_list|,
name|prefix_length
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|checkout_file
argument_list|(
name|p
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
operator|<
name|arg
operator|||
name|p
operator|>
name|arg
operator|+
name|strlen
argument_list|(
name|arg
argument_list|)
condition|)
name|free
argument_list|(
operator|(
name|char
operator|*
operator|)
name|p
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|read_from_stdin
condition|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|,
name|nbuf
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|all
condition|)
name|die
argument_list|(
literal|"git checkout-index: don't mix '--all' and '--stdin'"
argument_list|)
expr_stmt|;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
operator|!=
name|EOF
condition|)
block|{
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|line_termination
operator|&&
name|buf
operator|.
name|buf
index|[
literal|0
index|]
operator|==
literal|'"'
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|nbuf
argument_list|)
expr_stmt|;
if|if
condition|(
name|unquote_c_style
argument_list|(
operator|&
name|nbuf
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|NULL
argument_list|)
condition|)
name|die
argument_list|(
literal|"line is badly quoted"
argument_list|)
expr_stmt|;
name|strbuf_swap
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
name|nbuf
argument_list|)
expr_stmt|;
block|}
name|p
operator|=
name|prefix_path
argument_list|(
name|prefix
argument_list|,
name|prefix_length
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
name|checkout_file
argument_list|(
name|p
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
operator|<
name|buf
operator|.
name|buf
operator|||
name|p
operator|>
name|buf
operator|.
name|buf
operator|+
name|buf
operator|.
name|len
condition|)
name|free
argument_list|(
operator|(
name|char
operator|*
operator|)
name|p
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|nbuf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|all
condition|)
name|checkout_all
argument_list|(
name|prefix
argument_list|,
name|prefix_length
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|newfd
operator|&&
operator|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
operator|&
name|lock_file
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"Unable to write new index file"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
